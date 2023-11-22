#include "user_adc.h"
#include "ms51_16k.h"


void adc_init(void)
{
	/*enable ADC*/
	ADCCON1 |= (1 << 0); 
	/*Fadc = Fsys/1*/
	ADCCON1 &= ~(1 << 4);
	ADCCON1 &= ~(1 << 5);
	
	/*channel 5*/
	ADCCON0 &= ~(1 << 3);
//	ADCCON0 |= (1 << 2);
	ADCCON0 &= ~(1 << 1);
//	ADCCON0 |= (1 << 0);
	
	/*enable interrupt ADC*/
	IE |= (1 << 6);
	/*enable interrupt global*/
	IE |= (1 << 7);
}

