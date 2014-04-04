/* WDB 04/27/13 
ADC source code that will initialize to setup.*/

//******** Include files
#include <p33FJ256GP710A.h>
#include "adc.h"

void ADC_Init()
/* This routine will set up two channels for ADC1.
The channels will both be setup to be sampling simultaneously as soon as a conversion is
done ASAM = 1.  The start of the coversion sequence will be based upon a timer */

	{
	/* sampling is set up first.  ADC clock period must be set.
	The only constraint is the clock period must be greater than
	65 ns.  The clock will be set off the primary oscillator that
	after PLL is 32 Mghz.
	Sampling will start immediately after a conversion
	Conversion will start based on a timer source. The source will
	start a conversion every .833 ms. The conversion time will be
	less than a few 100 us.  Therefore the sampling period will be over 500 us
	Refer to the following in ADC family reference
	Two channels will be samples sumultaneously.
	Figure 16-3,16-7,16-8,16-9,16-13
	Table 16-2
	*/  
	_SSRC = 0b010;// Start conversion with timer 3 (16 bit mode see 16.4.7.2)
	_SIMSAM = 1; //simultaneous sampling
	_ASAM = 1; //start sampling immediately after conversion
	_ALTS = 0; //Always use channel input selects for Sample A
	_ADRC = 0; //clock derived from system clock
	_ADCS = 1; //Tad = 1/16e6*2 = 125 ns
	/* ADC configuration will now be done.  Only ADC1 is used in this design
	channel 0 and channel 1 of ADC1 will be used.  The conversion will be 
	10 bits since this mode allows simultaneous sampling. The voltage
	reference for high and low will be the analog power pins AVdd and AVss.
	The ADC will output data in an unsigned integer format.
	The ADC will be set to generate an interrupt after every conversion.
	AN0 and AN3 will be used for analog input channels.  Channels will be read
	single ended with the negative terminal hooked to Vrefl = AVss
	Figures 16-14
	*/
	_AD12B=0; //10 bit mode (bits defined only for ADC 1    
	_CHPS = 0b01; // convert CHO and CH1
	// by reset channels are configured as an inputs so no code needs
	// to be generated but for illustration it is done
	_PCFG0 = 0;
	_PCFG1 = 0;
	_TRISB0 = 1;
	_TRISB3 = 1;
	_VCFG = 0b000; //voltage references are AVdd and AVss
	_FORM =0;	//unsigned integer format
	_SMPI = 0; //generate and interrupt for every conversion
	_CH0NA = 0;	// negative input is VREF- on channel 0
	_CH0SA = 0;	//positive input is connected to AN0
	_CH123NA = 0; // negative input is VREF- on channel 1
	_CH123SA = 1;	// positive input for channel 1 is AN3
/* Now set up DMA.  DMA will transfer 1 word ADC values to dual port RAM.
DMA will have a buffer size of 240 samples.  The DMA will write the values in 
conversion order that is ANO,AN3,AN0,AN3 and so forth.  The DMA will generate 
an interrupt every time the 240 samples are written, the interrupt routine will
copy these values into a vectors, it will be assumed that the first DMA address
can be read before the next sample occurs so no data will be lost.*/
	_SIZE = 0; //pound define for bits is only for chan 0 this sets a word size
	_DIR = 0; //Read from ADC write to DMA
	_HALF = 0; //DMA interrupt upon buffer bein full NOT half full
	_NULLW = 0; //function turned off normal operation 
	_AMODE = 0; // DMA writes value and increments the address register for the next write   
	_MODE = 0; // Continious mode ping pong disabled.
	_IRQSEL = 0b0001101;	//ADC1 will interrupt DMA
	DMA0STA = BeginDMA;	//This is the begenning of DMA
	DMA0PAD = 0x300; //This is the location of ADC1BUF0
	DMA0CNT = 239; //there are 240 places to write in the buffer
	_ADDMABM =1; //DMA channels are written in the order of conversion
	_DMABL = 0; // according to section 16.7.2 should be ignored
	//set up timer 1 to trigger conversion
	PR3=13333;	//set up interrupt to occur 13,333*62.5ns = 0.0008333125
	TMR3=0;
	T3CON = 0b1000000000000000;//internal clock 16 Mhz,prescale 1:1, 62.5 ns per tic, turn on 
	//enable DMA interrupt
	_IPL = 0; //ensure that interrupt will occur
	_DMA0IF = 0; //make sure DAM Channel 0 interrupt flag is reset
 	_DMA0IE =1; //enable DMA interrupt
	//turn on DMA Channel 0
	_CHEN =1;
	//turn on ADC1
	_ADON=1;
	} 

