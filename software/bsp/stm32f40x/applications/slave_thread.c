#include "stm32f4xx.h"
#include "rtthread.h"
#include "mb.h"
#include "mb_m.h"
#include "user_mb_app.h"
//#include "delay_conf.h"

extern BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
		eMBParity eParity);
void rt_slave_thread_entry(void* parameter)
{	
	rt_uint32_t e = -1;
	rt_thread_delay(1);
	logic_init();
//	 xMBPortSerialInit(3,  115200,  8, MB_PAR_NONE );
	eMBInit( MB_RTU, 0x01, 3, 115200, MB_PAR_NONE );
	// Enable the Modbus Protocol Stack.
	eMBEnable(  );
	rt_thread_delay(1);
	while(1)
	{
		eMBPoll( );

		rt_thread_delay(5);
	}
}


int slave_thread_start(void)
{
	rt_thread_t slave_thread;

	slave_thread = rt_thread_create("slave_thread",
								rt_slave_thread_entry, RT_NULL,
								4096, 3, 20);

	if (slave_thread != RT_NULL)
		rt_thread_startup(slave_thread);
    
	return 0;
}


















