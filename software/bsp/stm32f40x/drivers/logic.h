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
	/*out*/
	mcu_run 	= 0,
	mcu_err		= 1,
	mcu_trig	= 2,
	mcu_wait	= 3,	
//	pwm_en		= 4,	
	usart3_dir 	= 5,
	eeprom_wp 	= 6,
	red_pin		= 7,
	shutter_pin = 8,//øÏ√≈
	/*in*/
	mkin0 		= 0,
	mkin1		= 1,
	mkin2		= 2,
	mkin3		= 3,
	mkin4		= 4,
	mkin5		= 5,
	mkin6		= 6,
	mkin7		= 7,	
	mkin8		= 8,
	mkin9		= 9,
	mkin10		= 10,
	
}logic_name;	

void logic_init(void);
uint8_t logic_out(uint8_t num, uint8_t value);
uint8_t logic_in(uint8_t num);



















#endif

