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

extern	NevolSystem_typedef	NevolSystem;
extern	UART_HandleTypeDef	huart1;

#define	UART_RX_BUF_SIZE	16
#define	UART_TX_BUF_SIZE	16

uint8_t	uart1_rx_buffer[UART_RX_BUF_SIZE];
uint8_t	uart1_tx_buffer[UART_TX_BUF_SIZE];

uint8_t	hmi_from_seq_mbx[sizeof(uint32_t)];
uint8_t	hmi_to_seq_mbx[sizeof(uint32_t)];

Presso_ee_TypeDef *current_presso_ee;

UART_Drv_TypeDef Uart1_Drv =
{
	.data = uart1_rx_buffer,
	.rx_max_len = UART_RX_BUF_SIZE,
	.uart = &huart1,
	.wakeup_id = WAKEUP_FROM_UART1_IRQ,
	.timeout = 20,
	.flags = UART_USES_DMA_TX | UART_USES_DMA_RX | UART_WAKEUP_ON_RXFULL | UART_WAKEUP_ON_TIMEOUT,
};

uint32_t	uart1_driver_handle;
uint32_t	uart1_rxcntr=0;

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

uint16_t val = 0;

uint32_t	rxcount;
void process_3_dwin_hmi(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint32_t	mbx_size;
uint32_t	cleared=0;

	uart1_driver_handle = uart_register(&Uart1_Drv);
	uart_start_receive(uart1_driver_handle);
	bzero(uart1_rx_buffer,UART_RX_BUF_SIZE);

	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);

	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_MBX | Uart1_Drv.wakeup_id);
		get_wakeup_flags(&wakeup,&flags);

		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			if ( cleared < 5 )
			{
				if (cleared == 0 )
					dwin_clear_fields(uart1_driver_handle);
				if (cleared == 1 )
					compile_and_send_5b_dwin_packet(uart1_driver_handle,PLAY_PAUSE_BTN_ADDR,PLAY_PAUSE_BTN_PLAY);
				if ( cleared == 2 )
					compile_and_send_5b_dwin_packet(uart1_driver_handle,HIGHLIGHT_RED,0);

				cleared ++;
			}
			led_process();
		}
		if (( wakeup & WAKEUP_FROM_MBX) == WAKEUP_FROM_MBX)
		{
			mbx_size = mbx_receive(PRESSO_HMI_MBX,hmi_from_seq_mbx);
			if ( mbx_size )
			{
				if ( hmi_from_seq_mbx[0] == UPDATE_LCD_PARAMS )
				{
					current_presso_ee = get_sequencer_params();
					dwin_update_fields(uart1_driver_handle,current_presso_ee);
				}
				if ( hmi_from_seq_mbx[0] == SEQUENCE_FINISHED )
				{
					compile_and_send_5b_dwin_packet(uart1_driver_handle,PLAY_PAUSE_BTN_ADDR,PLAY_PAUSE_BTN_PLAY);
					dwin_state_machine_reset();
				}
				if ( hmi_from_seq_mbx[0] == ACTIVATE_CODE )
				{
					dwin_highlight_field(uart1_driver_handle,(hmi_from_seq_mbx[2]<<8 ) | hmi_from_seq_mbx[3]);
				}
				if ( hmi_from_seq_mbx[0] == DEACTIVATE_CODE )
				{
					dwin_highlight_field(uart1_driver_handle,(hmi_from_seq_mbx[2]<<8 ) | hmi_from_seq_mbx[3]);
				}
			}
		}
		if (( wakeup & Uart1_Drv.wakeup_id) == Uart1_Drv.wakeup_id)
		{
			if (( flags & WAKEUP_FLAGS_UART_RX) == WAKEUP_FLAGS_UART_RX )
			{
				rxcount = uart_get_rxlen(uart1_driver_handle);
				process_from_dwin(uart1_driver_handle,uart1_rx_buffer,uart_get_rxlen(uart1_driver_handle));
				bzero(uart1_rx_buffer,UART_RX_BUF_SIZE);
			}
		}
	}
}

