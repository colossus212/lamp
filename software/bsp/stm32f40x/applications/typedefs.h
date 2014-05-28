#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#define array_num (25000/50)//25ms/50us一个点
#define pwm_num		(32)//32组
#define time_max	25000

#define PIN_LASER		GPIO_Pin_6
#define PIN_RED			GPIO_Pin_5
#define PIN_MSEL0		GPIO_Pin_4
#define PIN_MSEL1		GPIO_Pin_3
#define PIN_MSEL2		GPIO_Pin_2

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
	uint16_t percent[16];	//
	uint16_t t[16];			//
	uint16_t CRC16_data;
}program;

//typedef struct save_program
//{
//	program program_data[32];
//	uint16_t CRC16_data;
//}save_program;
//typedef enum PLC
//{
//	Y0 		= 0,
//	Y1 		= 1,
//	Y2 		= 2,
//	Y3 		= 3,
//	Y4 		= 4,
//	Y5 		= 5,
//	Y6 		= 6,
//	Y7 		= 7,
//	Y8 		= 8,
//	Y9 		= 9,
//	Y10		= 10,
//	Y11		= 11,
//	Y12		= 12,
//	Y13		= 13,
//	Y14		= 14,
//	Y15		= 15,
//	
//	X0		= 1000,
//	X1		= 1001,
//	X2		= 1002,
//	X3		= 1003,
//	X4		= 1004,
//	X5		= 1005,
//	X6		= 1006,
//	X7		= 1007,
//	X8		= 1008,
//	X9		= 1009,
//	X10		= 1010,
//	X11		= 1011,
//	X12		= 1012,
//	X13		= 1013,
//	X14		= 1014,
//	X15		= 1015,
//	X16		= 1016,
//	X17		= 1017,
//	X18		= 1018,
//	X19		= 1019,
//	X20		= 1020,
//	X21		= 1021,
//	X22		= 1022,
//	X23		= 1023,
//	
//}PLC;

typedef enum master_reg
{
	percent1_m 		= 0,
	
}master_reg;

typedef enum master_coil
{
	Y0 		= 0,
	Y1 		= 1,
	Y2 		= 2,
	Y3 		= 3,
	Y4 		= 4,
	Y5 		= 5,
	Y6 		= 6,
	Y7 		= 7,
	Y8 		= 8,
	Y9 		= 9,
	Y10		= 10,
	Y11		= 11,
	Y12		= 12,
	Y13		= 13,
	Y14		= 14,
	Y15		= 15,
	
	X0		= 1000,
	X1		= 1001,
	X2		= 1002,
	X3		= 1003,
	X4		= 1004,
	X5		= 1005,
	X6		= 1006,
	X7		= 1007,
	X8		= 1008,
	X9		= 1009,
	X10		= 1010,
	X11		= 1011,
	X12		= 1012,
	X13		= 1013,
	X14		= 1014,
	X15		= 1015,
	X16		= 1016,
	X17		= 1017,
	X18		= 1018,
	X19		= 1019,
	X20		= 1020,
	X21		= 1021,
	X22		= 1022,
	X23		= 1023,
	
}master_coil;

typedef enum slave_reg
{
	t0_s			= (10-1),//
	t1_s			= (11-1),//unit 1us
	t2_s			= (12-1),
	t3_s			= (13-1),
	t4_s			= (14-1),
	t5_s			= (15-1),
	t6_s			= (16-1),
	t7_s			= (17-1),
	t8_s			= (18-1),
	t9_s			= (19-1),
	t10_s			= (20-1),
	t11_s			= (21-1),
	t12_s			= (22-1),
	t13_s			= (23-1),
	t14_s			= (24-1),
	t15_s			= (25-1),
//	t16_s			= (26-1),
	
	percent0_s 		= (30-1),
	percent1_s 		= (31-1),
	percent2_s 		= (32-1),
	percent3_s 		= (33-1),
	percent4_s 		= (34-1),
	percent5_s 		= (35-1),
	percent6_s 		= (36-1),
	percent7_s 		= (37-1),
	percent8_s 		= (38-1),
	percent9_s 		= (39-1),
	percent10_s 	= (40-1),
	percent11_s 	= (41-1),
	percent12_s 	= (42-1),
	percent13_s 	= (43-1),
	percent14_s 	= (44-1),
	percent15_s 	= (45-1),
//	percent16_s 	= 16,
	
	frq_max			= (50-1),
	frq_set			= (51-1),
	current_peak	= (52-1),
	wave_sel		= (53-1),
	energy_pulse	= (54-1),
	water_temp		= (55-1),
	vol_cap			= (56-1),
	total_num_low_s	= (57-1),
	total_num_high_s= (58-1),
	
	shutter			= (59-1),
	tr_mode			= (60-1),//trigger_mode
	power_mean		= (61-1),
	power_peak		= (62-1),
	vol_bat			= (63-1),
	
	AIN0			= (70-1),
	AIN1			= (71-1),
	AIN2			= (72-1),
	AIN3			= (73-1),
	AIN4			= (74-1),
	AIN5			= (75-1),
	AIN6			= (76-1),
	AIN7			= (77-1),
	VBAT			= (78-1),
	TCHIP			= (79-1),
	DAC0			= (80-1),
	DAC1			= (81-1),
	
}slave_reg;

typedef enum slave_coil
{
//	cpu_self_s 		= 0,
//	power_self_s	= 1,
//	r_self_s		= 2,
//	flux_self_s		= 3,
//	tc_self_s		= 4,
//	key_s			= 5,
//	permission_s	= 6,
//	cc_mode_s		= 7,
	standby_s		= (10-1),
	read_s			= (11-1),
	save_s			= (12-1),
	feedback		= (13-1),
	start_up_s		= (14-1),
	laser_sw		= (15-1),
	red_sw			= (16-1),
	laser_on_ena	= (17-1),
	laser_off_ena	= (18-1),
	red_on_ena		= (19-1),
	red_off_ena		= (20-1),
	
	ready_ena		= (21-1),
	stand_by_ena	= (22-1),
	
	ext_ena			= (23-1),
	int_ena			= (24-1),
	
	io_pg2			= (25-1),
	io_pd8			= (30-1),
	io_pc4			= (38-1),
	
	miss_phase		= (40-1),
	IGBT_t_alarm	= (41-1),
	water_t_alarm	= (42-1),//水温报警
	water_f_alarm	= (43-1),//流量报警
	water_l_alarm	= (44-1),//水位报警
	supply_alarm	= (45-1),//电源报警
	soft_start		= (46-1),
	charge_sta		= (47-1),
	discharge_sta	= (48-1),
	lamp1_sta		= (49-1),
	lamp2_sta		= (50-1),
	interlock_sta	= (51-1),
	water_r_alarm	= (52-1),//水质报警
	key_switch		= (53-1),
	emergency_sw	= (54-1),
	main_supply_sw	= (55-1),
	ignition_sw		= (56-1),//点火开关
	pre_burn_fail	= (57-1),
	red_sta			= (58-1),//红光状态
	contactor_err	= (59-1),
	
	x0_s			= (100-1),
	x8_s			= (108-1),
	x16_s			= (116-1),
	y0_s			= (130-1),
	y8_s			= (138-1),
	 
	
}slave_coil;

#endif

