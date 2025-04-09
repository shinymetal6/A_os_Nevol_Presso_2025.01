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
 * process_1_comm_wav_parser.h
 *
 *  Created on: Jan 16, 2025
 *      Author: fil
 */
#ifndef PRESSOMASTER_PROCESS_1_COMM_WAV_PARSER_H_
#define PRESSOMASTER_PROCESS_1_COMM_WAV_PARSER_H_

#define	WAV_MAX_SIZE	65536

extern	uint32_t wav_flash_to_bank2(uint8_t *wav_ptr,uint32_t wav_file_len , uint32_t offset);

#endif /* PRESSOMASTER_PROCESS_1_COMM_WAV_PARSER_H_ */
