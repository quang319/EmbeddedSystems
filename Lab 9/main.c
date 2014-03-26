// 
/* CPE 490 Lab 9
This lab we will set up our own primative real time operating system
The kernal will manage a clock tick that comes from an ISR on timer1
There will be two tasks one that keeps and displays a real time clock
the other will monitor two push buttons.  One push button will be for
displaying a count that increments every time the push button is pressed
the other will be to display the real time clock. Both tasks will be
functions that when called will return the number of ticks that will 
expire before the function should be called again. 
*/
//******** Include files
#include <p33FJ256GP710A.h>
#include "lcd.h" //contains the LCD routines
#include <string.h>
//store strings in program memory by using const data memory is not reserved
const char mytext[] =  "Count =";
//******** definitions
#define TRUE 1
#define FALSE 0
//******** configuration bits
_FGS(GSS_OFF & GWRP_OFF);
_FOSCSEL(FNOSC_PRIPLL & IESO_ON);
_FOSC(FCKSM_CSDCMD & POSCMD_XT & OSCIOFNC_OFF);
_FWDT(FWDTEN_OFF);
//************* Prototypes *********
unsigned int ButtonPushCounter(void); 
void WriteLCD(void);
unsigned int RTCTimer(void);
//*********** Global Variables
char RTCFlag;
unsigned int	TickCounter;
char LCDDisplay[17];
//******* ISRs
void __attribute__((interrupt)) _T1Interrupt(void)
{
	TickCounter++;
	_T1IF = 0;
}
//************************************************ Main *******************************8
main()
{
// Initialization code
PLLFBD = 0x001E; //external osc is 8MHz using defaults Fosc=8MHZ /8 (0x1E+2)=32 MHz
Init_LCD();//Initialize the LCD this must be done first before the LCD is to be used.
while (check_busy())
	{
	} 
home_clr(); // start display at top line left most character
T1CON = 0b1000000000000000; //internal clock,1:1prescalar,start timer
// 1 time tic = 2/32 MHz= 62.5 ns
PR1 = 1600-1; //timer will interuppt at 1600 * 62.5 ns = 100 us
_T1IF = 0; //make sure interrupt flag is cleared
_T1IE = 1; //enable the timer 1 interrupt 
_IPL = 0; //lower the priority level so that the timer inteerupt at level 4 
		  //by default will cause an interrupt
// End of Initilization code
//Variable decleration
unsigned int bpInitialTick=0, rtcInitialTick=0, Temp, bpWaitCount=0, rtcWaitCount=0;
while (TRUE)//start of the kernal that should never be exited
	{
	//Since button push has higher priority we will take care of it first.
	if (bpInitialTick <= TickCounter) Temp = TickCounter - bpInitialTick;
	else Temp = (65535-bpInitialTick)+ 1 + TickCounter;
	if (Temp >= bpWaitCount)
		{
		bpWaitCount = ButtonPushCounter();
		bpInitialTick = TickCounter;
		}
	//Now check to see if the RTC needs to run (it runs every .5 second)
	if (rtcInitialTick <= TickCounter) Temp = TickCounter - rtcInitialTick;
	else Temp = (65535 - rtcInitialTick)+ 1 + TickCounter;
	if (Temp >= rtcWaitCount)
		{
		rtcWaitCount = RTCTimer();
		rtcInitialTick = TickCounter;
		}
	//This last part is the lowest priority and it prints to the LCD display
	//Needs to be written so that if inteface is busy then return but don't 
	//loose your place in writing the LCD  	
	WriteLCD();
	} // end of main program loop
} //end of main function call (should never be reached)
//***********************end Main ************************************************
