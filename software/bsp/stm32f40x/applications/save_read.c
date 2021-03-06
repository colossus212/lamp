#include "stm32f4xx.h"
#include "24cxx.h"
#include "typedefs.h"
#include "string.h"
#include "variables.h"

#define program_addr 0
#define para_addr	4096
//static const save_program program_init;
static const program program_init[pwm_num];



//uint8_t read_para(void)
//{
////	save_program program_buff;
//	program program_buff[pwm_num];
//	read_bytes(0, &program_buff, pwm_num*sizeof(program));
//	
//	if(memcmp(&program_data, &program_buff, pwm_num*sizeof(program)) == 0)
//	{
//		
//	}
//	return 0;
//}

void copy_default(void)
{
	memcpy( &program_data, &program_init, pwm_num*sizeof(program));	
}

void save_scheme(uint16_t num)//save current scheme
{
//	if(num > 0) num = num - 1;
	if(num < scheme_max)
	{
		memcpy(program_data[num].t, &usSRegHoldBuf[t0_s], 32);
		memcpy( program_data[num].percent, &usSRegHoldBuf[percent0_s], 32);
		program_data[num].feedback_mode = xMBUtilGetBits(ucSCoilBuf, feedback, 1 );
		program_data[num].current_max = usSRegHoldBuf[current_peak];
		program_data[num].max_frq = usSRegHoldBuf[frq_max];
		program_data[num].CRC16_data = usMBCRC16((UCHAR*)(program_data[num].percent), (sizeof(program)-2));
		write_bytes(sizeof(program)*num, program_data[num].percent, sizeof(program));
	}	
}

char read_scheme(uint16_t num)//read current scheme
{
	if(num >= scheme_max ) num = scheme_max - 1;
	if((usMBCRC16((UCHAR*)(program_data[num].percent), sizeof(program)) == 0)&&(num < scheme_max))
	{
		memcpy(&usSRegHoldBuf[t0_s], program_data[num].t , 32);
		memcpy(&usSRegHoldBuf[percent0_s], program_data[num].percent , 32);
		usSRegHoldBuf[frq_max] = program_data[num].max_frq;
		usSRegHoldBuf[current_peak] = program_data[num].current_max;
		xMBUtilSetBits( ucSCoilBuf, feedback, 1, program_data[num].feedback_mode );
		return 0;
	}
	return 1;
}
program program_buf;
uint8_t para_init(void)
{
	uint8_t i = 0;

	for(i = 0; i< scheme_max; i++)
	{
		if(usMBCRC16((UCHAR*)(program_data[i].percent), (sizeof(program))) != 0)
		{
			read_bytes(sizeof(program)*i, program_buf.percent, sizeof(program));
			if(usMBCRC16((UCHAR*)(program_buf.percent), sizeof(program)) == 0)
			{
				memcpy(&program_data[i].percent, program_buf.percent , sizeof(program));
			}
			else
			{
				memset(program_data[i].t, 0, 32);
				memset( program_data[i].percent, 0, 32);
				program_data[i].feedback_mode = 0;
				program_data[i].max_frq = 1000;
				program_data[i].current_max = 100;
				program_data[i].CRC16_data = usMBCRC16((UCHAR*)(program_data[i].percent), (sizeof(program)-2));
				write_bytes(sizeof(program)*i, program_data[i].percent, sizeof(program));
			}
			goto deal_loop;
		}
		else
		{
			deal_loop:
			calculate_array(i);
			read_bytes(sizeof(program)*i, program_buf.percent, sizeof(program));
			if(usMBCRC16((UCHAR*)(program_buf.percent), sizeof(program)) != 0)
			{
				write_bytes(sizeof(program)*i, program_data[i].percent, sizeof(program));
			}
		}
	}
	if(usSRegHoldBuf[wave_sel] > 0)
		wave = usSRegHoldBuf[wave_sel] - 1;
	read_scheme(wave);
	return 0;
}












