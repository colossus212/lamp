#include "stm32f4xx.h"
#include "rtthread.h"
#include "mb.h"
#include "mb_m.h"
#include "user_mb_app.h"
//#include "delay_conf.h"
extern USHORT   usSRegHoldBuf[S_REG_HOLDING_NREGS] ;
void rt_slave_thread_entry(void* parameter)
{	
	rt_uint32_t e = 0;
//	rt_thread_delay(1);
//	eMBInit( MB_RTU, 0x01, 3, 115200, MB_PAR_NONE );
//	// Enable the Modbus Protocol Stack.
//	eMBEnable(  );
	rt_thread_delay(1);
	while(1)
	{
//		eMBPoll( );
//		/*errorCode = */eMBMasterReqWriteCoil(1,8,0xFF00,RT_WAITING_FOREVER);
//		eMBMasterReqWriteHoldingRegister(1,3,usSRegHoldBuf[0],RT_WAITING_FOREVER);
		//		errorCode = eMBMasterReqReadDiscreteInputs(1,3,8,RT_WAITING_FOREVER);
//		eMBMasterReqWriteMultipleCoils(1,3,5,ucModbusUserData,RT_WAITING_FOREVER);
//		errorCode = eMBMasterReqWriteCoil(1,8,0xFF00,RT_WAITING_FOREVER);
		eMBMasterReqReadCoils(1,0,32,RT_WAITING_FOREVER);
//		errorCode = eMBMasterReqReadInputRegister(1,3,2,RT_WAITING_FOREVER);
//		errorCode = eMBMasterReqWriteHoldingRegister(1,3,usModbusUserData[0],RT_WAITING_FOREVER);
//		errorCode = eMBMasterReqWriteMultipleHoldingRegister(1,3,2,usModbusUserData,RT_WAITING_FOREVER);
//		eMBMasterReqReadHoldingRegister(1,3,2,RT_WAITING_FOREVER);
//		errorCode = eMBMasterReqReadWriteMultipleHoldingRegister(1,3,2,usModbusUserData,5,2,RT_WAITING_FOREVER);
//		rt_thread_delay(5);
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


















