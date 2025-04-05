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
 *  Created on: Apr 3, 2025
 *      Author: fil
 */

#include "main.h"
#include "../A_os_includes.h"
#include "../presso.h"
#include "dwin_common.h"

DWIN_packet_t	DWIN_packet;

static void dwinsend(uint32_t uart_driver_handle,uint32_t pktlen)
{
	uart_send(uart_driver_handle, (uint8_t *)&DWIN_packet,pktlen);
}

uint32_t compile_and_send_7b_dwin_packet(uint32_t uart_driver_handle,uint16_t address,uint32_t data)
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
	dwinsend(uart_driver_handle,10);
	return 0;
}

uint32_t compile_and_send_5b_dwin_packet(uint32_t uart_driver_handle,uint16_t address,uint16_t data)
{
	DWIN_packet.start_flag_1 = HMI_HEADER1;
	DWIN_packet.start_flag_2 = HMI_HEADER2;
	DWIN_packet.number_of_bytes = 0x05;
	DWIN_packet.command = HMI_WRITE_CMD;
	DWIN_packet.address_h = address>>8;
	DWIN_packet.address_l = address;
	DWIN_packet.pktbytes[0] = data >> 8;
	DWIN_packet.pktbytes[1] = data;
	dwinsend(uart_driver_handle,8);
	return 0;
}
