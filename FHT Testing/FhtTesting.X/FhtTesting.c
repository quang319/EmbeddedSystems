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
#define LOWER_FREQUENCY 16
#define HIGHER_FREQUENCY 23
#define LOWER_FREQUENCY_LIMIT 71
#define UPPER_FREQUENCY_LIMIT 128

/*******************************************************************************
 *
 *                      Global Variable Declarations
 *
 *******************************************************************************/
int FhtFlag = 1, FhtCounter = 0;
int FhtAverageStackLowerFreq[8] = {100,85,96,80,29,50,59,90};
int FhtAverageLowerFreq = 0;
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
        while(1)
          {
            // Collect the bin's value for averaging
            if (FhtCounter < 8)
            {
        //      FhtAverageStackLowerFreq[FhtCounter] = (int)fht_log_out[LOWER_FREQUENCY];
              FhtCounter++;
            }
            else
            {
              // Add the frequency stack together
                int i = 0;
              for (i = 0; i < 8; i++)
              {
                FhtAverageLowerFreq += FhtAverageStackLowerFreq[i];
              }
              // divide by 8 to get the average
              FhtAverageLowerFreq = FhtAverageLowerFreq >> 3;
              // if the average is greater than or equal to the upper limit
              // then we need to break off the infinite while loop
              if (FhtAverageLowerFreq >= LOWER_FREQUENCY_LIMIT)
                break;
              // else we need to reset the counter and clear the average
              else
              {
                FhtCounter = 0;
                FhtAverageLowerFreq = 0;
              }
            }
          }

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
