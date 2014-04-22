#include "stm32f4xx.h"
#include "rtthread.h"
//#include "stm32adc.h"
#include "arm_math.h"
#include "my_math.h"
#define IGBT_FREQ 20000

typedef struct pwm_struct
{
	uint8_t mode;//power or current mode
	uint16_t current_max;
	uint16_t power_max;
	uint16_t positive_pulse;
	uint16_t negative_pulse;

	uint16_t c_array[501];//the last is 0//50us*500=25ms
	
}pwm_st;



void pwm_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	uint16_t pwm_frequence = 0;
	uint16_t pwm_period = 0;
	
	DBGMCU_APB2PeriphConfig( DBGMCU_TIM8_STOP, ENABLE);//核心停止后终止PWM
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	/*TIM8 CH1*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM8);
	/*TIM8 CH2*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM8); 
	/*TIM8 CH3*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM8);
	/*TIM8 CH4*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_TIM8);
	
	pwm_frequence = IGBT_FREQ;//默认开关频率20KHz
	pwm_period = SystemCoreClock / 2/ 20000 - 1;//计算设置开关频率计数周期
	TIM_TimeBaseStructure.TIM_Period = pwm_period;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
	//设置PWM输出
	
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = pwm_period/2;
//	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
//	TIM_OCInitStructure.TIM_OCNPolarity =  TIM_OCNPolarity_Low;
//	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
//	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OC1Init(TIM8, &TIM_OCInitStructure);
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);
	TIM_OC3Init(TIM8, &TIM_OCInitStructure);
	TIM_OC4Init(TIM8, &TIM_OCInitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_CtrlPWMOutputs(TIM8, ENABLE);//pwm enable
	TIM_Cmd(TIM8, ENABLE);			//tim8 enable
	
	TIM_SelectOutputTrigger(TIM8, TIM_TRGOSource_OC1);
	TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);//interrupt
}

void pid_1(void)
{
	uint16_t i = 0;
}

void pid_2(void)
{
	uint16_t i = 0;
}

void pid_buck(uint16_t I)
{
	uint16_t i = 0;
//	float Position_PID(float In, float Ref);
}

void control_current(uint16_t I1, uint16_t I2)//I1 is real current,I2 is reference current
{
	float percent = 0;
	percent = Position_PID((float) I1, (float) I2);
	percent = percent;
}

void TIM8_UP_TIM13_IRQHandler(void)
{
	rt_interrupt_enter();
//	ADC_SoftwareStartConv(ADC2);
//	ADC_SoftwareStartConv(ADC3);
	if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
	{
//		control_current(0);
	
	}
	rt_interrupt_leave();
}























