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
 * presso.h
 *
 *  Created on: Nov 25, 2024
 *      Author: fil
 */

#ifndef PRESSO_H_
#define PRESSO_H_

#define	PRESSO_SEQUENCER_PROCESS	2
#define	PRESSO_SEQUENCER_MBX		0
#define	PRESSO_COMM_MBX				1
#define	PRESSO_HMI_MBX				3

/*
#define	xmodem_data_area	0x30010000
#define	xmodem_decoded_area	0x30000000
*/
#define	xmodem_data_len		0x1ffff

#define	USB_RX_BUF_SIZE	256
#define	USB_TX_BUF_SIZE	256

typedef struct
{
	uint8_t				status;
	uint8_t				machine;
	uint8_t				state;
	uint8_t				powerup_val;
	uint32_t			password;
	uint32_t			password_index;
	uint8_t				dwin_rxpacket[6];
	uint8_t				dwin_rxpacket_index;
	uint8_t				command_from_host[32];
	uint8_t				param_from_host;
	uint8_t				usb_rx_buffer_index;
	uint8_t				usb_flags;
}NevolSystem_t;

/* status */
#define	DWIN_STATUS_OPERATIONAL		0x80
#define	DWIN_STATUS_DISPLAY_OK		0x40
/* state */


/* usb_flags */
#define	USB_FLAGS_HEADEROK		0x40
#define	USB_FLAGS_PKTCOMPLETE	0x80

#define	COMM_NORMAL_MODE		0
#define	COMM_XMODEM_MODE		1

#define	TRANSFER_XMODEM_CSV		0
#define	TRANSFER_XMODEM_WAV		1

#define	XMODEM_TIMEOUT			5

#define	PRESSO_PROGRAM_SIZE		4096

#define	PRESSO_PROGRAM_0_INDEX	0
#define	PRESSO_MAX_PROGRAMS		32

#define BANK_2_ADDRESS	0x08100000

#include "process_1_comm_cmdparser.h"
#include "process_1_comm_csv_parser.h"
#include "process_1_comm_wav_parser.h"
#include "process_2_sequencer.h"
#include "process_2_sequencer_memparser.h"
#include "process_2_sequencer_audio.h"
#include "process_2_sequencer_pressure.h"

extern	NevolSystem_t				NevolSystem;
extern	Presso_ee_TypeDef			Presso_ee;
extern	Presso_ee_TypeDef			Presso_opening_ee;
extern	Presso_ee_TypeDef			Presso_closing_ee;
extern	Presso_Sequencer_TypeDef	Presso_Sequencer;

extern	uint32_t analyze_dwin_packet(uint8_t *packet , uint16_t len);

#endif /* PRESSO_H_ */
