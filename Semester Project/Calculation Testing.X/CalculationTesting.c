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
int i;
/*******************************************************************************
 *
 *                      Function Prototype
 *
 *******************************************************************************/
void initialize();
int intSquareRoot(int Value);

/*******************************************************************************
 *
 *                      Main Function
 *
 *******************************************************************************/
int main() {
    int Voltage [120];
    for (i = 0; i < 120; i++)
        Voltage[i] = 1024;
    // setting up everything
    initialize();

    long VoltageSum       = 0;
    long VoltageSquareSum = 0;
    long long VoltageSumSquare  = 0;
    long VoltageSumSquareAverage = 0;
    int VoltagePreSquareRoot = 0;
        
    /*************** Main Loop ***********************************/
    while (1) 
    {
        // Summing Voltage
        for (i = 0; i < 120; i++)
            VoltageSum += Voltage[i];
        // Summing of Voltage Square
        for (i = 0; i < 120; i++)
            VoltageSquareSum += (long)Voltage[i] * (long)Voltage[i];
        // Square of Voltage Sum
        VoltageSumSquare = (long long)VoltageSum * (long long)VoltageSum;
        VoltageSumSquareAverage = (long)(VoltageSumSquare/120);
        VoltagePreSquareRoot = (VoltageSquareSum - VoltageSumSquareAverage)/120;
        intSquareRoot(77);

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