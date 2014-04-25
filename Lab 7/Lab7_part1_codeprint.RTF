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
 *                       Configuration Bits
 *
 ******************************************************************************/
#include <xc.h>

// FOSCSEL
#pragma config FNOSC = PRIPLL           // Oscillator Mode (Primary Oscillator (XT, HS, EC) w/ PLL)
#pragma config IESO = ON                // Two-speed Oscillator Start-Up Enable (Start up with FRC, then switch)

// FOSC
#pragma config POSCMD = XT              // Primary Oscillator Source (XT Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function (OSC2 pin has clock out function)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Both Clock Switching and Fail-Safe Clock Monitor are disabled)

// FWDT
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog timer enabled/disabled by user software)


/*******************************************************************************
 *
 *                      Library includes
 *
 *******************************************************************************/
#include <p33FJ256GP710A.h>
#include "lcd.h"
#include <delay.h>
#include <string.h>

/*******************************************************************************
 *
 *                      Constant Declarations
 *
 *******************************************************************************/
const char LcdData1[] = "Embedded Systems";     //sizeof() will result in 17
const char LcdData2[] = "Are Great";            //sizeof() will result in 10
const char LcdSpace[] = " ";
/*******************************************************************************
 *
 *                      Global Variable Declarations
 *
 *******************************************************************************/

/*******************************************************************************
 *
 *                      Function Prototype
 *
 *******************************************************************************/
void initialize();

/*******************************************************************************
 *
 *                      Main Function
 *
 *******************************************************************************/
int main() {
    // setting up everything
    initialize();
    int LcdCounter = 0;
    int LcdFlag = 0, LcdLine = 1;
    /*************** Main Loop ***********************************/
    while (1) {

        // Next press of RD6 = the printing of one char
        if ((_RD6 == 0) && (LcdFlag == 1))
        {
            // If the character can still be printed on the 1st line
            if (LcdCounter < (sizeof(LcdData1)-1))
            {
                // Print the current character and increment
                puts_lcd(&LcdData1[LcdCounter], 1);
                LcdCounter++;
            }
            // if the character is on the last (16th) position of the 1st line
            if (LcdCounter == (sizeof(LcdData1)-1))
            {
                // increment
//                puts_lcd(&LcdData1[LcdCounter], 1);
                LcdCounter++;
                // Move to line 2 and indicate that we are now in line 2
                line_2();
                LcdLine = 2;
            }
            // If the Lcd is pointing to LcdData2
            else if (LcdCounter >= (sizeof(LcdData1)- 1))
            {
//                if ((LcdCounter == (sizeof(LcdData1) - 1 )) && (LcdLine == 1))
//                {
//                    line_2();
//                    LcdLine = 2;
//                }
                // If The pointer is still pointing at legitimate data
                if (LcdCounter <= ((sizeof(LcdData1) + sizeof(LcdData2)) -2))
                {
                    // Print the current character and increment
                    puts_lcd(&LcdData2[(LcdCounter - sizeof(LcdData1))], 1);
                    LcdCounter++;
                }
                // Else don't do anything. Just sit and wait
            }
            Delay_Us(1000);                                  // Debouncing
        }
        // First press of RD6 = clear the screen
        if ((_RD6 == 0) && (LcdFlag == 0))
        {
            home_clr();
            home_it();
            LcdFlag = 1;
            Delay_Us(1000);                                 // Debouncing
        }

        // If RD7 was pressed
        if ((_RD7 == 0))
        {
            if ((LcdCounter == sizeof(LcdData1)) && (LcdLine == 2))
            {
                lcd_cmd(0x8F);
                puts_lcd(&LcdSpace, 1);
                lcd_cmd(0x8F);
                LcdCounter--;
                LcdCounter--;
//                LcdLine = 1;
            }
            else
            {
                cursor_left();
                puts_lcd(&LcdSpace, 1);
                cursor_left();
                LcdCounter--;
            }
            
            
            Delay_Us(1000);                                 // Debouncing
        }

    } // End of the infinite While loop

    return 1;
}

/*******************************************************************************
 *
 *                      Initialize Function
 *
 *******************************************************************************/
void initialize() {
    /**************************************************
     *  Setting up for Clock (PLL, M, N1, N2)
     *  for 32 MHz and Fcy = 16 MHz
     *************************************************/
    // Fosc = Fin(M/(N1*N2)) = 8 MHz (32/(2*4)) = 32 MHz
    PLLFBD = 30; // M = 32
    // N1 default is 2
    // N2 default is 4
    // Fcy = Fosc/2 by default

     // Setting up RD6 and RD7
    _TRISD6 = 1;
    _TRISD7 = 1;                                    // Set RD6 & 7 to inputs

    // Initialize LCD
    Init_LCD();

    lcd_cmd(0xD);

    // Making sure that we are starting at line 1, column 0
    home_it();
    // Print the LcdData1
    puts_lcd(LcdData1, sizeof(LcdData1)-1);
    // Move to next line
    line_2();
    // Print the LCDData2
    puts_lcd(LcdData2, sizeof(LcdData2)-1);
}

/*******************************************************************************
 *
 *                      Interrupt Service Routine
 *
 *******************************************************************************/

/*******************************************************************************
 *
 *                      Other Functions
 *
 *******************************************************************************/
