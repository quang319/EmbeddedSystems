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
#include <math.h>
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
#define SAMPLES 120
#define SAMPLES_SQUARE 14400
#define FILTER_MAX 20
#define VRMS_CONSTANT 0.75269
#define IRMS_CONSTANT 0.10753
#define PAVG_CONSTANT 0.0080934
#define PAPP_CONSTANT 0.0080937
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
    unsigned int VrmsWhole;
    unsigned int VrmsFractional;
    unsigned int IrmsWhole;
    unsigned int IrmsFractional;
    unsigned int PavgWhole;
    unsigned int PavgFractional;
    unsigned int PappWhole;
    unsigned int PappFractional;
} ADCData;
/*******************************************************************************
 *
 *                      Global Variable Declarations
 *
 *******************************************************************************/
xQueueHandle LCDDisplayinfo;
unsigned int VSignal[120], ISignal[120];

//long VoltageSum = 0, VoltageSquareSum = 0;
//long VoltageSumSquareAverage = 0;          // Contains result of ( ( sum(V)^2) / N)
//long double VoltagePreSquareRoot = 0;
//long double VoltageRMS = 0;

// long CurrentSum = 0, CurrentSquareSum = 0;
// long CurrentSumSquareAverage = 0;          // Contains result of ( ( sum(V)^2) / N)
// long double CurrentPreSquareRoot = 0;
// long double CurrentRMS = 0;

//long PowerSum = 0;                         // Contains sum(V*I)
//long double PowerOp1 = 0;                          // Contains ( sum(V*I) / N)
//long double PowerOp2 = 0;                          // Contains ( ( sum(V) * (sum(I) ) ) / N^2)
//long double Pavg     = 0;
//long double Papp     = 0;
//
//long double VrmsResult[FILTER_MAX];
//long double IrmsResult[FILTER_MAX];
//long double PavgResult[FILTER_MAX];
//long double PappResult[FILTER_MAX];

int         FilterIndex = 0;
int         DataReadyFlag = 0;

int i;
ADCData FilteredData;
ADCData PreviousData;

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

int intSquareRoot(int Value);
void wholeConvert (char *String, unsigned int Value);
void fractionConvert (char *String, unsigned int Value);

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
   long CurrentSum = 0, CurrentSquareSum = 0;
  long CurrentSumSquareAverage = 0;          // Contains result of ( ( sum(V)^2) / N)
  long double CurrentPreSquareRoot = 0;
  long double CurrentRMS = 0;

  long PowerSum = 0;                         // Contains sum(V*I)
long double PowerOp1 = 0;                          // Contains ( sum(V*I) / N)
long double PowerOp2 = 0;                          // Contains ( ( sum(V) * (sum(I) ) ) / N^2)
long double Pavg     = 0;
long double Papp     = 0;

long double VrmsResult[FILTER_MAX];
long double IrmsResult[FILTER_MAX];
long double PavgResult[FILTER_MAX];
long double PappResult[FILTER_MAX];

long VoltageSum = 0, VoltageSquareSum = 0;
long VoltageSumSquareAverage = 0;          // Contains result of ( ( sum(V)^2) / N)
long double VoltagePreSquareRoot = 0;
long double VoltageRMS = 0;
   for( ;; )
   {
       // Block waiting for the semaphore to become available.
       xSemaphoreTake(DmaSemaphore, portMAX_DELAY );
           /****************************
           *   Getting raw data from the DMA
           *****************************/
       asm("NOP");
       DMAptr = BeginDMA;
      for (i=0; i<SAMPLES; i++)
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
           *   Paverage = ( sum(V*I) / N) - ( ( sum(V) * (sum(I) ) ) / N^2)
           *   Papp = Vrms * Irms
           ****************************/
        VoltageSum = 0;
        VoltageSquareSum = 0;
        CurrentSum = 0;
        CurrentSquareSum = 0;
        PowerSum = 0;
        
       /*******************************************************************
        *             Performing Summations
        *******************************************************************/
       for (i = 0; i < SAMPLES; i++)
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
       /*******************************************************************
        *             Vrms Calcuation
        *******************************************************************/
       // ( sum(V)^2) / N)
       VoltageSumSquareAverage = (long)( ( (long long)VoltageSum * (long long)VoltageSum ) / SAMPLES);
       // ( ( sum(V^2) - ( ( sum(V)^2) / N) ) / N)
       VoltagePreSquareRoot = (long double)( (long double)( VoltageSquareSum - VoltageSumSquareAverage) / SAMPLES);
       // Performing square root
       VoltageRMS = sqrt(VoltagePreSquareRoot);

       /*******************************************************************
        *             Irms Calculation
        *******************************************************************/
       // Now do the same for Current
       CurrentSumSquareAverage = (long)( ( (long long)CurrentSum * (long long)CurrentSum ) / SAMPLES);
       // ( ( sum(V^2) - ( ( sum(V)^2) / N) ) / N)
       CurrentPreSquareRoot = (long double)( (long double)( CurrentSquareSum - CurrentSumSquareAverage) / SAMPLES);
       // Performing square root
       CurrentRMS = sqrt(CurrentPreSquareRoot);

       /*******************************************************************
        *             Power Calculation
        *******************************************************************/
        PowerOp1 = (long double)PowerSum / SAMPLES;
        PowerOp2 = (long double)( ( (long double)VoltageSum * (long double)CurrentSum ) / SAMPLES_SQUARE);
        Pavg = PowerOp1 - PowerOp2;
        Papp = VoltageRMS * CurrentRMS;

        /*******************************************************************
        *             Storing the Result
        *******************************************************************/
         VrmsResult[FilterIndex] = VoltageRMS;
         IrmsResult[FilterIndex] = CurrentRMS;
         PavgResult[FilterIndex] = Pavg;
         PappResult[FilterIndex] = Papp;
         FilterIndex++;
         
        /*******************************************************************
        *             Performing low pass filter
        *******************************************************************/
        if (FilterIndex >= FILTER_MAX)
        {
          // Set index to zero
          FilterIndex = 0;
          // Setting the flag
          DataReadyFlag = 1;
          // Clear all the variables first
          VoltageRMS = 0;
          CurrentRMS = 0;
          Pavg       = 0;
          Papp       = 0;
          i = 0;
          //Sum up the samples
          for (i = 0; i < FILTER_MAX; i++)
          {
            VoltageRMS += VrmsResult[i];
            CurrentRMS += IrmsResult[i];
            Pavg       += PavgResult[i];
            Papp       += PappResult[i];
          }

          // And then average them
          VoltageRMS = VoltageRMS / FILTER_MAX;
          CurrentRMS = CurrentRMS / FILTER_MAX;
          Pavg       = Pavg / FILTER_MAX;
          Papp       = Papp / FILTER_MAX;

          /*******************************************************************
          *             Include Fudge factor
          *******************************************************************/
          VoltageRMS = VoltageRMS * (long double)VRMS_CONSTANT;
          CurrentRMS = CurrentRMS * (long double)IRMS_CONSTANT;
           Pavg       = Pavg * (long double)PAVG_CONSTANT;
           Papp       = Papp * (long double)PAPP_CONSTANT;
        /*******************************************************************
          *             Convert to Whole and Fractional Part
          *******************************************************************/
          
          FilteredData.VrmsWhole = (unsigned int)VoltageRMS;
          FilteredData.VrmsFractional = (unsigned int)( (VoltageRMS - (unsigned int)VoltageRMS ) * 100);

          FilteredData.IrmsWhole = (unsigned int)CurrentRMS;
          FilteredData.IrmsFractional = (unsigned int)( (CurrentRMS - (unsigned int)CurrentRMS ) * 100);

          FilteredData.PavgWhole = (unsigned int)Pavg;
          FilteredData.PavgFractional = (unsigned int)( (Pavg - (unsigned int)Pavg ) * 100);

          FilteredData.PappWhole = (unsigned int)Papp;
          FilteredData.PappFractional = (unsigned int)( (Papp - (unsigned int)Papp ) * 100);
        }
          // I should be putting the result onto a queue here.

        asm("NOP");
//           xSemaphoreGive( DmaSemaphore );
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

//    ADCData PreviousData;
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
      if (DataReadyFlag == 1)
      {
        switch (DisplayIndex)
        {
          case 1:
            if ( (PreviousData.VrmsWhole != FilteredData.VrmsWhole) || (PreviousData.VrmsFractional != FilteredData.VrmsFractional)
              || (PreviousData.IrmsWhole != FilteredData.IrmsWhole) || (PreviousData.IrmsFractional != FilteredData.IrmsFractional))
            {
              PreviousData.VrmsWhole      = FilteredData.VrmsWhole;
              PreviousData.VrmsFractional = FilteredData.VrmsFractional;
              PreviousData.IrmsWhole      = FilteredData.IrmsWhole;
              PreviousData.IrmsFractional = FilteredData.IrmsFractional;
              DisplayFlag = 1; 
            }
            break;
          case 2:
            if ( (PreviousData.PavgWhole != FilteredData.PavgWhole) || (PreviousData.PavgFractional || FilteredData.PavgFractional) )
            {
              PreviousData.PavgWhole      = FilteredData.PavgWhole;
              PreviousData.PavgFractional = FilteredData.PavgFractional;
              // DisplayFlag = 1;
            }
            break;
          case 3:
            if ( (PreviousData.PappWhole != FilteredData.PappWhole) || (PreviousData.PappFractional || FilteredData.PappFractional) )
            {
              PreviousData.PappWhole      = FilteredData.PappWhole;
              PreviousData.PappFractional = FilteredData.PappFractional;
              // DisplayFlag = 1;
            }
            break;
        }
      }
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
            if (DisplayIndex >= 5)
                DisplayIndex = 1;
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
        if ((DisplayIndex == 1) && (DisplayFlag == 1))
        {
            asm("NOP");
           strcpy(LCDDisplay.Line1, "Vrms = XXX.XX V ");
           strcpy(LCDDisplay.Line2, "Irms = XXX.XX A ");
           wholeConvert(&LCDDisplay.Line1[7], PreviousData.VrmsWhole);
           fractionConvert(&LCDDisplay.Line1[11], PreviousData.VrmsFractional);
           asm("NOP");
           asm("NOP");
           wholeConvert(&LCDDisplay.Line2[7], PreviousData.IrmsWhole);
           fractionConvert(&LCDDisplay.Line2[11], PreviousData.IrmsFractional);
            asm("NOP");
        }
        else if ((DisplayIndex == 2) && (DisplayFlag == 1))
        {
            asm("NOP");
            strcpy(LCDDisplay.Line1, "Average Power   ");
            strcpy(LCDDisplay.Line2, "= XXX.XX W      ");

            wholeConvert(&LCDDisplay.Line2[2], PreviousData.PavgWhole);
            fractionConvert(&LCDDisplay.Line2[6], PreviousData.PavgFractional);
        }
        else if ((DisplayIndex == 3) && (DisplayFlag == 1))
        {
            strcpy(LCDDisplay.Line1, "Instan. Power   ");
            strcpy(LCDDisplay.Line2, "= XXX.XX W      ");

            wholeConvert(&LCDDisplay.Line2[2], PreviousData.PappWhole);
            fractionConvert(&LCDDisplay.Line2[6], PreviousData.PappFractional);
        }
        else if ((DisplayIndex == 4) && (DisplayFlag == 1))
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
int intSquareRoot(int Value)
{
    int i = 0, SquareResult = 0;
    while (SquareResult < Value)
    {
        i++;
        SquareResult = i * i;
    }
    if (SquareResult > Value)
    {
        i--;
    }
    return i;
}

void wholeConvert (char *String, unsigned int Value)
{
    unsigned char Hundred = 0, Ten = 0, One = 0;
    while (Value > 99)
    {
        Value -= 100;
        Hundred++;
    }
    while (Value > 9)
    {
        Value -= 10;
        Ten++;
    }
    while (Value > 0)
    {
        Value -= 1;
        One++;
    }
    *String = Hundred + '0';
    String++;
    *String = Ten + '0';
    String++;
    *String = One + '0';
}

void fractionConvert (char *String, unsigned int Value)
{
    unsigned char  Ten = 0, One = 0;

    while (Value > 9)
    {
        Value -= 10;
        Ten++;
    }
    while (Value > 0)
    {
        Value -= 1;
        One++;
    }
    *String = Ten + '0';
    String++;
    *String = One + '0';
}