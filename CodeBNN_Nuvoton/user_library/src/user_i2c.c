#include "user_i2c.h"
#include "ms51_16k.h"

#define SCL_PIN		P02
#define SDA_PIN		P16

#define SCL_MCR		2
#define SDA_MCR		6


void user_i2c_scl_set_output(void)
{
//	user_gpio_output_init(SCL_GPIO, SCL_PIN);
	P0M1 &= ~(1 << SCL_MCR);
  P0M2 |= (1 << SCL_MCR);  // 0b0001 0000  = 0x10
}
void user_i2c_scl_output_high(void)
{
	SCL_PIN = 1;
}

void user_i2c_scl_output_low(void)
{
	SCL_PIN = 0;
}

void user_i2c_scl_tonggle(void)
{
	SCL_PIN = !SCL_PIN;
}


void user_i2c_sda_set_output(void)
{
	P1M1 &= ~(1 << SDA_MCR);
  P1M2 |= (1 << SDA_MCR);  // 0b0001 0000  = 0x10
}

void user_i2c_sda_output_high(void)
{
	SDA_PIN = 1;
}
void user_i2c_sda_output_low(void)
{
	SDA_PIN = 0;
}

void user_i2c_sda_set_input(void)
{
  P1M1 |= (1 << SDA_MCR);  // 0b0001 0000  = 0x10
	P1M2 &= ~(1 << SDA_MCR);
}

bit user_i2c_sda_get_data(void)
{
	bit temp;

//	temp = user_gpio_input_read(SDA_GPIO, SDA_PIN);
	temp = P16;

	return temp;
}


