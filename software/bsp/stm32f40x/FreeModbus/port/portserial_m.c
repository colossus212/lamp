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
 * File: $Id: portserial_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions $
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "finsh.h"

#define UART6_GPIO_TX	    GPIO_Pin_14
#define UART6_TX_PIN_SOURCE GPIO_PinSource14
#define UART6_GPIO_RX	    GPIO_Pin_9
#define UART6_RX_PIN_SOURCE GPIO_PinSource9
#define UART6_GPIO	    	GPIOG

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR(void);
static void prvvUARTRxISR(void);
/* ----------------------- Start implementation -----------------------------*/

void vMBMasterPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
	if (xRxEnable)
	{
//		vMBDelay(1000);
//		MASTER_RS485_RECEIVE_MODE;
		USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
	}
	else
	{
//		MASTER_RS485_SEND_MODE;
		USART_ITConfig(USART6, USART_IT_RXNE, DISABLE);
	}
	if (xTxEnable)
	{
		USART_ITConfig(USART6, USART_IT_TXE, ENABLE);
	}
	else
	{
		USART_ITConfig(USART6, USART_IT_TXE, DISABLE);
	}
}

void vMBMasterPortClose(void)
{
	USART_ITConfig(USART6, USART_IT_TXE | USART_IT_RXNE, DISABLE);
	USART_Cmd(USART6, DISABLE);
}
//Ĭ��һ������ ����2 �����ʿ�����  ��ż���������
BOOL xMBMasterPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
		eMBParity eParity)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//======================ʱ�ӳ�ʼ��=======================================
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
	//======================IO��ʼ��=======================================	
	//USART6_TX
	GPIO_InitStructure.GPIO_Pin   = UART6_GPIO_TX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	//USART6_RX
	GPIO_InitStructure.GPIO_Pin   = UART6_GPIO_RX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(UART6_GPIO, UART6_TX_PIN_SOURCE, GPIO_AF_USART6);
    GPIO_PinAFConfig(UART6_GPIO, UART6_RX_PIN_SOURCE, GPIO_AF_USART6);
	//======================���ڳ�ʼ��=======================================
	USART_InitStructure.USART_BaudRate = ulBaudRate;
	//����У��ģʽ
	switch (eParity)
	{
	case MB_PAR_NONE: //��У��
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		break;
	case MB_PAR_ODD: //��У��
		USART_InitStructure.USART_Parity = USART_Parity_Odd;
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;
		break;
	case MB_PAR_EVEN: //żУ��
		USART_InitStructure.USART_Parity = USART_Parity_Even;
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;
		break;
	default:
		return FALSE;
	}

	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_HardwareFlowControl =
			USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	if (ucPORT != 6)
		return FALSE;

	ENTER_CRITICAL_SECTION(); //��ȫ���ж�

	USART_Init(USART6, &USART_InitStructure);
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART6, ENABLE);

	//=====================�жϳ�ʼ��======================================
	//����NVIC���ȼ�����ΪGroup2��0-3��ռʽ���ȼ���0-3����Ӧʽ���ȼ�
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	EXIT_CRITICAL_SECTION(); //��ȫ���ж�

	return TRUE;
}

BOOL xMBMasterPortSerialPutByte(CHAR ucByte)
{
	USART_SendData(USART6, ucByte);
	return TRUE;
}
void send_usart6(CHAR ucByte)
{
	USART_SendData(USART6, ucByte);
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(send_usart6, send_usart6);
#endif

BOOL xMBMasterPortSerialGetByte(CHAR * pucByte)
{
	*pucByte = USART_ReceiveData(USART6);
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
	pxMBMasterFrameCBTransmitterEmpty();
}

/* 
 * Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
void prvvUARTRxISR(void)
{
	pxMBMasterFrameCBByteReceived();
}
/*******************************************************************************
 * Function Name  : USART6_IRQHandler
 * Description    : This function handles USART6 global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void USART6_IRQHandler(void)
{
	rt_interrupt_enter();
	//�������
	if (USART_GetFlagStatus(USART6, USART_FLAG_ORE) == SET)
	{
		prvvUARTRxISR();
	}
	//�����ж�
	if (USART_GetITStatus(USART6, USART_IT_RXNE) == SET)
	{
		USART_ClearITPendingBit(USART6, USART_IT_RXNE);
		prvvUARTRxISR();
	}
	//�����ж�
	if (USART_GetITStatus(USART6, USART_IT_TXE) == SET)
	{
		prvvUARTTxReadyISR();
	}
	rt_interrupt_leave();
}

#endif
