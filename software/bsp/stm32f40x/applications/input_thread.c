#include "stm32f4xx.h"
#include "rtthread.h"
#include "variables.h"

void rt_input_thread_entry(void* parameter)
{	
	rt_uint32_t e = 0;
	rt_thread_delay(1);

	while(1)
	{
		if(rt_event_recv( exit_event, 1, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
			RT_WAITING_FOREVER, &e ) == RT_EOK)
		{
			
		}
	}
}


int input_thread_start(void)
{
	rt_thread_t input_thread;

	input_thread = rt_thread_create("input_thread",
								rt_input_thread_entry, RT_NULL,
								4096, 5, 20);

	if (input_thread != RT_NULL)
		rt_thread_startup(input_thread);
    
	return 0;
}


















