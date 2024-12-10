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
 * dwin_criocob_state_machine.c
 *
 *  Created on: Dec 10, 2024
 *      Author: fil
 */

#include "main.h"
#include "../A_os_includes.h"
#include "../presso.h"

#ifdef	MACHINE_IS_CRIOCOB

#include "../process_3_dwin_hmi.h"
#include "dwin_criocob_state_machine.h"
#include "dwin_common.h"

DWIN_criocob_t	DWIN_criocob;

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
	case DWIN_STATE_BIOCRIO_PAGE :
		kbd_k = get_kbd_key(dwin_rx_packet);
		if ( kbd_k == BIO_KEY )
		{
			compile_and_send_7b_dwin_packet(uart2_driver_handle,DWIN_PAGE_ADDRESS,DWIN_BIO_PAGE_DATA);
			NevolSystem.state = DWIN_STATE_BIO_PAGE;
		}
		if ( kbd_k == CRIO_KEY )
		{
			compile_and_send_7b_dwin_packet(uart2_driver_handle,DWIN_PAGE_ADDRESS,DWIN_CRIO_PAGE_DATA);
			NevolSystem.state = DWIN_STATE_CRIO_PAGE;
		}
		break;
	case DWIN_STATE_BIO_PAGE:
		kbd_k = get_kbd_key(dwin_rx_packet);
		switch(kbd_k)
		{
		case	BIO_TIMEPLUS_KEY	:
			DWIN_criocob.bioactive_time++;
			compile_and_send_5b_dwin_packet(uart2_driver_handle,0x2100,0,DWIN_criocob.bioactive_time);
			break;
		case	BIO_TIMEMINUS_KEY	:
			if ( DWIN_criocob.bioactive_time )
			{
				DWIN_criocob.bioactive_time--;
				compile_and_send_5b_dwin_packet(uart2_driver_handle,0x2100,0,DWIN_criocob.bioactive_time);
			}
			break;
		case 	BIO_POWERPLUS_KEY	:
			DWIN_criocob.bioactive_power++;
			compile_and_send_5b_dwin_packet(uart2_driver_handle,0x2100,0,DWIN_criocob.bioactive_power);
			break;
		case	BIO_POWERMINUS_KEY	:
			if ( DWIN_criocob.bioactive_power )
			{
				DWIN_criocob.bioactive_power--;
				compile_and_send_5b_dwin_packet(uart2_driver_handle,0x2100,0,DWIN_criocob.bioactive_power);
			}
			break;
		}
		break;

	case DWIN_STATE_CRIO_PAGE:
		kbd_k = get_kbd_key(dwin_rx_packet);
		switch(kbd_k)
		{
		case	CRIO_TIMEPLUS_KEY	:
			DWIN_criocob.crio_time++;
			compile_and_send_5b_dwin_packet(uart2_driver_handle,0x2100,0,DWIN_criocob.crio_time);
			break;
		case	CRIO_TIMEMINUS_KEY	:
			if ( DWIN_criocob.crio_time )
			{
				DWIN_criocob.crio_time--;
				compile_and_send_5b_dwin_packet(uart2_driver_handle,0x2100,0,DWIN_criocob.crio_time);
			}
			break;
		}
		break;
	case DWIN_STATE_COB_PAGE :
		kbd_k = get_kbd_key(dwin_rx_packet);
		switch(kbd_k)
		{
		case COB_TIMEPLUS_KEY:
			DWIN_criocob.cobroll_time++;
			DWIN_criocob.cobroll_counter = DWIN_criocob.cobroll_counter_timeA = DWIN_criocob.cobroll_counter_timeB = DWIN_criocob.cobroll_counter_timeC = DWIN_criocob.cobroll_time;
			compile_and_send_5b_dwin_packet(uart2_driver_handle,0x2101,0,DWIN_criocob.cobroll_time);
			break;
		case COB_TIMEMINUS_KEY:
			if ( DWIN_criocob.cobroll_time )
			{
				DWIN_criocob.cobroll_time--;
				DWIN_criocob.cobroll_counter = DWIN_criocob.cobroll_counter_timeA = DWIN_criocob.cobroll_counter_timeB = DWIN_criocob.cobroll_counter_timeC = DWIN_criocob.cobroll_time;
				compile_and_send_5b_dwin_packet(uart2_driver_handle,0x2101,0,DWIN_criocob.cobroll_time);
			}
			break;
		case COB_GOSTART_KEY:
			if ( DWIN_criocob.cobroll_time )
			{
				int_time=0;
				DWIN_criocob.biocriocob_status = DWIN_COB_ACTIVE;
				compile_and_send_7b_dwin_packet(uart2_driver_handle,DWIN_PAGE_ADDRESS,DWIN_COB_GOSTART_PAGE_DATA);
			}
			break;
		case COB_GOSTOP_KEY:
			DWIN_criocob.biocriocob_status = 0;
			compile_and_send_7b_dwin_packet(uart2_driver_handle,DWIN_PAGE_ADDRESS,DWIN_COB_PAGE_DATA);
			NevolSystem.state = DWIN_STATE_COB_INIT_PAGE;
			break;
		case COB_A_KEY:
			if ( DWIN_criocob.cobroll_time )
			{
				int_time=0;
				DWIN_criocob.biocriocob_status = DWIN_COB_A_ACTIVE;
				compile_and_send_7b_dwin_packet(uart2_driver_handle,DWIN_PAGE_ADDRESS,DWIN_COB_PAGE_A_DATA);
				DWIN_criocob.cobroll_counter_timeA = DWIN_criocob.cobroll_time;
				NevolSystem.state = DWIN_STATE_COB_A_INIT_PAGE;
			}
			break;
		case COB_B_KEY:
			if ( DWIN_criocob.cobroll_time )
			{
				DWIN_criocob.biocriocob_status = DWIN_COB_B_ACTIVE;
				compile_and_send_7b_dwin_packet(uart2_driver_handle,DWIN_PAGE_ADDRESS,DWIN_COB_PAGE_B_DATA);
				DWIN_criocob.cobroll_counter_timeB = DWIN_criocob.cobroll_time;
				NevolSystem.state = DWIN_STATE_COB_B_INIT_PAGE;
			}
			break;
		case COB_C_KEY:
			if ( DWIN_criocob.cobroll_time )
			{
				DWIN_criocob.biocriocob_status = DWIN_COB_C_ACTIVE;
				compile_and_send_7b_dwin_packet(uart2_driver_handle,DWIN_PAGE_ADDRESS,DWIN_COB_PAGE_C_DATA);
				DWIN_criocob.cobroll_counter_timeC = DWIN_criocob.cobroll_time;
				NevolSystem.state = DWIN_STATE_COB_C_INIT_PAGE;
			}
			break;
		}
		break;
	case DWIN_STATE_COB_A_PAGE :
	case DWIN_STATE_COB_B_PAGE :
	case DWIN_STATE_COB_C_PAGE :
		kbd_k = get_kbd_key(dwin_rx_packet);
		if ( kbd_k == COB_GOSTOP_KEY )
		{
			DWIN_criocob.biocriocob_status = 0;
			compile_and_send_7b_dwin_packet(uart2_driver_handle,DWIN_PAGE_ADDRESS,DWIN_COB_PAGE_DATA);
			NevolSystem.state = DWIN_STATE_COB_INIT_PAGE;
		}
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
				NevolSystem.state = DWIN_STATE_COB_INIT_PAGE;
			}
		}
		break;
	case DWIN_STATE_BIOCRIO_PAGE:
		break;
	case DWIN_STATE_BIO_INIT_PAGE:
		NevolSystem.state = DWIN_STATE_BIO_PAGE;
		break;
	case DWIN_STATE_BIO_PAGE:
			break;
	case DWIN_STATE_CRIO_INIT_PAGE:
		NevolSystem.state = DWIN_STATE_CRIO_PAGE;
		break;
	case DWIN_STATE_CRIO_PAGE:
		break;
	case DWIN_STATE_COB_INIT_PAGE:
		compile_and_send_5b_dwin_packet(uart2_driver_handle,0x2101,0,DWIN_criocob.cobroll_time);
		NevolSystem.state = DWIN_STATE_COB_PAGE;
		break;
	case DWIN_STATE_COB_PAGE:
		if (DWIN_criocob.biocriocob_status == DWIN_COB_ACTIVE)
		{
			int_time++;
			if ( int_time == 10 )
			{
				int_time = 0;
				if ( DWIN_criocob.cobroll_counter )
				{
					DWIN_criocob.cobroll_counter --;
					compile_and_send_5b_dwin_packet(uart2_driver_handle,0x2101,0,DWIN_criocob.cobroll_counter);
				}
				else
				{
					DWIN_criocob.biocriocob_status &= ~DWIN_COB_ACTIVE;
					DWIN_criocob.cobroll_counter = DWIN_criocob.cobroll_time;
					compile_and_send_7b_dwin_packet(uart2_driver_handle,DWIN_PAGE_ADDRESS,DWIN_COB_PAGE_DATA);
					NevolSystem.state = DWIN_STATE_COB_ENDPAGE;
				}
			}
		}
		break;
	case DWIN_STATE_COB_A_INIT_PAGE :
		compile_and_send_5b_dwin_packet(uart2_driver_handle,0x2101,0,DWIN_criocob.cobroll_time);
		NevolSystem.state = DWIN_STATE_COB_A_PAGE;
		break;
	case DWIN_STATE_COB_A_PAGE :
		if (DWIN_criocob.biocriocob_status == DWIN_COB_A_ACTIVE)
		{
			int_time++;
			if ( int_time == 10 )
			{
				int_time = 0;
				if ( DWIN_criocob.cobroll_counter_timeA )
				{
					DWIN_criocob.cobroll_counter_timeA --;
					compile_and_send_5b_dwin_packet(uart2_driver_handle,0x2101,0,DWIN_criocob.cobroll_counter_timeA);
				}
				else
				{
					DWIN_criocob.biocriocob_status &= ~DWIN_COB_A_ACTIVE;
					DWIN_criocob.cobroll_counter_timeA = DWIN_criocob.cobroll_time;
					compile_and_send_7b_dwin_packet(uart2_driver_handle,DWIN_PAGE_ADDRESS,DWIN_COB_PAGE_DATA);
					NevolSystem.state = DWIN_STATE_COB_ENDPAGE;
				}
			}
		}
		break;
	case DWIN_STATE_COB_B_INIT_PAGE :
		compile_and_send_5b_dwin_packet(uart2_driver_handle,0x2101,0,DWIN_criocob.cobroll_time);
		NevolSystem.state = DWIN_STATE_COB_B_PAGE;
		break;
	case DWIN_STATE_COB_B_PAGE :
		if (DWIN_criocob.biocriocob_status == DWIN_COB_B_ACTIVE)
		{
			int_time++;
			if ( int_time == 10 )
			{
				int_time = 0;
				if ( DWIN_criocob.cobroll_counter_timeB )
				{
					DWIN_criocob.cobroll_counter_timeB --;
					compile_and_send_5b_dwin_packet(uart2_driver_handle,0x2101,0,DWIN_criocob.cobroll_counter_timeB);
				}
				else
				{
					DWIN_criocob.biocriocob_status &= ~DWIN_COB_A_ACTIVE;
					DWIN_criocob.cobroll_counter_timeB = DWIN_criocob.cobroll_time;
					compile_and_send_7b_dwin_packet(uart2_driver_handle,DWIN_PAGE_ADDRESS,DWIN_COB_PAGE_DATA);
					NevolSystem.state = DWIN_STATE_COB_ENDPAGE;
				}
			}
		}
		break;
	case DWIN_STATE_COB_C_INIT_PAGE :
		NevolSystem.state = DWIN_STATE_COB_C_PAGE;
		break;
	case DWIN_STATE_COB_C_PAGE :
		if (DWIN_criocob.biocriocob_status == DWIN_COB_C_ACTIVE)
		{
			int_time++;
			if ( int_time == 10 )
			{
				int_time = 0;
				if ( DWIN_criocob.cobroll_counter_timeC )
				{
					DWIN_criocob.cobroll_counter_timeC --;
					compile_and_send_5b_dwin_packet(uart2_driver_handle,0x2101,0,DWIN_criocob.cobroll_counter_timeC);
				}
				else
				{
					DWIN_criocob.biocriocob_status &= ~DWIN_COB_A_ACTIVE;
					DWIN_criocob.cobroll_counter_timeC = DWIN_criocob.cobroll_time;
					compile_and_send_7b_dwin_packet(uart2_driver_handle,DWIN_PAGE_ADDRESS,DWIN_COB_PAGE_DATA);
					NevolSystem.state = DWIN_STATE_COB_ENDPAGE;
				}
			}
		}
		break;
	case DWIN_STATE_COB_ENDPAGE:
		compile_and_send_5b_dwin_packet(uart2_driver_handle,0x2101,0,DWIN_criocob.cobroll_time);
		NevolSystem.state = DWIN_STATE_COB_PAGE;
		break;
	}
	return 0;
}

#endif
