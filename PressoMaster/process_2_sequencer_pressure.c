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
 * process_2_sequencer_pressure.c
 *
 *  Created on: Apr 4, 2025
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"
#include "presso.h"
#include "process_2_sequencer_pressure.h"

Presso_SequencerPressure_TypeDef	Presso_SequencerPressure;

/*
1  bar 6422
.5 bar 6392
.25 bar 6372
0  bar 6350

15 = .25 bar

0 bar 6350
.25 -> 6365
.5 -> 6380
.75 -> 6395
1 -> 6410
*/
//Presso_SequencerPressure.pressure = 157 @ 1bar
//Presso_SequencerPressure.pressure = 69 @ 0.5bar
//Presso_SequencerPressure.pressure = 40 @ 0.25bar
void pressure_manager(void)
{
	if ( Presso_SequencerPressure.adc_zero_acqcounter < 10 )
	{
		if ( Presso_SequencerPressure.adc_zero_data )
		{
			Presso_SequencerPressure.adc_zero_data+=adc_Drv.adc_buffer[0];
			Presso_SequencerPressure.adc_zero_data/=2;
		}
		else
			Presso_SequencerPressure.adc_zero_data+=adc_Drv.adc_buffer[0];
		Presso_SequencerPressure.adc_zero_acqcounter++;
	}
	else
	{
		Presso_SequencerPressure.adc_data+=adc_Drv.adc_buffer[0];
		Presso_SequencerPressure.adc_data/=2;
		if ( Presso_SequencerPressure.adc_data > Presso_SequencerPressure.adc_zero_data)
			Presso_SequencerPressure.pressure = Presso_SequencerPressure.adc_data - Presso_SequencerPressure.adc_zero_data;
		else
			Presso_SequencerPressure.pressure = 0;
	}
}

uint32_t get_pressure(void)
{
	return Presso_SequencerPressure.pressure;
}

