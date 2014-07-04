#include "variables.h"
#include <absacc.h>
#include "string.h"
#include "finsh.h"

uint8_t select_pwm = 0;

//uint16_t master_datas[M_REG_HOLDING_NREGS] = {0};

pwm_st pwm_struct[pwm_num];
//save_program program1 __at(0x40024000);//将数组定义在0x40024000 #include <absacc.h>
program program_data[pwm_num] __at(0x40024000) = {0};

void clear_bkpsram(void)
{
	memset(program_data, 0, sizeof(program)*pwm_num);
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(clear_bkpsram, clear_bkpsram);
#endif



















