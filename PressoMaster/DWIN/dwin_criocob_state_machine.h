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
 * dwin_criocob_state_machine.h
 *
 *  Created on: Dec 10, 2024
 *      Author: fil
 */
#ifndef PRESSOMASTER_DWIN_DWIN_CRIOCOB_STATE_MACHINE_H_
#define PRESSOMASTER_DWIN_DWIN_CRIOCOB_STATE_MACHINE_H_

#include "../presso.h"
#ifdef	MACHINE_IS_BIOCRIOCOB

#define	DWIN_PAGE_ADDRESS				0x0084
#define	DWIN_KBD_PAGE_DATA				0x5a01000b
#define	DWIN_BIOACTIVE_CRIO_PAGE_DATA	0x5a01000c
#define	DWIN_BIOACTIVE_RUN_PAGE_DATA	0x5a01000e
#define	DWIN_COB_PAGE_DATA				0x5a010011
#define	DWIN_COB_GOSTART_PAGE_DATA		0x5a010010
#define	DWIN_COB_PAGE_A_DATA			0x5a010012
#define	DWIN_COB_PAGE_B_DATA			0x5a010013
#define	DWIN_COB_PAGE_C_DATA			0x5a010014
#define	DWIN_BIO_PAGE_DATA				0x5a01000d
#define	DWIN_CRIO_PAGE_DATA				0x5a01000f
#define	DWIN_CRIO_RUN_PAGE_DATA			0x5a010010

#define	COB_TIMEPLUS_KEY				0x4464
#define	COB_TIMEMINUS_KEY				0x4363
#define	COB_GOSTART_KEY					0x5878
#define	COB_GOSTOP_KEY					0x5a7a
#define	COB_A_KEY						0x5070
#define	COB_B_KEY						0x5071
#define	COB_C_KEY						0x5272

#define	BIO_TIMEPLUS_KEY				0x4464
#define	BIO_TIMEMINUS_KEY				0x4363
#define	BIO_POWERPLUS_KEY				0x4666
#define	BIO_POWERMINUS_KEY				0x4565
#define	BIO_START_KEY					0x5878
#define	BIO_STOP_KEY					0x5a7a
#define	CRIO_TIMEPLUS_KEY				0x4464
#define	CRIO_TIMEMINUS_KEY				0x4363
#define	CRIO_START_KEY					0x5878
#define	CRIO_STOP_KEY					0x5a7a

#define	BIO_TIME_SX_VAR_ADDR			0x2100
#define	BIO_TIME_DX_VAR_ADDR			0x2101
#define	BIO_POWER_VAR_ADDR				0x2102
#define	CRIO_TIME_SX_VAR_ADDR			0x2100
#define	CRIO_TIME_DX_VAR_ADDR			0x2101
#define	COB_TIME_SX_VAR_ADDR			0x2100
#define	COB_TIME_DX_VAR_ADDR			0x2101

#define	BIO_KEY							0x4161
#define	CRIO_KEY						0x4262

typedef struct
{
	uint8_t				biocriocob_status;
	uint16_t			biocrio_timeout;
	uint16_t			biocrio_counter;
	uint16_t			bioactive_time;
	uint16_t			bioactive_counter;
	uint16_t			bioactive_power;
	uint16_t			crio_time;
	uint16_t			crio_counter;
	uint16_t			cobroll_time;
	uint16_t			cobroll_counter;
	uint16_t			cobroll_counter_timeA;
	uint16_t			cobroll_counter_timeB;
	uint16_t			cobroll_counter_timeC;
}DWIN_criocob_t;

/* biocriocob_status */
#define	DWIN_BIO_ACTIVE				0x01
#define	DWIN_CRIO_ACTIVE			0x02
#define	DWIN_COB_A_ACTIVE			0x10
#define	DWIN_COB_B_ACTIVE			0x20
#define	DWIN_COB_C_ACTIVE			0x40
#define	DWIN_COB_ACTIVE				0x80

/* biocriocob_timeout*/
#define	DWIN_BRIOCRIO_TIMEOUT		300

/* states in NevolSystem */

#define	DWIN_STATE_RESET			0
#define	DWIN_STATE_WAIT_LCD_READY	1
#define	DWIN_STATE_POWER_ON			2
#define	DWIN_STATE_KBD				3
#define	DWIN_STATE_BIOCRIO_PAGE		4
#define	DWIN_STATE_BIO_START_PAGE	5
#define	DWIN_STATE_BIO_INIT_PAGE 	6
#define	DWIN_STATE_BIO_INIT1_PAGE 	7
#define	DWIN_STATE_BIO_INIT2_PAGE 	8
#define	DWIN_STATE_BIO_PAGE 		9
#define	DWIN_STATE_CRIO_START_PAGE	10
#define	DWIN_STATE_CRIO_INIT_PAGE 	11
#define	DWIN_STATE_CRIO_INIT1_PAGE 	12
#define	DWIN_STATE_CRIO_PAGE 		13
#define	DWIN_STATE_COB_INIT_PAGE	14
#define	DWIN_STATE_COB_PAGE 		15
#define	DWIN_STATE_COB_A_INIT_PAGE	16
#define	DWIN_STATE_COB_A_PAGE 		17
#define	DWIN_STATE_COB_B_INIT_PAGE	18
#define	DWIN_STATE_COB_B_PAGE 		19
#define	DWIN_STATE_COB_C_INIT_PAGE	20
#define	DWIN_STATE_COB_C_PAGE 		21
#define	DWIN_STATE_COB_ENDPAGE 		22


#define	DWIN_BIO_PROGRAM_NUMBER		1
#define	DWIN_CRIO_PROGRAM_NUMBER	2
#define	DWIN_COB_A_PROGRAM_NUMBER	3
#define	DWIN_COB_B_PROGRAM_NUMBER	4
#define	DWIN_COB_C_PROGRAM_NUMBER	5

extern	uint8_t dwin_state_machine(uint32_t uart2_driver_handle);
extern	void 	process_from_dwin(uint32_t uart2_driver_handle,uint8_t  *dwin_rx_packet,uint8_t  dwin_rx_packet_len);
#endif // #ifdef	MACHINE_IS_BIOCRIOCOB

#endif /* PRESSOMASTER_DWIN_DWIN_CRIOCOB_STATE_MACHINE_H_ */
