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

__attribute__ ((aligned (32)))	Presso_ee_TypeDef			Presso_ee;
__attribute__ ((aligned (32)))	Presso_ee_TypeDef			Presso_opening_ee;
__attribute__ ((aligned (32)))	Presso_Sequencer_TypeDef	Presso_Sequencer;
__attribute__ ((aligned (32)))	Presso_soundseq_TypeDef		Presso_soundseq;

uint8_t		seq_from_comm_mbx_rxbuf[sizeof(uint32_t)];
uint8_t		prc2_mbx_data[sizeof(uint32_t)];
uint8_t		seq_from_hmi_mbx_rxbuf[sizeof(uint32_t)];
//extern	uint8_t		prc3_mbx_data[sizeof(uint32_t)];

uint8_t		mbx_seq_2_hmi[sizeof(uint32_t)];
uint32_t	program_loaded = 0;


extern	VCA_Effect_TypeDef	VCA_Effect1;
extern	VCA_Effect_TypeDef	VCA_Effect2;

void process_2_sequencer(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint32_t	mbx_size;
uint8_t		sequencer_prescaler;

	process_2_sequencer_init();
	bzero((uint8_t *)&Presso_ee,sizeof(Presso_ee_TypeDef));
	create_timer(TIMER_ID_0,PROCESS_SCHEDULE_TIME,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	create_timer(TIMER_ID_1,10,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	sound_seq_start(&Presso_Initial_sound[0]);
	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_MBX | i2c_24xx_Drv.wakeup_id | EVENT_ADC1_IRQ );
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & EVENT_ADC1_IRQ) == EVENT_ADC1_IRQ)
			pressure_manager();

		if ((( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER) && ((flags & TIMER_ID_1) == TIMER_ID_1))
			sound_seq_run();
		if ((( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER) && ((flags & TIMER_ID_0) == TIMER_ID_0))
		{
			if ((Presso_Sequencer.state == SEQUENCER_STATE_OPENING ) || (Presso_Sequencer.state == SEQUENCER_STATE_RUNNING ))
			{
				if ( sequencer_prescaler )
					sequencer_prescaler--;
				if ( sequencer_prescaler == 0 )
				{
					sequencer_prescaler = SEQUENCER_TICK_TIME;
					sequencer_sm();
				}
			}

			if (Presso_Sequencer.state == SEQUENCER_STATE_FINISHED )
			{
				Presso_Sequencer.state = SEQUENCER_STATE_IDLE;
				Presso_Sequencer.sequence = 0;
				mbx_seq_2_hmi[0] = SEQUENCE_FINISHED;
				mbx_seq_2_hmi[1] = 0;
				mbx_send(PRESSO_HMI_PROCESS,PRESSO_HMI_MBX,mbx_seq_2_hmi,2);
			}
		}
		if (( wakeup & WAKEUP_FROM_MBX) == WAKEUP_FROM_MBX)
		{
			mbx_size = mbx_receive(PRESSO_COMM_MBX,seq_from_comm_mbx_rxbuf);
			if ( mbx_size )
			{
				switch (seq_from_comm_mbx_rxbuf[0])
				{
				case CMDPARSER_RET_LOADRUN:
					sequencer_prescaler = SEQUENCER_TICK_TIME;
					if ( load_program(seq_from_comm_mbx_rxbuf[1]) == 0 )
						execute_program(seq_from_comm_mbx_rxbuf[1]);
					break;
				case CMDPARSER_RET_RUN:
					execute_program(seq_from_comm_mbx_rxbuf[1]);
					break;
				case CMDPARSER_RET_LOAD:
					load_program(seq_from_comm_mbx_rxbuf[1]);
					break;
				case CMDPARSER_RET_HLT:
					halt_program(seq_from_comm_mbx_rxbuf[1]);
					break;
				case CMDPARSER_RET_PLAY:
					dac_play_wav(dac_driver_handle,(uint16_t *)(BANK_2_ADDRESS + ((seq_from_comm_mbx_rxbuf[1]-1) * WAV_MAX_SIZE)) );
					break;
				case CMDPARSER_RET_MUTE:
					dac_stop_wav(dac_driver_handle);
					break;
				}
			}

			mbx_size = mbx_receive(PRESSO_HMI_MBX,seq_from_hmi_mbx_rxbuf);
			if ( mbx_size )
			{
				switch (seq_from_hmi_mbx_rxbuf[0])
				{
				case CMDPARSER_RET_RUN:
					execute_program(seq_from_hmi_mbx_rxbuf[1]);
					break;
				case CMDPARSER_RET_PAUSE:
					pause_sequencer(seq_from_hmi_mbx_rxbuf[1]);
					break;
				case CMDPARSER_RET_UNPAUSE:
					unpause_sequencer(seq_from_hmi_mbx_rxbuf[1]);
					break;
				case CMDPARSER_RET_LOAD:
					if ( load_program(seq_from_hmi_mbx_rxbuf[1]) == 0 )
					{
						mbx_seq_2_hmi[0] = UPDATE_LCD_PARAMS;
						mbx_seq_2_hmi[1] = seq_from_hmi_mbx_rxbuf[1];
						mbx_send(PRESSO_HMI_PROCESS,PRESSO_HMI_MBX,mbx_seq_2_hmi,2);
					}
					break;
				case CMDPARSER_RET_HLT:
					halt_program(seq_from_hmi_mbx_rxbuf[1]);
					break;
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

