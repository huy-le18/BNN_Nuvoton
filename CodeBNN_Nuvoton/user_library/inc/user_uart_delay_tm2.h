#ifndef USER_UART_DELAY_TM2_H_
#define USER_UART_DELAY_TM2_H_


void delay_us(unsigned int cnt);
void delay_193us(void);
void delay_ms(unsigned int cnt);

void user_gpio_output_tx_high(void);
void user_gpio_output_tx_low(void);
void user_UART_software_init(void);
void user_UART_software_send_byte(unsigned char TxData);
void user_UART_software_send_string(unsigned char *TxData, unsigned long length);
void user_UART_software_send_number(unsigned long number);

#endif /**/