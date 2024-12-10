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

uint32_t mem_load_all_programs(void)
{
	return 0xffffffff;
}

uint32_t mem_load_program_by_number(uint8_t program_number)
{
uint32_t address = (program_number-1) * sizeof(Presso_ee_TypeDef);

	bzero((uint8_t *)&Presso_ee,sizeof(Presso_ee_TypeDef));
	return extflash_read(i2cflash_driver_handle,address,(uint8_t *)&Presso_ee,sizeof(Presso_ee_TypeDef));
}

uint32_t mem_load_program_by_name(char *name)
{
	return 0xffffffff;
}

