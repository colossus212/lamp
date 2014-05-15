#include "stm32f4xx.h"
#include <absacc.h>
#include "finsh.h"

uint16_t test_bkpsram __at(0x40024ff0);

void bkpsram_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_BackupAccessCmd(ENABLE);
//	PWR_BackupRegulatorCmd(ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
//	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
//	RCC_RTCCLKCmd(ENABLE);
//	
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(bkpsram_init, bkpsram_init);
#endif

void write_bkpsram(uint16_t num)
{
	test_bkpsram = num;
	RTC_WriteBackupRegister(RTC_BKP_DR0, num);
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(write_bkpsram, write_bkpsram);
#endif

void read_bkpsram(void)
{
	rt_kprintf("test_bkpsram = %d\n",test_bkpsram);
	rt_kprintf("RTC_BKP_DR0 = %d\n",RTC_ReadBackupRegister( RTC_BKP_DR0));
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(read_bkpsram, read_bkpsram);
#endif


























