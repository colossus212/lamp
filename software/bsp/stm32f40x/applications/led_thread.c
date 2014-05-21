#include "stm32f4xx.h"
#include "rtthread.h"
#include "logic.h"
#include "variables.h"

uint8_t Y[2] = {0};
void rt_led_thread_entry(void* parameter)
{	
	uint8_t i = 0;
	rt_thread_delay(1);
//	logic_init();
	adc_initialize();
	pwm_init();
	
	while(1)
	{
		eMBMasterReqReadCoils(1, 1000, 24, RT_WAITING_FOREVER );//X0-X23
		eMBMasterReqWriteMultipleCoils(1, 0, 16, Y, RT_WAITING_FOREVER);//Y0-Y15
		logic_out(mcu_run, (i++%2 == 0)?0:1);
//		rt_thread_delay(10);
	}
}


int led_thread_start(void)
{
	rt_thread_t led_thread;

	led_thread = rt_thread_create("led_thread",
								rt_led_thread_entry, RT_NULL,
								4096, 10, 20);

	if (led_thread != RT_NULL)
		rt_thread_startup(led_thread);
    
	return 0;
}


















