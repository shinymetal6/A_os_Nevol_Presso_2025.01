/* 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Project : A_os
*/
/*
 * process_3_dwin_hmi.c
 *
 *  Created on: Dec 9, 2024
 *      Author: fil
 */
#include "main.h"
#include "A_os_includes.h"
#include "presso.h"
#include "process_3_dwin_hmi.h"
#include "DWIN/dwin_state_machine.h"
#include "DWIN/dwin_common.h"

extern	NevolSystem_t	NevolSystem;
extern	DWIN_packet_t			DWIN_packet;

extern	UART_HandleTypeDef	huart2;

#define	UART_RX_BUF_SIZE	64
#define	UART_TX_BUF_SIZE	64
uint8_t	uart2_rx_buffer[UART_RX_BUF_SIZE];
uint8_t	uart2_tx_buffer[UART_TX_BUF_SIZE];


UART_Drv_TypeDef Uart2_Drv =
{
	.data = uart2_rx_buffer,
	.rx_max_len = UART_RX_BUF_SIZE,
	.uart = &huart2,
	.wakeup_id = WAKEUP_FROM_UART2_IRQ,
	.timeout = 50,
	.flags = UART_USES_DMA_TX | UART_USES_DMA_RX | UART_WAKEUP_ON_RXFULL | UART_WAKEUP_ON_TIMEOUT,
	//.flags = UART_WAKEUP_ON_TIMEOUT,
};

uint32_t	uart2_driver_handle;
uint32_t	uart2_rxcntr=0;

uint8_t led_cntr=0;
void led_process(void)
{
	led_cntr++;
	switch(led_cntr)
	{
	case 7 :
	case 9 :
		  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
		  break;
	case 10 :
		led_cntr = 0;
		break;
	default :
		  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
		  break;
	}
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

void process_3_dwin_hmi(uint32_t process_id)
{
uint32_t	wakeup,flags;

	NevolSystem.machine = BIOACTIVE_CRIO;
	NevolSystem.powerup_val = POWERUP_WAIT;

	uart2_driver_handle = uart_register(&Uart2_Drv);
	uart_start_receive(uart2_driver_handle);

	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	bzero(uart2_rx_buffer,UART_RX_BUF_SIZE);
	while(1)
	{
		wait_event(EVENT_TIMER | Uart2_Drv.wakeup_id);
		get_wakeup_flags(&wakeup,&flags);

		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			led_process();
			dwin_state_machine(uart2_driver_handle);
		}
		if (( wakeup & Uart2_Drv.wakeup_id) == Uart2_Drv.wakeup_id)
		{
			if (( flags & WAKEUP_FLAGS_UART_RX) == WAKEUP_FLAGS_UART_RX )
			{
				process_from_dwin(uart2_driver_handle,uart2_rx_buffer,uart_get_rxlen(uart2_driver_handle));
				bzero(uart2_rx_buffer,UART_RX_BUF_SIZE);
			}
		}
	}
}

