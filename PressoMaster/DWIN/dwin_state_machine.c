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
 * dwin_state_machine.c
 *
 *  Created on: Apr 2, 2025
 *      Author: fil
 */
#include "main.h"
#include "../A_os_includes.h"
#include "../presso.h"
#include "dwin_common.h"
#include "dwin_state_machine.h"

uint8_t		loaded_program = 0;
uint8_t		running = 0;
uint8_t		pause = 0;
uint8_t		dwin_value = 0;

extern	uint8_t	hmi_to_seq_mbx[sizeof(uint32_t)];

uint32_t dwin_state_machine_reset (void)
{
	running = 0;
	pause = 0;
	return 0;
}

uint32_t process_from_dwin(uint32_t uart_driver_handle,uint8_t *uart1_rx_buffer,uint32_t uart_rxlen)
{
	switch(uart1_rx_buffer[4] & DWIN_CMDS_MASK)
	{
	case DWIN_PROG_CMD_HB :
		if ( loaded_program )
		{
			if ( uart1_rx_buffer[4] == DWIN_PROG_PLAY)
			{
				if ( running == 0 )
				{
					compile_and_send_5b_dwin_packet(uart_driver_handle,PLAY_PAUSE_BTN_ADDR,PLAY_PAUSE_BTN_PAUSE);
					hmi_to_seq_mbx[0] = CMDPARSER_RET_RUN;
					running = 1;
				}
				else
				{
					if ( pause == 0 )
					{
						hmi_to_seq_mbx[0] = CMDPARSER_RET_PAUSE;
						compile_and_send_5b_dwin_packet(uart_driver_handle,PLAY_PAUSE_BTN_ADDR,PLAY_PAUSE_BTN_PLAY);
						pause = 1;
					}
					else
					{
						hmi_to_seq_mbx[0] = CMDPARSER_RET_UNPAUSE;
						compile_and_send_5b_dwin_packet(uart_driver_handle,PLAY_PAUSE_BTN_ADDR,PLAY_PAUSE_BTN_PAUSE);
						pause = 0;
					}
				}
				hmi_to_seq_mbx[1] = loaded_program;
				mbx_send(PRESSO_SEQUENCER_PROCESS,PRESSO_HMI_MBX,hmi_to_seq_mbx,2);
			}
			if ( uart1_rx_buffer[4] == DWIN_PROG_STOP)
			{
				hmi_to_seq_mbx[0] = CMDPARSER_RET_HLT;
				hmi_to_seq_mbx[1] = loaded_program;
				mbx_send(PRESSO_SEQUENCER_PROCESS,PRESSO_HMI_MBX,hmi_to_seq_mbx,2);
				compile_and_send_5b_dwin_packet(uart_driver_handle,PLAY_PAUSE_BTN_ADDR,PLAY_PAUSE_BTN_PLAY);
				dwin_clear_fields(uart_driver_handle);
				loaded_program = 0;
				running = 0;
				pause = 0;
			}
		}
		break;
	case	DWIN_PROG_LOAD_HB:
		hmi_to_seq_mbx[0] = CMDPARSER_RET_LOAD;
		hmi_to_seq_mbx[1] = loaded_program = (uart1_rx_buffer[4] & 0x0f) + 1;
		mbx_send(PRESSO_SEQUENCER_PROCESS,PRESSO_HMI_MBX,hmi_to_seq_mbx,2);
		break;
	}
	return 0;
}
