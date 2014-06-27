#include "stm32f4xx.h"
#include "rtthread.h"
#include "mb.h"
#include "mb_m.h"
#include "user_mb_app.h"
#include "variables.h"
//#include "delay_conf.h"

uint16_t wave = 0;
uint8_t soft_count_flag = 0;
uint8_t contact_buf[5] = {0};

extern BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
		eMBParity eParity);





void set_coil(uint8_t n, uint8_t value)
{
	xMBUtilSetBits( ucSCoilBuf, n, 1, value );
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(set_coil, set_coil num value);
#endif
extern void tim1_init(void);
void rt_slave_thread_entry(void* parameter)
{	
//	rt_uint32_t e = -1;
	rt_thread_delay(1);
	para_init();
	logic_init();
	tim1_init();
//	pwm_init();
//	 xMBPortSerialInit(3,  115200,  8, MB_PAR_NONE );
	eMBInit( MB_ASCII, 0x01, 3, 115200, MB_PAR_NONE );
	// Enable the Modbus Protocol Stack.
	eMBEnable(  );
	rt_thread_delay(1);
	usSRegHoldBuf[wave_sel] = 1;
	usSRegHoldBuf[frq_max] = 100;
	usSRegHoldBuf[current_peak] = 100;
//	xMBUtilSetBits( ucSCoilBuf, standby_s, 1, 1 );
	while(1)
	{
		eMBPoll( );	
//		xMBUtilSetBits( ucSCoilBuf, 35, 3, 0xff );
//		xMBUtilSetBits( ucSCoilBuf, 35, 3, 0x01);
		
		rt_thread_delay(5);
	}
}


int slave_thread_start(void)
{
	rt_thread_t slave_thread;

	slave_thread = rt_thread_create("slave_thread",
								rt_slave_thread_entry, RT_NULL,
								4096, 4, 20);

	if (slave_thread != RT_NULL)
		rt_thread_startup(slave_thread);
    
	return 0;
}


















