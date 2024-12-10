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
 * process_1_io.c
 *
 *  Created on: May 20, 2024
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"

#ifdef NEVOL_2416176_01

#include "command_parser.h"
#include "nevol_system.h"

NevolSystem_TypeDef	NevolSystem;
extern	TIM_HandleTypeDef htim2;
extern	TIM_HandleTypeDef htim3;

uint8_t	usb_rx_buffer[USB_BUF_LEN];
uint8_t	usb_tx_buffer[USB_BUF_LEN];

USB_Drv_TypeDef	USB_Drv =
{
		.data = NevolSystem.usb_rx_buf_rxed,
		.data_index = 0,
		.requested_len = USB_BUF_LEN,
		.timeout = 100,
		.wakeup_id = WAKEUP_FROM_USB_DEVICE_IRQ,
};
uint32_t		usb_driver_handle;
uint8_t			usb_buf_len;

extern	TIM_HandleTypeDef htim4;


Pwm_Control_TypeDef	Pwm_Control1 =
{
		.pwm_timer = &htim3,
		.pwm_channel = TIM_CHANNEL_1,
		.pulse_width[4] = 1000,
};

Pwm_Control_TypeDef	Pwm_Control2 =
{
		.pwm_timer = &htim3,
		.pwm_channel = TIM_CHANNEL_2,
		.pulse_width[4] = 1000,
};

Pwm_Control_TypeDef	Pwm_Control3 =
{
		.pwm_timer = &htim3,
		.pwm_channel = TIM_CHANNEL_3,
		.pulse_width[4] = 1000,
};

Pwm_Control_TypeDef	Pwm_Control4 =
{
		.pwm_timer = &htim3,
		.pwm_channel = TIM_CHANNEL_4,
		.pulse_width[4] = 1000,
};


Pwm_Control_TypeDef	Pwm_Control5 =
{
		.pwm_timer = &htim2,
		.pwm_channel = TIM_CHANNEL_1,
		.pulse_width[4] = 1000,
};

uint32_t		pwm_driver_handle1;
uint32_t		pwm_driver_handle2;
uint32_t		pwm_driver_handle3;
uint32_t		pwm_driver_handle4;
uint32_t		pwm_driver_handle5;

extern	ADC_HandleTypeDef hadc1;
extern	TIM_HandleTypeDef htim6;

ADC_Drv_TypeDef	ADC_Drv =
{
		.adc = &hadc1,
		.adc_buffer = &NevolSystem.adc_data,
		.num_channels = 1,
		.adc_timer = &htim6,
		.flags = ADC_FLAGS_FULL_WAKEUP,
};
uint32_t		adc_driver_handle;
uint32_t		adc_ops=0;

extern	void buzzer_timer_callback(void);

void process_1_io(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t		led_cntr=0;

	usb_driver_handle = usb_device_driver_register(&USB_Drv);

	pwm_driver_handle1 = pwm_register(&Pwm_Control1,0);
	pwm_init(pwm_driver_handle1);
	pwm_driver_handle2 = pwm_register(&Pwm_Control2,0);
	pwm_init(pwm_driver_handle2);
	pwm_driver_handle3 = pwm_register(&Pwm_Control3,0);
	pwm_init(pwm_driver_handle3);
	pwm_driver_handle4 = pwm_register(&Pwm_Control4,0);
	pwm_init(pwm_driver_handle4);
	pwm_driver_handle5 = pwm_register(&Pwm_Control5,0);
	pwm_init(pwm_driver_handle5);
	pwm_set_width(pwm_driver_handle1,0);
	pwm_set_width(pwm_driver_handle2,0);
	pwm_set_width(pwm_driver_handle3,0);
	pwm_set_width(pwm_driver_handle4,0);
	pwm_set_width(pwm_driver_handle5,0);

	pwm_start(pwm_driver_handle1);
	pwm_start(pwm_driver_handle2);
	pwm_start(pwm_driver_handle3);
	pwm_start(pwm_driver_handle4);
	pwm_start(pwm_driver_handle5);

	adc_driver_handle = int_adc_register(&ADC_Drv,0);
	adc_start(adc_driver_handle);

	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);

	while(1)
	{
		wait_event(EVENT_TIMER|EVENT_USB_DEVICE_IRQ|EVENT_UART2_IRQ|EVENT_ADC2_IRQ);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			led_cntr++;
			switch(led_cntr)
			{
			case 7 :
			case 9 :
				  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
				  break;
			case 10 :
				led_cntr = 0;
				break;
			default :
				  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
				  break;
			}
			HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		}

		if (( wakeup & WAKEUP_FROM_USB_DEVICE_IRQ) == WAKEUP_FROM_USB_DEVICE_IRQ)
		{
			if ( Host_pack_USB_packet(usb_get_rx_len(usb_driver_handle)) == 0 )
			{
				if ( parse_packet(NevolSystem.usb_rx_buf) )
				{
					System_Process_host_Commands();
					usb_send(usb_driver_handle,NevolSystem.system_tx_buf, NevolSystem.system_tx_buf_len);
				}
			}
		}

		if (( wakeup & WAKEUP_FROM_ADC1_IRQ) == WAKEUP_FROM_ADC1_IRQ)
		{
			adc_ops++;
		}
	}
}

#endif // #ifdef	NEVOL_2416176_01
