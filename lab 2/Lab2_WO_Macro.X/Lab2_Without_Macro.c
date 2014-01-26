/*************************************************************/
//  Project:    Lab 2
//
//  Name:   Quang Nguyen
//  Date:   1/24/2013
//
//  Function:
//      Turn on RA0-7 for 256 instructions and then off for 256 instructions
//
//  Pins used:  Lower Byte of port A
//
//  Comments
//
/*************************************************************/

/*************** Library includes*****************************/
#include <p33FJ256GP710A.h>


/*************** Constants Declaration ***********************/


/*************** Function Prototype ***************************/
void initialize();

/*************** Main Program ********************************/
int main() {
    // setting up everything
    initialize();

    /*************** Main Loop ***********************************/
    while (1) 
    {
        int Timer1Counter = 0;
        // Turn PORT A ON
        PORTA = 0xFF;

        // Delay 0.5 second
        while (Timer1Counter <10)
        {
            if (TMR1 == 3125)
            {
                Timer1Counter++;
                TMR1 = 0;
            }
        }      // End of Timer1Counter while loop

        // Turn PORT A OFF
        PORTA = 0;                  // Turn port off

        // Delay 0.5 second
        Timer1Counter = 0;
        while (Timer1Counter < 10)
        {
            if (TMR1 == 3125)
            {
                Timer1Counter++;
                TMR1 = 0;
            }
        }       // End of Timer1Counter while loop
    }           // End of infinite while loop

    return 1;
}

void initialize()
{
    // Setting up PORT A
    AD1PCFGH = 0xFF;                // Turn of ADC for Module 1
    TRISA = 0x00;                   // Port A to output
    // Setting up Timer 1 module
    T1CON = 0b0100000000110000;
            // TON = OFF
            // TGATE = OFF
            // TCKPS (Prescaler) = 256
                // 2/16 MHz = 1.25*10^-7
                // 1.25*10^-7 * 256 = 0.000032
                // 3125 instructions * .000032 = 0.1 second ***
                // We are going to let this overflow 5 times to make 0.5 second
            //  TSYNC = OFF
            // TCS = Internal Clock
    // Start Timer 1
    T1CONbits.TON = 1;
}
