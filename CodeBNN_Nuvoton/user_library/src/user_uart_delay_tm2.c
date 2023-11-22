#include "user_uart_delay_tm2.h"
#include "ms51_16k.h"
#include "user_ms51_macro.h"

#define DELAY_FRAME 200
#define BAUDRATE 81	
void delay_us(unsigned int cnt)
{
	T2MOD |= (1 << T2DIV1);   // f = fsys/16 = 1MHz
	
	TL2 = (65535 - cnt ) & 0xff;
	TH2 = (65535 - cnt ) >> 8;
	T2CON |= (1 << TR2);
	while((1 & (T2CON >> TF2)) == 0);
	T2CON &= ~(1 << TF2);
	T2CON &= ~(1 << TR2);
}

void delay_193us(void)
{
	T2MOD |= (1 << T2DIV1);   // f = fsys/16 = 1MHz
	
	TL2 = (65535 - 200 ) & 0xff;
	TH2 = (65535 - 200 ) >> 8;
	T2CON |= (1 << TR2);
	while((1 & (T2CON >> TF2)) == 0);
	T2CON &= ~(1 << TF2);
	T2CON &= ~(1 << TR2);
}

void delay_ms(unsigned int cnt)
{
	T2MOD |= (1 << T2DIV1);   // f = fsys/16 = 1MHz
	while(cnt)
	{
		TL2 = (65535 - 1000 ) & 0xff;
		TH2 = (65535 - 1000 ) >> 8;
		T2CON |= (1 << TR2);
		while((1 & (T2CON >> TF2)) == 0);
		T2CON &= ~(1 << TF2);
		T2CON &= ~(1 << TR2);
		cnt --;
	}
}

void user_gpio_output_tx_high(void)
{
		P16 = 1;
}
void user_gpio_output_tx_low(void)
{
		P16 = 0;
}

void user_UART_software_init(void)
{
		P1M1 &= ~(1 << 6);
  	P1M2 |= (1 << 6);  // 0b0001 0000  = 0x10
}

void user_UART_software_send_byte(unsigned char TxData)
{
	uint8_t i;

	user_gpio_output_tx_low();   // start 
//	delay_193us();
	delay_us(BAUDRATE);
	for (i = 0; i < 8; i++) //send 8-bit data
	{

		if (TxData & 0x01)
		{
			user_gpio_output_tx_high();
		}
		else
		{
			user_gpio_output_tx_low();
		}
		delay_us(BAUDRATE);
		TxData >>= 1;
	}
	user_gpio_output_tx_high();
	delay_us(BAUDRATE);
}

void user_UART_software_send_string(unsigned char *TxData, unsigned long length)
{
	uint32_t temp = 0;

	for(temp = 0; temp < length; temp ++)
	{
		user_UART_software_send_byte(TxData[temp]);
		delay_us(DELAY_FRAME);
	}
}

void user_UART_software_send_number(unsigned long number)
{
	uint8_t temp = 0;

	temp = number / 1000;
	user_UART_software_send_byte(temp + 48);
	delay_us(DELAY_FRAME);

	temp = number % 1000 / 100;
	user_UART_software_send_byte(temp + 48);
	delay_us(DELAY_FRAME);

	temp = number % 100 / 10;
	user_UART_software_send_byte(temp + 48);
	delay_us(DELAY_FRAME);

	temp = number % 10;
	user_UART_software_send_byte(temp + 48);
	delay_us(DELAY_FRAME);
}









