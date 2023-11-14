/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2020 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include "ms51_16k.h"
#include "user_uart_delay_tm2.h"
#include "user_i2c.h"
/************************************************************************************************************/
/* FUNCTION_PURPOSE: Main Loop                                                                              */
/************************************************************************************************************/

void master_process(void);

unsigned char CRC8(volatile unsigned char *Data, unsigned char length);

// define command send from slave
#define TEST_CG_CMD 1  // test chong giat
#define RESET_CG_CMD 2 // rest chong giat
#define PUMP_ON_CMD 3  // pump on
#define PUMP_OFF_CMD 4 // pump off

// define error send from master
#define CONNECT_UI_BOARD_ERR_BIT 0
#define OUT_TEMP_HIGHT_ERR_BIT 1
#define FLOW_LOW_ERR_BIT 2
#define TESTING_CB_NOTIFY_BIT 3
#define ADC_OUTPUT_ERR_BIT 4
#define ADC_INPUT_ERR_BIT 5
#define PUMP_STATUS_BIT 6

#define MIN_FLOW_PULSE_ACTIVATED 8
#define LOW_FLOW_PULSE_LIMIT 28

#define IN 1
#define OUT 0

#define MAX_2MS_CNT 16 // for 125us step
#define MAX_12MS_CNT 96
volatile unsigned char count2ms;
volatile unsigned char count10ms, count20ms, count100ms, count200ms, count1s, count2s, count3s, count5s, count1min;
volatile uint8_t count12ms;

volatile bit b_2ms;
volatile bit b_10ms;
volatile bit b_20ms;
volatile bit b_100ms;
volatile bit b_200ms;
volatile bit b_1s;
volatile bit b_2s;
volatile bit b_3s;
volatile bit b_5s;
volatile bit b_1min;

volatile bit b_adc;
volatile uint8_t outTemp_ref;

volatile bit FM_rx_done;
volatile bit FM_tx;
volatile bit M_SCL;
volatile bit M_SDA;
volatile bit M_SDA_IO;
volatile bit rise_m_sda;
volatile uint8_t cnt_clk;
volatile uint8_t m_tx_data;
volatile uint8_t m_rx_data[3];

void master_process();

#define ADC_IN_LOW_LIMIT 10
#define ADC_IN_HIGH_LIMIT 1000

#define ADC_OUT_LOW_LIMIT 10
#define ADC_OUT_HIGH_LIMIT 1000
#define ADC_OUT_TEMP_HIGH 410 // 58 degree

volatile uint16_t u16AdcInValue;
volatile uint16_t u16AdcOutValue;
volatile bit b_adcInOut;
volatile uint8_t tempConvertResult;
volatile uint8_t tempInputValue;
volatile uint8_t tempOutputValue;

volatile uint8_t machineError;
volatile bit b_relayOnAfter65;
volatile bit b_triac_can_on;
volatile uint8_t triac_can_on_cnt;
volatile uint8_t pump_on_cnt;
volatile uint8_t error_present_cnt;
volatile uint8_t off_heater_flow_reduction_cnt;

volatile uint16_t u16FlowPulse;
volatile uint16_t u16PrevFlowPulse;

#define MAX_TRIAC_PULSE 20
#define MAX_250US_DELAY 2
#define MAX_LEFT_PULSE_ON_CNT (MAX_2MS_CNT - MAX_250US_DELAY)
#define MAX_ADDITION_INTERVAL 10
#define FLOW_PULSE_AVG_INTERVAL 5
#define ADC_INVALUE_AVG_INTERVAL 5

volatile uint8_t triac_pulse_cnt;
volatile uint8_t num_active_pulse;          // power from 0 to 20
volatile uint8_t num_active_pulse_with_add; // from 0 to 20
volatile uint8_t pulse_trigger_off_cnt;
volatile bit b_triggered;
volatile uint8_t num_addition;
volatile uint8_t additionInterval;
volatile uint16_t u16NumActive; // raw number of active pulse * 10
volatile bit b_boost;
volatile bit b_trigger_set_cg;
volatile bit b_trigger_reset_cg;
volatile uint8_t trigger_set_cg_cnt;
volatile uint8_t trigger_reset_cg_cnt;

volatile bit b_test_cg;  // test chong giat
volatile bit b_reset_cg; // reset chong giat
volatile uint8_t test_cg_cnt;
volatile uint8_t reset_cg_cnt;
volatile bit b_off_relay_due_to_cg_trigger;

void tmr3_init(void);

void Timer3_ISR(void) interrupt 16        // Vector @  0x83
{
  _push_(SFRS);
  count2ms++;
  if (count2ms >= MAX_2MS_CNT) // 400 <-> 2ms
  {
    count2ms = 0;
    b_2ms = 1;
    count10ms++;
    count20ms++;
    count100ms++;
    count200ms++;

    if (count10ms >= 5) // 2*5=10ms
    {
      b_10ms = 1;
      count10ms = 0;

      // test chong giat
      if (b_test_cg == 1)
      {
        if (test_cg_cnt < 17)
        {
//          _pc1 = !_pc1;
          test_cg_cnt++;
        }
        else
        {
          b_test_cg = 0;
          test_cg_cnt = 0;
//          _pc1 = 0;
        }
      }

      // reset chong giat
      if (b_reset_cg == 1)
      {
        if (reset_cg_cnt < 30)
        {
          reset_cg_cnt++;
//          _pc2 = 1;
        }
        else
        {
          b_reset_cg = 0;
          reset_cg_cnt = 0;
//          _pc2 = 0;
        }
      }
    }
    if (count20ms >= 10) // 2*10=20ms
    {
      b_20ms = 1;
      count20ms = 0;
      count1s++;

      if (count1s >= 50)
      { // 20ms*50=1s
        b_1s = 1;
        count1s = 0;
        count2s++;
        count3s++;
        count5s++;
        count1min++;

        if (count2s >= 2) // 2 seconds
        {
          count2s = 0;
          b_2s = 1;
        }

        if (count3s >= 3) // 3 seconds
        {
          count3s = 0;
          b_3s = 1;
        }

        if (count5s >= 5) // 5 seconds
        {
          count5s = 0;
          b_5s = 1;
        }

        if (count1min >= 60) // 1 minute counter
        {
          count1min = 0;
          b_1min = 1;
        }
      }
    }
    if (count100ms >= 50)
    { // 100ms
      b_100ms = 1;
      count100ms = 0;
    }
    if (count200ms >= 100)
    { // 200ms
      b_200ms = 1;
      count200ms = 0;
    }
  }

  // control triac off
  count12ms++;
  if (count12ms >= MAX_12MS_CNT)
  {
    count12ms = 0;
  }
  if (b_triggered == 1)
  {
    if (count12ms == pulse_trigger_off_cnt)
    {
//      _pb0 = 1; // off triac
      b_triggered = 0;
    }
  }
	
	clr_T3CON_TF3;
  _pop_(SFRS);
}


void main (void) 
{
		user_UART_software_init();
		tmr3_init();
		P0M1 &= ~(1 << 3);
  	P0M2 |= (1 << 3);  // 0b0001 0000  = 0x10
		
		M_SDA_IO = OUT;
		user_i2c_scl_set_output();
		user_i2c_sda_set_output();
		//	gpio_init_sda();

		user_i2c_scl_output_high();
		user_i2c_sda_output_high();

		FM_tx = 0;
    while(1)
		{
		if (M_SDA_IO == IN)
		{
			M_SDA = user_i2c_sda_get_data();
		}

		if (b_2ms == 1)
		{
			b_2ms = 0;
			master_process();
//			task_2ms_handle();
		}
		// process 100ms task
		if (b_100ms == 1)
		{
			b_100ms = 0;
//			task_100ms_handle();
		}
		// process 200ms task
		if (b_200ms == 1)
		{
			b_200ms = 0;

			if (FM_tx == 0)
			{
				FM_tx = 1;
				cnt_clk = 0;
			}
//			task_200ms_handle();
		}

		// process 1s task
		if (b_1s == 1)
		{
			b_1s = 0;
//			task_1s_handle();
		}


		if (M_SDA_IO == IN)
		{
		}
		else
		{
			if (M_SDA == 1)

				user_i2c_sda_output_high();
			else

				user_i2c_sda_output_low();
		}

		if (M_SCL == 1)
		{
			user_i2c_scl_output_high();
		}
		else
		{
			user_i2c_scl_output_low();
		}
		}
		
}

void tmr3_init(void)
{
	
	/*F = Fsys/16*/
	T3CON |= (1 << 2);
	
	RL3 = (65535 - 124) & 0xff;
	RH3 = (65535 - 124) >> 8;
	
	/*enable interupt tmr3*/
	EIE1 |= (1 << 1);
	
	/*enable interrupt global*/
	IE |= (1 << 7);
	
	/*START*/
	T3CON |= (1 << 3);
}

void master_process(void)
{
	if (FM_tx)
	{
		// CLK output
		M_SCL = !(cnt_clk % 2 == 0);

		// Start and stop bit
		if (cnt_clk == 1 || cnt_clk == 18)
		{
			M_SDA = 1;
		}
		else if (cnt_clk == 0 || cnt_clk == 19)
		{
			M_SDA = 0;
		}

		// Data send
		else if (cnt_clk > 1 && cnt_clk < 18)
		{
			M_SDA = (m_tx_data >> (cnt_clk / 2 - 1)) & 0x01;
		}

		// ACK and change RX mode
		else if (cnt_clk == 20)
		{
			M_SDA_IO = IN;
			user_i2c_sda_set_input();  // them 1
		}

		// Data receiver
		else if (cnt_clk > 20 && cnt_clk < 70)
		{
			if (M_SCL == 1)
				rise_m_sda = M_SDA;
			else
			{
				// Bit 1
				if (rise_m_sda && M_SDA)
				{
					m_rx_data[(cnt_clk - 22) / 16] = m_rx_data[(cnt_clk - 22) / 16] | (0x01 << ((((cnt_clk - 22) / 2) % 8)));
				}
				// Bit 0
				else if (!rise_m_sda && !M_SDA)
				{
				}
				// Error
				else
				{
					// error bit
				}
			}
		}

		//
		if (cnt_clk < 70)
			cnt_clk++;
		else
		{
			FM_rx_done = 1;

			// TODO: process received data
			if (m_rx_data[2] == CRC8(m_rx_data, 2))
			{
				// machineError &= ~(1 << CONNECT_UI_BOARD_ERR_BIT);
				// control temperature using number of active pulse
				// TODO:
				if (m_rx_data[0] <= 26)
				{
					if (m_rx_data[0] == 0)
					{ // set off
						outTemp_ref = 0;
					}
					else
					{
						outTemp_ref = m_rx_data[0] + 29;
					}
				}

				if (m_rx_data[1] == TEST_CG_CMD)
				{
					if (b_trigger_reset_cg == 0)
					{
						b_trigger_set_cg = 1;
						num_active_pulse = 0;
						outTemp_ref = 0;
						machineError |= (1 << TESTING_CB_NOTIFY_BIT);

						// Off pump at test cg mode
						//						PUMP_OFF();
//						user_gpio_output_pump_low();
						m_tx_data &= ~(1 << PUMP_STATUS_BIT);
					}
				}
				else if (m_rx_data[1] == RESET_CG_CMD)
				{
					if (b_trigger_set_cg == 0)
					{
						b_triac_can_on = 0;
						b_trigger_reset_cg = 1;
						b_reset_cg = 1;
						//						_pa3 = 1; // on relay
//						user_gpio_output_outac_high();
						machineError &= ~(1 << TESTING_CB_NOTIFY_BIT);
					}
				}
				else if (m_rx_data[1] == PUMP_ON_CMD)
				{
					// only on pump when NOT in CB mode
					if ((machineError & (1 << TESTING_CB_NOTIFY_BIT)) == 0)
					{
						//						PUMP_ON();
//						user_gpio_output_pump_high();
						pump_on_cnt = 0;
						m_tx_data |= (1 << PUMP_STATUS_BIT);
					}
				}
				else if (m_rx_data[1] == PUMP_OFF_CMD)
				{
					if (pump_on_cnt >= 2)
					{
						//						PUMP_OFF();
//						user_gpio_output_pump_low();
						m_tx_data &= ~(1 << PUMP_STATUS_BIT);
					}
				}
				else
				{
					/* Not occur */
				}
			}
			else
			{
				// Invalid checksum ==> TODO:
				// means connection between two boards has problem
				// machineError |= (1 << CONNECT_UI_BOARD_ERR_BIT);
			}
			user_UART_software_send_string("data1: ",7);
			//			user_usart_string_transmit(USART2,buff1,3);
			user_UART_software_send_number(m_rx_data[0]);
			user_UART_software_send_byte('\n');

			user_UART_software_send_string("data2: ",7);
			user_UART_software_send_number(m_rx_data[1]);
			//			user_usart_string_transmit(USART2,buff2,3);
			user_UART_software_send_byte('\n');
			m_rx_data[0] = 0;
			m_rx_data[1] = 0;
			m_rx_data[2] = 0;
			cnt_clk = 0;
			FM_tx = 0;
			M_SCL = 1;
			M_SDA_IO = OUT;
			user_i2c_sda_set_output(); // them 2
			M_SDA = 1;
		}

	}
}

unsigned char CRC8(volatile unsigned char *Data, unsigned char length)
{
	unsigned char crc = 0x00;
	unsigned char extract;
	unsigned char sum;
	unsigned char i;
	unsigned char tempI;
	
	for (i = 0; i < length; i++)
	{
		extract = *Data;
		for (tempI = 8; tempI; tempI--)
		{
			sum = (crc ^ extract) & 0x01;
			crc >>= 1;
			if (sum)
				crc ^= 0x8C;
			extract >>= 1;
		}
		Data++;
	}
	return crc;
}











