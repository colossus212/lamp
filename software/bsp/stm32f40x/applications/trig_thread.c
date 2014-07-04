#include "stm32f4xx.h"
#include "rtthread.h"
#include "typedefs.h"
#include "variables.h"


rt_event_t trig_event;

void rt_trig_thread_entry(void* parameter)
{	
	rt_uint32_t e = 0;
	rt_thread_delay(1);
	trig_event = rt_event_create("trig_event", RT_IPC_FLAG_FIFO);
	
	while(1)
	{
		rt_event_recv(trig_event,manual_trig | exit_trig,
			RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, &e);
		
		switch(e)
		{
			case manual_trig:if(usSRegHoldBuf[tr_mode] == 2)
							{
								trig_laser_out(frq);
								select_pwm = wave;
							}
				break;
//			case exit_trig:	if(usSRegHoldBuf[tr_mode] == 1)//外部触发模式
//							{
////								select_pwm = wave;//外部IO读取得到.input_thread中.
//								usSRegHoldBuf[wave_sel] = select_pwm + 1;
//								trig_laser_out(0);
//							}
//				break;
//			case 0:break;
			default:break;
		}
		
		
//		rt_thread_delay(50);
	}
}


int trig_thread_start(void)
{
	rt_thread_t trig_thread;

	trig_thread = rt_thread_create("trig_thread",
								rt_trig_thread_entry, RT_NULL,
								4096, 2, 20);

	if (trig_thread != RT_NULL)
		rt_thread_startup(trig_thread);
    
	return 0;
}


















