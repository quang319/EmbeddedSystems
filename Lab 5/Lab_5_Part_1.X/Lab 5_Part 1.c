/*******************************************************************************
 *  Project:            Lab 5 Part 1
 *
 *  Name:               Quang Nguyen
 *
 *  Date:               2/14/2014
 *
 *  Function:           Multiply 2 unsigned 9 bit numbers together using
 *                      repetitive addition and place the answer in an
 *                      unsigned 16 bit number
 *                      Timer 1 will be used to keep track of how long
 *                      the additon takes
 *
 *  Pins used:
 *                      Lower byte of PORTA
 *                          All 8 bits of PORTA will be turned ON if
 *                          the Timer overflow to indicate that the
 *                          prescaller needs to be a bigger number
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


/*******************************************************************************
 *
 *                      Constant Declarations
 *
 *******************************************************************************/
#define OP1 7.234
#define OP2 11.567
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
    *   Function name : repetitiveAddition
    *
    *   Returns :       Unsigned int
    *
    *   Parameters :    2 unsigned char
    *
    *   Purpose :       Perform addition using 8 bit repetitive addition
    *
    **************************************************************/
unsigned int repetitiveAddition (unsigned char FirstOperand, unsigned char SecondOperand);

/*******************************************************************************
 *
 *                      Main Function
 *
 *******************************************************************************/
int main() {
    // setting up everything
    initialize();

    /*******************************************************
     *
     *                  Infinite Loop
     *
     * ****************************************************/
    while (1) {
        
        T1CONbits.TON = 1;                              // Turn on Timer 1
        repetitiveAddition (255, 255);
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
                                                    // and that the prescaler should be increased
}
/*******************************************************************************
 *
 *                      Other Functions
 *
 *******************************************************************************/
unsigned int repetitiveAddition (unsigned char FirstOperand, unsigned char SecondOperand)
{
    unsigned int ResultOfAddition = 0;
    unsigned char i = 0;
    for (i = 0; i < SecondOperand; i++ )
    {
        ResultOfAddition = ResultOfAddition + FirstOperand;
    }
    return ResultOfAddition;
}