#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__



typedef struct pwm_struct
{
	uint8_t mode;//1 = power mode ; 0 = current mode
	uint8_t busy_flag;//1 = busy
	uint16_t current_max;
	uint16_t power_max;
	uint16_t positive_pulse;
	uint16_t negative_pulse;
	
	uint16_t c_array[501];//precision is 0.1A//the last is 0//50us*500=25ms
	uint16_t p_array[501];//precision is 1 W//
}pwm_st;




#endif

