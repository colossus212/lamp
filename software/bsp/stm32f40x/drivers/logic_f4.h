#ifndef __LOGIC_H__
#define __LOGIC_H__


#include "stm32f4xx.h"

#define pwm_ena(x) logic_out(4,x)

typedef struct logic_io
{
	GPIO_TypeDef* GPIOx ;
	uint16_t GPIO_Pin;
}logic_io;

typedef enum logic_name
{
	mcu_run = 0,
	usart3_dir = 5,
}logic_name;	

void logic_init(void);
uint8_t logic_out(uint8_t num, uint8_t value);
uint8_t logic_in(uint8_t num);



















#endif

