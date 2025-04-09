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
 * process_1_comm_wav_parser.c
 *
 *  Created on: Jan 16, 2025
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"
#include "presso.h"
#include "process_1_comm_wav_parser.h"

extern	NevolSystem_typedef	NevolSystem;

#define	BANK_BACKUP_ADDRESS	0x30000000

void wav_flash_to_bank2OLD(uint8_t *wav_ptr,uint32_t wav_file_len , uint32_t offset)
{
Wav_Header_TypeDef *Wav = (Wav_Header_TypeDef *)wav_ptr;
uint32_t			i,hdr_len;
uint8_t				*backup_addr;
uint8_t				*rw_flash_ptr;

	if ( wav_file_len >= WAV_MAX_SIZE)
		return;

	rw_flash_ptr = (uint8_t *)(BANK_2_ADDRESS+(offset >> 1)*2*WAV_MAX_SIZE);
	A_copy32((uint8_t *)(rw_flash_ptr),(uint8_t *)BANK_BACKUP_ADDRESS,FLASH_SECTOR_SIZE);

	hdr_len = wav_ptr - (uint8_t *)&Wav->first_audio_sample;
	backup_addr = (uint8_t *)(BANK_BACKUP_ADDRESS+((offset & 1)*WAV_MAX_SIZE));

	for(i=0;i<WAV_MAX_SIZE;i++)
	{
		if ( i <= wav_file_len+hdr_len)
			backup_addr[i] = wav_ptr[i];
		else
			backup_addr[i] = 0xff;
	}

	backup_addr = (uint8_t *)BANK_BACKUP_ADDRESS;

	flash_update((uint8_t *)backup_addr,rw_flash_ptr,FLASH_SECTOR_SIZE);
}


uint32_t wav_flash_to_bank2(uint8_t *wav_ptr,uint32_t wav_file_len , uint32_t offset)
{
uint8_t		*rw_flash_ptr;
uint8_t		*rw_bk_ptr;
uint32_t	flash_ret;
	if ( wav_file_len >= WAV_MAX_SIZE)
		return 1;


	__disable_irq();

	switch(offset)
	{
	case	0	:
	case	1	:	rw_flash_ptr = (uint8_t *)(BANK_2_ADDRESS + FLASH_SECTOR_SIZE*0);break;
	case	2	:
	case	3	:	rw_flash_ptr = (uint8_t *)(BANK_2_ADDRESS + FLASH_SECTOR_SIZE*1);break;
	case	4	:
	case	5	:	rw_flash_ptr = (uint8_t *)(BANK_2_ADDRESS + FLASH_SECTOR_SIZE*2);break;
	case	6	:
	case	7	:	rw_flash_ptr = (uint8_t *)(BANK_2_ADDRESS + FLASH_SECTOR_SIZE*3);break;
	default		:	return 1;
	}
	if ( flash_probe_address(rw_flash_ptr))
	{
		flash_erase_by_address(rw_flash_ptr,FLASH_SECTOR_SIZE);
		__enable_irq();
		return 1;
	}
	// 1 - copy 128k from flash in backup area
	A_copy32(rw_flash_ptr,(uint8_t *)BANK_BACKUP_ADDRESS,FLASH_SECTOR_SIZE);
	// 2 - copy wav to appropriate 64k address in BANK_BACKUP_ADDRESS
	rw_bk_ptr = (uint8_t *)BANK_BACKUP_ADDRESS;
	if (offset&1)
		rw_bk_ptr += WAV_MAX_SIZE;
	A_copy32(wav_ptr,rw_bk_ptr,wav_file_len);

	// 3 - flash
	flash_ret = flash_update((uint8_t *)BANK_BACKUP_ADDRESS,rw_flash_ptr,FLASH_SECTOR_SIZE);
	// 4 - read back the written data
	A_copy32(rw_flash_ptr,(uint8_t *)BANK_BACKUP_ADDRESS,FLASH_SECTOR_SIZE);
	__enable_irq();
	return flash_ret;
}
