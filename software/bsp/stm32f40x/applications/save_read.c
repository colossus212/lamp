#include "stm32f4xx.h"
#include "24cxx.h"

uint16_t para[4096] = {0};


void save_para(void)
{
	 write_bytes(0, para, 4096);
}

void read_para(void)
{
	read_bytes(0, para, 4096);
}

void read_default(void)
{
	read_bytes(4096, para, 4096);
}

void save_default(void)
{
	write_bytes(4096, para, 4096);
}

















