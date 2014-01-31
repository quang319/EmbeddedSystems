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
//      This code was written with macro
//
/*************************************************************/

/*************** Library includes*****************************/
#include <p33FJ256GP710A.h>
#define FCY 16000000            // Constant needed for the libpic30.h library
#include <libpic30.h>           // library for the delay function

/*************** Constants Declaration ***********************/
#pragma config FWDTEN = OFF

/*************** Function Prototype ***************************/
void initialize();

/*************** Main Program ********************************/
int main() {
    // setting up everything
    initialize();

    /*************** Main Loop ***********************************/
    while (1)
    {
        PORTA = 0xFF;
        __delay32(256);             // Delay for 256 instructions
        PORTA = 0;                  // Turn port off
        __delay32(256);             // Delay for 256 instructions
    }

    return 1;
}

/****************** Setup function *******************************/
void initialize()
{
    // Setting up PORT A
    AD1PCFGH = 0xFF;                // Turn of ADC for Module 1
    TRISA = 0x00;                   // Port A to output
}
