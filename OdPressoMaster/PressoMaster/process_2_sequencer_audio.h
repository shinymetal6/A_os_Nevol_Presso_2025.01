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
 * process_2_sequencer_audio.h
 *
 *  Created on: Apr 4, 2025
 *      Author: fil
 */

#ifndef PRESSOMASTER_PROCESS_2_SEQUENCER_AUDIO_H_
#define PRESSOMASTER_PROCESS_2_SEQUENCER_AUDIO_H_

extern	Presso_sound_TypeDef		Presso_Initial_sound[];
extern	Presso_soundseq_TypeDef		Presso_soundseq;

extern	void sound_seq_run ( void  );
extern	void sound_seq_start ( Presso_sound_TypeDef *sound );

#endif /* PRESSOMASTER_PROCESS_2_SEQUENCER_AUDIO_H_ */
