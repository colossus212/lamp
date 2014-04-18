#include "stm32f4xx.h"
#include "rtthread.h"
#include "logic_f4.h"

void rt_led_thread_entry(void* parameter)
{	
	uint8_t i = 0;
	rt_thread_delay(1);
	logic_init();
	
	while(1)
	{
		logic_out(mcu_run, (i++%2 == 0)?0:1);
		
		rt_thread_delay(50);
	}
}


int led_thread_start(void)
{
	rt_thread_t led_thread;

	led_thread = rt_thread_create("led_thread",
								rt_led_thread_entry, RT_NULL,
								4096, 5, 20);

	if (led_thread != RT_NULL)
		rt_thread_startup(led_thread);
    
	return 0;
}


















