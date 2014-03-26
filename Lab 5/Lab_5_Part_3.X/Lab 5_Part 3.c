/*******************************************************************************
 *  Project:            Lab 5 Part 1
 *
 *  Name:               Quang Nguyen
 *
 *  Date:               2/14/2014
 *
 *  Function:           Convert 2 floating point numbers to fixed integer format.
 *                      These 2 values are then multiplied with each other.
 *
 *  Pins used:
 *
 *  Peripherals used:
 *                      TIMER 1
 *                          Used to get an accurate measure of time
 *                          Internal Clock = 16 MHz
 *                          1/16 MHz = 0.0625 us
 *                          with prescaler at 1
 *                          0.0625 us * 1 = 0.0625 us
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
#include <libq.h>


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
    /*************************************************************
    *
    *   Function name : Q16Mult
    *
    *   Returns :       Q16.16 multiplication result of 2 Q16.16
    *
    *   Parameters :    2 variables in Q16.16 format
    *
    *   Purpose :       Multiply 2 Q16.16 numbers and then return
    *                   the result
    *
    **************************************************************/
_Q16 Q16Mult (_Q16 Op1, _Q16 Op2);
_Q16 Q16Add (_Q16 Op1, _Q16 Op2);

/*******************************************************************************
 *
 *                      Main Function
 *
 *******************************************************************************/
int main() {
    // setting up everything
    initialize();

    /*************** Main Loop ***********************************/
    // Initialize Time
    float Operand1 = 3.1, Operand2 = 4.1;
    _Q16 ResultQ16     = 0;
    _Q16 QOperand1 = 0, QOperand2 = 0;
    float ResultFloat = 0;

    while (1) {
        QOperand1 = _Q16ftoi(Operand1);
        QOperand2 = _Q16ftoi(Operand2);

        T1CONbits.TON = 1;                              // Turn on Timer 1
//        ResultQ16 = Q16Add (QOperand1,QOperand2);
        ResultFloat = Operand1 + Operand2;
        T1CONbits.TON = 0;                              // Turn OFF Timer 1

        while(1);                                       // Endless loop

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

    // Setting up Timer 1 module
        /****************************************
         *  Internal Clock = 16 MHz
         *  1/16 MHz = 0.0625 us
         *  with prescaler at 1
         *  0.0625 us * 1 = 0.0625 us
         ****************************************/
    T1CONbits.TCS = 0;                              // Use internal clock
    T1CONbits.TCKPS = 0;                            // Set prescaler to 1

    // Set initial value for PR1 to throw an interrupt if the
    // prescaller is too smaller and the timer is overflowing
    PR1 = 65535;

    TMR1 = 0;                                       // Clear TMR1
    _T1IE = 1;                                      // Turn on TMR1 interrupt
                                                    // The default priority is 4 so lets not worry about it for now
        // Setting up PORT A
    AD1PCFGH = 0xFF;                                // Turn off ADC for Module 1
    TRISA = 0x00;                                   // Port A to output
    PORTA = 0x00;                                   // Keep PORTA off to begin with

}

/*******************************************************************************
 *
 *                      Interrupt Service Routine
 *
 *******************************************************************************/
void __attribute__ ((interrupt, no_auto_psv)) _T1Interrupt (void)
{                                                   //
        // Turn off flag                            //
    IFS0bits.T1IF = 0;                              //
    PORTA = 0xff;                                   // Turn ON PORTA to indicate that TMR1 has overflowed

}
/*******************************************************************************
 *
 *                      Other Functions
 *
 *******************************************************************************/

_Q16 Q16Mult (_Q16 Op1, _Q16 Op2)
{
    _Q16 ResultOfQ16 = 0;
    ResultOfQ16 = ((Op1 >> 8) * (Op2 >> 8));
    return ResultOfQ16;
}

_Q16 Q16Add (_Q16 Op1, _Q16 Op2)
{
    _Q16 ResultOfQ16 = 0;
    ResultOfQ16 = (Op1 + Op2);
    return ResultOfQ16;
}