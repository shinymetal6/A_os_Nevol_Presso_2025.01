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
 * command_parser.h
 *
 *  Created on: Jul 9, 2024
 *      Author: fil
 */

#ifndef STM32H743_PRESSC_COMMAND_PARSER_H_
#define STM32H743_PRESSC_COMMAND_PARSER_H_

extern	uint8_t parse_packet(uint8_t *rx_buf);
extern	uint8_t System_Process_host_Commands(void);
extern	uint8_t Host_pack_USB_packet(uint8_t len);

#endif /* STM32H743_PRESSC_COMMAND_PARSER_H_ */
