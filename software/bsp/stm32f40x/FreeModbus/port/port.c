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
  * File: $Id: portevent.c,v 1.60 2013/12/28 9:18:05 Armink $
  */

/* ----------------------- System includes --------------------------------*/

/* ----------------------- Modbus includes ----------------------------------*/
#include "port.h"
/* ----------------------- Variables ----------------------------------------*/
extern uint8_t flag3_rxen, flag3_txen, flag6_rxen, flag6_txen;
/* ----------------------- Start implementation -----------------------------*/
void EnterCriticalSection(void)
{
//	//关闭全局中断
//	__disable_irq();
	USART_ITConfig(USART6, USART_IT_RXNE, DISABLE);
	USART_ITConfig(USART6, USART_IT_TXE, DISABLE);
	USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
	USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
}

void ExitCriticalSection(void)
{
//	//开启全局中断
//	__enable_irq();
	if (flag3_rxen)
	{
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	}
	else
	{
		USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
	}
	if (flag3_txen)
	{
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
	}
	else
	{
		USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
	}

	
	if (flag6_rxen)
	{
		USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
	}
	else
	{
		USART_ITConfig(USART6, USART_IT_RXNE, DISABLE);
	}
	if (flag6_txen)
	{
		USART_ITConfig(USART6, USART_IT_TXE, ENABLE);
	}
	else
	{
		USART_ITConfig(USART6, USART_IT_TXE, DISABLE);
	}
}

void vMBDelay(ULONG nCount)
{
  for(; nCount > 0;nCount--);
}
