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
 *  Created on: Dec 10, 2024
 *      Author: fil
 */

#include "main.h"
#include "../A_os_includes.h"
#include "../presso.h"
#include "../process_3_dwin_hmi.h"
#include "dwin_state_machine.h"
#include "dwin_common.h"

uint8_t	int_time = 0;
void process_from_dwin(uint32_t uart2_driver_handle,uint8_t  *dwin_rx_packet,uint8_t  dwin_rx_packet_len)
{
uint16_t kbd_k;
	switch(NevolSystem.state)
	{
	case DWIN_STATE_POWER_ON:
		if ( check_std_reply(dwin_rx_packet) == 0 )
			NevolSystem.status |= DWIN_STATUS_DISPLAY_OK;
		break;
	case DWIN_STATE_KBD :
		if (( dwin_rx_packet[2] == 0x06 ) && ( dwin_rx_packet[4] == 0x20 ))
		{
			if ( password_manager(uart2_driver_handle,dwin_rx_packet,dwin_rx_packet_len) == 0 )
				NevolSystem.status |= DWIN_STATUS_OPERATIONAL;
		}
		break;
	case DWIN_STATE_COB_PAGE :
		kbd_k = get_kbd_key(dwin_rx_packet);
		switch(kbd_k)
		{
		case COB_TIMEPLUS_KEY:
			int_time++;
			compile_and_send_5b_dwin_packet(uart2_driver_handle,0x2101,0x43,int_time);
			break;
		case COB_TIMEMINUS_KEY:
			if ( int_time )
			{
				int_time--;
				compile_and_send_5b_dwin_packet(uart2_driver_handle,0x2101,0x43,int_time);
			}
			break;
		case COB_GOSTART_KEY:
			compile_and_send_7b_dwin_packet(uart2_driver_handle,DWIN_PAGE_ADDRESS,DWIN_COB_GOSTART_PAGE_DATA);
			break;
		case COB_A_KEY:
			compile_and_send_7b_dwin_packet(uart2_driver_handle,DWIN_PAGE_ADDRESS,DWIN_COB_PAGE_A_DATA);
			break;
		case COB_B_KEY:
			compile_and_send_7b_dwin_packet(uart2_driver_handle,DWIN_PAGE_ADDRESS,DWIN_COB_PAGE_B_DATA);
			break;
		case COB_C_KEY:
			compile_and_send_7b_dwin_packet(uart2_driver_handle,DWIN_PAGE_ADDRESS,DWIN_COB_PAGE_C_DATA);
			break;
		}
		case DWIN_STATE_BIOCRIO_PAGE :
			kbd_k = get_kbd_key(dwin_rx_packet);
			if ( kbd_k == BIO_KEY )
				compile_and_send_7b_dwin_packet(uart2_driver_handle,DWIN_PAGE_ADDRESS,DWIN_BIO_PAGE_DATA);
			if ( kbd_k == CRIO_KEY )
				compile_and_send_7b_dwin_packet(uart2_driver_handle,DWIN_PAGE_ADDRESS,DWIN_CRIO_PAGE_DATA);

			break;
	}
}

uint8_t dwin_state_machine(uint32_t uart2_driver_handle)
{
	switch(NevolSystem.state)
	{
	case DWIN_STATE_RESET:
		NevolSystem.status &= ~DWIN_STATUS_OPERATIONAL;
		NevolSystem.password = 0;
		NevolSystem.password_index = 10000;
		if ( NevolSystem.powerup_val )
		{
			NevolSystem.powerup_val--;
			if ( NevolSystem.powerup_val == 0 )
			{
				compile_and_send_7b_dwin_packet(uart2_driver_handle,DWIN_PAGE_ADDRESS,DWIN_KBD_PAGE_DATA);
				NevolSystem.state = DWIN_STATE_POWER_ON;
			}
		}
		break;
	case DWIN_STATE_POWER_ON:
		if (( NevolSystem.status & DWIN_STATUS_DISPLAY_OK ) == DWIN_STATUS_DISPLAY_OK)
			NevolSystem.state = DWIN_STATE_KBD;
		break;
	case DWIN_STATE_KBD:
		if (( NevolSystem.status & DWIN_STATUS_OPERATIONAL) == DWIN_STATUS_OPERATIONAL)
		{
			if ( NevolSystem.machine == BIOACTIVE_CRIO)
			{
				compile_and_send_7b_dwin_packet(uart2_driver_handle,DWIN_PAGE_ADDRESS,DWIN_BIOACTIVE_CRIO_PAGE_DATA);
				NevolSystem.state = DWIN_STATE_BIOCRIO_PAGE;
			}
			if ( NevolSystem.machine == COBROLL)
			{
				compile_and_send_7b_dwin_packet(uart2_driver_handle,DWIN_PAGE_ADDRESS,DWIN_COB_PAGE_DATA);
				NevolSystem.state = DWIN_STATE_COB_PAGE;
			}
		}
		break;
	case DWIN_STATE_BIOCRIO_PAGE:

		break;
	case DWIN_STATE_COB_PAGE:
		break;
	}
	return 0;
}

