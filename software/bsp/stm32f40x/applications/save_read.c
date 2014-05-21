#include "stm32f4xx.h"
#include "24cxx.h"
#include "typedefs.h"
#include "string.h"
#include "variables.h"

#define program_addr 0
#define para_addr	4096
//static const save_program program_init;
static const program program_init[pwm_num];

uint8_t para_init(void)
{

}

uint8_t read_para(void)
{
//	save_program program_buff;
	program program_buff[pwm_num];
	read_bytes(0, &program_buff, pwm_num*sizeof(program));
	
	if(memcmp(&program_data, &program_buff, pwm_num*sizeof(program)) == 0)
	{
		
	}
	return 0;
}

void copy_default(void)
{
//	uint16_t i = 0;
//	save_program program_init;
//	read_bytes(4096, &program_init, sizeof(save_program));
	memcpy( &program_data, &program_init, pwm_num*sizeof(program));	
}

//void save_default(void)
//{
//	write_bytes(4096, para, 4096);
//}

void save_scheme(uint8_t num)//save current scheme
{
	if(num < 32)
	{
		memcpy(program_data[num].t, &usSRegHoldBuf[t0_s], 16);
		memcpy( program_data[num].percent, &usSRegHoldBuf[percent0_s], 16);
		program_data[num].CRC16_data = usMBCRC16((UCHAR*)(program_data[num].percent), (sizeof(program)-2));
		write_bytes(sizeof(program)*num, program_data[num].percent, sizeof(program));
	}	
}

char read_scheme(uint8_t num)//read current scheme
{
	if((usMBCRC16((UCHAR*)(program_data[num].percent), sizeof(program)) == 0)&&(num < 32))
	{
		memcpy(&usSRegHoldBuf[t0_s], program_data[num].t , 16);
		memcpy(&usSRegHoldBuf[percent0_s], program_data[num].percent , 16);
		return 0;
	}
	return -1;
}














