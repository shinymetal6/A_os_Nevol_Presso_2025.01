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
 * dwin_common.h
 *
 *  Created on: Dec 10, 2024
 *      Author: fil
 */
#ifndef PRESSOMASTER_DWIN_DWIN_COMMON_H_
#define PRESSOMASTER_DWIN_DWIN_COMMON_H_


#define	DWIN_PKT_MAX_LEN	4

#define	DWIN_PKT_QUEUE_LEN	8


typedef struct
{
	uint8_t				start_flag_1;
	uint8_t				start_flag_2;
	uint8_t				number_of_bytes;
	uint8_t				command;
	uint8_t				address_h;
	uint8_t				address_l;
	uint8_t				pktbytes[DWIN_PKT_MAX_LEN];
}DWIN_packet_typedef;

#define	HMI_HEADER1		0x5a
#define	HMI_HEADER2		0xa5
#define	HMI_WRITE_CMD	0x82
#define	HMI_READ_CMD	0x83

#define	DWIN_KBD_OK		0xf1
#define	DWIN_KBD_BACK	0xf0

#define	PLAY_PAUSE_BTN_ADDR		0x8100
#define	PLAY_PAUSE_BTN_PLAY		0x0000
#define	PLAY_PAUSE_BTN_PAUSE	0x0001

#define	PRESSURE_BASE_ADDRESS	0x5000
#define	TSECTOR_BASE_ADDRESS	0x6000


extern	DWIN_packet_typedef			DWIN_packet;
extern	NevolSystem_typedef			NevolSystem;

extern	uint32_t process_from_dwin(uint32_t uart1_driver_handle,uint8_t *uart1_rx_buffer,uint32_t uart_rxlen);
extern	uint32_t compile_and_send_7b_dwin_packet(uint32_t uart_driver_handle,uint16_t address,uint32_t data);
extern	uint32_t compile_and_send_5b_dwin_packet_nowait(uint32_t uart_driver_handle,uint16_t address,uint16_t data);
extern	uint32_t compile_and_send_5b_dwin_packet(uint32_t uart_driver_handle,uint16_t address,uint16_t data);
extern	void 	 dwin_update_fields(uint32_t uart_driver_handle, Presso_ee_TypeDef *current_presso_ee);
extern	void 	 dwin_clear_fields(uint32_t uart_driver_handle);
extern	uint32_t dwin_state_machine_reset (void);


#endif /* PRESSOMASTER_DWIN_DWIN_COMMON_H_ */
