/*******************************************************************************
 *  Project:        Lab 9
 *
 *  Name:           Quang Nguyen
 *
 *  Date:           3/26/2014
 *
 *  Function:
 *       This lab we will set up our own primative real time operating system
 *       The kernal will manage a clock tick that comes from an ISR on timer1
 *       There will be two tasks one that keeps and displays a real time clock
 *       the other will monitor two push buttons.  One push button will be for
 *       displaying a count that increments every time the push button is pressed
 *       the other will be to display the real time clock. Both tasks will be
 *       functions that when called will return the number of ticks that will
 *       expire before the function should be called again.
 *
 *  Pins used:
 *       S3 on RD6 and S6 on RD7
 *       Pins associated with LCD display
 *
 *  Peripherals used:
 *       TMR1 for an interrupt every 100 us
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
#include "lcd.h" //contains the LCD routines
#include <string.h>


/*******************************************************************************
 *
 *                      Constant Declarations
 *
 *******************************************************************************/
const char mytext[] =  "Count =";
//******** definitions
#define TRUE 1
#define FALSE 0


/*******************************************************************************
 *
 *                      Global Variable Declarations
 *
 *******************************************************************************/
//*********** Global Variables
char            RTCFlag;
unsigned int	TickCounter;
char LCDDisplay[17] = "Count = 00000   ";
/*******************************************************************************
 *
 *                      Function Prototype
 *
 *******************************************************************************/
void initialize();

void __attribute__ ((interrupt, no_auto_psv)) _T1Interrupt (void);
unsigned int ButtonPushCounter(void);
void WriteLCD(void);
unsigned int RTCTimer(void);

void BinaryToDecimal ( int Value, char *TenthPlace, char *OnePlace);
void displayCounter(int Count);
void digitCheck(int *FirstDigit, int *SecondDigit);


/*******************************************************************************
 *
 *                      Main Function
 *
 *******************************************************************************/
int main() {
    // setting up everything
    initialize();

    unsigned int BpInitialTick=0, RtcInitialTick=0, Temp, BpWaitCount=0, RtcWaitCount=0;

    /*************** Main Loop ***********************************/
    while (TRUE)//start of the kernal that should never be exited
	{
            //Since button push has higher priority we will take care of it first.
            if (BpInitialTick <= TickCounter) Temp = TickCounter - BpInitialTick;
            else Temp = (65535-BpInitialTick)+ 1 + TickCounter;
            if (Temp >= BpWaitCount)
                    {
                    BpWaitCount = ButtonPushCounter();
                    BpInitialTick = TickCounter;
                    }
            //Now check to see if the RTC needs to run (it runs every .5 second)
            if (RtcInitialTick <= TickCounter) Temp = TickCounter - RtcInitialTick;
            else Temp = (65535 - RtcInitialTick)+ 1 + TickCounter;
            if (Temp >= RtcWaitCount)
                    {
                    RtcWaitCount = RTCTimer();
                    RtcInitialTick = TickCounter;
                    }
            //This last part is the lowest priority and it prints to the LCD display
            //Needs to be written so that if inteface is busy then return but don't
            //loose your place in writing the LCD
            WriteLCD();
	} // end of main program loop
    return 1;
} //end of main function call (should never be reached)


/*******************************************************************************
 *
 *                      Initialize Function
 *
 *******************************************************************************/
void initialize() {
        // Initialization code
    PLLFBD = 0x001E; //external osc is 8MHz using defaults Fosc=8MHZ /8 (0x1E+2)=32 MHz
    Init_LCD();//Initialize the LCD this must be done first before the LCD is to be used.
    while (checkBusy());
    home_clr(); // start display at top line left most character
    T1CON = 0b1000000000000000; //internal clock,1:1prescalar,start timer
    // 1 time tic = 2/32 MHz= 62.5 ns
    PR1 = 1600-1; //timer will interuppt at 1600 * 62.5 ns = 100 us
    _T1IF = 0; //make sure interrupt flag is cleared
    _T1IE = 1; //enable the timer 1 interrupt
    _IPL = 0; //lower the priority level so that the timer inteerupt at level 4
                      //by default will cause an interrupt
    // End of Initilization code
}

/*******************************************************************************
 *
 *                      Interrupt Service Routine
 *
 *******************************************************************************/
void __attribute__ ((interrupt, no_auto_psv)) _T1Interrupt (void)
{
	TickCounter++;
	_T1IF = 0;
}

/*******************************************************************************
 *
 *                      Other Functions
 *
 *******************************************************************************/
/***************************************************
 *
 * Name:            ButtonPushCounter
 * Priority:        2  (0 = lowest and 2 = highest)
 * Parameters:      void
 * Return:  
 *          int -- The a value of how many clock ticks or counts of TickCounter 
 *          must pass before this function should be called again.
 * Pins use:        S3 on RD6 and S6 on RD7
 * Global Var used:
 *          char            RTCFlag;
 *          unsigned int    TickCounter;
 *          char            LCDDisplay[17];
 * Function: 
 *          The inputs will be scanned every 10 ms until a push button is activated.  
 *          The switch must be released before any new switch scanning can be done.
 *          To avoid switch bounce once a button is pressed the state of the input switches 
 *          will not be scanned for 100 ms and then if released the scanning will resume to every 10 ms
 * 
 *          S3 switch will cause a 16 bit counter to increment and display the count on the LCD
 *          The 16 bit counter will roll over to 0.  
 *          This task will prepare the string that will be displayed on the LCD showing the count.
 * 
 *          The S6 switch will cause the real time clock to be displayed.  
 *          The RTC will be displayed until the S3 switch is pressed at which 
 *          point the count display will return.  
 * 
 ***************************************************/
unsigned int ButtonPushCounter(void)
{
    static int Rd6ChangeStateFlag = 0;
    static int Rd7ChangeStateFlag = 0;
    static unsigned int Rd6Counter = 0;

    // if RD6(S3) is pressed
    // Display count on LCD
    if ( (_RD6 == 0) && (Rd6ChangeStateFlag == 0))
    {
        RTCFlag = 0;
        Rd6ChangeStateFlag = 1;

        //Increment Counter
        Rd6Counter++;
        // Load the LCD with the count display
        displayCounter(Rd6Counter);

        //Scan again every 100 ms
        return 1000;
    }
    // If the user is holding down the button
    else if ( (_RD6 == 0) && (Rd6ChangeStateFlag == 1))
    {
        return 1000;
    }
    // If the user just released the button
    else if ( (_RD6 == 1) && (Rd6ChangeStateFlag == 1))
    {
        Rd6ChangeStateFlag = 0;
        return 100;
    }

    // if RD7(S6) is pressed
    // Display RTC time on LCD
    if ( (_RD7 == 0) && (Rd7ChangeStateFlag == 0))
    {
        RTCFlag = 1;
        Rd7ChangeStateFlag = 1;

        //Scan again every 100 ms
        return 1000;
    }
    // If the user is holding down the button
    else if ( (_RD7 == 0) && (Rd7ChangeStateFlag == 1))
    {
        return 1000;
    }
    // If the user just released the button
    else if ( (_RD7 == 1) && (Rd7ChangeStateFlag == 1))
    {
        Rd7ChangeStateFlag = 0;
        return 100;
    }

    // else tell the program to scan every 10 ms
    return 100;
}

/***************************************************
 *
 * Name:            RTCTimer
 * Priority:        1  (0 = lowest and 2 = highest)
 * Parameters:      void
 * Return:
 *          int -- The a value of how many clock ticks or counts of TickCounter
 *          must pass before this function should be called again.
 *          In this case, it should be 500 ms or 0.5 s
 * Pins used:        None
 * Global Var used:
 *          char            RTCFlag;
 *          char            LCDDisplay[17];
 * Function:
 *          The task will tell the kernel to call it again every 500 ms so that it can keep the current time.
 *          This task prepares the string to be printed on the LCD when the RTC is chosen.
 *
 *          The display will look like ? RTC HH:MM:SS.Z?
 *          where H is the hours in military time (range of 00 to 23),
 *          M is for minutes, S is for seconds, and Z is either 0 or 5 for the half second.
 *
 ***************************************************/
unsigned int RTCTimer(void)
{
    static int Z = 0, S = 0, M = 0, H = 0;      //This will only get initizialize at compiler's time
    // Z counts in half a second increment.
    // If Z overflowed
    Z++;
    if ( Z == 2)
    {
        Z = 0;
        S++;
    }
    // If S overflowed
    if ( S > 60)
    {
        S = 0;
        M++;
    }
    // if M overflowed
    if ( M > 60)
    {
        M = 0;
        H++;
    }
    // if H overflowed
    if ( H > 24)
    {
        H = 0;
    }

    // Load the LCDDispaly array with the time if the RTCFlag is HIGH
    if (RTCFlag == 1)
    {
        char Temp[5] = "RTC ";
        int Counter;
        for (Counter =0 ; Counter <= 3; Counter++)
        {
            LCDDisplay[Counter] = Temp[Counter];
        }
        BinaryToDecimal( H, &LCDDisplay[4], &LCDDisplay[5]);
        LCDDisplay[6] = ':';
        BinaryToDecimal( M, &LCDDisplay[7], &LCDDisplay[8]);
        LCDDisplay[9] = ':';
        BinaryToDecimal( S, &LCDDisplay[10], &LCDDisplay[11]);
        LCDDisplay[12] = '.';
        if ( Z == 1)
            LCDDisplay[13] = '5';
        else LCDDisplay[13] = '0';
    }

    //Call again every 0.5 s
    return 5000;
}

/***************************************************
 *
 * Name:            WriteLCD
 * Priority:        0  (0 = lowest and 2 = highest)
 * Parameters:      void
 * Return:          void
 * Pins used:       Pins associate with LCD
 * Global Var used:
 *          char    LCDDisplay[17];
 * Function:
 *          It will send the current display string one character at a time to the LCD.
 *          Do not use the puts_lcd() function in your code.
 *
 *
 ***************************************************/
void WriteLCD(void)
{
    static int LCDCounter = 0;
    //Check if busy
    if (checkBusy() == 0)
    {
        // Printer Character on LCD
        lcd_data( LCDDisplay[LCDCounter] );
        LCDCounter++;                           // Increment the counter


        // if LCDCounter is at the end of the character
        if (LCDCounter >= 17)
        {
            LCDCounter = 0;
            //Return the LCD back to home
            home_it();
        }
    }
}
/***************************************************
 *
 * Name:            BinaryToDecimal
 * Parameters:
 *          int value : The value of time
 *          char *TenthPlace : Pointer to the variable for the tenth place
 *          char *OnePlace : Pointer to the variable for the one place
 * Return:
 *          void
 * Pins used:        None
 * Function:
 *          Function will convert will convert a binary value to decimal value
 *
 ***************************************************/
void BinaryToDecimal ( int Value, char *TenthPlace, char *OnePlace)
{
    int Temp = 0;
    if (Value > 9)
    {
        while(Value > 9)
        {
            Value -= 10;
            Temp++;
        }
    }
    // Assign value to the variable and convert it to ASCII
    *TenthPlace = 48 + Temp;
    *OnePlace = 48 + Value;
}

/***************************************************
 *
 * Name:            displayCounter
 * Parameters:
 *          int Count
 * Return:  void
 * Pins used:        None
 * Global Var used:
 *          char            LCDDisplay[17];
 * Function:
 *          Similiar to the binaryToDecimal Function.
 *          This function will take the value of count and convert
 *          it to decimal.
 *
 ***************************************************/
void displayCounter(int Count)
{
    //Goal: to display "Count = XXXXX   "
    char TempChar[9] = "Count = ";
    int Counter;
    for (Counter =0 ; Counter < 8; Counter++)
    {
        LCDDisplay[Counter] = TempChar[Counter];
    }


    int Ten = 0, Hundred = 0, Thousand = 0, TenThousand = 0;
    if (Count > 9)
    {
        while(Count > 9)
        {
            Count -= 10;
            Ten++;
            digitCheck(&Ten, &Hundred);
            digitCheck(&Hundred, &Thousand);
            digitCheck(&Thousand, &TenThousand);
        }
    }

    LCDDisplay[12] = Count + 48;
    LCDDisplay[11] = Ten + 48;
    LCDDisplay[10] = Hundred + 48;
    LCDDisplay[9] = Thousand + 48;
    LCDDisplay[8] = TenThousand + 48;
    LCDDisplay[13] = ' ';
}

/***************************************************
 *
 * Name:            digitCheck
 * Parameters:
 *          int *FirstDigit
 *          int *SecondDigit
 * Return:
 *          void
 * Pins used:        None
 * Global Var used:
 * Function:
 *          This function will look at the pointer for the first digit
 *          and increment the second digit if it is greater than 9.
 *
 ***************************************************/
void digitCheck(int *FirstDigit, int *SecondDigit)
{
    if ((*FirstDigit) > 9)
    {
        *FirstDigit -= 10;
        (*SecondDigit)++;
    }
}