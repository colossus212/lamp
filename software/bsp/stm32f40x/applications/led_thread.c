#include "stm32f4xx.h"
#include "rtthread.h"
#include "logic.h"
#include "variables.h"

uint8_t ss = 0;
void rt_led_thread_entry(void* parameter)
{	
	uint8_t i = 0;
	rt_thread_delay(1);
//	logic_init();
	adc_initialize();
	pwm_init();
	
	while(1)
	{
		logic_out(mcu_run, (i++%2 == 0)?0:1);
//		//		/*errorCode = */eMBMasterReqWriteCoil(1,8,0xFF00,RT_WAITING_FOREVER);
////		eMBMasterReqWriteHoldingRegister(1,3,usSRegHoldBuf[0],RT_WAITING_FOREVER);
//		//		errorCode = eMBMasterReqReadDiscreteInputs(1,3,8,RT_WAITING_FOREVER);
////		eMBMasterReqWriteMultipleCoils(1,3,5,ucModbusUserData,RT_WAITING_FOREVER);
////		errorCode = eMBMasterReqWriteCoil(1,8,0xFF00,RT_WAITING_FOREVER);
//		eMBMasterReqReadCoils(1,0,32,RT_TICK_PER_SECOND );
////		errorCode = eMBMasterReqReadInputRegister(1,3,2,RT_WAITING_FOREVER);
////		eMBMasterReqWriteHoldingRegister(1,3,e++ ,RT_TICK_PER_SECOND);
////		eMBMasterReqWriteHoldingRegister(1,4,e++ ,RT_TICK_PER_SECOND);
////		eMBMasterReqWriteHoldingRegister(1,5,e++ ,RT_TICK_PER_SECOND);
////		errorCode = eMBMasterReqWriteMultipleHoldingRegister(1,3,2,usModbusUserData,RT_WAITING_FOREVER);
//		eMBMasterReqReadHoldingRegister(1,3,8,RT_TICK_PER_SECOND );
////		errorCode = eMBMasterReqReadWriteMultipleHoldingRegister(1,3,2,usModbusUserData,5,2,RT_WAITING_FOREVER);
//		if(ss) 
//		{
//			TIM_Cmd(TIM8, ENABLE);
//			ss = 0;
//		}
		rt_thread_delay(50);
	}
}


int led_thread_start(void)
{
	rt_thread_t led_thread;

	led_thread = rt_thread_create("led_thread",
								rt_led_thread_entry, RT_NULL,
								4096, 30, 20);

	if (led_thread != RT_NULL)
		rt_thread_startup(led_thread);
    
	return 0;
}


















