//#include "stm32f10x.h"
#include "stm32f4xx.h"
#include <rtthread.h>
#include "soft_i2c.h"

#define I2C_PORT_RCC	RCC_AHB1Periph_GPIOB	
#define I2C_PORT	GPIOB
#define SCL_IO GPIO_Pin_6
#define SDA_IO GPIO_Pin_7

#define CPU 407//103

#define oneus 60 //72MHz//
#define fiveus 360//72MHz
// #define uint8_t unsigned char 
// #define uint16_t unsigned short

#define SCL(b)	GPIO_WriteBit(I2C_PORT, SCL_IO, (b == 0)?Bit_RESET:Bit_SET)
#define SDA(b)	GPIO_WriteBit(I2C_PORT, SDA_IO, (b == 0)?Bit_RESET:Bit_SET)
#define RSDA()	GPIO_ReadInputDataBit(I2C_PORT, SDA_IO)

rt_mutex_t i2c_lock;

void init_soft_i2c(void)
{	
 	GPIO_InitTypeDef GPIO_InitStructure;
 	
#ifdef STM32F10x
	
	RCC_APB2PeriphClockCmd(I2C_PORT_RCC  , ENABLE);	
	/* gpio init*/
	GPIO_InitStructure.GPIO_Pin = SCL_IO |SDA_IO;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(I2C_PORT, &GPIO_InitStructure);	
#endif
	
#ifdef STM32F4XX
 	RCC_AHB1PeriphClockCmd(I2C_PORT_RCC, ENABLE);
 	/* gpio init*/
	GPIO_StructInit(&GPIO_InitStructure);
 	GPIO_InitStructure.GPIO_Pin = SCL_IO |SDA_IO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//
	GPIO_Init(I2C_PORT, &GPIO_InitStructure);	
#endif
	i2c_lock = rt_mutex_create("I2cLock", RT_IPC_FLAG_FIFO);
//	if(i2c_lock != RT_NULL)
//	{
//		rt_kprintf("init i2c success");
//	}
}


void ee_delay(uint16_t us)
{
	while(us--);
}
/**********************************************************/
void start(void)
 //
{
   SCL(0);
   ee_delay(oneus);
   SDA(1);
   ee_delay(oneus);
   SCL(1);
   ee_delay(oneus);
   SDA(0);
   ee_delay(oneus);
   SCL(0);
   ee_delay(oneus);
}

void send_NACK(void)
{
	SCL(0);
	ee_delay(oneus);
	SDA(1);
	ee_delay(oneus);
	SCL(1);
	ee_delay(oneus);
	SCL(0);
	ee_delay(oneus);
}

void send_ACK(void)
{
	SCL(0);
	ee_delay(oneus);
	SDA(0);
	ee_delay(oneus);
	SCL(1);
	ee_delay(oneus);
	SCL(0);
	ee_delay(oneus);
}
/**********************************************************/
void stop(void)
 // 停止位
{
   SCL(0);
   ee_delay(oneus);
   SDA(0);
   ee_delay(oneus);
   SCL(1);
   ee_delay(oneus);
   SDA(1);
   ee_delay(oneus);
   SCL(0);
   ee_delay(oneus);

}

/**********************************************************/
uint8_t I2C_rx(void)
// 从AT24C02移出数据到MCU
{
   uint8_t i,read_data;
   SDA(1);
   for(i = 0; i < 8; i++)
   {
   	ee_delay(oneus);
    SCL(1);
    read_data <<= 1;
    read_data |= RSDA();
		ee_delay(oneus);
    SCL(0);
		ee_delay(oneus);
   }
    return(read_data);
}

/**********************************************************/
uint8_t I2C_tx(uint8_t write_data) 
// 从MCU移出数据到AT24C02
{
   uint8_t i;
   uint8_t ack_bit;
   for(i = 0; i < 8; i++)   // 循环移入8个位
   {
		SDA((BitAction)(write_data & 0x80));
		ee_delay(oneus);
		SCL(1);
		ee_delay(oneus);
		SCL(0);
		write_data <<= 1;
	}
		SDA(1); //准备读取应答位                    
		ee_delay(oneus);
		SCL(1);
		ee_delay(oneus);
		ack_bit = RSDA(); // 读取应答
		ee_delay(oneus);
		SCL(0);
		ee_delay(oneus);		 
		return ack_bit;            // 返回AT24C02应答位
}

	
	
	
	
