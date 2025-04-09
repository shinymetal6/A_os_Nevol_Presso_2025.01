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
 * dwin_state_machine.h
 *
 *  Created on: Apr 2, 2025
 *      Author: fil
 */

#ifndef PRESSOMASTER_DWIN_DWIN_STATE_MACHINE_H_
#define PRESSOMASTER_DWIN_DWIN_STATE_MACHINE_H_

typedef struct
{
	uint8_t				char_id;
	uint8_t				ee_program;
}DWIN_menus_t;

typedef struct
{
	uint8_t				index;
	uint8_t				value;
}DWIN_values_t;

#define	DWIN_PROG_PLAY		0x81
#define	DWIN_PROG_STOP		0x80

#define	DWIN_T_SECTOR	0x67
#define	DWIN_PRESSURE	0x57

#endif /* PRESSOMASTER_DWIN_DWIN_STATE_MACHINE_H_ */
