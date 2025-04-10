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
 * process_1_comm_csv_parser.c
 *
 *  Created on: Nov 26, 2024
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"
#include "presso.h"

char	program_name[EE_PROG_NAME_SIZE];
char	outconfig[32];
int		heater_values[5],line_number,audionumber;
int		program_number , program_number_of_lines , program_pressure , program_has_opening , program_step_time , program_complete_time,program_close_eoc;

static uint32_t find_csv_cr(uint8_t *data_ptr)
{
uint32_t i;
	for(i=0;i<1024;i++)
	{
		if ( data_ptr[i] == '\n' )
			return i+1;
	}
	return 0;
}

uint16_t convert_gpio(void)
{
uint8_t		i;
uint16_t	gpioval = 0;

	for(i=0;i<16;i++)
	{
		if ( outconfig[i] == '1' )
			gpioval |= 1<<i;
	}
	return gpioval;
}

uint32_t decode_csv(uint8_t *data_ptr,uint32_t data_len)
{
uint32_t cr_index = 0;
uint32_t char_processed = 0;
uint32_t line_index = 0;
Presso_ee_TypeDef	*pstruct;
int	pnum;

	if ( NevolSystem.param_from_host == 0 )
		pstruct = &Presso_opening_ee;
	else
		pstruct = &Presso_ee;
	bzero((uint8_t *)pstruct,sizeof(Presso_ee_TypeDef));
	char_processed = 0;
	while(1)
	{
		switch(*data_ptr)
		{
		case 'S' :
			cr_index = find_csv_cr(data_ptr);
			if ( cr_index == 0 )
				return 0;
//			linetype , program_number , program_number_of_lines , program_pressure , program_has_opening , program_step_time , program_complete_time program_name
			pnum = sscanf((char *)data_ptr,"S,%d,%d,%d,%d,%d,%d,%d,%s",
					&program_number,
					&program_number_of_lines,
					&program_pressure,
					&program_has_opening,
					&program_step_time,
					&program_complete_time,
					&program_close_eoc,
					program_name
					);
			if ( pnum == 8 )
			{
				pstruct->program_number = program_number;
				pstruct->program_number_of_lines = program_number_of_lines;
				pstruct->program_pressure = program_pressure;
				pstruct->program_has_opening = program_has_opening;
				pstruct->program_step_time = program_step_time;
				pstruct->program_complete_time = program_complete_time;
				pstruct->program_close_eoc = program_close_eoc;
				pstruct->program_valid_flag = EE_PROG_VALID_FLAG;
				sprintf(pstruct->program_name,"%s",program_name);
				if ( char_processed > sizeof(Presso_ee_TypeDef))
					return 0;
				char_processed +=EE_PROG_HDR_SIZE;
				data_ptr += cr_index;
			}
			else
				return 0;
			break;
		case 'L' :
			cr_index = find_csv_cr(data_ptr);
			if ( cr_index == 0 )
				return 0;

			pnum = sscanf((char *)data_ptr,"L,%d,%d,%d,%d,%d,%d,%d,%s",
					&line_number,
					&heater_values[0],
					&heater_values[1],
					&heater_values[2],
					&heater_values[3],
					&heater_values[4],
					&audionumber,
					outconfig
					);
			if ( pnum == 8 )
			{
				pstruct->Presso_ee_line[line_index].line_number = line_number;
				pstruct->Presso_ee_line[line_index].heater_values[0] = heater_values[0];
				pstruct->Presso_ee_line[line_index].heater_values[1] = heater_values[1];
				pstruct->Presso_ee_line[line_index].heater_values[2] = heater_values[2];
				pstruct->Presso_ee_line[line_index].heater_values[3] = heater_values[3];
				pstruct->Presso_ee_line[line_index].heater_values[4] = heater_values[4];
				pstruct->Presso_ee_line[line_index].audionumber = audionumber;
				pstruct->Presso_ee_line[line_index].gpio = convert_gpio();
				pstruct->Presso_ee_line[line_index].sector_time = program_step_time;
				pstruct->Presso_ee_line[line_index].sector_pressure = program_pressure;
				char_processed +=EE_PROG_LINE_SIZE;
				line_index++;
				data_ptr += cr_index;
			}
			else
				return 0;
			break;
		case 'E' :
			cr_index = find_csv_cr(data_ptr);
			if ( cr_index == 0 )
				return 0;
			char_processed ++;
			return char_processed;
			break;
		case '/' :
			cr_index = find_csv_cr(data_ptr);
			if ( cr_index == 0 )
				return 0;
			data_ptr += cr_index;
			break;
		default:
			return 0;
		}
	}
	return 0;
}
