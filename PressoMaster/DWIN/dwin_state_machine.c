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

DWIN_menus_t	DWIN_menus[] =
{
		{
				0x70,
				1,
		},
		{
				0x71,
				2,
		},
		{
				0x72,
				3,
		},
		{
				0x73,
				4,
		},
		{
				0x74,
				5,
		},
		{
				0,
				0,
		},
};

DWIN_values_t	DWIN_time_values[] =
{
	{
		0x60,0,
	},
	{
		0x61,0,
	},
	{
		0x62,0,
	},
	{
		0x63,0,
	},
	{
		0x64,0,
	},
	{
		0x65,0,
	},
	{
		0x66,0,
	},
	{
		0x67,0,
	},
	{
		0,0,
	},
};

DWIN_values_t	DWIN_pressure_values[] =
{
	{
		0x50,0,
	},
	{
		0x51,0,
	},
	{
		0x52,0,
	},
	{
		0x53,0,
	},
	{
		0x54,0,
	},
	{
		0x55,0,
	},
	{
		0x56,0,
	},
	{
		0x57,0,
	},
	{
		0,0,
	},
};

uint32_t dwin_state_machine (uint32_t	uart_driver_handle)
{
	return 0;
}

uint8_t		prc3_mbx_data[2];
uint8_t		loaded_program = 0;
uint8_t		dwin_value = 0;

uint32_t process_from_dwin(uint32_t uart_driver_handle,uint8_t *uart1_rx_buffer,uint32_t uart_rxlen)
{
uint32_t	i=0;
	while(DWIN_menus[i].char_id )
	{
		if ( uart1_rx_buffer[4] == DWIN_menus[i].char_id)
		{
			prc3_mbx_data[0] = CMDPARSER_RET_LOAD;
			prc3_mbx_data[1] = loaded_program = DWIN_menus[i].ee_program;
			mbx_send(PRESSO_SEQUENCER_PROCESS,PRESSO_HMI_MBX,prc3_mbx_data,2);
			return 0;
		}
		i++;
	}

	i = 0;
	while(DWIN_time_values[i].index )
	{
		if ( uart1_rx_buffer[4] == DWIN_time_values[i].index)
		{
			DWIN_time_values[i].value = uart1_rx_buffer[8];
			return 0;
		}
		i++;
	}

	i = 0;
	while(DWIN_pressure_values[i].index )
	{
		if ( uart1_rx_buffer[4] == DWIN_pressure_values[i].index)
		{
			DWIN_pressure_values[i].value = uart1_rx_buffer[8];
			compile_and_send_7b_dwin_packet(uart_driver_handle,0x2000 + DWIN_pressure_values[i].index,0x55);
			return 0;
		}
		i++;
	}

	switch(uart1_rx_buffer[4])
	{
	case	DWIN_PROG_PLAY:
		if ( loaded_program )
		{
			prc3_mbx_data[0] = CMDPARSER_RET_EXEC;
			prc3_mbx_data[1] = loaded_program;
			mbx_send(PRESSO_SEQUENCER_PROCESS,PRESSO_HMI_MBX,prc3_mbx_data,2);
		}
		break;
	case	DWIN_PROG_STOP:
		if ( loaded_program )
		{
			prc3_mbx_data[0] = CMDPARSER_RET_HLT;
			prc3_mbx_data[1] = loaded_program;
			mbx_send(PRESSO_SEQUENCER_PROCESS,PRESSO_HMI_MBX,prc3_mbx_data,2);
		}
		break;
	}
	return 0;
}
