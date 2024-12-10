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
#include "process_3_dwin_hmi.h"


 __attribute__ ((aligned (32)))	Presso_ee_TypeDef			Presso_ee;
 __attribute__ ((aligned (32)))	Presso_Sequencer_TypeDef	Presso_Sequencer;

uint8_t			prc1_mbx_rxbuf[sizeof(uint32_t)];
uint32_t		program_loaded = 0;

void run_sequencer(void)
{
uint32_t	sequence;

	Presso_Sequencer.time--;
	if ( Presso_Sequencer.time == 0 )
	{
		sequence = Presso_Sequencer.sequence;
		Presso_Sequencer.time = Presso_ee.program_time;
		if ( Presso_Sequencer.sequence >= Presso_ee.program_number_of_lines)
		{
			if ( Presso_ee.program_valid_flag == EE_PROG_VALID_LOOP_FLAG)
			{
				Presso_Sequencer.sequence = 0;
			}
			else
			{
				Presso_Sequencer.sequence = 0;
				Presso_Sequencer.state = SEQUENCER_STATE_FINISHED;
				process_2_sequencer_set_timers(0,0,0,0,0);
				process_2_sequencer_set_gpio(0);
				process_2_sequencer_set_motor(0);
			}
		}
		else
		{
			process_2_sequencer_set_gpio(Presso_ee.Presso_ee_line[sequence].gpio);
			process_2_sequencer_set_timers(
					Presso_ee.Presso_ee_line[sequence].heater_values[0],
					Presso_ee.Presso_ee_line[sequence].heater_values[1],
					Presso_ee.Presso_ee_line[sequence].heater_values[2],
					Presso_ee.Presso_ee_line[sequence].heater_values[3],
					Presso_ee.Presso_ee_line[sequence].heater_values[4]
					);
			process_2_sequencer_set_motor(Presso_ee.Presso_ee_line[sequence].motor);
			Presso_Sequencer.sequence++;
		}
		Presso_Sequencer.sequence ++;
	}
}

void halt_sequencer(void)
{
	process_2_sequencer_set_timers(0,0,0,0,0);
	process_2_sequencer_set_gpio(0);
	Presso_Sequencer.sequence = 0;
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
	}
}

void process_2_sequencer(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint32_t	mbx_size;
uint8_t		initial_beep = 0;
uint8_t		program_run_number;

	process_2_sequencer_init();
	bzero((uint8_t *)&Presso_ee,sizeof(Presso_ee_TypeDef));
	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);

	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_MBX | i2c_24xx_Drv.wakeup_id);
		get_wakeup_flags(&wakeup,&flags);

		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			if (Presso_Sequencer.state == SEQUENCER_STATE_RUNNING )
			{
				run_sequencer();
			}

			if (Presso_Sequencer.state == SEQUENCER_STATE_FINISHED )
			{
				Presso_Sequencer.state = SEQUENCER_STATE_IDLE;
				Presso_Sequencer.sequence = 0;
			}
			do_sound(initial_beep);
			initial_beep++;
			if ( initial_beep > 8 )
				initial_beep = 8;
		}
		if (( wakeup & WAKEUP_FROM_MBX) == WAKEUP_FROM_MBX)
		{
			mbx_size = mbx_receive(PRESSO_COMM_MBX,prc1_mbx_rxbuf);
			if ( mbx_size )
			{
				if ( prc1_mbx_rxbuf[0] == CMDPARSER_RET_RUN)
				{
					program_run_number = prc1_mbx_rxbuf[1];
					if ( program_run_number < 16 )
					{
						if ( mem_load_program_by_number(program_run_number) == 0)
						{
							Presso_Sequencer.time = Presso_ee.program_time;
							Presso_Sequencer.state = SEQUENCER_STATE_RUNNING;
						}
					}
				}
				if ( prc1_mbx_rxbuf[0] == CMDPARSER_RET_HLT)
				{
					program_run_number = prc1_mbx_rxbuf[1];
					if ( program_run_number < 16 )
					{
						Presso_Sequencer.state = SEQUENCER_STATE_IDLE;
						halt_sequencer();
					}
				}
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

