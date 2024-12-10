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
 * activators.h
 *
 *  Created on: Jul 9, 2024
 *      Author: fil
 */

#ifndef STM32H743_PRESSC_ACTIVATORS_H_
#define STM32H743_PRESSC_ACTIVATORS_H_

extern	void heater_set(uint8_t heater_num,uint8_t duty);
extern	void heater_reset(void);
extern	void line_reset(void);
extern	void line_set(uint8_t line_num , uint8_t line_state);
extern	void motor_set(uint8_t motor_state);
extern	void buzzer_set(uint16_t time);
extern	void buzzer_reset(void);
extern	void heater_prescaler_set(uint16_t prescaler);
extern	void temperature_set(uint8_t heater_num,uint8_t temperature);

#endif /* STM32H743_PRESSC_ACTIVATORS_H_ */
