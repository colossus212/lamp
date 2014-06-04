#include "stm32f4xx.h"
#include "rtthread.h"


void rt_trig_thread_entry(void* parameter)
{	
	rt_uint32_t e = 0;
	rt_thread_delay(1);

	while(1)
	{
		
		rt_thread_delay(5);
	}
}


int trig_thread_start(void)
{
	rt_thread_t trig_thread;

	trig_thread = rt_thread_create("trig_thread",
								rt_trig_thread_entry, RT_NULL,
								4096, 5, 20);

	if (trig_thread != RT_NULL)
		rt_thread_startup(trig_thread);
    
	return 0;
}


















