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
#include "process_1_comm_csv_parser.h"
#include "process_2_sequencer.h"
#include "process_2_sequencer_memparser.h"

int pnum;
char line_type;
int time;
char program_name[EE_PROG_NAME_SIZE];
char program_type[8];
int number_of_lines;

int line_number;
int tim1_w;
int tim2_w;
int tim3_w;
int tim4_w;
int tim5_w;
int motor;

char	outconfig[32];
char	csv_ee_line[64];

static uint32_t find_csv_cr_subst(uint8_t *data_ptr)
{
uint32_t i;
	for(i=0;i<1024;i++)
	{
		if ( data_ptr[i] == '\n' )
		{
			csv_ee_line[i] = 0;
			return i+1;
		}
		else if ( data_ptr[i] == ',' )
			csv_ee_line[i] = ' ';
		else
			csv_ee_line[i] = data_ptr[i];

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

	bzero((uint8_t *)&Presso_ee,sizeof(Presso_ee_TypeDef));
	char_processed = 0;
	while(1)
	{
		switch(*data_ptr)
		{
		case 'S' :
			cr_index = find_csv_cr_subst(data_ptr);
			if ( cr_index == 0 )
				return 0;
			pnum = sscanf((char *)csv_ee_line,"%c %s %d %d %s",
					&line_type,
					program_name,
					(int *)&Presso_ee.program_number_of_lines,
					(int *)&Presso_ee.program_time,
					program_type);
			if ( pnum == 5 )
			{
				if ( strcmp(program_type,"LOOP") == 0 )
					Presso_ee.program_valid_flag = EE_PROG_VALID_LOOP_FLAG;
				else if ( strcmp(program_type,"SINGLE") == 0 )
					Presso_ee.program_valid_flag = EE_PROG_VALID_SINGLE_FLAG;
				else
					return 0;
				if ( char_processed > sizeof(Presso_ee_TypeDef))
					return 0;
				char_processed +=cr_index;
				data_ptr += cr_index;
			}
			else
				return 0;
			break;
		case 'L' :
			cr_index = find_csv_cr_subst(data_ptr);
			if ( cr_index == 0 )
				return 0;
			pnum = sscanf(&csv_ee_line[2],"%d %d %d %d %d %d %d %s",
					(int *)&Presso_ee.Presso_ee_line[line_index].line_number,
					(int *)&Presso_ee.Presso_ee_line[line_index].heater_values[0],
					(int *)&Presso_ee.Presso_ee_line[line_index].heater_values[1],
					(int *)&Presso_ee.Presso_ee_line[line_index].heater_values[2],
					(int *)&Presso_ee.Presso_ee_line[line_index].heater_values[3],
					(int *)&Presso_ee.Presso_ee_line[line_index].heater_values[4],
					(int *)&Presso_ee.Presso_ee_line[line_index].motor,
					outconfig
					);
			if ( pnum == 8 )
			{
				Presso_ee.Presso_ee_line[line_index].gpio = convert_gpio();
				char_processed +=cr_index;
				if ( char_processed > sizeof(Presso_ee_TypeDef))
					return 0;
				line_index++;
				data_ptr += cr_index;
			}
			else
				return 0;
			break;
		case 'E' :
			cr_index = find_csv_cr_subst(data_ptr);
			if ( cr_index == 0 )
				return 0;
			char_processed +=cr_index;
			return char_processed;
			break;
		default:
			return 0;
		}
	}
	return 0;
}

