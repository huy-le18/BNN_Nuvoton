#include "delay_ms_tm3.h"
#include "ms51_16k.h"
void delay_ms(unsigned int cnt)
{
	T3CON |= (1 << 2);   // f = fsys/16 = 1MHz
	while(cnt)
	{
		RL3 = (65535 - 1000) & 0xff;
	  RH3 = (65535 - 1000) >> 8;
		T3CON |= (1 << 3);
		while((1 & (T3CON >> 4)) == 0);
		T3CON &= ~(1 << 4);
		T3CON &= ~(1 << 3);
		cnt --;
	}
}