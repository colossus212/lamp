#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#define array_num (25000/50)//25ms/50us一个点
#define pwm_num		(32)//32组
#define time_max	25000

typedef struct pwm_struct
{
	uint8_t mode;//1 = power mode ; 0 = current mode
	uint8_t busy_flag;//1 = busy
	uint16_t current_max;
	uint16_t power_max;
	uint16_t positive_pulse;
	uint16_t negative_pulse;
	
	uint16_t p_array[array_num];//precision is 0.1%//the last is 0//50us*500=25ms
}pwm_st;

typedef struct program
{
	uint16_t percent[17];	//percent[0] 始终为0
	uint16_t t[17];			//t[0] = 0;
	uint16_t CRC16_data;
}program;

//typedef struct save_program
//{
//	program program_data[32];
//	uint16_t CRC16_data;
//}save_program;

typedef enum master_reg
{
	percent1_m 		= 0,
	
}master_reg;

typedef enum master_coil
{
	cpu_self_m		= 0,
	
}master_coil;

typedef enum slave_reg
{
	percent0_s 		= 0,
	percent1_s 		= 1,
	percent2_s 		= 2,
	percent3_s 		= 3,
	percent4_s 		= 4,
	percent5_s 		= 5,
	percent6_s 		= 6,
	percent7_s 		= 7,
	percent8_s 		= 8,
	percent9_s 		= 9,
	percent10_s 	= 10,
	percent11_s 	= 11,
	percent12_s 	= 12,
	percent13_s 	= 13,
	percent14_s 	= 14,
	percent15_s 	= 15,
	percent16_s 	= 16,
	
	t0_s			= 20,//
	t1_s			= 21,//unit 1us
	t2_s			= 22,
	t3_s			= 23,
	t4_s			= 24,
	t5_s			= 25,
	t6_s			= 26,
	t7_s			= 27,
	t8_s			= 28,
	t9_s			= 29,
	t10_s			= 30,
	t11_s			= 31,
	t12_s			= 32,
	t13_s			= 33,
	t14_s			= 34,
	t15_s			= 35,
	t16_s			= 36,
	
	pow_peak_s		= 40,
	energy_max_s	= 41,
	energy_min_s	= 42,
	num_s			= 43,
	frq_s			= 44,
	energy_single_s	= 45,
	temp_s			= 46,
	current_max_s	= 47,
	total_num_low_s	= 48,
	total_num_high_s= 49,
	
	
}slave_reg;

typedef enum slave_coil
{
	cpu_self_s 		= 0,
	power_self_s	= 1,
	r_self_s		= 2,
	flux_self_s		= 3,
	tc_self_s		= 4,
	key_s			= 5,
	permission_s	= 6,
	cc_mode_s		= 7,
	
}slave_coil;

#endif

