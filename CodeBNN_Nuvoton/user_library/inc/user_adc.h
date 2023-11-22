#ifndef USER_ADC_H_
#define USER_ADC_H_


#define ADC_START()         \
  {                         \
    ADCCON0 |= (1 << 6);    \
  }
	
#define ADC_SELECT_TEMP_OUT()           \
  {                                     \
    ADCCON0 &= ~(1 << 3);               \
    ADCCON0 &= ~(1 << 2);               \
    ADCCON0 |= (1 << 1);                \
    ADCCON0 |= (1 << 0);                \
  } // P06 - AN3
#define ADC_SELECT_TEMP_IN()            \
  {                                     \
    ADCCON0 &= ~(1 << 3);               \
    ADCCON0 &= ~(1 << 2);               \
    ADCCON0 |= (1 << 1);                \
    ADCCON0 &= ~(1 << 0);               \
  } // P07 - AN2

void adc_init(void);
#endif
