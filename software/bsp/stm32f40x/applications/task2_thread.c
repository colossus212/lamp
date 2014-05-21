#include "stm32f4xx.h"
#include "rtthread.h"
#include "logic.h"
#include "variables.h"
#include "finsh.h"
#include "stdlib.h"

uint8_t ss = 0;
uint8_t start_flag = 0;
uint8_t started_flag = 0;

void rt_task2_thread_entry(void* parameter)
{	
	uint8_t ready_num = 0;//Ԥȼ����
	uint16_t buf = 0, result = 0;
	char i = 0;
	rt_thread_delay(RT_TICK_PER_SECOND*10);

	while(1)
	{
		xMBUtilSetBits( Y, Y11, 1, 1 );//Y11=1;//ˮ�ÿ��رպ�
		rt_thread_delay(RT_TICK_PER_SECOND*5);
	START_LOOP:	
		if((start_flag == 1)&&(started_flag == 0))
		{			
			if((start_flag == 1)/*&&(xMBUtilGetBits(ucMCoilBuf[0], X13, 1 ) == 1)*/)
			{
				xMBUtilSetBits( Y, Y10, 1, 1 );//Y10=1;//�ܵ�Դ�̵����պ�
			}
			else goto EER_LOOP;
			rt_thread_delay(RT_TICK_PER_SECOND*3);
			
			if((start_flag == 1)&&(xMBUtilGetBits(ucMCoilBuf[0], X21, 1 ) == 1))//�ܵ�Դ�̵����պ�
			{
				xMBUtilSetBits( Y, Y9, 1, 1 );//Y9=1;//�������̵����պ�
			}
			else goto EER_LOOP;
			rt_thread_delay(RT_TICK_PER_SECOND*3);
			
			if((start_flag == 1)&&(xMBUtilGetBits(ucMCoilBuf[0], X22, 1 ) == 1))//�������̵����պ�
			{
				xMBUtilSetBits( Y, Y14, 1, 1 );//Y14=1;//�����ѹ�̵����պ�
			}
			else goto EER_LOOP;
			rt_thread_delay(RT_TICK_PER_SECOND*3);
			ready:
			if((start_flag == 1)&&(xMBUtilGetBits(ucMCoilBuf[0], X22, 1 ) == 1))//�������̵����պ�
			{
				ready_num++;
				xMBUtilSetBits( Y, Y12, 1, 1 );//Y12=1;//��Ԥȼ�̵����պ�
				rt_thread_delay(RT_TICK_PER_SECOND/2);
				xMBUtilSetBits( Y, Y12, 1, 0 );//Y12=0;//
				if(xMBUtilGetBits(ucMCoilBuf[0], X20, 1 ) == 1)//Ԥȼ�ɹ�
				{ready_num = 0;}
				else
				{
					rt_thread_delay(RT_TICK_PER_SECOND*10);
					if(ready_num < 5)
					goto ready;
				}
			}
			else goto EER_LOOP;
			rt_thread_delay(RT_TICK_PER_SECOND*3);
			
			if((start_flag == 1)&&(xMBUtilGetBits(ucMCoilBuf[0], X20, 1 ) == 1))//Ԥȼ�ɹ�
			{
				xMBUtilSetBits( Y, Y8, 1, 1 );//Y8=1;//�ŵ�̵����պ�
				rt_thread_delay(RT_TICK_PER_SECOND*5);
			}
			else goto EER_LOOP;
			rt_thread_delay(RT_TICK_PER_SECOND*3);
			
			if((start_flag == 1)&&(xMBUtilGetBits(ucMCoilBuf[0], X23, 1 ) == 1))
			{
//				xMBUtilSetBits( Y, Y14, 1, 1 );//Y14=1;//�����ѹ�̵����պ�
				started_flag = 1;
				goto END_LOOP;
			}
			else goto EER_LOOP;
			rt_thread_delay(RT_TICK_PER_SECOND*3);
		}
		else
		{
			if(start_flag == 0)
			{
				started_flag = 0;
			}
			else
			{
				if(started_flag == 1)//&&(xMBUtilGetBits(ucMCoilBuf[0], X13, 11 ) == 0X7F) \
					&&(xMBUtilGetBits(ucMCoilBuf[0], X20, 4 ) == 0X0F))
				{
					goto END_LOOP;
				}
			}
		}
		EER_LOOP:
		xMBUtilSetBits( Y, Y8, 7, 0X08 );
//		xMBUtilSetBits( ucSCoilBuf, start_up_s, 1, 0 );//
		
		END_LOOP:
//		if(ss) 
//		{
//			TIM_Cmd(TIM8, ENABLE);
//			ss = 0;
//		}
		rt_thread_delay(10);
	}
}

void printf_x(void)
{
	rt_kprintf("x7-x0 = 0x%.2x  ",xMBUtilGetBits(ucMCoilBuf[0], X0, 8 ));
	rt_kprintf("x15-x8 = 0x%.2x  ",xMBUtilGetBits(ucMCoilBuf[0], X8, 8 ));
	rt_kprintf("x23-x16 = 0x%.2x\n",xMBUtilGetBits(ucMCoilBuf[0], X16, 8 ));
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(printf_x, printf_x);
#endif

int task2_thread_start(void)
{
	rt_thread_t task2_thread;

	task2_thread = rt_thread_create("task2_thread",
								rt_task2_thread_entry, RT_NULL,
								4096, 6, 20);

	if (task2_thread != RT_NULL)
		rt_thread_startup(task2_thread);
    
	return 0;
}

















