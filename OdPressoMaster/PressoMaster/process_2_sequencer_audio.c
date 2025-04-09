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
 * process_2_sequencer_audio.c
 *
 *  Created on: Apr 4, 2025
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"
#include "presso.h"

Presso_sound_TypeDef	Presso_Initial_sound1[] =
{
	{MIDI_NOTE_ON_FLAG|MIDI_NOTE_OFF_FLAG,67,100},
	{MIDI_NOTE_ON_FLAG|MIDI_NOTE_OFF_FLAG,69,100},
	{MIDI_NOTE_ON_FLAG|MIDI_NOTE_OFF_FLAG,71,100},
	{MIDI_NOTE_ON_FLAG|MIDI_NOTE_OFF_FLAG,69,100},
	{MIDI_NOTE_ON_FLAG|MIDI_NOTE_OFF_FLAG,67,100},
	{MIDI_NOTE_MUTE_FLAG,0,0},
};

Presso_sound_TypeDef	Presso_Initial_sound[] =
{
	{MIDI_NOTE_ON_FLAG,67,200},
	{MIDI_NOTE_ON_FLAG,69,200},
	{MIDI_NOTE_ON_FLAG,71,200},
	{MIDI_NOTE_ON_FLAG,74,200},
	{MIDI_NOTE_ON_FLAG,79,600},
	{MIDI_NOTE_MUTE_FLAG,0,0},
};

void sound_seq_run ( void  )
{
Presso_sound_TypeDef *sound = Presso_soundseq.sound;
	if ( Presso_soundseq.state == SOUND_SEQ_IDLE )
		return;
	if ( Presso_soundseq.beep_time == 0 )
	{
		Presso_soundseq.state=SOUND_SEQ_IDLE;
		return;
	}
	Presso_soundseq.beep_time--;
	if ( Presso_soundseq.beep_time == 0 )
	{

		if (( sound[Presso_soundseq.beep_index].flags & MIDI_NOTE_OFF_FLAG) == MIDI_NOTE_OFF_FLAG)
			NoteOFF( sound[Presso_soundseq.beep_index].midi_note , 1);
		Presso_soundseq.beep_index++;
		if (( sound[Presso_soundseq.beep_index].flags & MIDI_NOTE_ON_FLAG) == MIDI_NOTE_ON_FLAG)
		{
			Presso_soundseq.beep_time = sound[Presso_soundseq.beep_index].time / 10;
			NoteON( sound[Presso_soundseq.beep_index].midi_note , 1);
		}
		else
		{
			Presso_soundseq.beep_time = Presso_soundseq.beep_index = 0;
			Presso_soundseq.state=SOUND_SEQ_IDLE;
			AllNoteOFF();
		}
	}
}

void sound_seq_start ( Presso_sound_TypeDef *sound )
{
	Presso_soundseq.state=SOUND_SEQ_STARTED;
	Presso_soundseq.beep_index=0;
	Presso_soundseq.sound=sound;
	Presso_soundseq.beep_time = sound[Presso_soundseq.beep_index].time / 10;
	if ( sound[Presso_soundseq.beep_index].flags )
		NoteON( sound[Presso_soundseq.beep_index].midi_note , 1);
}
