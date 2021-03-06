#include "stm32f4xx.h"
#include "rtthread.h"
#include "mb.h"
#include "mb_m.h"
#include "user_mb_app.h"
#include "variables.h"
//#include "delay_conf.h"

void rt_master_thread_entry(void* parameter)
{	
//	rt_uint32_t e = 0;
	
	rt_thread_delay(1);
	// Initialize protocol stack in RTU mode for a Master
	eMBMasterInit( MB_RTU, 6, 115200, MB_PAR_NONE );
	// Enable the Modbus Protocol Stack.
	eMBMasterEnable(  );
	rt_thread_delay(1);
	while(1)
	{
		eMBMasterPoll( );
		
		rt_thread_delay(5);
	}
}


int master_thread_start(void)
{
	rt_thread_t master_thread;

	master_thread = rt_thread_create("master_thread",
								rt_master_thread_entry, RT_NULL,
								4096, 5, 20);

	if (master_thread != RT_NULL)
		rt_thread_startup(master_thread);
    
	return 0;
}


















