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
extern uint8_t start_flag;
// extern save_program program1;
extern program program_data[32];
extern pwm_st pwm_struct[pwm_num] ;
extern USHORT   usSRegHoldBuf[S_REG_HOLDING_NREGS] ;
extern uint8_t Y[2];



#if      M_COIL_NCOILS%8
extern UCHAR    ucMCoilBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_COIL_NCOILS/8+1];
#else
extern UCHAR    ucMCoilBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_COIL_NCOILS/8];
#endif

#if S_COIL_NCOILS%8
extern UCHAR    ucSCoilBuf[S_COIL_NCOILS/8+1]                ;
#else
extern UCHAR    ucSCoilBuf[S_COIL_NCOILS/8]                  ;
#endif
/*functions*/
extern void adc_initialize(void);
extern void pwm_init(void);
extern uint8_t read_para(void);
extern uint16_t CRC16( uint8_t * psrc, uint16_t Len );
extern void timer4_init(void);
extern void trig_laser(void);
extern UCHAR xMBUtilGetBits( UCHAR * ucByteBuf, USHORT usBitOffset, UCHAR ucNBits );
extern void xMBUtilSetBits( UCHAR * ucByteBuf, USHORT usBitOffset, UCHAR ucNBits, UCHAR ucValue );
extern USHORT usMBCRC16( UCHAR * pucFrame, USHORT usLen );
extern char read_scheme(uint8_t num);
extern void save_scheme(uint8_t num);





#endif