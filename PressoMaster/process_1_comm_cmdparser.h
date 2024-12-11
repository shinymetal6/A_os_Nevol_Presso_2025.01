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
 * process_1_comm_cmdparser.h
 *
 *  Created on: Dec 5, 2024
 *      Author: fil
 */
#ifndef PRESSOMASTER_PROCESS_1_COMM_CMDPARSER_H_
#define PRESSOMASTER_PROCESS_1_COMM_CMDPARSER_H_


#define	CMDPARSER_RET_PRG		1
#define	CMDPARSER_RET_WAV		2
#define	CMDPARSER_RET_RUN		3
#define	CMDPARSER_RET_HLT		4

extern	uint8_t Host_pack_USB_packet(uint8_t *usb_rx_buffer,uint8_t len);
extern	uint8_t parse_packet(uint8_t *rx_buf);

#endif /* PRESSOMASTER_PROCESS_1_COMM_CMDPARSER_H_ */
