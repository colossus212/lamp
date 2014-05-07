#include "rtthread.h"
#include "finsh.h"
#include "spi_user.h"
#include "logic.h"

#define spi_delay 2000
rt_mutex_t spi1lock;
rt_mutex_t spi2lock;

#define USING_SPI1
#define USING_SPI2

//#define SPI1_RCC	RCC_APB2Periph_SPI1
#define SPI1_PORT 				GPIOA
#define SPI1_SCK 				GPIO_Pin_5 
#define SPI1_SCK_PIN_SOURCE		GPIO_PinSource5
#define SPI1_MISO 				GPIO_Pin_6
#define SPI1_MISO_PIN_SOURCE	GPIO_PinSource6
#define SPI1_MOSI				GPIO_Pin_7
#define SPI1_MOSI_PIN_SOURCE	GPIO_PinSource7

#define SPI2_PORT 				GPIOB
#define SPI2_SCK 				GPIO_Pin_13 
#define SPI2_SCK_PIN_SOURCE		GPIO_PinSource13
#define SPI2_MISO 				GPIO_Pin_14
#define SPI2_MISO_PIN_SOURCE	GPIO_PinSource14
#define SPI2_MOSI 				GPIO_Pin_15
#define SPI2_MOSI_PIN_SOURCE	GPIO_PinSource15

void spi_initialize(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
#ifdef STM32F4XX
	#ifdef USING_SPI1
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		
		GPIO_InitStructure.GPIO_Pin = SPI1_SCK | SPI1_MISO | SPI1_MOSI;
		GPIO_Init(SPI1_PORT, &GPIO_InitStructure);
		
		GPIO_PinAFConfig(SPI1_PORT, SPI1_SCK_PIN_SOURCE, GPIO_AF_SPI1);
		GPIO_PinAFConfig(SPI1_PORT, SPI1_MISO_PIN_SOURCE, GPIO_AF_SPI1);
		GPIO_PinAFConfig(SPI1_PORT, SPI1_MOSI_PIN_SOURCE, GPIO_AF_SPI1);
		
		/* SPI1 configuration */ 
		SPI_StructInit(&SPI_InitStructure);
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //SPI1设置为两线全双工
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	                   //设置SPI1为主模式
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  //SPI发送接收8位帧结构
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;	 		               //串行时钟在不操作时，时钟为高电平
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;		               //第二个时钟沿开始采样数据
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			               //NSS信号由软件（使用SSI位）管理
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64; //定义波特率预分频的值:波特率预分频值为8
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				   //数据传输从MSB位开始
		SPI_InitStructure.SPI_CRCPolynomial = 7;						   //CRC值计算的多项式
		SPI_Init(SPI1, &SPI_InitStructure);
		/* Enable SPI1  */
		SPI_Cmd(SPI1, ENABLE); 
		spi1lock = rt_mutex_create("spi1lock", RT_IPC_FLAG_FIFO);
	#endif

	#ifdef USING_SPI2
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		
		GPIO_InitStructure.GPIO_Pin = SPI2_SCK | SPI2_MISO | SPI2_MOSI;
		GPIO_Init(SPI2_PORT, &GPIO_InitStructure);
		
		GPIO_PinAFConfig(SPI2_PORT, SPI2_SCK_PIN_SOURCE, GPIO_AF_SPI2);
		GPIO_PinAFConfig(SPI2_PORT, SPI2_MISO_PIN_SOURCE, GPIO_AF_SPI2);
		GPIO_PinAFConfig(SPI2_PORT, SPI2_MOSI_PIN_SOURCE, GPIO_AF_SPI2);
		
		/* SPI2 configuration */ 
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //SPI2设置为两线全双工
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	                   //设置SPI2为主模式
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  //SPI发送接收8位帧结构
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;	 		               //串行时钟在不操作时，时钟为高电平
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;		               //第二个时钟沿开始采样数据
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			               //NSS信号由软件（使用SSI位）管理
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32; //定义波特率预分频的值:波特率预分频值为8
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				   //数据传输从MSB位开始
		SPI_InitStructure.SPI_CRCPolynomial = 7;						   //CRC值计算的多项式
		SPI_Init(SPI2, &SPI_InitStructure);
		/* Enable SPI2  */
		SPI_Cmd(SPI2, ENABLE); 	
		spi2lock = rt_mutex_create("spi2lock", RT_IPC_FLAG_FIFO);
	#endif
#endif
	
#ifdef STM32F10X
	#ifdef USING_SPI1
		/* Enable SPI1 and GPIOA clocks */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1|RCC_APB2Periph_AFIO, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOA, ENABLE);
		
		/* Configure SPI1 pins: NSS, SCK, MISO and MOSI */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		//SPI1 NSS 
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_SetBits(GPIOA, GPIO_Pin_4);   

		/* SPI1 configuration */ 
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //SPI1设置为两线全双工
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	                   //设置SPI1为主模式
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  //SPI发送接收8位帧结构
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;	 		               //串行时钟在不操作时，时钟为高电平
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;		               //第二个时钟沿开始采样数据
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			               //NSS信号由软件（使用SSI位）管理
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64; //定义波特率预分频的值:波特率预分频值为8
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				   //数据传输从MSB位开始
		SPI_InitStructure.SPI_CRCPolynomial = 7;						   //CRC值计算的多项式
		SPI_Init(SPI1, &SPI_InitStructure);
		/* Enable SPI1  */
		SPI_Cmd(SPI1, ENABLE); 											  //使能SPI1外设	
		spi1lock = rt_mutex_create("spi1lock", RT_IPC_FLAG_FIFO);
	#endif

	#ifdef USING_SPI2
		/* Enable SPI2 and GPIOA clocks */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB  | RCC_APB2Periph_GPIOC , ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
		
		/* Configure SPI2 pins: NSS, SCK, MISO and MOSI */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 |  GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14 ;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		//SPI2 CS 
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		//	GPIO_SetBits(GPIOB, GPIO_Pin_4);   

		/* SPI2 configuration */ 
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //SPI2设置为两线全双工
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	                   //设置SPI2为主模式
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  //SPI发送接收8位帧结构
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;	 		               //串行时钟在不操作时，时钟为高电平
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;		               //第二个时钟沿开始采样数据
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			               //NSS信号由软件（使用SSI位）管理
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32; //定义波特率预分频的值:波特率预分频值为8
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				   //数据传输从MSB位开始
		SPI_InitStructure.SPI_CRCPolynomial = 7;						   //CRC值计算的多项式
		SPI_Init(SPI2, &SPI_InitStructure);
		/* Enable SPI2  */
		SPI_Cmd(SPI2, ENABLE);
		spi2lock = rt_mutex_create("spi2lock", RT_IPC_FLAG_FIFO);	
	#endif
#endif
}

//SPI1 读写一个字节
//返回值:读取到的字节
uint8_t SPI1_ReadWriteByte(uint8_t TxData)
{		
	uint32_t retry=0;				 
	while((SPI1->SR&(1<<1))==0)//等待发送区空	
	{
		retry++;
		if(retry>spi_delay) 
		{
			rt_kprintf("SPI1 transmitter isn't empty!\n");
			return 0;
		}
	}			  
	SPI1->DR=TxData;	 	  //发送一个byte 
	retry=0;
	while((SPI1->SR&(1<<0))==0) //等待接收完一个byte  
	{
		retry++;
		if(retry > spi_delay)
		{
			rt_kprintf("SPI1 can't recieve data!\n");
			return 0;
		}
	}	  						    
	return SPI1->DR;          //返回收到的数据				    
}

//SPI2 读写一个字节
//返回值:读取到的字节
uint8_t SPI2_ReadWriteByte(uint8_t TxData)
{		
	uint16_t retry=0;				 
	while((SPI2->SR&(1<<1))==0)//等待发送区空	
	{
		retry++;
		if(retry>spi_delay) 
		{
			rt_kprintf("SPI2 transmitter isn't empty!\n");
			return 0;
		}
	}			  
	SPI2->DR=TxData;	 	  //发送一个byte 
	retry=0;
	while((SPI2->SR&(1<<0))==0) //等待接收完一个byte  
	{
		retry++;
		if(retry>spi_delay) 
		{
			rt_kprintf("SPI2 can't recieve data!\n");
			return 0;
		}
	}	  						    
	return SPI2->DR;          //返回收到的数据				    
}





























