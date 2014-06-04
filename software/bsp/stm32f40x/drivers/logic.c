//#include "logicio_out.h"
#include "logic.h"
#include "stm32f4xx.h"
#include "finsh.h"
#include "color.h"

#define in_size 4
#define out_size 7

rt_event_t exit_event;

logic_io GPIO_in[in_size] = 
	{{GPIOE,GPIO_Pin_2}, {GPIOE,GPIO_Pin_3}, {GPIOE,GPIO_Pin_4}, {GPIOE,GPIO_Pin_5}};
	
logic_io GPIO_out[out_size] = 
	{{GPIOG,GPIO_Pin_10},{GPIOG,GPIO_Pin_11},{GPIOE,GPIO_Pin_0},{GPIOE,GPIO_Pin_1},
	 {GPIOG,GPIO_Pin_8}, {GPIOE,GPIO_Pin_13},{GPIOB,GPIO_Pin_5}};//¸÷½Å¹¦¼ûLOGIC.H

void logic_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	uint8_t i = 0;
	uint16_t jj = 0xffff;
/************************************** stm32f4xx *****************************************/
#ifdef STM32F4XX
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/*************************** GPIO OUT *****************************/	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	for(i = 0; i < out_size; i++)
	{
		GPIO_InitStructure.GPIO_Pin   = GPIO_out[i].GPIO_Pin;
		GPIO_Init(GPIO_out[i].GPIOx, &GPIO_InitStructure);
	}
	
	/*************************** GPIO IN ******************************/	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	for(i = 0; i < in_size; i++)
	{
		GPIO_InitStructure.GPIO_Pin   = GPIO_in[i].GPIO_Pin;
		GPIO_Init(GPIO_in[i].GPIOx, &GPIO_InitStructure);
		
	}
	while(jj--);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource5);
	
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_Init(&EXTI_InitStructure);
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_Init(&EXTI_InitStructure);
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_Init(&NVIC_InitStructure);

#endif
	
/*********************************** stm32f10x  ***************************************/
#ifdef STM32F10X_HD	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	for(i = 0; i < out_size; i++)
	{
		GPIO_InitStructure.GPIO_Pin   = GPIO_out[i].GPIO_Pin;
		GPIO_Init(GPIO_out[i].GPIOx, &GPIO_InitStructure);
	}
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	for(i = 0; i < in_size; i++)
	{
		GPIO_InitStructure.GPIO_Pin   = GPIO_in[i].GPIO_Pin;
		GPIO_Init(GPIO_in[i].GPIOx, &GPIO_InitStructure);
	}
#endif	
	exit_event = rt_event_create("exit_event",  RT_IPC_FLAG_FIFO);
}


uint8_t logic_out(uint8_t num, uint8_t value)
{
	if(num >= out_size)	
	{
		rt_kprintf(red"para num is err!\n"no_color);
		return 255;		
	}
	
	GPIO_WriteBit(GPIO_out[num].GPIOx, GPIO_out[num].GPIO_Pin, (value == 0)?Bit_RESET:Bit_SET);	
	return 0;
}

uint8_t logic_in(uint8_t num)
{	
	if(num >= in_size)	
	{
		rt_kprintf(red"para num is err!\n"no_color);
		return 255;		
	}
	
//	rt_kprintf("io%d is %d\n",num,GPIO_ReadInputDataBit(GPIO_in[num].GPIOx, GPIO_in[num].GPIO_Pin));	
	return GPIO_ReadInputDataBit(GPIO_in[num].GPIOx, GPIO_in[num].GPIO_Pin);
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(logic_init, logic io init);
FINSH_FUNCTION_EXPORT(logic_out, logic io out);
FINSH_FUNCTION_EXPORT(logic_in, logic io in);
#endif

void EXTI0_IRQHandler(void)
{
	rt_interrupt_enter();
	
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
	{	
		
		EXTI_ClearITPendingBit(EXTI_Line0);
	}

	rt_interrupt_leave();
}

void EXTI1_IRQHandler(void)
{
	rt_interrupt_enter();
	
	if(EXTI_GetITStatus(EXTI_Line1) != RESET)
	{	
		
		EXTI_ClearITPendingBit(EXTI_Line1);
	}

	rt_interrupt_leave();
}

void EXTI2_IRQHandler(void)
{
	rt_interrupt_enter();
	logic_out(3,1);
	if(EXTI_GetITStatus(EXTI_Line2) != RESET)
	{	
		
		EXTI_ClearITPendingBit(EXTI_Line2);
	}

	rt_interrupt_leave();
}

void EXTI3_IRQHandler(void)
{
	rt_interrupt_enter();
	logic_out(3,1);
	if(EXTI_GetITStatus(EXTI_Line3) != RESET)
	{	
		
		EXTI_ClearITPendingBit(EXTI_Line3);
	}

	rt_interrupt_leave();
}

void EXTI4_IRQHandler(void)
{
	rt_interrupt_enter();
	logic_out(3,1);
	if(EXTI_GetITStatus(EXTI_Line4) != RESET)
	{	
		
		EXTI_ClearITPendingBit(EXTI_Line4);
	}

	rt_interrupt_leave();
}

void EXTI9_5_IRQHandler(void)
{
	rt_interrupt_enter();
	logic_out(3,1);
	EXTI_ClearITPendingBit(EXTI_Line6);
	EXTI_ClearITPendingBit(EXTI_Line7);
	EXTI_ClearITPendingBit(EXTI_Line8);
	EXTI_ClearITPendingBit(EXTI_Line9);
	if(EXTI_GetITStatus(EXTI_Line5) != RESET)
	{	
		
		EXTI_ClearITPendingBit(EXTI_Line5);
	}

	rt_interrupt_leave();
}

void EXTI15_10_IRQHandler(void)
{
	rt_interrupt_enter();
	logic_out(3,1);
	EXTI_ClearITPendingBit(EXTI_Line10);
	EXTI_ClearITPendingBit(EXTI_Line11);
	EXTI_ClearITPendingBit(EXTI_Line12);
	EXTI_ClearITPendingBit(EXTI_Line13);
	EXTI_ClearITPendingBit(EXTI_Line14);
	if(EXTI_GetITStatus(EXTI_Line15) != RESET)
	{		
		EXTI_ClearITPendingBit(EXTI_Line15);
	}

	rt_interrupt_leave();
}


