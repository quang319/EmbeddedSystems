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
/*******************************************************************************
 *
 *                      Global Variable Declarations
 *
 *******************************************************************************/
xQueueHandle LCDDisplayinfo;
char RTCFlag = 1; //This global variable can be used to signal that the button to display the RTC
//has been pressed.  Ideally we could have done this with a semiphore.


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
void buttonPushCounter( void *pvParameters);
void rtcTimer(void *pvParameters);
void writeLcd (void *pvParameters);
void prvSetupHardware( void );
void vApplicationIdleHook(void);

void dmaHandler (void *pvParameters);

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

    // Setting up ADC for DMA Interrupt
    ADC_Init();
    // Create semaphore for DMA interrupt
    vSemaphoreCreateBinary(DmaSemaphore);
    //RTOS setup of hardware
    prvSetupHardware();
    Init_LCD();//Initialize the LCD this must be done first before the LCD is to be used.
    while (check_busy())
    {
    }
    home_clr(); // start display at top line left most character
    /* Create Tasks*/
    xTaskCreate(dmaHandler, "Task for DMA", 500, NULL, 3, NULL );
    xTaskCreate(buttonPushCounter,"Task Read Swtchs", 200, NULL, 2, NULL);
    xTaskCreate(rtcTimer, "Task RTC", 200, NULL, 2, NULL);
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
    static portBASE_TYPE HigherPriority;
    xSemaphoreGiveFromISR (DmaSemaphore, &HigherPriority);
    _DMA0IF = 0;
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
    int	*DMAptr;
    unsigned int VSignal[120], ISignal[120];
    int Vrms = 0;
    int Irms = 0;
    while(1)
    {
        xSemaphoreTake(DmaSemaphore, portMAX_DELAY);

        int i;
	DMAptr = BeginDMA;
	for (i=0; i<120; i++)
        {
            VSignal[i] = *DMAptr;
            DMAptr++;
            ISignal[i] = *DMAptr;
            DMAptr++;
        }
        Vrms = rmsCalc(VSignal);
    }
}

/*-----------------------------------------------------------*/
void buttonPushCounter( void *pvParameters)
{
    /* This task will read the pushbuttons, since most pushes of the button will take more than
    10 ms it needs to only run every 10 ms or l00ms if in the middle of a button push read.
    The API for a delay is: vTaskDelay(10); to delay for 10ms before being active again */

    /*
    char LCDDisplay[17] = "Count = 00000   ";

    int Rd6ChangeStateFlag = 0;
    int Rd7ChangeStateFlag = 0;
    unsigned int Rd6Counter = 0;
    int BlockingFlag = 0;           // 0 = 10 ms
                                    // 1 = 100 ms
    xQueueSendToBack (LCDDisplayinfo, LCDDisplay, 0);
    */

    /* start endless loop*/
    for( ;; )
    {

        /*
        // if RD6(S3) is pressed
        // Display count on LCD
        if ( (_RD6 == 0) && (Rd6ChangeStateFlag == 0))
        {
            RTCFlag = 0;
            Rd6ChangeStateFlag = 1;

            //Increment Counter
            Rd6Counter++;
            // Load the LCD with the count display
            displayCounter(Rd6Counter, LCDDisplay);

            //Put the updated count into the queue
            xQueueSendToBack (LCDDisplayinfo, LCDDisplay, 100);

            //Scan again every 100 ms
            BlockingFlag = 1;
        }
        // If the user just released the button
        else if ( (_RD6 == 1) && (Rd6ChangeStateFlag == 1))
        {
            Rd6ChangeStateFlag = 0;
            BlockingFlag = 0;
        }

        // if RD7(S6) is pressed
        // Display RTC time on LCD
        if ( (_RD7 == 0) && (Rd7ChangeStateFlag == 0))
        {
            RTCFlag = 1;
            Rd7ChangeStateFlag = 1;
            //Scan again every 100 ms
            BlockingFlag = 1;
        }
        // If the user just released the button
        else if ( (_RD7 == 1) && (Rd7ChangeStateFlag == 1))
        {
            Rd7ChangeStateFlag = 0;
            BlockingFlag = 0;
        }

        if (BlockingFlag == 0)
            vTaskDelay(10);
//            vTaskDelay( 100 / portTICK_RATE_MS);
        else
            vTaskDelay(100);
//            vTaskDelay( 1000 / portTICK_RATE_MS);
        */
        vTaskDelay( 1000 / portTICK_RATE_MS);
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
                else if ((LCDCounter >= 16) && (SecondLineFlag == 1) )
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

void rtcTimer(void *pvParameters)
{ //This is where you place your code to keep track of the RTC and the display that it will show.
  //Initialize and declare
    /* In order to get true timing you should use the periodic delay function to set this up
    some code is given... see lecture 17 for details
    in addition you will want to write a string to queue for either RTC display or count display
    sample API function call is given to do this. */

    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    xLCD LCDDisplay = {.Line1 = "Hello World!    ", .Line2 = "I am awesome!   "};
    for( ;; )
    {
        
        xQueueSendToBack(LCDDisplayinfo, &LCDDisplay, 0);
        // The LCD needs more than .5 second to print the 2 lines
        vTaskDelayUntil(&xLastWakeTime,700); // wait 0.5 seconds
    }
}


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
