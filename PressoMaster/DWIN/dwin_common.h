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


#define	DWIN_PKT_MAX_LEN	8


typedef struct
{
	uint8_t				start_flag_1;
	uint8_t				start_flag_2;
	uint8_t				number_of_bytes;
	uint8_t				command;
	uint8_t				address_h;
	uint8_t				address_l;
	uint8_t				pktbytes[DWIN_PKT_MAX_LEN];
}DWIN_packet_t;

#define	HMI_HEADER1		0x5a
#define	HMI_HEADER2		0xa5
#define	HMI_WRITE_CMD	0x82
#define	HMI_READ_CMD	0x83

typedef struct
{
	uint8_t				state;
}DWIN_combo_t;


#define	DWIN_KBD_OK		0xf1
#define	DWIN_KBD_BACK	0xf0
extern	DWIN_packet_t	DWIN_packet;
extern	NevolSystem_t	NevolSystem;

extern	uint32_t compile_and_send_7b_dwin_packet(uint32_t uart2_driver_handle,uint16_t address,uint32_t data);
extern	uint32_t compile_and_send_5b_dwin_packet(uint32_t uart2_driver_handle,uint16_t address,uint8_t data0,uint8_t data1);
extern	uint32_t password_manager(uint32_t uart2_driver_handle,uint8_t  *dwin_rx_packet,uint8_t  dwin_rx_packet_len);
extern	uint16_t get_kbd_key(uint8_t  *dwin_rx_packet);
extern	uint32_t check_std_reply(uint8_t  *dwin_rx_packet);


#endif /* PRESSOMASTER_DWIN_DWIN_COMMON_H_ */
