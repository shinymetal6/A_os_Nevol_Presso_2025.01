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
 * process_1_comm_cmdparser.c
 *
 *  Created on: Dec 5, 2024
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"
#include "presso.h"
#include "process_1_comm_cmdparser.h"
#include <ctype.h>

extern 	NevolSystem_t	NevolSystem;
extern	uint8_t packed_usb_rx_buffer[USB_RX_BUF_SIZE];

#define USB_BUF_LEN	64

uint8_t Host_pack_USB_packet(uint8_t *usb_rx_buffer,uint8_t len)
{
uint32_t	i;

	for(i=0;i<len;i++)
	{
		i &= (USB_BUF_LEN-1);
		if ((NevolSystem.usb_flags & USB_FLAGS_HEADEROK) != USB_FLAGS_HEADEROK )
		{
			if ( usb_rx_buffer[i] == '<')
			{
				packed_usb_rx_buffer[0] = usb_rx_buffer[i];
				NevolSystem.usb_rx_buffer_index = 1;
				NevolSystem.usb_flags |= USB_FLAGS_HEADEROK;
			}
		}
		else
		{
			packed_usb_rx_buffer[NevolSystem.usb_rx_buffer_index ] = usb_rx_buffer[i];
			if ( packed_usb_rx_buffer[NevolSystem.usb_rx_buffer_index] == '>')
			{
				NevolSystem.usb_flags |= USB_FLAGS_PKTCOMPLETE;
				NevolSystem.usb_flags &= ~USB_FLAGS_HEADEROK;
				NevolSystem.usb_rx_buffer_index = 0;
				return	0;
			}
			NevolSystem.usb_rx_buffer_index++;
		}
	}
	return 1;
}

uint8_t parse_packet(uint8_t *rx_buf)
{
uint16_t	pnum;

	pnum = sscanf((char * )rx_buf,"< %s %d >", (char *)NevolSystem.command_from_host, (int *)&NevolSystem.param_from_host);
	if ( pnum == 1)
	{
		if ( strcmp((char *)NevolSystem.command_from_host,"HLT") == 0 )
			return CMDPARSER_RET_HLT;
	}
	if ( pnum == 2)
	{
		if ( strcmp((char *)NevolSystem.command_from_host,"PRG") == 0 )
			return CMDPARSER_RET_PRG;
		if ( strcmp((char *)NevolSystem.command_from_host,"WAV") == 0 )
			return CMDPARSER_RET_WAV;
		if ( strcmp((char *)NevolSystem.command_from_host,"RUN") == 0 )
			return CMDPARSER_RET_RUN;
		if ( strcmp((char *)NevolSystem.command_from_host,"HLT") == 0 )
			return CMDPARSER_RET_HLT;
	}
	return 0;
}

