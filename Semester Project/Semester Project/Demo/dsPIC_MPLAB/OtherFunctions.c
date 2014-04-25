#include "OtherFunctions.h"
#include <math.h>

/***************************************************
 *
 * Name:            rmsCalc
 * Parameters:
 *          int *Value
 * Return:  unsigned int
 * Pins used:        None
 * Function:
 *          This function will calculate the RMS value
 *          from 120 data points and return the result
 *          as a int
 *
 *          RMS is calculated by first squaring the value,
 *          summing the value, averaging the result, and finally
 *          finding the square root of the result
 *
 ***************************************************/

unsigned int rmsCalc(int *Value)
{
    int TestingResult = 0;
    int i;
    int *Temp = Value;
    int TempVar = 0;
//    Result  = 0;
    // Squaring and summing the data points
    for (i = 0; i < 120; i++)
    {
        Temp += i;
        TempVar = *Temp;
        TestingResult = TempVar * TempVar;
    }
    //Finding the average
    TestingResult = (long int)(TestingResult / 120);
    // Square root the result
    int Square = 0;
    int SquareRootResult = 0;
    while (Square <= TestingResult)
    {
        SquareRootResult++;
        Square = SquareRootResult * SquareRootResult;
    }
    if (Square > TestingResult)
        SquareRootResult--;
    // Store the result of the square root
    TestingResult = SquareRootResult;

    return TestingResult;
}


/***************************************************
 *
 * Name:            displayCounter
 * Parameters:
 *          int Count
 * Return:  void
 * Pins used:        None
 * Global Var used:
 *                   None
 * Function:
 *          Similiar to the binaryToDecimal Function.
 *          This function will take the value of count and convert
 *          it to decimal.
 *
 ***************************************************/
void displayCounter(int Count, char *LCD)
{
    //Goal: to display "Count = XXXXX   "
    char TempChar[9] = "Count = ";
    int Counter;
    for (Counter =0 ; Counter < 8; Counter++)
    {
        LCD[Counter] = TempChar[Counter];
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
    LCD[13] = ' ';
    LCD[12] = Count + 48;
    LCD[11] = Ten + 48;
    LCD[10] = Hundred + 48;
    LCD[9] = Thousand + 48;
    LCD[8] = TenThousand + 48;
    
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
