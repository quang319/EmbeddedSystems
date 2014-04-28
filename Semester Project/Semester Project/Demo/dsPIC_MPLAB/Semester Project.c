/*******************************************************************************
 *  Project:       
 *
 *  Name:
 *
 *  Date:
 *
 *  Function:
 *
 *  Pins used:
 *
 *  Peripherals used:
 *
 *  Comments:
 *
 ********************************************************************************/

/*******************************************************************************
 *
 *                      Library includes
 *
 *******************************************************************************/
#include <p33FJ256GP710A.h>
#include "ADC.h"
#include <string.h>
/* Standard includes. */
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include <portmacro.h>
#include <semphr.h>

/* Demo application includes. */
#include "BlockQ.h"
#include "crflash.h"
#include "blocktim.h"
#include "integer.h"
#include "comtest2.h"
#include "partest.h"
#include "lcd.h"
#include "timertest.h"

/* User's includes  */
#include "ADC.h"

/*******************************************************************************
 *
 *                       Configuration Bits
 *
 ******************************************************************************/
_FGS(GSS_OFF & GWRP_OFF);
_FOSCSEL(FNOSC_PRIPLL & IESO_ON);
_FOSC(FCKSM_CSDCMD & POSCMD_XT & OSCIOFNC_OFF);
_FWDT(FWDTEN_OFF);
/*******************************************************************************
 *
 *                      Constant Declarations
 *
 *******************************************************************************/
#define TRUE 1
#define FALSE 0

/*******************************************************************************
 *
 *                      Type definition
 *
 *******************************************************************************/
typedef struct
{
    char Line1[17];
    char Line2[17];
} xLCD;

typedef struct 
{
    int Vrms;
    int Irms;
    int Paverage;
    int Papparent;
    int Phase;
} xCalc;
/*******************************************************************************
 *
 *                      Global Variable Declarations
 *
 *******************************************************************************/
xQueueHandle LCDDisplayinfo;
unsigned int VSignal[120], ISignal[120];

long VoltageSum = 0, VoltageSquareSum = 0;
long VoltageSumSquareAverage = 0;          // Contains result of ( ( sum(V)^2) / N)
int VoltagePreSquareRoot = 0;

long CurrentSum = 0, CurrentSquareSum = 0;
long CurrentSumSquareAverage = 0;          // Contains result of ( ( sum(V)^2) / N)
int CurrentPreSquareRoot = 0;

long PowerSum = 0;                         // Contains sum(V*I)
/*******************************************************************************
 *
 *                      Semaphore Declarations
 *
 *******************************************************************************/
xSemaphoreHandle DmaSemaphore;
/*******************************************************************************
 *
 *                      Task Declarations
 *
 *******************************************************************************/
void buttonPush( void *pvParameters);
void writeLcd (void *pvParameters);
void prvSetupHardware( void );
void vApplicationIdleHook(void);

static void dmaHandler (void *pvParameters);

/*******************************************************************************
 *
 *                      Function Prototype
 *
 *******************************************************************************/

void initialize();

char IntToChar(unsigned int Digit);


void displayCounter(int Count, char *LCD);
void digitCheck(int *FirstDigit, int *SecondDigit);
void BinaryToDecimal ( int Value, char *TenthPlace, char *OnePlace);

/*******************************************************************************
 *
 *                      Main Function
 *
 *******************************************************************************/
int main() {
    // setting up everything
    initialize();

    /*************** Main Loop ***********************************/
    /* Will only reach here if there is insufficient heap available to start
    the scheduler. */
    while (1) {

    } // End of the infinite While loop

    return 1;
}

/*******************************************************************************
 *
 *                      Initialize Function
 *
 *******************************************************************************/
void initialize() {
    /* Configure any hardware required for this demo. */
    PLLFBD = 0x001E; //external osc is 8MHz using defaults Fosc=8MHZ /8 (0x1E+2)=32 MHz

    //RTOS setup of hardware
    prvSetupHardware();
    /*****************************************************
    *               Setting up DMA and Semaphore
    *****************************************************/
     // Setting up ADC for DMA Interrupt
      ADC_Init();
    // Create semaphore for DMA interrupt
    vSemaphoreCreateBinary(DmaSemaphore);
    // Create Semaphore handle
    xTaskCreate(dmaHandler, "Task for DMA", 1000, NULL, 3, NULL );
    
    /*****************************************************
    *               Initialize LCD
    *****************************************************/
    Init_LCD();//Initialize the LCD this must be done first before the LCD is to be used.
    while (check_busy())
    {
    }
    home_clr(); // start display at top line left most character

    /*****************************************************
    *               Create Tasks and Queue
    *****************************************************/
    xTaskCreate(buttonPush,"Task Read Swtchs", 200, NULL, 2, NULL);
    xTaskCreate(writeLcd, "Write LCD",200, NULL, 1, NULL); //notice that this task has lower priority.
    /* create the queue for the string to be displayed.  Length = 2 each chunk holds 16 char string*/
    LCDDisplayinfo = xQueueCreate(1,sizeof (xLCD));

    /* start the scheduler. */
    vTaskStartScheduler();
}

/*******************************************************************************
 *
 *                      Interrupt Service Routine
 *
 *******************************************************************************/
void __attribute__((interrupt)) _DMA0Interrupt(void)
 {
    static signed portBASE_TYPE xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
     // Unblock the task by releasing the semaphore.
     xSemaphoreGiveFromISR( DmaSemaphore, &xHigherPriorityTaskWoken );
     _DMA0IF = 0;
    if (xHigherPriorityTaskWoken == pdTRUE)
        taskYIELD();
 }
/*******************************************************************************
 *
 *                      ROS tasks
 *
 *******************************************************************************/
//This is required by the RTOS, DO NOT MOTIFY
void prvSetupHardware( void )
{
    vParTestInitialise();
}

void dmaHandler (void *pvParameters)
{
   int *DMAptr;
   xCalc Data;
   int i;
   for( ;; )
   {
       // Block waiting for the semaphore to become available.
       xSemaphoreTake(DmaSemaphore, portMAX_DELAY );
           /****************************
           *   Getting raw data from the DMA
           *****************************/
       asm("NOP");
       DMAptr = BeginDMA;
      for (i=0; i<120; i++)
      {
          VSignal[i] = *DMAptr;
          DMAptr++;
          ISignal[i] = *DMAptr;
          DMAptr++;
      }
           /***************************
           *   Calculate RMS, Average Power, Apparent Power, and Phase Angle
           *
           *   Vrms = sqrt( ( sum(V^2) - ( ( sum(V)^2) / N) ) / N)
           *   Irms = sqrt( ( sum(I^2) - ( ( sum(I)^2) / N) ) / N)
           *   Pavg = ( sum(V*I) / N) - ( ( sum(V) * (sum(I) ) ) / N^2)
           *   Papp = Vrms * Irms
           ****************************/

           long PowerSum = 0;                         // Contains sum(V*I)

           // Performing all summations
           for (i = 0; i < 120; i++)
           {
               //sum(V)
               VoltageSum += VSignal[i];
               //sum(V^2)
               VoltageSquareSum += (long)VSignal[i] * (long)VSignal[i];

               //sum(I)
               CurrentSum += ISignal[i];
               //sum(I^2)
               CurrentSquareSum += (long)ISignal[i] * (long)ISignal[i];

               //sum(V*I)
               PowerSum += (long)VSignal[i] * (long)ISignal[i];
           }
          // xSemaphoreGive( DmaSemaphore );
           // We have finished our task.  Return to the top of the loop where
           // we will block on the semaphore until it is time to execute
           // again.  Note when using the semaphore for synchronisation with an
           // ISR in this manner there is no need to 'give' the semaphore back.
   }
}

/*-----------------------------------------------------------*/
void buttonPush( void *pvParameters)
{
    /* This task will read the pushbuttons, since most pushes of the button will take more than
    10 ms it needs to only run every 10 ms or l00ms if in the middle of a button push read.
    The API for a delay is: vTaskDelay(10); to delay for 10ms before being active again */

    
    xLCD LCDDisplay = {.Line1 = "Welcome! Please ", .Line2 = "Press S3 to Cont"};
    char DisplayIndex = 0;
    char DisplayFlag = 0;

    int Rd6ChangeStateFlag = 0;     // Use to indicate whether the user is holding down the button
    int BlockingFlag = 0;           // 0 = 10 ms
                                    // 1 = 100 ms
    xQueueSendToFront(LCDDisplayinfo, &LCDDisplay,0);
    /* start endless loop*/
    for( ;; )
    {
        /*****************************************************
        *   Capture button presses
        *   Increment DisplayIndex if S3 is pressed
        *   Roll back to 0 after 3
        *****************************************************/
        // if RD6(S3) is pressed
        // Display count on LCD
        if ( (_RD6 == 0) && (Rd6ChangeStateFlag == 0))
        {
            Rd6ChangeStateFlag = 1;
            DisplayIndex++;
            DisplayFlag = 1;
            if (DisplayIndex >= 4)
                DisplayIndex = 0;
            //Scan again every 100 ms
            BlockingFlag = 1;
        }
        // If the user just released the button
        else if ( (_RD6 == 1) && (Rd6ChangeStateFlag == 1))
        {
            Rd6ChangeStateFlag = 0;
            BlockingFlag = 0;
        }

        /*****************************************************
        *  Loading the queue with the proper message
        *   Screen 0
        *       -   Vrms = XXX.XX V
        *       -   Irms = XXX.XX A
        *   Screen 1
        *       -   Average Power
        *       -   = XXX.XX W
        *   Screen 2
        *       -   Instan. Power
        *       -   = XXX.XX VA
        *   Screen 3
        *       -   Phase Angle
        *       -   = Leading/Lagging
        *
        *****************************************************/
        if ((DisplayIndex == 0) && (DisplayFlag == 1))
        {
            strcpy(LCDDisplay.Line1, "Vrms = XXX.XX V ");
            strcpy(LCDDisplay.Line2, "Irms = XXX.XX A ");
        }
        else if ((DisplayIndex == 1) && (DisplayFlag == 1))
        {
            strcpy(LCDDisplay.Line1, "Average Power   ");
            strcpy(LCDDisplay.Line2, "= XXX.XX W      ");
        }
        else if ((DisplayIndex == 2) && (DisplayFlag == 1))
        {
            strcpy(LCDDisplay.Line1, "Instan. Power   ");
            strcpy(LCDDisplay.Line2, "= XXX.XX W      ");
        }
        else if ((DisplayIndex == 3) && (DisplayFlag == 1))
        {
            strcpy(LCDDisplay.Line1, "Phase Angle     ");
            strcpy(LCDDisplay.Line2, "= Leading       ");
        }

        if (DisplayFlag == 1)
        {
            xQueueSendToFront(LCDDisplayinfo, &LCDDisplay,0);
            DisplayFlag = 0;
        }

        // How long to block depends on if the user is holding the button down or not.
        // If hold = delay for 100 ms
        // if not holding = delay for 10 ms
        if (BlockingFlag == 0){
            vTaskDelay(10/ portTICK_RATE_MS); }
        else
            vTaskDelay(100/ portTICK_RATE_MS);
    } // end of for loop
}

void writeLcd(void *pvParameters)
{
    // This is lower priority task it will write charaters to the LCD screen in response to the button
    //pushes an example API of reading the queue is given

    int LCDCounter = 0;
    char SecondLineFlag = 0;

    portBASE_TYPE NewString;
    xLCD LCDDisplayString;
    for ( ; ; )
    {
        NewString = xQueueReceive( LCDDisplayinfo,&LCDDisplayString,10);

        // If we recieve a new message.
        // Need to bring the display back to the beginning and start displayin the new data
        if (NewString == pdPASS)
        {// get new string and start displaying
            LCDCounter = 0;
            SecondLineFlag = 0;
            home_it();
            vTaskDelay( 10 / portTICK_RATE_MS);
        }

        // if not, we are going to print one character at a time
        // We are going to wait for a new queque once we reached the end
        // of the string
        else
        {
            //Check if busy
            if (check_busy() == 0)
            {
                // Print line 1
                if (LCDCounter < 16)
                {
                    // Printer Character on LCD
                    lcd_data( LCDDisplayString.Line1[LCDCounter] );
                    LCDCounter++;
                }
                // move to next line
                else if ( (LCDCounter >= 16) && (SecondLineFlag == 0) )
                {
                    line_2();
                    SecondLineFlag = 1;
                }
                // Print line 2
                else if ((LCDCounter >= 16) && (LCDCounter <= 32) && (SecondLineFlag == 1) )
                {
                    // Printer Character on LCD
                    lcd_data( LCDDisplayString.Line2[LCDCounter - 16] );
                    LCDCounter++;
                }
                vTaskDelay( 10 / portTICK_RATE_MS);
            }
        }
    }//end of for loop
}

//
//void rtcTimer(void *pvParameters)
//{ //This is where you place your code to keep track of the RTC and the display that it will show.
//  //Initialize and declare
//     In order to get true timing you should use the periodic delay function to set this up
//    some code is given... see lecture 17 for details
//    in addition you will want to write a string to queue for either RTC display or count display
//    sample API function call is given to do this.
//
//    portTickType xLastWakeTime;
//    xLastWakeTime = xTaskGetTickCount();
//   // xLCD LCDDisplay = {.Line1 = "Hello World!    ", .Line2 = "I am awesome!   "};
//    for( ;; )
//    {
//
//        //xQueueSendToBack(LCDDisplayinfo, &LCDDisplay, 0);
//        // The LCD needs more than .5 second to print the 2 lines
//        vTaskDelayUntil(&xLastWakeTime,700); // wait 0.5 seconds
//    }
//}


void vApplicationIdleHook( void)
{
//this is a function that should return quickly and will run as part of the idle task.
// You don't need to put any code in here you can leave as is.
// this function returns so if you want a variabe to be
//non volatile declare it static.
}

/*******************************************************************************
 *
 *                      Other Functions
 *
 *******************************************************************************/
