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
 * process_2_sequencer_sm.c
 *
 *  Created on: Apr 10, 2025
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"
#include "presso.h"

extern	Presso_ee_TypeDef			Presso_ee;
extern	Presso_ee_TypeDef			Presso_opening_ee;
extern	Presso_Sequencer_TypeDef	Presso_Sequencer;
extern	Presso_soundseq_TypeDef		Presso_soundseq;

void setup_state(Presso_ee_TypeDef	*next_pstruct)
{
	process_2_sequencer_set_gpio(next_pstruct->Presso_ee_line[Presso_Sequencer.sequence].gpio);
	process_2_sequencer_set_timers(
			next_pstruct->Presso_ee_line[Presso_Sequencer.sequence].heater_values[0],
			next_pstruct->Presso_ee_line[Presso_Sequencer.sequence].heater_values[1],
			next_pstruct->Presso_ee_line[Presso_Sequencer.sequence].heater_values[2],
			next_pstruct->Presso_ee_line[Presso_Sequencer.sequence].heater_values[3],
			next_pstruct->Presso_ee_line[Presso_Sequencer.sequence].heater_values[4]
			);
	if ( next_pstruct->Presso_ee_line[Presso_Sequencer.sequence].audionumber != 0 )
		dac_play_wav(dac_driver_handle,(uint16_t *)(BANK_2_ADDRESS + (next_pstruct->Presso_ee_line[Presso_Sequencer.sequence].audionumber * WAV_MAX_SIZE)) );
}

void halt_sequencer(void)
{
	Presso_Sequencer.sequence = 0;
	process_2_sequencer_set_timers(0,0,0,0,0);
	process_2_sequencer_set_gpio(0);
	Presso_Sequencer.sequence = 0;
	Presso_Sequencer.state = SEQUENCER_STATE_FINISHED;
}

uint8_t load_program_and_execute(uint8_t program_number)
{
	if ( program_number < PRESSO_MAX_PROGRAMS )
	{
		if ( mem_load_program(program_number) == 0)
		{
			Presso_Sequencer.program_number = program_number;
			Presso_Sequencer.step_time = Presso_ee.program_step_time;
			Presso_Sequencer.complete_time = Presso_ee.program_complete_time;
			if ( Presso_ee.program_has_opening == 0 )
				Presso_Sequencer.state = SEQUENCER_STATE_RUNNING;
			else
			{
				Presso_Sequencer.complete_time += Presso_opening_ee.program_complete_time;
				Presso_Sequencer.state = SEQUENCER_STATE_OPENING;
			}
			Presso_Sequencer.sequence = 0;
			return 0;
		}
	}
	return 1;
}

uint8_t load_program(uint8_t program_number)
{
	if ( program_number < PRESSO_MAX_PROGRAMS )
	{
		if ( mem_load_program(program_number) == 0)
		{
			Presso_Sequencer.program_number = program_number;
			Presso_Sequencer.sequence = 0;
			return 0;
		}
	}
	return 1;
}

uint8_t execute_program(uint8_t program_number)
{
	if ( program_number < PRESSO_MAX_PROGRAMS )
	{
		Presso_Sequencer.step_time = Presso_ee.program_step_time;
		Presso_Sequencer.complete_time = Presso_ee.program_complete_time;
		if ( Presso_ee.program_has_opening == 0 )
			Presso_Sequencer.state = SEQUENCER_STATE_RUNNING;
		else
		{
			Presso_Sequencer.complete_time += Presso_opening_ee.program_complete_time;
			Presso_Sequencer.state = SEQUENCER_STATE_OPENING;
		}
		Presso_Sequencer.sequence = 0;
		return 0;
	}
	return 1;
}

uint8_t pause_sequencer(uint8_t program_number)
{
	if ( program_number < PRESSO_MAX_PROGRAMS )
		Presso_Sequencer.state |= SEQUENCER_STATE_PAUSE;
	return 0;
}

uint8_t unpause_sequencer(uint8_t program_number)
{
	if ( program_number < PRESSO_MAX_PROGRAMS )
		Presso_Sequencer.state &= ~SEQUENCER_STATE_PAUSE;
	return 0;
}

uint8_t halt_program(uint8_t program_number)
{
	if ( program_number < PRESSO_MAX_PROGRAMS )
	{
		Presso_Sequencer.state = SEQUENCER_STATE_IDLE;
		Presso_Sequencer.program_number = 0;
		halt_sequencer();
		return 0;
	}
	return 1;
}

Presso_ee_TypeDef *get_sequencer_params(void)
{
	return &Presso_ee;
}

void sequencer_sm(void)
{
Presso_ee_TypeDef	*pstruct;

	if (( Presso_Sequencer.state & SEQUENCER_STATE_PAUSE) == SEQUENCER_STATE_PAUSE)
		return;
	if (( Presso_Sequencer.state & SEQUENCER_STATE_OPENING) == SEQUENCER_STATE_OPENING)
		pstruct = &Presso_opening_ee;
	else if (( Presso_Sequencer.state & SEQUENCER_STATE_RUNNING ) == SEQUENCER_STATE_RUNNING)
		pstruct = &Presso_ee;
	else
		return;

	if ( Presso_Sequencer.step_time )
		Presso_Sequencer.step_time--;

	if ( Presso_Sequencer.step_time == 0 )
	{
		if ( Presso_Sequencer.complete_time )
			Presso_Sequencer.complete_time--;
		if (( Presso_Sequencer.complete_time == 0 ) && ( pstruct->program_close_eoc == 0 ))
		{
			halt_sequencer();
			return;
		}
		if ( Presso_Sequencer.sequence >= pstruct->program_number_of_lines-1)
		{
			if (( Presso_Sequencer.state & SEQUENCER_STATE_OPENING) == SEQUENCER_STATE_OPENING)
			{
				pstruct = &Presso_ee;
				Presso_Sequencer.step_time = pstruct->Presso_ee_line[0].sector_time;
				Presso_Sequencer.state = SEQUENCER_STATE_RUNNING;
			}
			else if (( Presso_Sequencer.state & SEQUENCER_STATE_RUNNING ) == SEQUENCER_STATE_RUNNING)
				Presso_Sequencer.sequence = 0;
			else
			{
				halt_sequencer();
				return;
			}
			if ( Presso_Sequencer.complete_time == 0 )
			{
				halt_sequencer();
				return;
			}
			Presso_Sequencer.sequence = 0;
		}
		else
		{
			Presso_Sequencer.sequence ++;
		}
		Presso_Sequencer.step_time = pstruct->Presso_ee_line[Presso_Sequencer.sequence].sector_time;
		setup_state(pstruct);
	}
}

