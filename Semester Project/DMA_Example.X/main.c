// 
/* CPE 490 
This code is an example of how to set up 
ADC1 using DMA and simultaneously sampling two channels
*/
//******** Include files
#include <p33FJ256GP710A.h>
#include "ADC.h"

//******** definitions
#define TRUE 1
#define FALSE 0

//******** configuration bits
_FGS(GSS_OFF & GWRP_OFF);
_FOSCSEL(FNOSC_PRIPLL & IESO_ON);
_FOSC(FCKSM_CSDCMD & POSCMD_XT & OSCIOFNC_OFF);
_FWDT(FWDTEN_OFF);
//************* Prototypes *********

//*********** Global Variables
	int	*DMAptr;
	volatile unsigned int	VSignal[120], ISignal[120];
	volatile char BufferFlag;

//******* ISRs
void __attribute__((interrupt)) _DMA0Interrupt(void)
	{
	/*copy ADC DMA buffer 240 spots total- 120 for voltage
	signal 120 for current.  ADC is set upt sample AN0 first
	then AN3, the pattern will repeat 120 times.  ISR wil copy
	DMA buffer into data vectors.  A sample occurs every .833ms
	so should be no problem getting buffer copied before first 
	spot is written to again.  
	 
	*/
	int i;
	DMAptr = BeginDMA;
	for (i=0; i<120; i++)
		{
		VSignal[i] = *DMAptr;
		DMAptr++;
		ISignal[i] = *DMAptr;
		DMAptr++;
		}
	BufferFlag = TRUE;
	_DMA0IF = 0;
	}
//************************************************ Main *******************************8
int main()
{
// Initialization code
PLLFBD = 0x001E; //external osc is 8MHz using defaults Fosc=8MHZ /8 (0x1E+2)=32 MHz
ADC_Init();

// End of Initilization code
for( ; ; )
	{
	if (BufferFlag)
		{
		//process BufferFlag
		Nop();
		BufferFlag=FALSE;
		}
	}  
}//end of main
