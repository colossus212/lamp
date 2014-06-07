#include "rtthread.h"
#include "stm32f4xx.h"
#include "finsh.h"
#include "variables.h"

static uint16_t tt = 0;//trigger times;
static uint8_t trigger_mode = 0;
uint16_t frq = 0;//1-1000:(0.1-100Hz)
uint8_t laser_flag = 0;//开激光的标志

void timer4_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//APB1 max frquency is 42M
														
	TIM_TimeBaseStructInit( &TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = 2000;
	TIM_TimeBaseStructure.TIM_Prescaler = 41999;//1 pulse is 0.5 ms(42M*2/(41999+1) Hz)
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);//interrupt
	TIM_Cmd(TIM4, DISABLE);
}

void TIM4_IRQHandler(void)
{
	static uint16_t test_frq = 0;
	rt_interrupt_enter();
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{	
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
//		if(trigger_mode == 1)
//		{
//			if(tt > 1)//定时器启动前已经触发了一次
//			{
//				trig_laser();
//				tt--;
//			}
//			else
//			{
//				TIM_Cmd(TIM4, DISABLE);
//			}
//		}
		
		if(trigger_mode == 2)
		{
			trig_laser();
		}	
	}

//	logic_out(1,(test_frq++%2));
	rt_interrupt_leave();
}

//void set_para(uint16_t f)//f*0.1hz
//{
//	frq = f;
//}
//#ifdef FINSH_USING_SYMTAB
//FINSH_FUNCTION_EXPORT(set_para, set_para );
//#endif
void trig_laser_out(uint16_t f)//num of times
{
	uint8_t mode = 0;
	if(f == 0)
		mode = 0;
	else
		mode = 2;
	switch (mode)
	{
		case 0://单次触发
			trigger_mode = 0;
			trig_laser();
			break;
//		case 1://多次触发
//			tt = nt;
//			TIM_SetAutoreload(TIM4, 20000/frq);
//			trigger_mode = 1;
//			trig_laser();
//			TIM_Cmd(TIM4, ENABLE);
//			break;
		case 2://连续触发
			TIM_SetAutoreload(TIM4, 20000/f);
			TIM_SetCounter(TIM4, 0);
			trigger_mode = 2;
			trig_laser();
			TIM_Cmd(TIM4, ENABLE);
			break;
		default:
			break;
	}
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(trig_laser_out, trig_mode single 0 serial 2);
#endif
void stop_trigger(void)
{
	laser_flag = 0;
	xMBUtilSetBits( ucSCoilBuf, laser_sw, 1, 0 );
//	TIM_Cmd(TIM8, DISABLE);
	TIM_Cmd(TIM4, DISABLE);
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(stop_trigger, stop_trigger);
#endif
















