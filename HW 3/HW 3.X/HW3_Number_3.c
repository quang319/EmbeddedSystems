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


/*******************************************************************************
 *
 *                      Constant Declarations
 *
 *******************************************************************************/

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
unsigned char a[10];

int main() {
    // setting up everything
    initialize();


    /*************** Main Loop ***********************************/
    while (1) {
        load();
    } // End of the infinite While loop

    return 1;
}
void load()
{
    int i = 0 ;
    for (i = 0; i < 10; i++)
    {
        a[i] = 65 + i;          //ASCII dec for A starts at 65
    }
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
