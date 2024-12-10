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
 * command_parser.c
 *
 *  Created on: Jul 9, 2024
 *      Author: fil
 */
#include "main.h"
#include "A_os_includes.h"

#ifdef NEVOL_2416176_01

#include "nevol_system.h"
#include "activators.h"
#include <ctype.h>

extern	NevolSystem_TypeDef	NevolSystem;

uint8_t Host_pack_USB_packet(uint8_t len)
{
uint32_t	i;

	for(i=0;i<len;i++)
	{
		i &= (USB_BUF_LEN-1);
		if ((NevolSystem.usb_flags & USB_FLAGS_HEADEROK) != USB_FLAGS_HEADEROK )
		{
			if ( NevolSystem.usb_rx_buf_rxed[i] == '<')
			{
				NevolSystem.usb_rx_buf[0] = NevolSystem.usb_rx_buf_rxed[i];
				NevolSystem.usb_rx_buf_index = 1;
				NevolSystem.usb_flags |= USB_FLAGS_HEADEROK;
			}
		}
		else
		{
			NevolSystem.usb_rx_buf[NevolSystem.usb_rx_buf_index ] = NevolSystem.usb_rx_buf_rxed[i];
			if ( NevolSystem.usb_rx_buf[NevolSystem.usb_rx_buf_index] == '>')
			{
				NevolSystem.usb_flags |= USB_FLAGS_PKTCOMPLETE;
				NevolSystem.usb_flags &= ~USB_FLAGS_HEADEROK;
				NevolSystem.usb_rx_buf_index = 0;
				return	0;
			}
			NevolSystem.usb_rx_buf_index++;
		}
	}
	return 1;
}

/*
 * commands
 * L <line> 1 | 0 : sets line <line> to the corresponding state
 * 	example < L 2 1 > -> sets line 2 out on
 * 	note : line ranges from 1 to 16 as on the pcb
 * H <heater_number> <duty> : sets  <heater_number> duty to duty
 * 	example < H 1 40 > -> sets heater 1 to 40%
 * 	note : heater ranges from 1 to 5 as on the pcb
 * 	note : duty ranges from 0 to 100
 * T <heater_number> <temperature> : sets  <number> duty to temperature
 * 	example < T 1 35 > -> sets heater 1 to 35 degrees
 * 	note : heater ranges from 1 to 5 as on the pcb
 * 	note : temperature ranges from 35 to 65 in step of 3 degrees
 * 	note : setting temperature, e.g. , to 36 falls to 35
 * M 1 | 0 : sets motor to the corresponding state
 * 	example < M 1 > -> sets motor out on
 * B <buzzer time> : sets buzzer on for <buzzer time> time
 *  <buzzer time> is in milliSec.
 * 	example < B 1000 > -> sets buzzer on for 1 Sec.
 * P : get pressure
 * 	example < P > -> reply with hex pressure value
 * D <prescaler> : set heater timers prescaler
 * 	example < D 1700 > -> set heater timers prescaler to 1700
 * 	note : minimum value is 1, maximum value is 65535
 * S : status , dumps all
 * 	example < S > -> dumps all variables excluding temperatures
 *				line_status,
				motor_status,
				htr1_period,
				htr2_period,
				htr3_period,
				htr4_period,
				htr5_period,
				adc_data
 * V : dumps temperature
 * 	example < V > -> dumps all temperatures
 * R : emergency reset
 * 	example < R > -> resets all
 */

uint8_t parse_packet(uint8_t *rx_buf)
{
uint16_t	pnum;
char p0;
int	p1,p2,p3;

	pnum = sscanf((char * )rx_buf,"< %c %d %d %d", &p0, &p1,&p2,&p3);

	switch(pnum)
	{
	case	1:
		NevolSystem.command_from_host = toupper(p0);
		NevolSystem.param1_from_host = 0;
		NevolSystem.param2_from_host = 0;
		return pnum;
		break;
	case	2:
		if ( p1 < 65536 )
		{
			NevolSystem.command_from_host = toupper(p0);
			NevolSystem.param1_from_host = p1;
			NevolSystem.param2_from_host = 0;
			return pnum;
		}
		break;
	case	3:
		if  (( p1 < 65536 ) && ( p2 < 65536 ))
		{
			NevolSystem.command_from_host = toupper(p0);
			NevolSystem.param1_from_host = p1;
			NevolSystem.param2_from_host = p2;
			return pnum;
		}
		break;
	default:	return 0;
	}
	return 0;
}

extern	uint16_t	adc_data;

uint8_t System_Process_host_Commands(void)
{
uint8_t		ret_param = 1;

	bzero(NevolSystem.system_tx_buf,USB_BUF_LEN);
	sprintf((char *)NevolSystem.system_tx_buf,"<%c %d %d>\n\r",NevolSystem.command_from_host,NevolSystem.param1_from_host,NevolSystem.param2_from_host);
	NevolSystem.system_tx_buf_len = strlen((char *)NevolSystem.system_tx_buf);
	switch(NevolSystem.command_from_host)
	{
	case	'L':	// set line
		ret_param = 2;
		if ((NevolSystem.param1_from_host < 17) && (NevolSystem.param1_from_host > 0))
		{
			if ( NevolSystem.param2_from_host  < 2)
			{
				line_set(NevolSystem.param1_from_host,NevolSystem.param2_from_host);
				ret_param = 0;
			}
		}
		break;
	case	'H':	// set heater
		ret_param = 2;
		if ((NevolSystem.param1_from_host < 6) && (NevolSystem.param1_from_host > 0))
		{
			if ( NevolSystem.param2_from_host  <= 100)
			{
				heater_set(NevolSystem.param1_from_host,NevolSystem.param2_from_host);
				ret_param = 0;
			}
		}
		break;
	case	'T':	// set heater temperature
		ret_param = 2;
		if ((NevolSystem.param1_from_host < 6) && (NevolSystem.param1_from_host > 0))
		{
			if (( NevolSystem.param2_from_host  <= 65) && ( NevolSystem.param2_from_host  >= 35))
			{
				temperature_set(NevolSystem.param1_from_host,NevolSystem.param2_from_host);
				ret_param = 0;
			}
		}
		break;
	case	'M':	// set motor
		ret_param = 2;
		if (NevolSystem.param1_from_host < 2)
		{
			motor_set(NevolSystem.param1_from_host);
			ret_param = 0;
		}
		break;
	case	'B':	// buzzer on for <param1_from_host> time
		ret_param = 2;
		if (NevolSystem.param1_from_host < 16000)
		{
			buzzer_set(NevolSystem.param1_from_host);
			ret_param = 0;
		}
		break;
	case	'D':	// prescaler set to <param1_from_host>
		ret_param = 2;
		heater_prescaler_set(NevolSystem.param1_from_host);
		ret_param = 0;
		break;
	case	'P':	// get pressure
		sprintf((char *)NevolSystem.system_tx_buf,"< P 0x%04x >\r\n", NevolSystem.adc_data);
		NevolSystem.system_tx_buf_len = strlen((char *)NevolSystem.system_tx_buf);
		ret_param = NevolSystem.command_from_host;
		ret_param = 0;
		break;
	case	'S':	// dump variables
		sprintf((char *)NevolSystem.system_tx_buf,"<0x%04x,%d,%d,%d,%d,%d,%d,0x%04x\r\n",
				NevolSystem.line_status,
				NevolSystem.motor_status,
				NevolSystem.htr1_period,
				NevolSystem.htr2_period,
				NevolSystem.htr3_period,
				NevolSystem.htr4_period,
				NevolSystem.htr5_period,
				NevolSystem.adc_data
				);
		NevolSystem.system_tx_buf_len = strlen((char *)NevolSystem.system_tx_buf);
		ret_param = NevolSystem.command_from_host;
		ret_param = 0;
		break;
	case	'V':	// dump temperatures
		sprintf((char *)NevolSystem.system_tx_buf,"<%d,%d,%d,%d,%d>\r\n",
				NevolSystem.htr1_temperature,
				NevolSystem.htr2_temperature,
				NevolSystem.htr3_temperature,
				NevolSystem.htr4_temperature,
				NevolSystem.htr5_temperature
				);
		NevolSystem.system_tx_buf_len = strlen((char *)NevolSystem.system_tx_buf);
		ret_param = NevolSystem.command_from_host;
		ret_param = 0;
		break;
	case	'R':	// reset all, emergency stop
		sprintf((char *)NevolSystem.system_tx_buf,"< EMERGENCY RESET >\r\n");
		NevolSystem.system_tx_buf_len = strlen((char *)NevolSystem.system_tx_buf);
		ret_param = NevolSystem.command_from_host;
		heater_reset();
		line_reset();
		motor_set(0);
		buzzer_reset();
		ret_param = 0;
		break;
	default:
		ret_param = 1;
		break;
	}
	if ( ret_param == 1 )
	{
		sprintf((char *)NevolSystem.system_tx_buf,"< ERROR : UNKNOWN COMMAND %c (0x%02x) >\r\n",NevolSystem.command_from_host,NevolSystem.command_from_host);
		NevolSystem.system_tx_buf_len = strlen((char *)NevolSystem.system_tx_buf);
	}
	if ( ret_param == 2 )
	{
		sprintf((char *)NevolSystem.system_tx_buf,"< ERROR : PARAMETER ERROR ON COMMAND %c %d %d >\r\n",NevolSystem.command_from_host,NevolSystem.param1_from_host,NevolSystem.param2_from_host);
		NevolSystem.system_tx_buf_len = strlen((char *)NevolSystem.system_tx_buf);
	}
	return ret_param;
}

#endif // #ifdef NEVOL_2416176_01

