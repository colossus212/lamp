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

void check_logic(void)
{
	switch (step)
	{
		case 1:if(xMBUtilGetBits(ucMCoilBuf[0], X21, 1) != 0x01)
				{
					xMBUtilSetBits( ucSCoilBuf, start_up_s, 1, 0 );
					contact_err_flag = 1;
					rt_kprintf("error in setp 1");
				}
			break;
		case 2:
		case 3:if(xMBUtilGetBits(ucMCoilBuf[0], X21, 2) != 0x03)
				{
					xMBUtilSetBits( ucSCoilBuf, start_up_s, 1, 0 );
					contact_err_flag = 1;
					rt_kprintf("error in setp 2 or 3");
				} 
			break;
		case 4:if(xMBUtilGetBits(ucMCoilBuf[0], X20, 3) != 0x07)
				{
					if(xMBUtilGetBits(ucMCoilBuf[0], X20, 1) != 0x01)
					{
						pre_burn_err = 1;
					}
					else
					{
						contact_err_flag = 1;
						rt_kprintf("error in setp 4");
					}
					xMBUtilSetBits( ucSCoilBuf, start_up_s, 1, 0 );
				} 
			break;
		
		case 5:if(xMBUtilGetBits(ucMCoilBuf[0], X20, 4) != 0x0f)
				{
					if(xMBUtilGetBits(ucMCoilBuf[0], X20, 1) != 0x01)
					{
						pre_burn_err = 1;
					}
					else
					{
						contact_err_flag = 1;
						rt_kprintf("error in setp 5");
					}
					xMBUtilSetBits( ucSCoilBuf, start_up_s, 1, 0 );
				} 
			break;
		default:break;
	}
	
	if((xMBUtilGetBits(ucMCoilBuf[0], X15, 1) == 0)/*||(xMBUtilGetBits(ucMCoilBuf[0], X12, 1) == 0)?*/)
	{
		xMBUtilSetBits( ucSCoilBuf, start_up_s, 1, 0 );
	}
	
	if(xMBUtilGetBits(ucSCoilBuf, start_up_s, 1 ) == 1)
	{
		start_flag = 1;
		pre_burn_err = 0;
		contact_err_flag = 0;
		
	}
	else
	{
		start_flag = 0;
		step = 0;
		xMBUtilSetBits( ucSCoilBuf, standby_s, 1, 0 );
		xMBUtilSetBits( ucSCoilBuf, laser_sw, 1, 0 );
		xMBUtilSetBits( Y, Y8, 7, 0X08 );
	}
	
	if((soft_count_flag == 1)&&(xMBUtilGetBits(ucMCoilBuf[0], X21, 1 ) == 1))
	{
		usSRegHoldBuf[remain_time] = 60 - (rt_tick_get() - soft_count)/RT_TICK_PER_SECOND;
		
		if(usSRegHoldBuf[remain_time] == 0)
		soft_count_flag = 0;
	}
	
	if(frq != usSRegHoldBuf[frq_set])
	{
		if(usSRegHoldBuf[frq_set] <=usSRegHoldBuf[frq_max] )
		{
			frq = usSRegHoldBuf[frq_set];
		}
	}
	
}

void operation(void)
{
	uint16_t buf = 0, buf2 = 0;
	if(usSRegHoldBuf[wave_sel] > 0)
		wave = usSRegHoldBuf[wave_sel] - 1;
	
	if (xMBUtilGetBits(ucSCoilBuf, read_s, 1 ) == 1)
	{		
		read_scheme(wave);
		check_disp_para();
		calculate_array(wave);
		select_pwm = wave;
		xMBUtilSetBits( ucSCoilBuf, read_s, 1, 0 );
	}
	
	if (xMBUtilGetBits(ucSCoilBuf, save_s, 1 ) == 1)
	{
		check_disp_para();
		save_scheme(wave);
		calculate_array(wave);
		select_pwm = wave;		
		xMBUtilSetBits( ucSCoilBuf, save_s, 1, 0 );
	}
	
	logic_out(red_pin, xMBUtilGetBits(ucSCoilBuf, red_sw, 1 ));

	
	if (xMBUtilGetBits(ucSCoilBuf, laser_sw, 1 )&&(xMBUtilGetBits(ucSCoilBuf, standby_s, 1 )))
	{
		if(laser_flag == 0)
		{
			rt_event_send(trig_event, manual_trig);
			xMBUtilSetBits( ucSCoilBuf, laser_sw, 1, (frq != 0)?1:0 );
			laser_flag = 1;
		}
	}
	else
	{
		if(laser_flag)
		stop_trigger();
	}
//	if(1)
//	{
//	
//	}
	
	xMBUtilSetBits( ucSCoilBuf, red_off_ena, 1, 
	xMBUtilGetBits(ucSCoilBuf, start_up_s, 1 )&&xMBUtilGetBits(ucSCoilBuf, red_sw, 1 )&&
	(0 == usSRegHoldBuf[tr_mode]) );
	
	xMBUtilSetBits( ucSCoilBuf, red_on_ena, 1, 
	xMBUtilGetBits(ucSCoilBuf, start_up_s, 1 )&&(0 == xMBUtilGetBits(ucSCoilBuf, red_sw, 1 ))&&
	(0 == usSRegHoldBuf[tr_mode]));
	
	xMBUtilSetBits( ucSCoilBuf, ready_ena, 1, started_flag &&(0 == xMBUtilGetBits(ucSCoilBuf, standby_s, 1 )));
	
	xMBUtilSetBits( ucSCoilBuf, stand_by_ena, 1, started_flag &&(1 == xMBUtilGetBits(ucSCoilBuf, standby_s, 1)));
	
	xMBUtilSetBits( ucSCoilBuf, laser_on_ena, 1, xMBUtilGetBits(ucSCoilBuf, standby_s, 1) 
					&&(0 == usSRegHoldBuf[tr_mode])&&(0 == xMBUtilGetBits(ucSCoilBuf, laser_sw, 1 )));
	
	xMBUtilSetBits( ucSCoilBuf, laser_off_ena, 1, xMBUtilGetBits(ucSCoilBuf, standby_s, 1) 
					&&(0 == usSRegHoldBuf[tr_mode])&&(1 == xMBUtilGetBits(ucSCoilBuf, laser_sw, 1)));	
//	xMBUtilSetBits( ucSCoilBuf, ready_ena, 1, 1 );
//	xMBUtilSetBits( ucSCoilBuf, stand_by_ena, 1, 1 );
	xMBUtilSetBits( ucSCoilBuf, ext_ena, 1, (~usSRegHoldBuf[tr_mode]) );
	xMBUtilSetBits( ucSCoilBuf, int_ena, 1, usSRegHoldBuf[tr_mode] );
	
}

void set_coil(uint8_t n, uint8_t value)
{
	xMBUtilSetBits( ucSCoilBuf, n, 1, value );
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(set_coil, set_coil num value);
#endif

void rt_slave_thread_entry(void* parameter)
{	
//	rt_uint32_t e = -1;
	rt_thread_delay(1);
	para_init();
	logic_init();
//	 xMBPortSerialInit(3,  115200,  8, MB_PAR_NONE );
	eMBInit( MB_RTU, 0x01, 3, 115200, MB_PAR_NONE );
	// Enable the Modbus Protocol Stack.
	eMBEnable(  );
	rt_thread_delay(1);
	usSRegHoldBuf[wave_sel] = 1;
	usSRegHoldBuf[frq_max] = 100;
//	xMBUtilSetBits( ucSCoilBuf, standby_s, 1, 1 );
	while(1)
	{
		eMBPoll( );
		
		operation();
//		xMBUtilSetBits( ucSCoilBuf, 35, 3, 0xff );
//		xMBUtilSetBits( ucSCoilBuf, 35, 3, 0x01);
		
		xMBUtilSetBits( ucSCoilBuf, x0_s, 8, xMBUtilGetBits(ucMCoilBuf[0], X0, 8 ) );
		xMBUtilSetBits( ucSCoilBuf, x8_s, 8, xMBUtilGetBits(ucMCoilBuf[0], X8, 8 ) );
		xMBUtilSetBits( ucSCoilBuf, x16_s, 8, xMBUtilGetBits(ucMCoilBuf[0], X16, 8 ) );
		xMBUtilSetBits( ucSCoilBuf, y0_s, 8, Y[0] );
		xMBUtilSetBits( ucSCoilBuf, y8_s, 8, Y[1] );
		xMBUtilSetBits( ucSCoilBuf, io_pg2, 5, (uint8_t)(GPIO_ReadInputData( GPIOG)>>2) );
		xMBUtilSetBits( ucSCoilBuf, io_pd8, 8, (uint8_t)(GPIO_ReadInputData( GPIOD)>>8) );
		xMBUtilSetBits( ucSCoilBuf, io_pc4, 2, (uint8_t)(GPIO_ReadInputData( GPIOC)>>4) );
		xMBUtilSetBits( ucSCoilBuf, soft_start, 1, xMBUtilGetBits(ucMCoilBuf[0], X22, 1 ) );
		xMBUtilSetBits( ucSCoilBuf, charge_sta, 1, xMBUtilGetBits(Y, 14, 1 ) );
		xMBUtilSetBits( ucSCoilBuf, lamp1_sta, 1, xMBUtilGetBits(ucMCoilBuf[0], X20, 1 ) );
		xMBUtilSetBits( ucSCoilBuf, interlock_sta, 1, xMBUtilGetBits(ucMCoilBuf[0], X13, 1 ) );
		xMBUtilSetBits( ucSCoilBuf, key_switch, 1, xMBUtilGetBits(ucMCoilBuf[0], X14, 1 ) );
		xMBUtilSetBits( ucSCoilBuf, emergency_sw, 1, xMBUtilGetBits(ucMCoilBuf[0], X15, 1 ) );
		xMBUtilSetBits( ucSCoilBuf, main_supply_sw, 1, xMBUtilGetBits(ucMCoilBuf[0], X21, 1 ) );
		xMBUtilSetBits( ucSCoilBuf, ignition_sw, 1, xMBUtilGetBits(Y, 12, 1 ) );
		xMBUtilSetBits( ucSCoilBuf, pre_burn_fail, 1, pre_burn_err );
		xMBUtilSetBits( ucSCoilBuf, contactor_err, 1, contact_err_flag );
		
		check_logic();
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


















