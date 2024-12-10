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
 * activators.c
 *
 *  Created on: Jul 9, 2024
 *      Author: fil
 */


#include "main.h"
#include "A_os_includes.h"

#ifdef NEVOL_2416176_01

#include "nevol_system.h"
#include <ctype.h>

extern	NevolSystem_TypeDef	NevolSystem;

NevolSystemGPIO_TypeDef	NevolSystemGPIO[16] =
{
		{
			OUT15_GPIO_Port,
			OUT15_Pin
		},
		{
			OUT14_GPIO_Port,
			OUT14_Pin
		},
		{
			OUT13_GPIO_Port,
			OUT13_Pin
		},
		{
			OUT12_GPIO_Port,
			OUT12_Pin
		},
		{
			OUT11_GPIO_Port,
			OUT11_Pin
		},
		{
			OUT10_GPIO_Port,
			OUT10_Pin
		},
		{
			OUT9_GPIO_Port,
			OUT9_Pin
		},
		{
			OUT8_GPIO_Port,
			OUT8_Pin
		},
		{
			OUT7_GPIO_Port,
			OUT7_Pin
		},
		{
			OUT6_GPIO_Port,
			OUT6_Pin
		},
		{
			OUT5_GPIO_Port,
			OUT5_Pin
		},
		{
			OUT4_GPIO_Port,
			OUT4_Pin
		},
		{
			OUT3_GPIO_Port,
			OUT3_Pin
		},
		{
			OUT2_GPIO_Port,
			OUT2_Pin
		},
		{
			OUT1_GPIO_Port,
			OUT1_Pin
		},
		{
			OUT0_GPIO_Port,
			OUT0_Pin
		},
};

void line_set(uint8_t line_num , uint8_t line_state)
{
	if ( line_state == 0 )
	{
		NevolSystem.line_status &= ~line_num;
		HAL_GPIO_WritePin(NevolSystemGPIO[line_num-1].GPIOx,  NevolSystemGPIO[line_num-1].GPIO_Pin,GPIO_PIN_RESET);
	}
	else
	{
		NevolSystem.line_status |= line_num;
		HAL_GPIO_WritePin(NevolSystemGPIO[line_num-1].GPIOx,  NevolSystemGPIO[line_num-1].GPIO_Pin,GPIO_PIN_SET);
	}
}
void line_reset(void)
{
uint8_t	i;
	NevolSystem.line_status = 0;
	NevolSystem.motor_status = 0;
	for(i=0;i<16;i++)
		HAL_GPIO_WritePin(NevolSystemGPIO[i].GPIOx,  NevolSystemGPIO[i].GPIO_Pin,GPIO_PIN_RESET);
}

#define	CCR_VAL 		15000
#define	CCR_VAL_UNIT	(CCR_VAL / 100 )

extern	uint32_t		pwm_driver_handle1;
extern	uint32_t		pwm_driver_handle2;
extern	uint32_t		pwm_driver_handle3;
extern	uint32_t		pwm_driver_handle4;
extern	uint32_t		pwm_driver_handle5;

void heater_set(uint8_t heater_num,uint8_t duty)
{
	/*
	switch(heater_num)
	{
	case 1 : TIM2->CCR1 = CCR_VAL_UNIT * duty;NevolSystem.htr1_period=duty;break;
	case 2 : TIM3->CCR4 = CCR_VAL_UNIT * duty;NevolSystem.htr2_period=duty;break;
	case 3 : TIM3->CCR3 = CCR_VAL_UNIT * duty;NevolSystem.htr3_period=duty;break;
	case 4 : TIM3->CCR2 = CCR_VAL_UNIT * duty;NevolSystem.htr4_period=duty;break;
	case 5 : TIM3->CCR1 = CCR_VAL_UNIT * duty;NevolSystem.htr5_period=duty;break;
	}
	*/
	switch(heater_num)
	{
	case 1 : pwm_set_width(pwm_driver_handle1,duty);break;
	case 2 : pwm_set_width(pwm_driver_handle2,duty);break;
	case 3 : pwm_set_width(pwm_driver_handle3,duty);break;
	case 4 : pwm_set_width(pwm_driver_handle4,duty);break;
	case 5 : pwm_set_width(pwm_driver_handle5,duty);break;
	}
}

void heater_reset(void)
{
	pwm_set_width(pwm_driver_handle1,0);
	pwm_set_width(pwm_driver_handle2,0);
	pwm_set_width(pwm_driver_handle3,0);
	pwm_set_width(pwm_driver_handle4,0);
	pwm_set_width(pwm_driver_handle5,0);
	NevolSystem.htr1_period=NevolSystem.htr2_period=NevolSystem.htr3_period=NevolSystem.htr4_period=NevolSystem.htr5_period=0;
}

void heater_prescaler_set(uint16_t prescaler)
{
	TIM2->PSC = TIM3->PSC = prescaler;
}

uint8_t NevolSystemTemperatureTab[11] =
{
	37,39,41,43,45,47,49,51,53,55,57
};


void temperature_set(uint8_t heater_num,uint8_t temperature)
{
uint8_t	local_t = temperature;
	local_t -= 33;
	local_t /= 2;
	local_t -= 1;
	if ( local_t < 11 )
	{
		heater_set(heater_num,NevolSystemTemperatureTab[local_t]);
		switch(heater_num)
		{
		case 1	:	NevolSystem.htr1_temperature = NevolSystemTemperatureTab[local_t]; break;
		case 2	:	NevolSystem.htr2_temperature = NevolSystemTemperatureTab[local_t]; break;
		case 3	:	NevolSystem.htr3_temperature = NevolSystemTemperatureTab[local_t]; break;
		case 4	:	NevolSystem.htr4_temperature = NevolSystemTemperatureTab[local_t]; break;
		case 5	:	NevolSystem.htr5_temperature = NevolSystemTemperatureTab[local_t]; break;
		}
	}
}

void motor_set(uint8_t motor_state)
{
	if ( motor_state )
	{
		HAL_GPIO_WritePin(MOTOR_ON_GPIO_Port,  MOTOR_ON_Pin,GPIO_PIN_SET);
		NevolSystem.motor_status = 1;
	}
	else
	{
		HAL_GPIO_WritePin(MOTOR_ON_GPIO_Port,  MOTOR_ON_Pin,GPIO_PIN_RESET);
		NevolSystem.motor_status = 0;
	}
}

void buzzer_timer_callback(void)
{
	if ( NevolSystem.buzzer_timer )
	{
		NevolSystem.buzzer_timer  --;
		if ( NevolSystem.buzzer_timer == 0 )
		{
			//buzzer_off();
		}
	}
}

void buzzer_set(uint16_t buzzer_time)
{
	NevolSystem.buzzer_timer = buzzer_time;
	//buzzer_on();
}

void buzzer_reset(void)
{
	NevolSystem.buzzer_timer = 0;
	//buzzer_off();
}

#endif // #ifdef NEVOL_2416176_01

