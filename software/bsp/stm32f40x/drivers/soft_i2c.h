#ifndef __SOFT_I2C_H__
#define __SOFT_I2C_H__

//#include <intrins.h>
#include "stdint.h"
#include "rtthread.h"


#define LM92_READ  0X91
#define LM92_WRITE 0X90
#define BMP085_READ		0XEF
#define BMP085_WRITE	0XEE
#define BQ32000_READ	0xD1
#define BQ32000_WRITE	0XD0

extern rt_mutex_t i2c_lock;
#define i2c_lock()    rt_mutex_take(i2c_lock, RT_WAITING_FOREVER);
#define i2c_unlock()  rt_mutex_release(i2c_lock);

void init_soft_i2c(void);
void start(void);
void send_NACK(void);
void send_ACK(void);
void stop(void);
uint8_t I2C_rx(void);
uint8_t I2C_tx(uint8_t write_data);
void ee_delay(uint16_t us);

#endif
