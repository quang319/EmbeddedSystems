//
// CPE 490 First Program
//
#include <p33FJ256GP710A.h>
#define FCY 16000000
#include <libpic30.h>

void initialize()
{
    AD1PCFGH = 0xFF;
    TRISA = 0x00;
}

int main()
{
    initialize();

    while(1)
    {
        PORTA = 0xFF;
        __delay32(256);
        PORTA = 0;
        __delay32(256);
    }

    return 1;
}
