#include "ArduinoLift.h"

// Need to remove when move to arduino
#include <p33FJ256GP710A.h>

    char Change;            // 0 = don't need to change state, 1 = need to change state
    char SetFlag;           // Just set flag



/***********************************************
 *
 *      Functions for master
 *
 ***********************************************/
void LiftMasterInitialize()
{
    // initialize all the varaibles

    Change = 0;
    SetFlag = 0;

    // Set all the pins to the proper mode
    TRISEbits.TRISE0 = 1;           // Busy pin = input
    TRISEbits.TRISE1 = 0;           // Data pin = output
    TRISEbits.TRISE2 = 0;           // Level pin = output

    LiftData = 0;
    LiftLevel = 0;
}

void LiftMasterSetLevel(int Shelf)
{
    LiftLevel = Shelf;
}

void LiftMasterRequestStateChange ()
{
    Change = 1;
}

void LiftMasterSpinOnce()
{
    // If the slave is not busy and it need to progress to the next state of the lift
    if ((LiftBusy == 0) && (Change == 1) && (SetFlag == 0))
    {
        LiftData = 1;
        SetFlag = 1;

    }
    // If the slave became busy because it register our request for a change in state
    else if ((LiftBusy == 1) && (SetFlag == 1)){
        LiftData = 0;
        SetFlag = 0;
        Change = 0;
    }
}

/***********************************************
 *
 *      Functions for Slave
 *
 ***********************************************/
void LiftSlaveInitialize()
{
    // Set all the pins to the proper mode
    TRISEbits.TRISE0 = 0;           // Busy pin = Output
    TRISEbits.TRISE1 = 1;           // Data pin = input
    TRISEbits.TRISE2 = 1;           // Level pin = input

    LiftBusy = 0;           // Not busy since the slave just got booted up
}