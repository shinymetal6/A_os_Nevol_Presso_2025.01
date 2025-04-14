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

DWIN_sm_typedef	DWIN_sm;

extern	uint8_t	hmi_to_seq_mbx[sizeof(uint32_t)];

uint32_t dwin_state_machine_reset (void)
{
	DWIN_sm.running = 0;
	DWIN_sm.pause = 0;
	return 0;
}

uint16_t	backup_gpio[EE_MAX_LINE_NUMBER];
uint16_t	new_gpio[EE_MAX_LINE_NUMBER];

void mod_prog_out_following_to_active(uint8_t program_step)
{
uint32_t	i;
uint32_t	thebit = 1 << program_step;
	for(i=0;i<Presso_ee.program_number_of_lines;i++)
	{
		backup_gpio[i] = Presso_ee.Presso_ee_line[i].gpio;
		if ( i >= program_step )
		{
			if ( Presso_ee.Presso_ee_line[i].gpio )
				new_gpio[i] = Presso_ee.Presso_ee_line[i].gpio | thebit;
		}
		else
			new_gpio[i] = Presso_ee.Presso_ee_line[i].gpio;
		Presso_ee.Presso_ee_line[i].gpio = new_gpio[i];
	}
}

void mod_prog_out_following_to_original(uint8_t program_step)
{
uint32_t	i;
	for(i=0;i<Presso_ee.program_number_of_lines;i++)
	{
		Presso_ee.Presso_ee_line[i].gpio = backup_gpio[i];
		backup_gpio[i] = 0;
	}
}

uint16_t mod_prog_time(uint8_t program , uint8_t data)
{
	Presso_ee.Presso_ee_line[program].sector_time = data;
	return Presso_ee.Presso_ee_line[program].sector_time;
}

uint16_t mod_prog_pressure(uint8_t program , uint8_t data)
{
	Presso_ee.Presso_ee_line[program].sector_pressure = data;
	return Presso_ee.Presso_ee_line[program].sector_pressure;
}

#ifdef OLD
uint32_t process_from_dwin_OLD(uint32_t uart_driver_handle,uint8_t *uart1_rx_buffer,uint32_t uart_rxlen)
{
uint16_t ret_val;
	if ( uart_rxlen  )
	{
		switch(uart1_rx_buffer[4] & DWIN_CMDS_MASK)
		{
		case DWIN_PROG_CMD_HB :
			if ( program_loaded )
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
					hmi_to_seq_mbx[1] = program_loaded;
					mbx_send(PRESSO_SEQUENCER_PROCESS,PRESSO_HMI_MBX,hmi_to_seq_mbx,2);
				}
				if ( uart1_rx_buffer[4] == DWIN_PROG_STOP)
				{
					hmi_to_seq_mbx[0] = CMDPARSER_RET_HLT;
					hmi_to_seq_mbx[1] = program_loaded;
					mbx_send(PRESSO_SEQUENCER_PROCESS,PRESSO_HMI_MBX,hmi_to_seq_mbx,2);
					compile_and_send_5b_dwin_packet(uart_driver_handle,PLAY_PAUSE_BTN_ADDR,PLAY_PAUSE_BTN_PLAY);
					dwin_clear_fields(uart_driver_handle);
					program_loaded = 0;
					running = 0;
					pause = 0;
				}
			}
			break;
		case	DWIN_PROG_LOAD_HB:
			hmi_to_seq_mbx[0] = CMDPARSER_RET_LOAD;
			hmi_to_seq_mbx[1] = program_loaded = (uart1_rx_buffer[4] & 0x0f) + 1;
			mbx_send(PRESSO_SEQUENCER_PROCESS,PRESSO_HMI_MBX,hmi_to_seq_mbx,2);
			break;
			/*
		case DWIN_PROG_CMD_MODCYCLE:
			if ( program_loaded )
			{
				if ( modified == 0 )
					hmi_to_seq_mbx[0] = CMDPARSER_RET_FORCEON;
				else
					hmi_to_seq_mbx[0] = CMDPARSER_RET_DEFAULT;
				modified++;
				modified &= 1;
				hmi_to_seq_mbx[1] = (uart1_rx_buffer[4] & 0x0f) + 1;
				mbx_send(PRESSO_SEQUENCER_PROCESS,PRESSO_HMI_MBX,hmi_to_seq_mbx,2);
			}
			break;
			*/
		case DWIN_PROG_CMD_INCTIME:
			if ( program_loaded )
			{
				ret_val = mod_prog_time_inc(uart1_rx_buffer[4]&0x0f);
				compile_and_send_5b_dwin_packet_nowait(uart_driver_handle,TSECTOR_BASE_ADDRESS+((uart1_rx_buffer[4]&0x0f)*0x100),ret_val);
			}
			break;
			/*
		case DWIN_PROG_CMD_DECTIME:
			if ( program_loaded )
			{
				ret_val = mod_prog_time_dec(uart1_rx_buffer[4]&0x0f);
				compile_and_send_5b_dwin_packet_nowait(uart_driver_handle,TSECTOR_BASE_ADDRESS+((uart1_rx_buffer[4]&0x0f)*0x100),ret_val);
			}
			break;
		case DWIN_PROG_CMD_INCPRESSURE:
			if ( program_loaded )
			{
				ret_val = mod_prog_pressure_inc(uart1_rx_buffer[4]&0x0f);
				compile_and_send_5b_dwin_packet_nowait(uart_driver_handle,PRESSURE_BASE_ADDRESS+((uart1_rx_buffer[4]&0x0f)*0x100),ret_val);
			}
			break;
		case DWIN_PROG_CMD_DECPRESSURE:
			if ( program_loaded )
			{
				ret_val = mod_prog_pressure_dec(uart1_rx_buffer[4]&0x0f);
				compile_and_send_5b_dwin_packet_nowait(uart_driver_handle,PRESSURE_BASE_ADDRESS+((uart1_rx_buffer[4]&0x0f)*0x100),ret_val);
			}
			break;
			*/
		}
	}
	return 0;
}
#else
uint32_t process_from_dwin(uint32_t uart_driver_handle,uint8_t *uart1_rx_buffer,uint32_t uart_rxlen)
{
DWIN_packet_typedef	*DWIN_rxed_packet = (DWIN_packet_typedef *)uart1_rx_buffer;

	if ( DWIN_rxed_packet->number_of_bytes != 6 )
		return 1;
	if ( DWIN_rxed_packet->address_h < 0x0f )
	{
		switch(DWIN_rxed_packet->address_h)
		{
		case	7 : DWIN_sm.program_loaded = 1;break;
		case	6 : DWIN_sm.program_loaded = 2;break;
		case	5 : DWIN_sm.program_loaded = 3;break;
		case	4 : DWIN_sm.program_loaded = 4;break;
		case	3 : DWIN_sm.program_loaded = 5;break;
		case	2 : DWIN_sm.program_loaded = 6;break;
		case	1 : DWIN_sm.program_loaded = 7;break;
		}
		hmi_to_seq_mbx[0] = CMDPARSER_RET_LOAD;
		hmi_to_seq_mbx[1] = DWIN_sm.program_loaded;
		mbx_send(PRESSO_SEQUENCER_PROCESS,PRESSO_HMI_MBX,hmi_to_seq_mbx,2);
	}
	switch(DWIN_rxed_packet->address_h & DWIN_CMDS_MASK)
	{
	case	DWIN_PROG_LOAD_HB:
		hmi_to_seq_mbx[0] = CMDPARSER_RET_LOAD;
		hmi_to_seq_mbx[1] = DWIN_sm.program_loaded = (DWIN_rxed_packet->address_h & 0x0f) + 1;
		mbx_send(PRESSO_SEQUENCER_PROCESS,PRESSO_HMI_MBX,hmi_to_seq_mbx,2);
		break;
	case DWIN_PROG_CMD_HB :
		if ( DWIN_sm.program_loaded )
		{
			if ( DWIN_rxed_packet->address_h == DWIN_PROG_PLAY)
			{
				if ( DWIN_sm.running == 0 )
				{
					compile_and_send_5b_dwin_packet(uart_driver_handle,PLAY_PAUSE_BTN_ADDR,PLAY_PAUSE_BTN_PAUSE);
					hmi_to_seq_mbx[0] = CMDPARSER_RET_RUN;
					DWIN_sm.running = 1;
				}
				else
				{
					if ( DWIN_sm.pause == 0 )
					{
						hmi_to_seq_mbx[0] = CMDPARSER_RET_PAUSE;
						compile_and_send_5b_dwin_packet(uart_driver_handle,PLAY_PAUSE_BTN_ADDR,PLAY_PAUSE_BTN_PLAY);
						DWIN_sm.pause = 1;
					}
					else
					{
						hmi_to_seq_mbx[0] = CMDPARSER_RET_UNPAUSE;
						compile_and_send_5b_dwin_packet(uart_driver_handle,PLAY_PAUSE_BTN_ADDR,PLAY_PAUSE_BTN_PAUSE);
						DWIN_sm.pause = 0;
					}
				}
				hmi_to_seq_mbx[1] = DWIN_sm.program_loaded;
				mbx_send(PRESSO_SEQUENCER_PROCESS,PRESSO_HMI_MBX,hmi_to_seq_mbx,2);
			}
			if ( DWIN_rxed_packet->address_h == DWIN_PROG_STOP)
			{
				hmi_to_seq_mbx[0] = CMDPARSER_RET_HLT;
				hmi_to_seq_mbx[1] = DWIN_sm.program_loaded;
				mbx_send(PRESSO_SEQUENCER_PROCESS,PRESSO_HMI_MBX,hmi_to_seq_mbx,2);
				compile_and_send_5b_dwin_packet(uart_driver_handle,PLAY_PAUSE_BTN_ADDR,PLAY_PAUSE_BTN_PLAY);
				dwin_clear_fields(uart_driver_handle);
				DWIN_sm.program_loaded = 0;
				DWIN_sm.running = 0;
				DWIN_sm.pause = 0;
			}
		}
		break;
	case DWIN_PROG_CMD_MODTIME:
		if ( DWIN_sm.program_loaded )
			mod_prog_time((DWIN_rxed_packet->address_h & 0x0f),DWIN_rxed_packet->data1_h);
		break;
	case DWIN_PROG_CMD_MODPRESSURE:
		if ( DWIN_sm.program_loaded )
			mod_prog_pressure((DWIN_rxed_packet->address_h & 0x0f),DWIN_rxed_packet->data1_h);
		break;
	case DWIN_PROG_CMD_MODCYCLE:
		if ( DWIN_sm.program_loaded )
		{
			if ( DWIN_sm.modified == 0 )
				mod_prog_out_following_to_active(DWIN_rxed_packet->address_h & 0x0f);
			else
				mod_prog_out_following_to_original(DWIN_rxed_packet->address_h & 0x0f);
			DWIN_sm.modified++;
			DWIN_sm.modified &= 1;
		}
		break;
	}
	return 0;
}
#endif
