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
 * process_3_dwin_hmi.h
 *
 *  Created on: Dec 9, 2024
 *      Author: fil
 */
#ifndef PRESSOMASTER_PROCESS_3_DWIN_HMI_H_
#define PRESSOMASTER_PROCESS_3_DWIN_HMI_H_


typedef struct
{
	uint16_t 			active_code;
	uint16_t 			last_active_code;
}Presso_DWIN_HMI_TypeDef;


extern	Presso_ee_TypeDef *get_sequencer_params(void);

#endif /* PRESSOMASTER_PROCESS_3_DWIN_HMI_H_ */
