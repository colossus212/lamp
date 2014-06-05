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

#define CONFIG_DEFAULT_L_KP						0.45
#define CONFIG_DEFAULT_L_KI						0.3
#define CONFIG_DEFAULT_L_KD						0.0//0.01
#define CONFIG_DEFAULT_P_KP						2
#define CONFIG_DEFAULT_P_KI						0.01
#define CONFIG_DEFAULT_P_KD						1

static arm_pid_instance_f32 L1_Pid_Instance;//L1氙灯电流PID参数
static arm_pid_instance_f32 L2_Pid_Instance;//L2氙灯电流PID参数
static arm_pid_instance_f32 P_Pid_Instance;//P背光能量PID参数

float pid_out[500] = {0};
float p_get[500] = {0};

static __inline void init_pid(void)
{//初始化PID使用默认参数填充结构体
	L1_Pid_Instance.Kp = CONFIG_DEFAULT_L_KP;
	L1_Pid_Instance.Ki = CONFIG_DEFAULT_L_KI;
	L1_Pid_Instance.Kd = CONFIG_DEFAULT_L_KD;
	
	L2_Pid_Instance.Kp = CONFIG_DEFAULT_L_KP;
	L2_Pid_Instance.Ki = CONFIG_DEFAULT_L_KI;
	L2_Pid_Instance.Kd = CONFIG_DEFAULT_L_KD;
	
	P_Pid_Instance.Kp = CONFIG_DEFAULT_P_KP;
	P_Pid_Instance.Ki = CONFIG_DEFAULT_P_KI;
	P_Pid_Instance.Kd = CONFIG_DEFAULT_P_KD;
	
	arm_pid_init_f32 (&L1_Pid_Instance, 1);//初始化PID参数
	arm_pid_init_f32 (&L2_Pid_Instance, 1);//初始化PID参数
	arm_pid_init_f32 (&P_Pid_Instance, 1);//初始化PID参数
}

void test_init_pid(uint16_t p, uint16_t i, uint16_t d)
{
	L1_Pid_Instance.Kp = (float)p/100;
	L1_Pid_Instance.Ki = (float)i/100;
	L1_Pid_Instance.Kd = (float)d/100;
	arm_pid_init_f32 (&L1_Pid_Instance, 1);//初始化PID参数
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(test_init_pid, test_init_pid );
#endif
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
	
	init_pid();
	
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

void control_power(uint16_t I1, uint8_t pwm_struct_num)
{
	
}

void trig_laser(void)
{
	TIM_Cmd(TIM8, ENABLE);
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(trig_laser, trigger laser);
#endif
void control_current(float I1, uint8_t pwm_struct_num)//I1 is real current, unit 1A,I2 is reference current
{
	float percent = 0 ;
	float i_percent = 0,ref_percent = 0;
	float error_pid = 0;
	static uint16_t interrupt_times = 0;
//	logic_out(1,1);
	i_percent = I1/350;//I1*1000/350,I1 uint 1A ,i_percent unit 0.1%
	
	if(interrupt_times < pwm_struct[pwm_struct_num]. positive_pulse)
	{
		pwm_struct[pwm_struct_num].busy_flag = 1;
		
		{			
			pwm_ena(1);
			ref_percent = (float)pwm_struct[pwm_struct_num].p_array[interrupt_times]/1000;
			error_pid = ref_percent - i_percent;
			percent = arm_pid_f32(&L1_Pid_Instance, error_pid);
//			percent = pid3(i_percent, pwm_struct[pwm_struct_num].p_array[interrupt_times]);			
//			if(interrupt_times < 20) percent = 0.98f;//测试98%占空比时电流最大有多大.2ms
//			else percent = 0;
			pid_out[interrupt_times] = percent;
			p_get[interrupt_times] = i_percent;
			if(percent > 0.98f) percent = 0.98f;
			if(percent < 0.0f) percent = 0;
			interrupt_times++;
		}
		
	}
	else
	{
		interrupt_times++;
		percent = 0;
		
//		pid_out[interrupt_times] = percent;
//		p_get[interrupt_times] = i_percent;
		if( interrupt_times > pwm_struct[pwm_struct_num].positive_pulse + pwm_struct[pwm_struct_num].negative_pulse)
		{
			TIM_Cmd(TIM8, DISABLE);
			pwm_ena(0);
			interrupt_times = 0;
			pwm_struct[pwm_struct_num].busy_flag = 0;
			arm_pid_reset_f32 (&L1_Pid_Instance);
//			init_pid();
		}
	}
	
	TIM_SetCompare1(TIM8, (uint16_t)(float)(percent * pwm_period));

//	logic_out(1,0);
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(control_current, control_current i1 i2);
#endif

extern void trig_adc(void);
extern float adc_get(uint8_t ch);
void TIM8_UP_TIM13_IRQHandler(void)
{
	float I1 = 0, I2 = 0, power = 0;
	rt_interrupt_enter();
//	logic_out(1,1);
	I1 = adc_get(0);
	I2 = adc_get(1);
	power = adc_get(2);
	trig_adc();
	if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
	{	
		TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
		if(pwm_struct[select_pwm].mode == 0)
		{
			control_current(I1, select_pwm);
		}
		else
		{
			control_power(power, select_pwm);
		}
	}
	
//	logic_out(1,0);
	rt_interrupt_leave();
}

extern uint8_t ss;
void open_tim8(void)
{
	uint16_t i = 0;
	for(i = 0;i<500;i++)
	{
		p_get[i] = 0;
		pid_out[i] = 0;
	}
	ss = 1;
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(open_tim8, open_tim8);
#endif

void print_pidout(void)
{
	uint16_t i = 0;
	for(i = 0;i<500;i++)
	{
		rt_kprintf("pidout[%.3d] = %.4d%, i_percent = %.4d%, set_percent = %.4d%,\n",
		i,(uint16_t)(pid_out[i]*100),(uint16_t)(p_get[i]*100),pwm_struct[0].p_array[i]/10);
	}
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(print_pidout, print_pidout);
#endif

















