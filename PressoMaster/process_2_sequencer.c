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
 * process_2_sequencer.c
 *
 *  Created on: Sep 13, 2023
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"
#include "presso.h"
#include "process_1_comm_cmdparser.h"

__attribute__ ((aligned (32)))	Presso_ee_TypeDef			Presso_ee;
__attribute__ ((aligned (32)))	Presso_ee_TypeDef			Presso_opening_ee;
__attribute__ ((aligned (32)))	Presso_ee_TypeDef			Presso_closing_ee;
__attribute__ ((aligned (32)))	Presso_Sequencer_TypeDef	Presso_Sequencer;

uint8_t		prc1_mbx_rxbuf[sizeof(uint32_t)];
uint8_t		prc3_mbx_rxbuf[sizeof(uint32_t)];
uint32_t	program_loaded = 0;

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
	process_2_sequencer_set_motor( next_pstruct->Presso_ee_line[Presso_Sequencer.sequence].force_motor_on );
}

void halt_sequencer(void)
{
	Presso_Sequencer.sequence = 0;
	process_2_sequencer_set_timers(0,0,0,0,0);
	process_2_sequencer_set_gpio(0);
	Presso_Sequencer.sequence = 0;
	Presso_Sequencer.state = SEQUENCER_STATE_FINISHED;
}

void run_sequencer(void)
{
Presso_ee_TypeDef	*pstruct;

	Presso_Sequencer.time--;
	if ( Presso_Sequencer.time == 0 )
	{
		if ( Presso_Sequencer.state == SEQUENCER_STATE_OPENING)
			pstruct = &Presso_opening_ee;
		else if ( Presso_Sequencer.state == SEQUENCER_STATE_RUNNING)
			pstruct = &Presso_ee;
		else if ( Presso_Sequencer.state == SEQUENCER_STATE_CLOSING)
			pstruct = &Presso_closing_ee;
		else
			return;
		Presso_Sequencer.time = pstruct->program_time;
		Presso_Sequencer.repetition_number = pstruct->program_repetition_number;

		if ( Presso_Sequencer.sequence >= pstruct->program_number_of_lines)
		{
			if ( Presso_Sequencer.state == SEQUENCER_STATE_OPENING)
			{
				Presso_Sequencer.sequence = 0;
				pstruct = &Presso_ee;
				setup_state(pstruct);
				Presso_Sequencer.state = SEQUENCER_STATE_RUNNING;
			}
			else if ( Presso_Sequencer.state == SEQUENCER_STATE_RUNNING)
			{
				Presso_Sequencer.sequence = 0;
				pstruct = &Presso_ee;
				setup_state(pstruct);
				if (( pstruct->program_valid_flag == EE_PROG_VALID_LOOP_FLAG) && ( pstruct->program_repetition_number > 1 ))
				{
					Presso_Sequencer.state = SEQUENCER_STATE_RUNNING;
					pstruct->program_repetition_number--;
				}
				else
				{
					if ( pstruct->program_has_closing )
					{
						Presso_Sequencer.sequence = 0;
						pstruct = &Presso_closing_ee;
						setup_state(pstruct);
						Presso_Sequencer.state = SEQUENCER_STATE_CLOSING;
					}
					else
						halt_sequencer();
				}
			}
			else if ( Presso_Sequencer.state == SEQUENCER_STATE_CLOSING)
				halt_sequencer();
		}
		else
		{
			setup_state(pstruct);
		}
		Presso_Sequencer.sequence ++;
	}
}


extern	VCA_Effect_TypeDef	VCA_Effect1;
extern	VCA_Effect_TypeDef	VCA_Effect2;

void do_sound ( uint8_t initial_beep)
{
	switch ( initial_beep)
	{
	case 3 :	NoteON( 67 , 1);break;
	case 4 :	NoteOFF( 67 , 1);NoteON( 69 , 1);break;
	case 5 :	NoteOFF( 69 , 1);NoteON( 71 , 1);break;
	case 6 : 	NoteOFF( 71 , 1);NoteON( 69 , 1);break;
	case 7 : 	NoteOFF( 69 , 1);NoteON( 67 , 1);break;
	case 8 : 	NoteOFF( 67 , 1);break;
	default : 	return;
	}
}

uint8_t load_program_and_execute(uint8_t program_number,uint8_t program_command)
{
	if ( program_command == CMDPARSER_RET_RUN)
	{
		if ( program_number < PRESSO_MAX_PROGRAMS )
		{
			if ( mem_load_program(program_number) == 0)
			{
				Presso_Sequencer.time = Presso_ee.program_time;
				if ( Presso_ee.program_has_opening == 0 )
					Presso_Sequencer.state = SEQUENCER_STATE_RUNNING;
				else
					Presso_Sequencer.state = SEQUENCER_STATE_OPENING;
				Presso_Sequencer.sequence = 0;
				return 0;
			}
		}
	}
	if ( program_command == CMDPARSER_RET_HLT)
	{
		if ( program_number < PRESSO_MAX_PROGRAMS )
		{
			Presso_Sequencer.state = SEQUENCER_STATE_IDLE;
			halt_sequencer();
			return 0;
		}
	}
	return 1;
}

void process_2_sequencer(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint32_t	mbx_size;
uint8_t		initial_beep = 0;
uint8_t		sequencer_prescaler;

	process_2_sequencer_init();
	bzero((uint8_t *)&Presso_ee,sizeof(Presso_ee_TypeDef));
	create_timer(TIMER_ID_0,PROCESS_SCHEDULE_TIME,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);

	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_MBX | i2c_24xx_Drv.wakeup_id | EVENT_ADC1_IRQ );
		get_wakeup_flags(&wakeup,&flags);

		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			if ((Presso_Sequencer.state == SEQUENCER_STATE_OPENING ) || (Presso_Sequencer.state == SEQUENCER_STATE_RUNNING )  || (Presso_Sequencer.state == SEQUENCER_STATE_CLOSING ))
			{
				sequencer_prescaler--;
				if ( sequencer_prescaler == 0 )
				{
					sequencer_prescaler = SEQUENCER_TICK_TIME;
					run_sequencer();
				}
			}

			if (Presso_Sequencer.state == SEQUENCER_STATE_FINISHED )
			{
				Presso_Sequencer.state = SEQUENCER_STATE_IDLE;
				Presso_Sequencer.sequence = 0;
			}
			do_sound(initial_beep);
			initial_beep++;
			if ( initial_beep > 8 )
				initial_beep = 9;
		}
		if (( wakeup & WAKEUP_FROM_MBX) == WAKEUP_FROM_MBX)
		{
			mbx_size = mbx_receive(PRESSO_COMM_MBX,prc1_mbx_rxbuf);
			if ( mbx_size )
			{
				if (( prc1_mbx_rxbuf[0] == CMDPARSER_RET_RUN) || ( prc1_mbx_rxbuf[0] == CMDPARSER_RET_HLT) )
				{
					sequencer_prescaler = SEQUENCER_TICK_TIME;
					load_program_and_execute(prc1_mbx_rxbuf[1],prc1_mbx_rxbuf[0]);
				}
				if ( prc1_mbx_rxbuf[0] == CMDPARSER_RET_PLAY)
					dac_play_wav(dac_driver_handle,(uint16_t *)(BANK_2_ADDRESS + ((prc1_mbx_rxbuf[1]-1) * WAV_MAX_SIZE)) );
				if ( prc1_mbx_rxbuf[0] == CMDPARSER_RET_MUTE)
					dac_stop_wav(dac_driver_handle);
				if ( prc1_mbx_rxbuf[0] == CMDPARSER_PLAY_SOUND)
					initial_beep = 0;
				if ( prc1_mbx_rxbuf[0] == CMDPARSER_TEST_MOTOR)
					process_2_sequencer_set_motor( prc1_mbx_rxbuf[1] & 0x01 );
				if ( prc1_mbx_rxbuf[0] == CMDPARSER_TEST_OPEN)
					process_2_sequencer_set_test_gpio( prc1_mbx_rxbuf[1] & 0x01 );
			}
			mbx_size = mbx_receive(PRESSO_HMI_MBX,prc3_mbx_rxbuf);
			if ( mbx_size )
			{
				if (( prc3_mbx_rxbuf[0] == CMDPARSER_RET_RUN) || ( prc1_mbx_rxbuf[0] == CMDPARSER_RET_HLT) )
					load_program_and_execute(prc1_mbx_rxbuf[1],prc1_mbx_rxbuf[0]);
			}
		}
		if (( wakeup & i2c_24xx_Drv.wakeup_id) == i2c_24xx_Drv.wakeup_id)
		{
			if ((flags & WAKEUP_FLAGS_I2C_RX) == WAKEUP_FLAGS_I2C_RX)
			{
				program_loaded = 1;
			}
		}
	}
}

