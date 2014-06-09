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
#include "finsh.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "logic.h"

#define UART3_GPIO_TX	    GPIO_Pin_10
#define UART3_TX_PIN_SOURCE GPIO_PinSource10
#define UART3_GPIO_RX	    GPIO_Pin_11
#define UART3_RX_PIN_SOURCE GPIO_PinSource11
#define UART3_GPIO	    	GPIOB

/*----------------------------- variable ------------------------------------*/
rt_device_t slave_dev;
/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR(void);
static void prvvUARTRxISR(void);
/* ----------------------- Start implementation -----------------------------*/
uint32_t x= 10000;
void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
	
	if (xRxEnable)
	{
		/* 485ͨ��ʱ���ȴ�������λ�Ĵ����е����ݷ�����ɺ���ȥʹ��485�Ľ��ա�ʧ��485�ķ���
		 * ����ʱʱ����Խ��CPU��Ƶ�����ڲ��������ʵ�����
		 * */
		vMBDelay(3200);
		SLAVE_RS485_RECEIVE_MODE;
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	}
	else
	{
		SLAVE_RS485_SEND_MODE;
		USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
	}
	if (xTxEnable)
	{
//		SLAVE_RS485_SEND_MODE;
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
	}
	else
	{
//		SLAVE_RS485_RECEIVE_MODE;
		USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
	}
}

void vMBPortClose(void)
{
	USART_ITConfig(USART3, USART_IT_TXE | USART_IT_RXNE, DISABLE);
	USART_Cmd(USART3, DISABLE);
}
//Ĭ��һ���ӻ� ����3 �����ʿ�����  ��ż���������
BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
		eMBParity eParity)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//======================ʱ�ӳ�ʼ��=======================================
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	//======================IO��ʼ��=======================================	
	//USART3_TX
	GPIO_InitStructure.GPIO_Pin   = UART3_GPIO_TX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(UART3_GPIO, &GPIO_InitStructure);
	//USART3_RX
	GPIO_InitStructure.GPIO_Pin   = UART3_GPIO_RX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(UART3_GPIO, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(UART3_GPIO, UART3_TX_PIN_SOURCE, GPIO_AF_USART3);
    GPIO_PinAFConfig(UART3_GPIO, UART3_RX_PIN_SOURCE, GPIO_AF_USART3);
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
	if (ucPORT != 3)
		return FALSE;

	ENTER_CRITICAL_SECTION(); //��ȫ���ж�

	USART_Init(USART3, &USART_InitStructure);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART3, ENABLE);

	//=====================�жϳ�ʼ��======================================
	//����NVIC���ȼ�����ΪGroup2��0-3��ռʽ���ȼ���0-3����Ӧʽ���ȼ�
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	EXIT_CRITICAL_SECTION(); //��ȫ���ж�
	return TRUE;
//	slave_dev = rt_device_find("uart3");
//	if(slave_dev == RT_NULL)
//	{
//		rt_kprintf("can't find usart3 !\n");
//		return FALSE;
//	}
//	else
//	{
//		rt_device_open(slave_dev,RT_DEVICE_OFLAG_RDWR);
////		SLAVE_RS485_RECEIVE_MODE;
//		return TRUE;
//	}
	
}

BOOL xMBPortSerialPutByte(CHAR ucByte)
{
//	SLAVE_RS485_SEND_MODE;
//	rt_device_write(slave_dev,0,&ucByte,1);
	USART_SendData(USART3, ucByte);
	return TRUE;
}

BOOL xMBPortSerialGetByte(CHAR * pucByte)
{
//	SLAVE_RS485_RECEIVE_MODE;
//	rt_device_read(slave_dev,0,pucByte,1);
	*pucByte = USART_ReceiveData(USART3);
	return TRUE;
}
void send_usart3(char dataa)
{
	xMBPortSerialPutByte( dataa);
}
#ifdef FINSH_USING_SYMTAB
FINSH_FUNCTION_EXPORT(send_usart3, send_usart3);
#endif
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
	rt_interrupt_enter();
	//�������
	if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) == SET)
	{
		USART_ClearITPendingBit(USART3, USART_FLAG_ORE);
		prvvUARTRxISR();
	}
	//�����ж�
	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		prvvUARTRxISR();
	}
	//�����ж�
	if (USART_GetITStatus(USART3, USART_IT_TXE) == SET)
	{
		USART_ClearITPendingBit(USART3, USART_IT_TXE);
		prvvUARTTxReadyISR();
	}
	rt_interrupt_leave();
}

//void USART3_IRQHandler(void)
//{
//	extern struct rt_device uart3_device;
//	extern void rt_hw_serial_isr(struct rt_device *device);
//	
//	rt_interrupt_enter();
//	//�����ж�
//	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
//	{
////		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
//		rt_hw_serial_isr(&uart3_device);
//		prvvUARTRxISR();
//	}
//	//�����ж�
//	if (USART_GetITStatus(USART3, USART_IT_TXE) == SET)
//	{
//		USART_ClearITPendingBit(USART3, USART_IT_TC);
//		prvvUARTTxReadyISR();
//	}
//	rt_interrupt_leave();
//}
