/*******************************************************************************
 *  Project:            Lab 5 Part 1
 *
 *  Name:               Quang Nguyen
 *
 *  Date:               2/14/2014
 *
 *  Function:           To find out the time that it takes to do the following multiplications:
 *                           Char = char  * char
 *                           Int = int * int
 *                           Long = long * long
 *                           Long long = long long * long long
 *                           Float = float * float
 *                           Long double = long double * long double
 *
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
#define OP1 3.1
#define OP2 4.1
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

    /*************** Main Loop ***********************************/
    char ResultOfChar = 0,          CharTemp1 = OP1,            CharTemp2 = OP2;
    int ResultOfInt = 0,            IntTemp1 = OP1,             IntTemp2 = OP2;
    long ResultOfLong = 0,          LongTemp1 = OP1,            LongTemp2 = OP2;
    long long ResultOfLongLong = 0, LongLongTemp1 = OP1,        LongLongTemp2 = OP2;
    float ResultOfFloat = 0,        FloatTemp1 = OP1,           FloatTemp2 = OP2;
    long double  ResultOfLongDouble = 0, LongDoubleTemp1 = OP1, LongDoubleTemp2 = OP2;

    int Time[6];
                //time[0] : Char
                //time[1] : Int
                //time[2] : Long
                //time[3] : Long Long
                //time[4] : Float
                //time[5] : Long Double

    // Initialize Time
    int i;
    for (i = 0; i < 6; i++)
        Time[i] = 0;
    i =0;

    while (1) {

/***************** Char = Char * Char **************************************/
                            // Note: TMR1 should be 0 at this point
        T1CONbits.TON = 1;                              // Turn on Timer 1
        ResultOfChar = CharTemp1 / CharTemp2;
        T1CONbits.TON = 0;                              // Turn OFF Timer 1
        Time[i] = TMR1;                                 // Store value of TMR1
        i++;                                            // Increment pointer
        TMR1 = 0;                                       // Clear TMR1

/*************** Int = Int * Int ******************************************/
        T1CONbits.TON = 1;                              // Turn on Timer 1
        ResultOfInt = IntTemp1 / IntTemp2;
        T1CONbits.TON = 0;                              // Turn OFF Timer 1
        Time[i] = TMR1;                                 // Store value of TMR1
        i++;                                            // Increment pointer
        TMR1 = 0;                                       // Clear TMR1

/*************** Long = Long * Long ******************************************/
        T1CONbits.TON = 1;                              // Turn on Timer 1
        ResultOfLong = LongTemp1 / LongTemp2;
        T1CONbits.TON = 0;                              // Turn OFF Timer 1
        Time[i] = TMR1;                                 // Store value of TMR1
        i++;                                            // Increment pointer
        TMR1 = 0;                                       // Clear TMR1

/*************** Long Long = Long Long * Long Long ******************************/
        T1CONbits.TON = 1;                              // Turn on Timer 1
        ResultOfLongLong = LongLongTemp1 / LongLongTemp2;
        T1CONbits.TON = 0;                              // Turn OFF Timer 1
        Time[i] = TMR1;                                 // Store value of TMR1
        i++;                                            // Increment pointer
        TMR1 = 0;                                       // Clear TMR1

/*************** Float = Float * Float ******************************************/
        T1CONbits.TON = 1;                              // Turn on Timer 1
        ResultOfFloat = FloatTemp1 / FloatTemp2;
        T1CONbits.TON = 0;                              // Turn OFF Timer 1
        Time[i] = TMR1;                                 // Store value of TMR1
        i++;                                            // Increment pointer
        TMR1 = 0;                                       // Clear TMR1

/*************** Long Double = Long Double * Long Double ************************/
        T1CONbits.TON = 1;                              // Turn on Timer 1
        ResultOfLongDouble = LongDoubleTemp1 / LongDoubleTemp2;
        T1CONbits.TON = 0;                              // Turn OFF Timer 1
        Time[i] = TMR1;                                 // Store value of TMR1
        i++;                                            // Increment pointer
        TMR1 = 0;                                       // Clear TMR1

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
