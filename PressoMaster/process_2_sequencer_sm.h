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
 * process_2_sequencer_sm.h
 *
 *  Created on: Apr 10, 2025
 *      Author: fil
 */

#ifndef PRESSOMASTER_PROCESS_2_SEQUENCER_SM_H_
#define PRESSOMASTER_PROCESS_2_SEQUENCER_SM_H_

extern	void setup_state(Presso_ee_TypeDef	*next_pstruct);
extern	void halt_sequencer(void);
extern	uint8_t execute_program(uint8_t program_number);
extern	uint8_t pause_sequencer(uint8_t program_number);
extern	uint8_t unpause_sequencer(uint8_t program_number);
extern	uint8_t halt_program(uint8_t program_number);
extern	Presso_ee_TypeDef *get_sequencer_params(void);
extern	void mod_prog_out_following_to_active(uint8_t program_step);
extern	void mod_prog_out_following_to_original(uint8_t program_step);

extern	void sequencer_sm(void);

#endif /* PRESSOMASTER_PROCESS_2_SEQUENCER_SM_H_ */
