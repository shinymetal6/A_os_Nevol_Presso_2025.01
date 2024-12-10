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
 * dwin_state_machine.h
 *
 *  Created on: Dec 10, 2024
 *      Author: fil
 */
#ifndef PRESSOMASTER_DWIN_DWIN_STATE_MACHINE_H_
#define PRESSOMASTER_DWIN_DWIN_STATE_MACHINE_H_

#define	DWIN_PAGE_ADDRESS				0x0084
#define	DWIN_KBD_PAGE_DATA				0x5a01000b
#define	DWIN_BIOACTIVE_CRIO_PAGE_DATA	0x5a01000c
#define	DWIN_COB_PAGE_DATA				0x5a010011
#define	DWIN_COB_GOSTART_PAGE_DATA		0x5a010010
#define	DWIN_COB_PAGE_A_DATA			0x5a010012
#define	DWIN_COB_PAGE_B_DATA			0x5a010013
#define	DWIN_COB_PAGE_C_DATA			0x5a010014
#define	DWIN_BIO_PAGE_DATA				0x5a01000d
#define	DWIN_CRIO_PAGE_DATA				0x5a01000f


#define	COB_TIMEPLUS_KEY		0x4464
#define	COB_TIMEMINUS_KEY		0x4363
#define	COB_GOSTART_KEY			0x5878
#define	COB_A_KEY				0x5070
#define	COB_B_KEY				0x5071
#define	COB_C_KEY				0x5272

#define	BIO_KEY					0x4161
#define	CRIO_KEY				0x4262


extern	uint8_t dwin_state_machine(uint32_t uart2_driver_handle);
extern	void 	process_from_dwin(uint32_t uart2_driver_handle,uint8_t  *dwin_rx_packet,uint8_t  dwin_rx_packet_len);

#endif /* PRESSOMASTER_DWIN_DWIN_STATE_MACHINE_H_ */
