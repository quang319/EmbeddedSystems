/*************************************************************/
//  Project:    Lab 2
//
//  Name:   Quang Nguyen
//  Date:   1/24/2013
//
//  Function:
//      Turn on RA0-7 for 256 instructions and then off for 256 instructions
//
//  Pins used:  Lower Byte of port A
//
//  Comments
//
/*************************************************************/

/*************** Configuration Bits **************************/
#include <xc.h>
// FBS
#pragma config BWRP = WRPROTECT_OFF     // Boot Segment Write Protect (Boot Segment may be written)
#pragma config BSS = NO_FLASH           // Boot Segment Program Flash Code Protection (No Boot program Flash segment)
#pragma config RBS = NO_RAM             // Boot Segment RAM Protection (No Boot RAM)

// FSS
#pragma config SWRP = WRPROTECT_OFF     // Secure Segment Program Write Protect (Secure Segment may be written)
#pragma config SSS = NO_FLASH           // Secure Segment Program Flash Code Protection (No Secure Segment)
#pragma config RSS = NO_RAM             // Secure Segment Data RAM Protection (No Secure RAM)

// FGS
#pragma config GWRP = OFF               // General Code Segment Write Protect (User program memory is not write-protected)
#pragma config GSS = OFF                // General Segment Code Protection (User program memory is not code-protected)

// FOSCSEL
#pragma config FNOSC = PRIPLL           // Oscillator Mode (Primary Oscillator (XT, HS, EC) w/ PLL)
#pragma config IESO = ON                // Two-speed Oscillator Start-Up Enable (Start up with FRC, then switch)

// FOSC
#pragma config POSCMD = XT              // Primary Oscillator Source (XT Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function (OSC2 pin has clock out function)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Both Clock Switching and Fail-Safe Clock Monitor are disabled)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler (1:32,768)
#pragma config WDTPRE = PR128           // WDT Prescaler (1:128)
#pragma config PLLKEN = ON              // PLL Lock Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog timer enabled/disabled by user software)

// FPOR
#pragma config FPWRT = PWR128           // POR Timer Value (128ms)

// FICD
#pragma config ICS = PGD1               // Comm Channel Select (Communicate on PGC1/EMUC1 and PGD1/EMUD1)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG is Disabled)

/*************** Library includes*****************************/
#include <p33FJ256GP710A.h>

/*************** Constants Declaration ***********************/


/*************** Function Prototype ***************************/
void initialize();

/*************** Main Program ********************************/
int main() {
    // setting up everything
    initialize();

    /*************** Main Loop ***********************************/
    while (1) 
    {
        int Timer1Counter = 0;
        // Turn PORT A ON
        PORTA = 0xFF;

        // Delay 0.5 second
        while (Timer1Counter <10)
        {
            if (TMR1 == 3125)
            {
                Timer1Counter++;
                TMR1 = 0;
            }
        }      // End of Timer1Counter while loop

        // Turn PORT A OFF
        PORTA = 0;                  // Turn port off

        // Delay 0.5 second
        Timer1Counter = 0;
        while (Timer1Counter < 10)
        {
            if (TMR1 == 3125)
            {
                Timer1Counter++;
                TMR1 = 0;
            }
        }       // End of Timer1Counter while loop
    }           // End of infinite while loop

    return 1;
}

void initialize()
{
 /******* Setting up for Clock (PLL, M, N1, N2) for 32 MHz and Fcy = 16 MHz *****/
        // Fosc = Fin(M/(N1*N2)) = 8 MHz (32/(2*4)) = 32 MHz
    PLLFBD = 34;        // M = 32
        // N1 default is 2
        // N2 default is 4
        // Fcy = Fosc/2 by default
/********************************************************************************/

    // Setting up PORT A
    AD1PCFGH = 0xFF;                // Turn of ADC for Module 1
    TRISA = 0x00;                   // Port A to output
    // Setting up Timer 1 module
    T1CON = 0b0100000000110000;
            // TON = OFF
            // TGATE = OFF
            // TCKPS (Prescaler) = 256
                // 2/16 MHz = 1.25*10^-7
                // 1.25*10^-7 * 256 = 0.000032
                // 3125 instructions * .000032 = 0.1 second ***
                // We are going to let this overflow 5 times to make 0.5 second
            //  TSYNC = OFF
            // TCS = Internal Clock
    // Start Timer 1
    T1CONbits.TON = 1;
}
