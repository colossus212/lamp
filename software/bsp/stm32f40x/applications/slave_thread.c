#include "stm32f4xx.h"
#include "rtthread.h"
#include "mb.h"
#include "mb_m.h"
#include "user_mb_app.h"
#include "variables.h"
//#include "delay_conf.h"

uint16_t wave = 0;

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

		
		if(wave != usSRegHoldBuf[wave_sel])
		{
			wave = usSRegHoldBuf[wave_sel];
			read_scheme(wave);
		}
		
		if (xMBUtilGetBits(ucSCoilBuf, save_s, 1 ) == 1)
		{
			save_scheme(wave);
			xMBUtilSetBits( ucSCoilBuf, save_s, 1, 0 );
		}
		
		xMBUtilSetBits( ucSCoilBuf, x0_s, 8, xMBUtilGetBits(ucMCoilBuf[0], X0, 8 ) );
		xMBUtilSetBits( ucSCoilBuf, x8_s, 8, xMBUtilGetBits(ucMCoilBuf[0], X8, 8 ) );
		xMBUtilSetBits( ucSCoilBuf, x16_s, 8, xMBUtilGetBits(ucMCoilBuf[0], X16, 8 ) );
		xMBUtilSetBits( ucSCoilBuf, y0_s, 8, Y[0] );
		xMBUtilSetBits( ucSCoilBuf, y8_s, 8, Y[1] );
		xMBUtilSetBits( ucSCoilBuf, io_pg2, 7, (GPIO_ReadInputData( GPIOG)>>2) );
		xMBUtilSetBits( ucSCoilBuf, io_pd8, 8, (GPIO_ReadInputData( GPIOD)>>8) );
		xMBUtilSetBits( ucSCoilBuf, io_pc4, 2, (GPIO_ReadInputData( GPIOC)>>4) );
		
		
		if(xMBUtilGetBits(ucMCoilBuf[0], X11, 1) == 0)
		{
			xMBUtilSetBits( ucSCoilBuf, start_up_s, 1, 0 );
		}
		else
		{
			xMBUtilSetBits( ucSCoilBuf, start_up_s, 1, 1 );
		}
		
		if(xMBUtilGetBits(ucSCoilBuf, start_up_s, 1 ) == 1)
		{
			start_flag = 1;
			
		}
		else
		{
			start_flag = 0;
			xMBUtilSetBits( Y, Y8, 7, 0X08 );
		}
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


















