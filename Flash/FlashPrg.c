/**************************************************************************//**
 * @file     FlashPrg.c
 * @brief    Flash Programming Functions adapted for New Device Flash
 * @version  V1.0.0
 * @date     10. January 2018
 ******************************************************************************/
/*
 * Copyright (c) 2010-2018 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include "../FlashOS.H"        // FlashOS Structures

/* 
   Mandatory Flash Programming Functions (Called by FlashOS):
                int Init        (unsigned long adr,   // Initialize Flash
                                 unsigned long clk,
                                 unsigned long fnc);
                int UnInit      (unsigned long fnc);  // De-initialize Flash
                int EraseSector (unsigned long adr);  // Erase Sector Function
                int ProgramPage (unsigned long adr,   // Program Page Function
                                 unsigned long sz,
                                 unsigned char *buf);

   Optional  Flash Programming Functions (Called by FlashOS):
                int BlankCheck  (unsigned long adr,   // Blank Check
                                 unsigned long sz,
                                 unsigned char pat);
                int EraseChip   (void);               // Erase complete Device
      unsigned long Verify      (unsigned long adr,   // Verify Function
                                 unsigned long sz,
                                 unsigned char *buf);

       - BlanckCheck  is necessary if Flash space is not mapped into CPU memory space
       - Verify       is necessary if Flash space is not mapped into CPU memory space
       - if EraseChip is not provided than EraseSector for all sectors is called
*/

#include <string.h>
#include "main.h"
#include "gpio.h"
#include "quadspi.h"
#include "w25qxx.h"

#define SECTOR_SIZE 	(4096)
#define PAGE_SIZE			(256)
#define DEVICE_ID			(0xEF16)
static unsigned long devAdr;

/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *                    fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */
int Init (unsigned long adr, unsigned long clk, unsigned long fnc)
{
	devAdr = adr;
	
	volatile int i;
	volatile unsigned char *ptr = (volatile unsigned char *)&hqspi;
	for (i = 0; i < sizeof(hqspi); i++) {
		*ptr++ = 0U;
	}

	/* Initialize system and hal library */
	__disable_irq();

	SystemInit();
	
	HAL_Init();
	
	SystemClock_Config();
	
	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_QUADSPI_Init();
	
	/* Check flash device id */
	if (w25qxx_read_deviceid() != DEVICE_ID) {
		return (1);
	}
	
	/* Mapping the qspi flash into memory (0x90000000-0x90800000) */
	/* Default Verify and BlankCheck function nead memory mapped mode  */
	if (w25qxx_memory_mapped() != 0) {
		return (1);
	}
	
	LED_ON;
  return (0);                                  // Finished without Errors
}

/*
 *  De-Initialize Flash Programming Functions
 *    Parameter:      fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

int UnInit (unsigned long fnc) 
{
	LED_OFF;
	
	/* Deinitialize all configured peripherals */
	if (HAL_OK != HAL_QSPI_DeInit(&hqspi)) {
		return (1);
	}
	HAL_GPIO_DeInit(LED_GPIO_Port, LED_Pin);
	HAL_GPIO_DeInit(KEY1_GPIO_Port, KEY1_Pin);
	HAL_GPIO_DeInit(KEY2_GPIO_Port, KEY2_Pin);
	
	/* Deinitialize system and hal library */
	if (HAL_OK != HAL_RCC_DeInit()) {
		return (1);
	}
	
	if (HAL_OK != HAL_DeInit()) {
		return (1);
	}
  return (0);                                  // Finished without Errors
}


/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */

int EraseChip (void) 
{
	// LED_ON;
	
  if (0 != w25qxx_erase_chip()) {
		goto errout;
	}
	
	if (0 != w25qxx_memory_mapped()) {
		goto errout;
	}
	
	// LED_OFF;
  return (0);                                  // Finished without Errors
	
errout:
	// LED_OFF;
	return (1);
}


/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */

int EraseSector (unsigned long adr) 
{
	// LED_ON;
	
  if (adr < devAdr) {
		goto errout;
	}
	
	if (0 != w25qxx_erase_sector(adr - devAdr)) {
		goto errout;
	}
	
	if (0 != w25qxx_memory_mapped()) {
		goto errout;
	}
	
	// LED_OFF;
	return (0);                                  // Finished without Errors
	
errout:
	// LED_OFF;
	return (1);
}


/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */

int ProgramPage (unsigned long adr, unsigned long sz, unsigned char *buf) 
{	
	// LED_ON;
	
  if (adr < devAdr) {
		goto errout;
	}

	if (0 != w25qxx_page_program(buf, adr - devAdr, sz)) {
		goto errout;
	}
	
/*
	if (0 != w25qxx_memory_mapped()) {
		goto errout;
	}
*/
	
	// LED_OFF;
  return (0);                                  // Finished without Errors
	
errout:
	// LED_OFF;
	return (1);
}
