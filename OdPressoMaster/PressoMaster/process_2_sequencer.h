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
 * process_2_sequencer.h
 *
 *  Created on: Nov 26, 2024
 *      Author: fil
 */

#ifndef PROCESS_2_SEQUENCER_H_
#define PROCESS_2_SEQUENCER_H_

#define	PRESSO_BUFFERSIZE	I2C_24XX_PAGESIZE*4
#define	PRESSO_GPIO_NR		16

typedef struct
{
	GPIO_TypeDef	 	*port;
	uint16_t			bit;
}Presso_GPIO_TypeDef;

typedef struct
{
	uint8_t				state;
	uint16_t			sequence;
	uint16_t			time;
	uint8_t				flags;
}Presso_Sequencer_TypeDef;
/*state*/
#define	SEQUENCER_STATE_IDLE		0
#define	SEQUENCER_STATE_OPENING		1
#define	SEQUENCER_STATE_RUNNING		2
#define	SEQUENCER_STATE_CLOSING		3
#define	SEQUENCER_STATE_FINISHED	4

typedef struct
{
	uint8_t				flags;
	uint16_t			midi_note;
	uint16_t			time;
}Presso_sound_TypeDef;

#define	MIDI_NOTE_MUTE_FLAG	0x00

#define	MIDI_NOTE_ON_FLAG	0x01
#define	MIDI_NOTE_OFF_FLAG	0x02

typedef struct
{
	uint8_t					state;
	uint8_t					beep_index;
	uint8_t					beep_time;
	Presso_sound_TypeDef	*sound;
}Presso_soundseq_TypeDef;

#define	SOUND_SEQ_STARTED	0x80
#define	SOUND_SEQ_IDLE		0x00

#define	PROCESS_SCHEDULE_TIME	100
#define	SEQUENCER_TICK_TIME		(1000 / PROCESS_SCHEDULE_TIME)

extern	I2C_24xx_Drv_TypeDef	i2c_24xx_Drv;
extern	Pwm_Control_TypeDef		Pwm2_Control;
extern	Pwm_Control_TypeDef		Pwm3_Control;

extern	uint32_t				i2cflash_driver_handle;
extern	uint32_t				adc_driver_handle;
extern	uint32_t				dac_driver_handle;
extern	uint32_t				tim2_3_driver_handle;
extern	uint32_t				tim3_1_driver_handle;
extern	uint32_t				tim3_2_driver_handle;
extern	uint32_t				tim3_3_driver_handle;
extern	uint32_t				tim3_4_driver_handle;

extern	void process_2_sequencer_init(void);
extern	void process_2_sequencer_set_gpio(uint16_t outconfig);
extern	void process_2_sequencer_set_timers(uint32_t ht1,uint32_t ht2,uint32_t ht3,uint32_t ht4,uint32_t ht5);
extern	void process_2_sequencer_set_motor(uint8_t motor);
extern	void process_2_sequencer_set_test_gpio(uint8_t level);
extern	uint8_t load_program_and_execute(uint8_t program_number);
extern	uint8_t load_program(uint8_t program_number);
extern	uint8_t halt_program(uint8_t program_number);



#endif /* PROCESS_2_SEQUENCER_H_ */
