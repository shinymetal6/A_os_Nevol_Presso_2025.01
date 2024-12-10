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
 * dwin_common.c
 *
 *  Created on: Dec 10, 2024
 *      Author: fil
 */

#include "main.h"
#include "../A_os_includes.h"
#include "../presso.h"
#include "../process_3_dwin_hmi.h"
#include "dwin_criocob_state_machine.h"
#include "dwin_common.h"

DWIN_packet_t			DWIN_packet;

static void dwinsend(uint32_t uart2_driver_handle,uint32_t pktlen)
{
	uart_send(uart2_driver_handle, (uint8_t *)&DWIN_packet,pktlen);
}

uint32_t compile_and_send_7b_dwin_packet(uint32_t uart2_driver_handle,uint16_t address,uint32_t data)
{
	DWIN_packet.start_flag_1 = HMI_HEADER1;
	DWIN_packet.start_flag_2 = HMI_HEADER2;
	DWIN_packet.number_of_bytes = 0x07;
	DWIN_packet.command = HMI_WRITE_CMD;
	DWIN_packet.address_h = (uint8_t )address>>8;
	DWIN_packet.address_l = (uint8_t )address;
	DWIN_packet.pktbytes[0] = (uint8_t )(data>>24);
	DWIN_packet.pktbytes[1] = (uint8_t )(data>>16);
	DWIN_packet.pktbytes[2] = (uint8_t )(data>>8);
	DWIN_packet.pktbytes[3] = (uint8_t )(data);
	dwinsend(uart2_driver_handle,10);
	return 0;
}

uint32_t compile_and_send_5b_dwin_packet(uint32_t uart2_driver_handle,uint16_t address,uint8_t data0,uint8_t data1)
{
	DWIN_packet.start_flag_1 = HMI_HEADER1;
	DWIN_packet.start_flag_2 = HMI_HEADER2;
	DWIN_packet.number_of_bytes = 0x05;
	DWIN_packet.command = HMI_WRITE_CMD;
	DWIN_packet.address_h = address>>8;
	DWIN_packet.address_l = address;
	DWIN_packet.pktbytes[0] = data0;
	DWIN_packet.pktbytes[1] = data1;
	dwinsend(uart2_driver_handle,8);
	return 0;
}

uint32_t password_manager(uint32_t uart2_driver_handle,uint8_t  *dwin_rx_packet,uint8_t  dwin_rx_packet_len)
{
	if (( dwin_rx_packet[2] == 0x06 ) && ( dwin_rx_packet[4] == 0x20 ))
	{
		if ( dwin_rx_packet[8] == DWIN_KBD_OK)
		{
			NevolSystem.password_index = 10000;
			return (NevolSystem.password == USER_PASSWORD ? 0 : 1);
		}
		else if ( dwin_rx_packet[8] == DWIN_KBD_BACK)
		{
		}
		else
		{
			NevolSystem.password += ((dwin_rx_packet[8]&0x0f) * NevolSystem.password_index);
			compile_and_send_5b_dwin_packet(uart2_driver_handle,0x2050,NevolSystem.password>> 8 , NevolSystem.password);
			NevolSystem.password_index /= 10;
		}
	}
	return 1;
}

uint16_t get_kbd_key(uint8_t  *dwin_rx_packet)
{
	if (( dwin_rx_packet[0] == 0x5a ) && ( dwin_rx_packet[1] == 0xa5 ))
	{
		if (( dwin_rx_packet[2] == 0x06 ) && ( dwin_rx_packet[3] == 0x83 ))
		{
			if (( dwin_rx_packet[4] == 0x20 ) && ( dwin_rx_packet[5] == 0x00 ))
			{
				return dwin_rx_packet[7] << 8 | dwin_rx_packet[8];
			}
		}
	}
	return 0;
}


uint32_t check_std_reply(uint8_t  *dwin_rx_packet)
{
	if (( dwin_rx_packet[0] == 0x5a ) && ( dwin_rx_packet[1] == 0xa5 ))
		if (( dwin_rx_packet[2] == 0x03 ) && ( dwin_rx_packet[3] == 0x82 ))
			if (( dwin_rx_packet[4] == 0x4f ) && ( dwin_rx_packet[5] == 0x4b ))
				return 0;
	return 1;
}
