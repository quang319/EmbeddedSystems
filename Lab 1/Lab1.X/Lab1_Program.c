//
// CPE 490 First Program
//
#include <p33FJ256GP710A.h>
main()
{
//    AD1PCFGHbits.PCFG22 = 1;
//    AD1PCFGHbits.PCFG23 = 1;
    AD1PCFGH = 0xFF;
    PMD1bits.AD1MD = 1;
    PMD3bits.AD2MD = 1;
    TRISA = 0x00;
    PORTA = 0xFF;
}