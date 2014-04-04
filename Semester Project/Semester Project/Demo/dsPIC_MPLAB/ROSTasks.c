#include "ROSTasks.h"
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