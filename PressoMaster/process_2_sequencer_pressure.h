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
 * process_2_sequencer_pressure.h
 *
 *  Created on: Apr 4, 2025
 *      Author: fil
 */

#ifndef PRESSOMASTER_PROCESS_2_SEQUENCER_PRESSURE_H_
#define PRESSOMASTER_PROCESS_2_SEQUENCER_PRESSURE_H_

typedef struct
{
	uint32_t	adc_zero_data;
	uint32_t	adc_data;
	uint32_t	adc_zero_acqcounter;
	uint32_t	pressure;
}Presso_SequencerPressure_TypeDef;

extern	ADC_Drv_TypeDef	adc_Drv;

extern	void pressure_manager(void);
extern	uint32_t get_pressure(void);

#endif /* PRESSOMASTER_PROCESS_2_SEQUENCER_PRESSURE_H_ */
