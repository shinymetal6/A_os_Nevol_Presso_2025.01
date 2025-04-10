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
 * process_2_sequencer_memparser.c
 *
 *  Created on: Nov 26, 2024
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"
#include "presso.h"
#include "process_2_sequencer.h"
#include "process_2_sequencer_memparser.h"

uint16_t	size_ss;
uint32_t mem_load_program(uint8_t program_number)
{
uint32_t 	address = 0;
uint32_t	ret_val;
uint32_t	i;

	size_ss = sizeof(Presso_ee_TypeDef);
	bzero((uint8_t *)&Presso_opening_ee,sizeof(Presso_ee_TypeDef));
	ret_val = i2c_extflash_read(i2cflash_driver_handle,address,(uint8_t *)&Presso_opening_ee,sizeof(Presso_ee_TypeDef));
	if ((ret_val == 0) && (Presso_opening_ee.program_valid_flag == EE_PROG_VALID_FLAG))
	{
		bzero((uint8_t *)&Presso_ee,sizeof(Presso_ee_TypeDef));
		address = program_number * sizeof(Presso_ee_TypeDef);
		ret_val = i2c_extflash_read(i2cflash_driver_handle,address,(uint8_t *)&Presso_ee,sizeof(Presso_ee_TypeDef));
		if ((ret_val == 0) && (Presso_ee.program_valid_flag == EE_PROG_VALID_FLAG))
		{
			for(i=0;i<Presso_ee.program_number_of_lines;i++)
			{
				Presso_ee.Presso_ee_line[i].sector_time = Presso_ee.program_step_time;
				Presso_ee.Presso_ee_line[i].sector_pressure = Presso_ee.program_pressure;
			}
			return 0;
		}
	}
	return 1;
}


