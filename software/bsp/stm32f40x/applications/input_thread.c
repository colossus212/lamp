#include "stm32f4xx.h"
#include "rtthread.h"
#include "variables.h"
#include "typedefs.h"
#include "logic.h"

void rt_input_thread_entry(void* parameter)
{	
	rt_uint32_t e = 0;
	rt_thread_delay(1);
	
//	logic_init();
	while(1)
	{
		if(rt_event_recv( exit_event, exit_io_event, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
			RT_WAITING_FOREVER, &e ) == RT_EOK)
		{
//			rt_thread_delay(2);
			switch(e)
			{
				case io_laser_event: 
						
						if((logic_in(mkin0) == 0) && (usSRegHoldBuf[tr_mode] == 1)
							&&(xMBUtilGetBits(ucSCoilBuf, standby_s, 1 )))
						{
							pwm_struct[select_pwm].busy_flag = 1;
							select_pwm = logic_in(mkin2) | (logic_in(mkin3)<<1) | (logic_in(mkin4)<<2)
							| (logic_in(mkin5)<<3) | (logic_in(mkin6)<<4);
							select_pwm ^= 0x1f; 
							usSRegHoldBuf[wave_sel] = select_pwm + 1;
							trig_laser_out(0);
//							rt_event_send(trig_event, exit_trig);
						}
					break;
				case io_red_event:
									if(usSRegHoldBuf[tr_mode] == 1)
									{
										if(logic_in(mkin1) == 0)
										{
											logic_out(red_pin, 1);
										}
										else
										{
											logic_out(red_pin, 0);
										}
									}
					break;
				default:break;
			}
			
//			rt_thread_delay(2);
//			rt_event_recv( exit_event, exit_io_event, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
//			2, &e );
			
		}
	}
}


int input_thread_start(void)
{
	rt_thread_t input_thread;

	input_thread = rt_thread_create("input_thread",
								rt_input_thread_entry, RT_NULL,
								4096, 3, 20);

	if (input_thread != RT_NULL)
		rt_thread_startup(input_thread);
    
	return 0;
}


















