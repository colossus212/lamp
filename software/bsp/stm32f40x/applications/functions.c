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
	for(k = 0; k < 16; k++)
	{	
			linear_offset( p , program_data[num].percent[k] ,
			program_data[num].percent[k+1], 
			(program_data[num].t[k+1] - program_data[num].t[k])/50);
			p += (program_data[num].t[k+1] - program_data[num].t[k])/50;//50us间隔

	}
	pwm_struct[num].p_array[499] = 0;//数组最后一字节为0；
	
	pwm_struct[num].positive_pulse = 500;
	pwm_struct[num].negative_pulse = 500;
	
}

void calculate_all(void)
{
	uint16_t i = 0;
	for(i = 0; i< pwm_num; i++)
	{
		calculate_array(i);
	}
}

void check_slave_para(uint8_t j)//
{
	uint16_t i = 0;
	
	if(j < pwm_num)//t[k+1] > t[k]
	{
		for(i = 0; i < 16; i++)
		{
			if(program_data[j].t[i+1] < program_data[j].t[i])
			{
				program_data[j].t[i+1] = program_data[j].t[i];
			}			
			if(program_data[j].t[i+1] > time_max)
			{
				program_data[j].t[i+1] = time_max;
			}
		}
	}
}

void check_all_para(void)
{
	uint8_t m = 0;
	for(m = 0; m < pwm_num; m++)
	{
		check_slave_para(m);
	}
}

void Boot_initialization(void)//开机初始化
{
	read_para();
	check_all_para();
	calculate_all();
	
}

void test_array(uint16_t pct, uint16_t t)
{
	uint16_t i = 0;
	program_data[0].percent[1] = pct;//50.0%
	program_data[0].t[1] = 0;
	program_data[0].percent[2] = pct;
	program_data[0].t[2] = t;//5000us
	program_data[0].percent[3] = 0;
	program_data[0].t[3] = t;
//	program_data[0].percent[4] = 0;
//	program_data[0].t[4] = t;
	
	program_data[0].CRC16_data = CRC16((uint8_t *)(program_data[0].percent), (sizeof(program)-2));
	
	check_slave_para(0);
	calculate_array(0);
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(test_array, test_array 0-1000/1000*350A width:0-25000us);
#endif




































