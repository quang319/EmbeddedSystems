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
#include <string.h>

/*******************************************************************************
 *
 *                      Constant Declarations
 *
 *******************************************************************************/
#define true 1

/*******************************************************************************
 *
 *                      Global Variable Declarations
 *
 *******************************************************************************/
const char  e[5] = {'H','E','L','L','0'};
const char  f[]  = "HELLO";
const char  a[]  = " CPE 490 Rocks ";
char        b[100] = "Initialized";
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

    /*************** Main Loop ***********************************/
    while (1) {

    int  strlength;
    strlength = strlen(e);

        AD1PCFGH = 0x00C0;
        TRISA = 0;
        strcpy(b, "MPLAB C30");
        PORTA = 1;
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
