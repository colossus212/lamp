#ifndef __MY_MATH_H__
#define __MY_MATH_H__

#include "stm32f4xx.h"

typedef struct linear_interp_table
{
	uint16_t x;
	uint16_t y;
}linear_interp_table;


uint16_t linear_interp_y(linear_interp_table * psource, uint16_t x, uint16_t size);//get Y from x
uint16_t linear_interp_x(linear_interp_table * psource, uint16_t y, uint16_t size);//get x from y
void linear_offset(uint16_t *psrc, uint16_t start, uint16_t end, uint16_t num);
uint16_t find_u16_max(uint16_t *p, uint16_t size);
int16_t find_s16_max(int16_t *p, uint16_t size);
int32_t sum_s16(int16_t *p, uint16_t size);
float Position_PID(float In, float Ref);
#endif
