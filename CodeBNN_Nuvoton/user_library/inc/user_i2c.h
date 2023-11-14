#ifndef USER_I2C_H_
#define USER_I2C_H_

void user_i2c_scl_set_output(void);
void user_i2c_scl_output_high(void);
void user_i2c_scl_output_low(void);
void user_i2c_scl_tonggle(void);

void user_i2c_sda_set_output(void);
void user_i2c_sda_output_high(void);
void user_i2c_sda_output_low(void);
void user_i2c_sda_set_input(void);

bit user_i2c_sda_get_data(void);


#endif