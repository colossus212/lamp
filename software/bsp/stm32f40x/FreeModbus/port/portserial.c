/*
 * FreeModbus Libary: STM32 Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.60 2013/08/13 15:07:05 Armink $
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
/*----------------------------- variable ------------------------------------*/
rt_device_t slave_dev;
/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR(void);
static void prvvUARTRxISR(void);
/* ----------------------- Start implementation -----------------------------*/

void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
	if (xRxEnable)
	{
		/* 485通信时，等待串口移位寄存器中的数据发送完成后，再去使能485的接收、失能485的发送
		 * 该延时时间可以结合CPU主频及串口波特率做适当调整
		 * */
		vMBDelay(1000);
		SLAVE_RS485_RECEIVE_MODE;
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	}
	else
	{
		SLAVE_RS485_SEND_MODE;
		USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	}
	if (xTxEnable)
	{
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	}
	else
	{
		USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
	}
}

void vMBPortClose(void)
{
	rt_device_close(slave_dev);
}
//默认一个从机 串口3 波特率可设置  奇偶检验可设置
BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
		eMBParity eParity)
{
	slave_dev = rt_device_find("uart3");
	if(slave_dev == RT_NULL)
	{
		rt_kprintf("can't find usart3 !\n");
		return FALSE;
	}
	else
	{
		rt_device_open(slave_dev,RT_DEVICE_OFLAG_RDWR);
		return TRUE;
	}
}

BOOL xMBPortSerialPutByte(CHAR ucByte)
{
	rt_device_write(slave_dev,0,&ucByte,1);
	return TRUE;
}

BOOL xMBPortSerialGetByte(CHAR * pucByte)
{
	rt_device_read(slave_dev,0,pucByte,1);
	return TRUE;
}

/* 
 * Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
void prvvUARTTxReadyISR(void)
{
	pxMBFrameCBTransmitterEmpty();
}

/* 
 * Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
void prvvUARTRxISR(void)
{
	pxMBFrameCBByteReceived();
}
/*******************************************************************************
 * Function Name  : USART1_IRQHandler
 * Description    : This function handles USART1 global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/

void USART3_IRQHandler(void)
{
	extern struct rt_device uart3_device;
	extern void rt_hw_serial_isr(struct rt_device *device);
	
	rt_interrupt_enter();
	//接收中断
	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		rt_hw_serial_isr(&uart3_device);
		prvvUARTRxISR();
	}
	//发送中断
	if (USART_GetITStatus(USART3, USART_IT_TXE) == SET)
	{
		prvvUARTTxReadyISR();
	}
	rt_interrupt_leave();
}
