#include "stm32f4xx.h"
#include "typedefs.h"
#include "my_math.h"
#include "variables.h"

uint16_t array_buf[array_num] = {0};

void calculate_array(uint8_t num)
{ 
	uint16_t k = 0;
	uint16_t *p ;
	float sum = 0.0f;
	if(num >= scheme_max ) num = scheme_max - 1;
	
	rt_memset(array_buf, 0, array_num*2);//将数组清0
	p = array_buf;
	
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
	for(k = 0; k <array_num; k++ )
	{
		pwm_struct[num].p_array[k] = (float)array_buf[k]/1000; 
	}
	
	pwm_struct[num].p_array[499] = 0.0f;//数组最后一字节为0；
	
	for(k = 499; k > 0; k--)
	{
		if(pwm_struct[num].p_array[k] > 0.0f)
		{
			pwm_struct[num].positive_pulse = k+1;
			break;
		}
//		pwm_struct[num].positive_pulse = 500;
	}
	
//	pwm_struct[num].negative_pulse = 20;
	for(k = 0; k < 500; k++)
	{
		sum += pwm_struct[num].p_array[k] * program_data[num].current_max ;//usSRegHoldBuf[current_peak];
	}
	pwm_struct[num].negative_pulse = (uint16_t)(sum/12.5f) + 20;
	usSRegHoldBuf[frq_max] = 200000/(pwm_struct[num].negative_pulse + pwm_struct[num].positive_pulse);
	if(usSRegHoldBuf[frq_max] >1000)
	{
		usSRegHoldBuf[frq_max] = 1000;
	}
	pwm_struct[num].negative_pulse = 200000/usSRegHoldBuf[frq_max] - pwm_struct[num].positive_pulse;
	pwm_struct[num].mode = program_data[num].feedback_mode; 
}

void print_array(uint8_t num)
{
	uint16_t i  = 0;
	if (num > 0) num = num - 1;
	for(i = 0; i < 500; i++)
	{
		rt_kprintf("array[%d] = %d\n",i, (uint16_t)(pwm_struct[num].p_array[i]*100));
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


































