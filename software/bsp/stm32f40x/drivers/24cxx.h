#ifndef __24CXX_H__
#define __24CXX_H__


#include "stm32f10x.h"



uint8_t write_bytes(uint16_t addr,void *write_data,uint8_t num);
uint8_t read_bytes(uint16_t addr,void *read_data,uint8_t num);




















#endif

