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
#include "ArduinoLift.h"


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
//struct ArduinoLift {
//    char Busy;              // Busy flag.      0 = ready , 1 = busy
//    char Data;              // Data flag.      0 = do nothing, 1 = move to next state
//    char Level;             // Tells which level the plate is located on
//                                // 0 = bottom shelf
//                                // 1 = top shelf
//    char Change;            // 0 = don't need to change state, 1 = need to change state
//    char SetFlag;           // Just set flag
//};
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


    AD1PCFGH = 0xFF00;   //turns off channels AN24-31
    AD1PCFGL = 0x8000; //turns off channel AN15 for ADC1
    AD2PCFGL = 0x8000; //turns off channel AN15 for ADC2

    LiftMasterInitialize ();
    LiftMasterRequestStateChange();

//    struct ArduinoLift Lift;
//    Lift.Busy = 0;
//    Lift.Data = 0;
//    Lift.Level = 0;
//    Lift.Change = 1;
//    Lift.SetFlag = 0;


//    TRISEbits.TRISE0 = 1;
//    TRISEbits.TRISE1 = 0;
//    TRISEbits.TRISE2 = 0;
//
//    Lift.Busy = _RE0;
//    _RE1 = Lift.Data;
//    _RE2 = Lift.Level;

    /*************** Main Loop ***********************************/
    while (1) {
        LiftMasterSpinOnce();

//        //Update Data
//        Lift.Busy = _RE0;
//
//        // If the slave is not busy and it need to progress to the next state of the lift
//        if ((Lift.Busy == 0) && (Lift.Change == 1) && (Lift.SetFlag == 0))
//        {
//            Lift.Data = 1;
//            Lift.SetFlag = 1;
//        }
//        // If the slave became busy because it register our request for a change in state
//        else if ((Lift.Busy == 1) && (Lift.SetFlag == 1)){
//            Lift.Data = 0;
//            Lift.SetFlag = 0;
//            Lift.Change = 0;
//        }

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
