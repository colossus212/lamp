#ifndef __SPI_USER_
#define __SPI_USER_

#include "rtthread.h"
#include "stm32f4xx.h"
extern rt_mutex_t spi1lock;
extern rt_mutex_t spi2lock;

#define spi1_lock()    rt_mutex_take(spi1lock, RT_WAITING_FOREVER);
#define spi1_unlock()  rt_mutex_release(spi1lock);
#define spi2_lock()    rt_mutex_take(spi2lock, RT_WAITING_FOREVER);
#define spi2_unlock()  rt_mutex_release(spi2lock);

void spi_initialize(void);
uint8_t SPI1_ReadWriteByte(uint8_t TxData);
uint8_t SPI2_ReadWriteByte(uint8_t TxData);

#endif


