#ifndef __24CXX_H__
#define __24CXX_H__


#include "stm32f4Xx.h"



uint8_t write_bytes(uint16_t addr,void *write_data,uint16_t num);
uint8_t read_bytes(uint16_t addr,void *read_data,uint16_t num);




















#endif

