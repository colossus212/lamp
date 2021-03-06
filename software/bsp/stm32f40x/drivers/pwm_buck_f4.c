#include "stm32f4xx.h"
#include "rtthread.h"
#include "finsh.h"
//#include "stm32adc.h"
#include "arm_math.h"
#include "my_math.h"
#include "logic.h"
#include "typedefs.h"
#include <absacc.h>
#include "variables.h"

#define IGBT_FREQ 20000
#define pwm_period (SystemCoreClock / IGBT_FREQ - 1)

float pid_out[500] = {0};
float p_get[500] = {0};
float i_get[500] = {0};
uint16_t code[500] = {0};


void pwm_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
	
	DBGMCU_APB2PeriphConfig( DBGMCU_TIM8_STOP, ENABLE);//核心停止后终止PWM
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);////APB2 max frquency is 84M
	
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
	TIM_OCInitStructure.TIM_Pulse = 0;
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
	TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);//interrupt
//	TIM_SelectOutputTrigger(TIM8, TIM_TRGOSource_OC1);//可作为触发源，触发ADC等外设
	TIM_CtrlPWMOutputs(TIM8, ENABLE);//pwm enable
//	TIM_Cmd(TIM8, ENABLE);			//tim8 enable

}
static float PrevError_C1 = 0, IntTerm_C1 = 0/*sum of integral*/;
//float	Kp1 = 200.0f, Ki1 = 50.0f/*25.0f*/, Kd1 = 0.0f;
float	Kp1 = 120.0f, Ki1 = 50.0f/*25.0f*/, Kd1 = 0.0f;
void set_pid1(uint32_t p, uint32_t i, uint32_t d)
{
	Kp1 = (float)p/10;
	Ki1 = (float)i/10;
	Kd1 = (float)d/10;
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(set_pid1, set_pid1 Kp1=p/10 Ki1=i/10 Kd1=d/10);
#endif
float_t pid1(float err1)//功率PID,算出电流大小,unit 1A
{	
	float Error = 0, Output = 0;//, R = 0;

	Error = err1;	
	IntTerm_C1 += Ki1*Error;//IntTerm_C = Ki*(e(0) + e(1) + ... + e(k))
	Output = Kp1 * Error;
	if(IntTerm_C1 > 350.0f)//积分限幅
	{
		IntTerm_C1 = 350.0f;
	}
//	if(IntTerm_C1 < (double)min)
//	{
//		IntTerm_C1 = (double)min;
//	}
  Output += IntTerm_C1;
  Output += Kd1 * (Error - PrevError_C1);

  PrevError_C1 = Error;

  return (Output);//u(k)
}

static float PrevError_C3 = 0, IntTerm_C3 = 0/*sum of integral*/;
float	Kp3 = 0.0020f, Ki3 = 0.0006f, Kd3 = 0.0007f;
void set_pid3(uint32_t p, uint32_t i, uint32_t d)
{
	Kp3 = (float)p/10000;
	Ki3 = (float)i/10000;
	Kd3 = (float)d/10000;
}  
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(set_pid3, set_pid3 Kp3=p/10000 Ki3=i/10000 Kd3=d/10000);
#endif
////float max = 1000, min = 200;
float_t pid3(float err3)//in and REF is percent
{
	
	float Error = 0, Output = 0;//, R = 0;

	Error = err3;	
	IntTerm_C3 += Ki3*Error;//IntTerm_C = Ki*(e(0) + e(1) + ... + e(k))
	Output = Kp3 * Error;
	if(IntTerm_C3 > 1.0f)//积分限幅
	{
		IntTerm_C3 = 1.0f;
	}
//	if(IntTerm_C3 < 0.0f)
//	{
//		IntTerm_C3 = 0.0f;
//	}
  Output += IntTerm_C3;
  Output += Kd3 * (Error - PrevError_C3);

  PrevError_C3 = Error;

  return (Output);//u(k)
}


void trig_laser(void)
{
	TIM_Cmd(TIM8, ENABLE);
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(trig_laser, trigger laser);
#endif


extern void trig_adc(void);
extern float adc_get(uint8_t ch);
void TIM8_UP_TIM13_IRQHandler(void)
{
	float I1 = 0, I2 = 0, power = 0;
	static float percent = 0, percent1 = 0 ;
	float error_pid = 0;
	static uint16_t interrupt_times = 0;
//	static uint8_t flag_modbus = 0;
//	rt_enter_critical();/*调度器上锁*/
	rt_interrupt_enter();
	logic_out(1,1);
	I1 = adc_get(0);//电流值
//	I2 = adc_get(1);
	power = adc_get(2);//功率百分比
	trig_adc();
	if(interrupt_times < 500) 
	{
		p_get[interrupt_times] = power;
		i_get[interrupt_times] = I1;
	}
	if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
	{	
		TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
		if(pwm_struct[select_pwm].mode == 0)
		{
//			TIM_SetCompare1(TIM8, (uint16_t)(percent * pwm_period));
			
			if(interrupt_times < pwm_struct[select_pwm]. positive_pulse)
			{			
					pwm_struct[select_pwm].busy_flag = 1;
					pwm_ena(1);
				
				{	
					error_pid = pwm_struct[select_pwm].p_array[interrupt_times] \
								* program_data[select_pwm].current_max - I1;
//					logic_out(1,1);
					percent = pid3(error_pid);	
//					logic_out(1,0);					

					if(percent > 0.98f) percent = 0.98f;
					if(percent < 0.0f) percent = 0.0f;
//					percent = 0.3f;
					interrupt_times++;
				}
				
			}
			else
			{
				interrupt_times++;
				percent = 0;

				if( interrupt_times > pwm_struct[select_pwm].positive_pulse + pwm_struct[select_pwm].negative_pulse)
				{
					TIM_Cmd(TIM8, DISABLE);
					TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
					pwm_ena(0);
					interrupt_times = 0;
					pwm_struct[select_pwm].busy_flag = 0;
					IntTerm_C3 = 0;
					PrevError_C3 = 0; 

				}
			}
	
			
		}
		else
		{
			if(interrupt_times < pwm_struct[select_pwm]. positive_pulse)
			{			
					pwm_struct[select_pwm].busy_flag = 1;
					pwm_ena(1);
				
				{	
//					logic_out(1,1);
					error_pid = pwm_struct[select_pwm].p_array[interrupt_times] - power;
					percent1 = pid1(error_pid);//percent1为计算得出的电流值
					if(percent1 > program_data[select_pwm].current_max)
						percent1 = program_data[select_pwm].current_max;
					if(percent1 < 0.0f) percent1 = 0.0f;
					error_pid = percent1 - I1;
					percent = pid3(error_pid);	
//					logic_out(1,0);					

					if(percent > 0.98f) percent = 0.98f;
					if(percent < 0.0f) percent = 0.0f;
//					percent = 0.3f;
					interrupt_times++;
				}
				
			}
			else
			{
				interrupt_times++;
				percent = 0;

				if( interrupt_times > pwm_struct[select_pwm].positive_pulse + pwm_struct[select_pwm].negative_pulse)
				{
					TIM_Cmd(TIM8, DISABLE);
					TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
					pwm_ena(0);
					interrupt_times = 0;
					pwm_struct[select_pwm].busy_flag = 0;
					IntTerm_C3 = 0;
					PrevError_C3 = 0;
					IntTerm_C1 = 0;
					PrevError_C1 = 0; 

				}
			}
		}
		TIM_SetCompare1(TIM8, (uint16_t)(pwm_period * percent));
		
		if(interrupt_times < 500) 
		{
			pid_out[interrupt_times] = percent;
			code[interrupt_times] = TIM8->CCR1;
		}
	}
	
	logic_out(1,0);
	rt_interrupt_leave();
//	rt_exit_critical();/* 退出临界区*/
}

void print_pidout(void)
{
	uint16_t i = 0;
	for(i = 0;i<500;i++)
	{
		rt_kprintf(" pidout[%.3d] = %.4d%,code = %.5d, i_percent = %.4d,p = %.4d%, set_percent = %.4d%,\n",
		i,(uint16_t)(pid_out[i]*100),code[i],(uint16_t)i_get[i],
					(uint16_t)(p_get[i]*100),(uint16_t)(pwm_struct[select_pwm].p_array[i]*100));
	}
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(print_pidout, print_pidout);
#endif




void pp(void)
{
	uint8_t i = 0, j = 0;
	for(j = 40; j > 0; j--)
	for(i = 0; i < 80; i++)
	{
		if((uint16_t)(p_get[i]*usSRegHoldBuf[current_peak]/10) >= j)
		rt_kprintf("*");
		else
		rt_kprintf(" ");
		if(i >= 80-1) rt_kprintf("\n");
	}
	for(i = 0; i < 40; i++)
	rt_kprintf("_");
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(pp, print_picture);
#endif











