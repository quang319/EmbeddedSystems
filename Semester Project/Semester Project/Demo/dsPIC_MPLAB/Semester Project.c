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

/* Standard includes. */
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include <portmacro.h>

/* Demo application includes. */
#include "BlockQ.h"
#include "crflash.h"
#include "blocktim.h"
#include "integer.h"
#include "comtest2.h"
#include "partest.h"
#include "lcd.h"
#include "timertest.h"

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
 *                      Global Variable Declarations
 *
 *******************************************************************************/
xQueueHandle LCDDisplayinfo;
char RTCFlag; //This global variable can be used to signal that the button to display the RTC
//has been pressed.  Ideally we could have done this with a semiphore.
/*******************************************************************************
 *
 *                      Function Prototype
 *
 *******************************************************************************/
void initialize();
static void prvSetupHardware( void );
char IntToChar(unsigned int Digit);
static void ButtonPushCounter( void *pvParameters);
void RTCTimer(void *pvParameters);
void WriteLCD (void *pvParameters);
void vApplicationIdleHook(void);

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
    Init_LCD();//Initialize the LCD this must be done first before the LCD is to be used.
    while (check_busy())
    {
    }
    home_clr(); // start display at top line left most character
    /* Create Tasks*/
    xTaskCreate(ButtonPushCounter,"Task Read Swtchs", 200, NULL, 2, NULL);
    xTaskCreate(RTCTimer, "Task RTC", 200, NULL, 2, NULL);
    xTaskCreate(WriteLCD, "Write LCD",200, NULL, 1, NULL); //notice that this task has lower priority.
    /* create the queue for the string to be displayed.  Length = 2 each chunk holds 16 char string*/
    LCDDisplayinfo = xQueueCreate(2,16);

    /* start the scheduler. */
    vTaskStartScheduler();
}

/*******************************************************************************
 *
 *                      Interrupt Service Routine
 *
 *******************************************************************************/

/*******************************************************************************
 *
 *                      ROS tasks
 *
 *******************************************************************************/
//This is required by the RTOS, DO NOT MOTIFY
static void prvSetupHardware( void )
{
	vParTestInitialise();
}


/*-----------------------------------------------------------*/
static void ButtonPushCounter( void *pvParameters)
{
    /* This task will read the pushbuttons, since most pushes of the button will take more than
    10 ms it needs to only run every 10 ms or l00ms if in the middle of a button push read.
    The API for a delay is: vTaskDelay(10); to delay for 10ms before being active again */

    char LCDDisplay[17] = "Count = 00000   ";

    int Rd6ChangeStateFlag = 0;
    int Rd7ChangeStateFlag = 0;
    unsigned int Rd6Counter = 0;
    int BlockingFlag = 0;           // 0 = 10 ms
                                    // 1 = 100 ms
    xQueueSendToBack (LCDDisplayinfo, LCDDisplay, 0);
    xQueueSendToBack (LCDDisplayinfo, LCDDisplay, 0);

    /* start endless loop*/
    for( ;; )
    {

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
    } // end of for loop
}

void WriteLCD(void *pvParameters)
{
    // This is lower priority task it will write charaters to the LCD screen in response to the button
    //pushes an example API of reading the queue is given

    int LCDCounter = 0;

    portBASE_TYPE NewString;
    char LCDDisplayString[16];
    for ( ; ; )
    {
        NewString = xQueueReceive( LCDDisplayinfo,LCDDisplayString,10);

        // If we recieve a new message.
        // Need to bring the display back to the beginning and start displayin the new data
        if (NewString == pdPASS)
        {// get new string and start displaying
            LCDCounter = 0;
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
                // if LCDCounter is not at the end of the string
                if (LCDCounter < 17)
                {
                    // Printer Character on LCD
                    lcd_data( LCDDisplayString[LCDCounter] );
                    LCDCounter++;
                }
                vTaskDelay( 10 / portTICK_RATE_MS);
            }
        }
    }//end of for loop
}

void RTCTimer(void *pvParameters)
{ //This is where you place your code to keep track of the RTC and the display that it will show.
  //Initialize and declare
    /* In order to get true timing you should use the periodic delay function to set this up
    some code is given... see lecture 17 for details
    in addition you will want to write a string to queue for either RTC display or count display
    sample API function call is given to do this. */

    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    char LCDDisplay[16];

    int Z = 0, S = 0, M = 0, H = 0;
    for( ;; )
    {
        // Z counts in half a second increment.
        // If Z overflowed
        Z++;
        if ( Z == 2)
        {
            Z = 0;
            S++;
        }
        // If S overflowed
        if ( S > 60)
        {
            S = 0;
            M++;
        }
        // if M overflowed
        if ( M > 60)
        {
            M = 0;
            H++;
        }
        // if H overflowed
        if ( H > 24)
        {
            H = 0;
        }

        // Load the LCDDispaly array with the time if the RTCFlag is HIGH
        if (RTCFlag == 1)
        {
            char Temp[5] = "RTC ";
            int Counter;
            for (Counter =0 ; Counter <= 3; Counter++)
            {
                LCDDisplay[Counter] = Temp[Counter];
            }
            BinaryToDecimal( H, &LCDDisplay[4], &LCDDisplay[5]);
            LCDDisplay[6] = ':';
            BinaryToDecimal( M, &LCDDisplay[7], &LCDDisplay[8]);
            LCDDisplay[9] = ':';
            BinaryToDecimal( S, &LCDDisplay[10], &LCDDisplay[11]);
            LCDDisplay[12] = '.';
            if ( Z == 1)
                LCDDisplay[13] = '5';
            else LCDDisplay[13] = '0';
            xQueueSendToBack(LCDDisplayinfo, LCDDisplay, 0);
        }
        vTaskDelayUntil(&xLastWakeTime,500); // wait 0.5 seconds
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
/***************************************************
 *
 * Name:            displayCounter
 * Parameters:
 *          int Count
 * Return:  void
 * Pins used:        None
 * Global Var used:
 *          char            LCDDisplay[17];
 * Function:
 *          Similiar to the binaryToDecimal Function.
 *          This function will take the value of count and convert
 *          it to decimal.
 *
 ***************************************************/
void displayCounter(int Count, char *LCD)
{
    //Goal: to display "Count = XXXXX   "
    char TempChar[9] = "Count = ";
    int Counter;
    for (Counter =0 ; Counter < 8; Counter++)
    {
        LCD[Counter] = TempChar[Counter];
    }


    int Ten = 0, Hundred = 0, Thousand = 0, TenThousand = 0;
    if (Count > 9)
    {
        while(Count > 9)
        {
            Count -= 10;
            Ten++;
            digitCheck(&Ten, &Hundred);
            digitCheck(&Hundred, &Thousand);
            digitCheck(&Thousand, &TenThousand);
        }
    }

    LCD[12] = Count + 48;
    LCD[11] = Ten + 48;
    LCD[10] = Hundred + 48;
    LCD[9] = Thousand + 48;
    LCD[8] = TenThousand + 48;
    LCD[13] = ' ';
}

/***************************************************
 *
 * Name:            digitCheck
 * Parameters:
 *          int *FirstDigit
 *          int *SecondDigit
 * Return:
 *          void
 * Pins used:        None
 * Global Var used:
 * Function:
 *          This function will look at the pointer for the first digit
 *          and increment the second digit if it is greater than 9.
 *
 ***************************************************/
void digitCheck(int *FirstDigit, int *SecondDigit)
{
    if ((*FirstDigit) > 9)
    {
        *FirstDigit -= 10;
        (*SecondDigit)++;
    }
}


/***************************************************
 *
 * Name:            BinaryToDecimal
 * Parameters:
 *          int value : The value of time
 *          char *TenthPlace : Pointer to the variable for the tenth place
 *          char *OnePlace : Pointer to the variable for the one place
 * Return:
 *          void
 * Pins used:        None
 * Function:
 *          Function will convert will convert a binary value to decimal value
 *
 ***************************************************/
void BinaryToDecimal ( int Value, char *TenthPlace, char *OnePlace)
{
    int Temp = 0;
    if (Value > 9)
    {
        while(Value > 9)
        {
            Value -= 10;
            Temp++;
        }
    }
    // Assign value to the variable and convert it to ASCII
    *TenthPlace = 48 + Temp;
    *OnePlace = 48 + Value;
}