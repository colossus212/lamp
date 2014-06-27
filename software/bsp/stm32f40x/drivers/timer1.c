#include "stm32f4xx.h"
#include "rtthread.h"
#include "finsh.h"

void tim1_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
	
//	DBGMCU_APB2PeriphConfig( DBGMCU_TIM1_STOP, ENABLE);//∫À–ƒÕ£÷π∫Û÷’÷πPWM
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);////APB2 max frquency is 84M
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	/*TIM1 CH1*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_TIM1);
	
	
	TIM_TimeBaseStructInit( &TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = 8399;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	//…Ë÷√PWM ‰≥ˆ	
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 4200;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity =  TIM_OCNPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	
	TIM_CtrlPWMOutputs(TIM1, ENABLE);//pwm enable
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(tim1_init, tim1_init);
#endif
void tim1_start(void)
{
	TIM_Cmd(TIM1, ENABLE);
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(tim1_start, tim1_start);
#endif
void tim1_stop(void)
{
	TIM_Cmd(TIM1, DISABLE);
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(tim1_stop, tim1_stop);
#endif

















