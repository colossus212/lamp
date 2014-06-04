#include "stm32f4xx.h"
#include "typedefs.h"
#include "my_math.h"
#include "variables.h"

void calculate_array(uint8_t num)
{ 
	uint16_t k = 0;
	uint16_t *p ;
	
	rt_memset(pwm_struct[num].p_array, 0, 500);//将数组清0
	p = pwm_struct[num].p_array;
	
	linear_offset( p , 0,
		program_data[num].percent[0], 
		program_data[num].t[0]*2);
	p += program_data[num].t[0]*2;
	for(k = 0; k < 15; k++)//0-14
	{	
		linear_offset( p , program_data[num].percent[k] ,
		program_data[num].percent[k+1], 
		(program_data[num].t[k+1] - program_data[num].t[k])*2);
		p += (program_data[num].t[k+1] - program_data[num].t[k])*2;//50us间隔

	}
	pwm_struct[num].p_array[499] = 0;//数组最后一字节为0；
	
	pwm_struct[num].positive_pulse = 500;
	pwm_struct[num].negative_pulse = 10;
	
}

void print_array(uint8_t num)
{
	uint16_t i  = 0;
	if (num > 0) num = num - 1;
	for(i = 0; i < 500; i++)
	{
		rt_kprintf("array[%d] = %d\n",i, pwm_struct[num].p_array[i]);
	}
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(print_array, print_array);
#endif
//void calculate_all(void)
//{
//	uint16_t i = 0;
//	for(i = 0; i< pwm_num; i++)
//	{
//		calculate_array(i);
//	}
//}



void check_disp_para(void)//
{
	uint16_t i = 0;
	
	for(i = 0; i < 15; i++)//0-14
	{
		if(usSRegHoldBuf[t0_s+i] > time_max)
		{
			usSRegHoldBuf[t0_s+i] = time_max;
		}
		
		if(usSRegHoldBuf[t0_s+i+1] < usSRegHoldBuf[t0_s+i])
		{
			usSRegHoldBuf[t0_s+i+1] = usSRegHoldBuf[t0_s+i];
		}			
	}
//	if(usSRegHoldBuf[t0_s] != 0)
//	{
//		usSRegHoldBuf[t0_s] = 0;//时间起点必须为0
//	}
	if(usSRegHoldBuf[t15_s] > time_max)
	{
		usSRegHoldBuf[t15_s] = time_max;
	}
}

void test_array(uint16_t pct, uint16_t t)
{
	uint16_t i = 0;
	for(i = 0; i < 16; i++)
	{
		program_data[0].percent[i] = 0;
		program_data[0].t[i] = 0;
	}
	program_data[0].percent[1] = pct;//50.0%
	program_data[0].t[1] = 0;
	program_data[0].percent[2] = pct;
	program_data[0].t[2] = t;//5000us
	program_data[0].percent[3] = 0;
	program_data[0].t[3] = t;
//	program_data[0].percent[4] = 0;
//	program_data[0].t[4] = t;
	
//	program_data[0].CRC16_data = CRC16((uint8_t *)(program_data[0].percent), (sizeof(program)-2));
	
	calculate_array(0);
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(test_array, test_array 0-1000/1000*350A width:0-25000us);
#endif


































