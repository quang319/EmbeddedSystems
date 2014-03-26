/* 
 * File:   ArduinoLift.h
 * Author: Quang
 *
 * Created on March 13, 2014, 7:07 PM
 */

#ifndef ARDUINOLIFT_H
#define	ARDUINOLIFT_H

#ifdef	__cplusplus
extern "C" {
#endif

    // Need to remove when move to arduino
    #include <p33FJ256GP710A.h>

#define LiftBusy    _RE0    // Busy flag.      0 = ready , 1 = busy
#define LiftData    _RE1    // Data flag.      0 = do nothing, 1 = move to next state
#define LiftLevel   _RE2    // Tells which level the plate is located on
                                // 0 = bottom shelf
                                // 1 = top shelf




    // Initialize functions for the slave and the master
    void LiftMasterInitialize ();
    void LiftSlaveInitialize ();

    void LiftMasterSetLevel(int Shelf);

    // This function is called to indicate that we want to request the slave to change state
    void LiftMasterRequestStateChange ();

    // This function is the heart of the library. It handles the communication between the master and the slave
    void LiftMasterSpinOnce();






#ifdef	__cplusplus
}
#endif

#endif	/* ARDUINOLIFT_H */

