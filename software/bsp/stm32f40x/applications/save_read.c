#include "stm32f4xx.h"
#include "24cxx.h"
#include "typedefs.h"
#include "string.h"
#include "variables.h"

#define program_addr 0
#define para_addr	4096
//static const save_program program_init;
static const program program_init[pwm_num];

void save_para(void)
{
	
	write_bytes(0, &program_data, pwm_num*sizeof(program));
}

uint8_t read_para(void)
{
//	save_program program_buff;
	program program_buff[pwm_num];
	read_bytes(0, &program_buff, pwm_num*sizeof(program));
	
	return (memcmp(&program_data, &program_buff, pwm_num*sizeof(program)) == 0)?0:-1;
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

















