//#include "logicio_out.h"
#include "stm32f4xx.h"
#include "finsh.h"
#include "logic_f4.h"

#define in_size 4
#define out_size 6

logic_io GPIO_in[in_size] = 
	{{GPIOE,GPIO_Pin_2}, {GPIOE,GPIO_Pin_3}, {GPIOE,GPIO_Pin_4}, {GPIOE,GPIO_Pin_5}};
	
logic_io GPIO_out[out_size] = 
	{{GPIOG,GPIO_Pin_10},{GPIOG,GPIO_Pin_11},{GPIOE,GPIO_Pin_0},{GPIOE,GPIO_Pin_1},
	 {GPIOG,GPIO_Pin_8}, {GPIOE,GPIO_Pin_13}};

void logic_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
	uint8_t i = 0;
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	for(i = 0; i < out_size; i++)
	{
		GPIO_InitStructure.GPIO_Pin   = GPIO_out[i].GPIO_Pin;
		GPIO_Init(GPIO_out[i].GPIOx, &GPIO_InitStructure);
	}
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	for(i = 0; i < in_size; i++)
	{
		GPIO_InitStructure.GPIO_Pin   = GPIO_in[i].GPIO_Pin;
		GPIO_Init(GPIO_in[i].GPIOx, &GPIO_InitStructure);
	}
	
}


uint8_t logic_out(uint8_t num, uint8_t value)
{
	if(num >= out_size)	
	{
		rt_kprintf("para num is err!\n");
		return 255;		
	}
	
	GPIO_WriteBit(GPIO_out[num].GPIOx, GPIO_out[num].GPIO_Pin, (value == 0)?Bit_RESET:Bit_SET);	
	return 0;
}

uint8_t logic_in(uint8_t num)
{	
	if(num >= in_size)	
	{
		rt_kprintf("para num is err!\n");
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















