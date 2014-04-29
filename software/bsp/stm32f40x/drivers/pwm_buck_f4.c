#include "stm32f4xx.h"
#include "rtthread.h"
#include "finsh.h"
//#include "stm32adc.h"
#include "arm_math.h"
#include "my_math.h"
#include "logic_f4.h"

#define IGBT_FREQ 20000
#define pwm_period (SystemCoreClock / IGBT_FREQ - 1)

typedef struct pwm_struct
{
	uint8_t mode;//1 = power mode ; 0 = current mode
	uint8_t busy_flag;//1 = busy
	uint16_t current_max;
	uint16_t power_max;
	uint16_t positive_pulse;
	uint16_t negative_pulse;
	
	uint16_t c_array[501];//precision is 0.1A//the last is 0//50us*500=25ms
	
}pwm_st;
float pid_out[500] = {0};

pwm_st buck_part;
	
void pwm_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
	
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
	
//	pwm_period = SystemCoreClock / IGBT_FREQ - 1;//计算设置开关频率计数周期
	
	TIM_TimeBaseStructInit( &TIM_TimeBaseStructure);
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
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity =  TIM_OCNPolarity_Low;
//	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
//	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OC1Init(TIM8, &TIM_OCInitStructure);
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);
	TIM_OC3Init(TIM8, &TIM_OCInitStructure);
	TIM_OC4Init(TIM8, &TIM_OCInitStructure);
	
//	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
//	TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
//	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
//	TIM_BDTRInitStructure.TIM_DeadTime = 5;
//	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
//	TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
//	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
//	TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);//interrupt
	TIM_SelectOutputTrigger(TIM8, TIM_TRGOSource_OC1);
	TIM_CtrlPWMOutputs(TIM8, ENABLE);//pwm enable
//	TIM_Cmd(TIM8, ENABLE);			//tim8 enable

}

void pid_1(void)
{
	uint16_t i = 0;
}

void pid_2(void)
{
	uint16_t i = 0;
}

void cal_array(uint16_t I, uint16_t width)
{
	uint16_t i = 0;
	if(I > 3500) I = 3500;
	for(i = 0; i < 500; i++)
	{
		buck_part.c_array[i] = I;
	}
	buck_part.positive_pulse = width/50;
	if(buck_part.positive_pulse >= 500) buck_part.positive_pulse = 499;
	buck_part.negative_pulse = 500;
	
//	pwm_ena(1);
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(cal_array, cal_array 0-3500*0.1A width:0-25000us);
#endif

float	Kp3 = 0.07f, Ki3 = 0.0f, Kd3 = 0.0f;
//float max = 1000, min = 200;
float_t pid3(float In, uint16_t Ref)
{
	static float PrevError_C3 = 0, IntTerm_C3 = 0/*sum of integral*/;
	float Error = 0, Output = 0, R = 0;

	R = (float)(Ref/10);
	Error = R - In;
	
	IntTerm_C3 += Ki3*Error;//IntTerm_C = Ki*(e(0) + e(1) + ... + e(k))
	Output = Kp3 * Error;
//	test_1 = IntTerm_C;
//	if(IntTerm_C3 > (double)max)
//	{
//		IntTerm_C3 = (double)max;
//	}
//	if(IntTerm_C3 < (double)min)
//	{
//		IntTerm_C3 = (double)min;
//	}
  Output += IntTerm_C3;
  Output += Kd3 * (Error - PrevError_C3);

  PrevError_C3 = Error;

  return (Output);//u(k)
}

void control_power(uint16_t I1, pwm_st part)
{
	
}

void start_tim8(void)
{
	TIM_Cmd(TIM8, ENABLE);
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(start_tim8, enable tim8);
#endif
void control_current(uint16_t I1, pwm_st part)//I1 is real current,I2 is reference current
{
	float percent = 0;
	static uint16_t interrupt_times = 0;
//	logic_out(1,1);
	
	if(interrupt_times < part.positive_pulse)
	{
		part.busy_flag = 1;
		interrupt_times++;
//		if(part.c_array[interrupt_times] == 0)
//		{
//			percent = 0;
//		}
//		else
		{
			pwm_ena(1);
			percent = pid3((float) I1, part.c_array[interrupt_times]);
			pid_out[interrupt_times] = percent;
			if(percent > 0.95f) percent = 0.95f;
			if(percent < 0.0f) percent = 0;
		}
	}
	else
	{
		interrupt_times++;
		percent = 0;
		if( interrupt_times > part.positive_pulse + part.negative_pulse)
		{
			TIM_Cmd(TIM8, DISABLE);
			pwm_ena(0);
			interrupt_times = 0;
			part.busy_flag = 0;
		}
	}
	
	TIM_SetCompare1(TIM8, (uint16_t)(float)(percent * pwm_period));

//	logic_out(1,0);
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(control_current, control_current i1 i2);
#endif

extern void trig_adc(void);
extern uint16_t adc_get(uint8_t ch);
void TIM8_UP_TIM13_IRQHandler(void)
{
	uint16_t I1 = 0, I2 = 0, power = 0;
	rt_interrupt_enter();
	logic_out(1,1);
	I1 = adc_get(0);
	I2 = adc_get(1);
	power = adc_get(2);
	if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
	{	
		TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
		if(buck_part.mode == 0)
		{
			control_current(I1, buck_part);
		}
		else
		{
			control_power(power, buck_part);
		}
	}
	trig_adc();
	logic_out(1,0);
	rt_interrupt_leave();
}

extern uint8_t ss;
void open_tim8(void)
{
	ss = 1;
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(open_tim8, open_tim8);
#endif




















