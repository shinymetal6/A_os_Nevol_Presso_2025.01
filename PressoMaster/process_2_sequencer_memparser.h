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
 * process_2_sequencer_memparser.h
 *
 *  Created on: Nov 26, 2024
 *      Author: fil
 */

#ifndef PROCESS_2_SEQUENCER_MEMPARSER_H_
#define PROCESS_2_SEQUENCER_MEMPARSER_H_

#define	PRESSO_FLASH_SIZE	65536

typedef struct
{
	uint16_t		line_number;
	uint16_t		heater_values[5];
	uint16_t		gpio;
	uint8_t			motor;
	uint8_t			status;
}Presso_ee_line_TypeDef;

/* assuming a program header is composed by 32 bytes
 * and a program line is composed by 16 bytes
*/
#define	EE_PROG_NAME_SIZE		25
#define	EE_MAX_LINE_NUMBER		254

typedef struct
{
	uint8_t					program_valid_flag;			// 1
	uint16_t				program_time;				// 2
	char					program_name[EE_PROG_NAME_SIZE];			// 27
	uint16_t				program_number_of_lines;	// 2
	Presso_ee_line_TypeDef	Presso_ee_line[EE_MAX_LINE_NUMBER];
}Presso_ee_TypeDef;
#define	EE_PROG_VALID_LOOP_FLAG		0x7e
#define	EE_PROG_VALID_SINGLE_FLAG	0xe7

extern	uint32_t mem_load_all_programs(void);
extern	uint32_t mem_load_program_by_number(uint8_t program_number);
extern	uint32_t mem_load_program_by_name(char *name);

#endif /* PROCESS_2_SEQUENCER_MEMPARSER_H_ */
