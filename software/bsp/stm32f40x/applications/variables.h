#ifndef __VARYABLES_H__
#define __VARYABLES_H__

#include "stm32f4xx.h"
#include "rtthread.h"
#include "typedefs.h"
#include "user_mb_app.h"
#include "finsh.h"



/*variables*/
extern uint16_t master_datas[M_REG_HOLDING_NREGS];
extern uint8_t select_pwm;
extern rt_event_t exit_event;
// extern save_program program1;
extern program program_data[32];
extern pwm_st pwm_struct[pwm_num] ;
extern USHORT   usSRegHoldBuf[S_REG_HOLDING_NREGS] ;


/*functions*/
extern void adc_initialize(void);
extern void pwm_init(void);
extern uint8_t read_para(void);
extern uint16_t CRC16( uint8_t * psrc, uint16_t Len );
extern void timer4_init(void);
extern void trig_laser(void);










#endif
