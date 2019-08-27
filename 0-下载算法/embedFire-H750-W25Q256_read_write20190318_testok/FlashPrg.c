/* -----------------------------------------------------------------------------
 * Copyright (c) 2016 ARM Ltd.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * $Date:        10. October 2018
 * $Revision:    V1.0.0
 *
 * Project:      Flash Device Description for
 *               STM32H743 W25Q256 SPIFI Flash
 * --------------------------------------------------------------------------- */

#include "..\FlashOS.H"        // FlashOS Structures
#include "stm32h7xx.h"


typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;
 
typedef volatile uint32_t  vu32;
typedef volatile uint16_t vu16;
typedef volatile uint8_t  vu8; 

#define PAGE_SIZE            4096

/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *                    fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */
 
uint8_t aux_buf[PAGE_SIZE];
uint32_t base_adr;


extern uint32_t QSPI_Init(void);
extern uint32_t QSPI_WritePage(unsigned long adr, unsigned long sz, unsigned char *buf);
extern uint32_t QSPI_ReadPage(unsigned long adr, unsigned long sz, unsigned char *buf);
extern uint32_t QSPI_EraseSector(uint32_t adr);

//uint32_t FirstSector = 0, NbOfSectors = 0;
uint32_t Address = 0, SECTORError = 0, Index = 0;
__IO uint64_t data64 = 0;


unsigned int  Bootloader_size(void);

extern const unsigned char BootloaderImage[];
//uint64_t FlashWord[4] = { 0x0102030405060708,
//                          0x1112131415161718,
//                          0x2122232425262728,    
//                          0x3132333435363738
//                        };

uint32_t ISBootloadrOK(void)
{
	__IO uint32_t MemoryProgramStatus = 0;
  /* 检查BootLoader是否完整
      MemoryProgramStatus = 0: 数据完整
      MemoryProgramStatus != 0: 数据缺失 */
  Address = 0x08000000;
  MemoryProgramStatus = 0x0;

  while (Address < 0x08000000+Bootloader_size())
  {

		data64 = *(uint64_t*)Address;
		__DSB();
		if(data64 != *(uint64_t*)((uint32_t)BootloaderImage)+Index)
		{
			MemoryProgramStatus++;
		}
		Address +=8;
		Index+=8;
  }
	return MemoryProgramStatus;
}

//这个函数在keil下载后，在擦除、编程都会重新调用一次此函数，而且在调用此函数之前keil会复位单片机，相当于单片机跑起来的初始化功能要在擦除、编程时都要初始化一遍
int Init (unsigned long adr, unsigned long clk, unsigned long fnc) {

	if(fnc==1)
	{/* WORKAROUND for BL: Disable interrupts to avoid SPI/DMA interrupts enabled by BL */
		__disable_irq();
			/* 使能指令缓存 */
		SCB_EnableICache();
		/* 使能数据缓存 */
		SCB_EnableDCache();
		SystemInit();
		/* Init value with the default clock after reset. Needed by HAL drivers */
//		SystemCoreClock = 64000000;
		if (ISBootloadrOK() != 0)
		{
			static FLASH_EraseInitTypeDef EraseInitStruct;

			/* 解锁Flash以启用闪存控制寄存器访问 */
			HAL_FLASH_Unlock();

			/* 配置擦除结构体*/
			EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
			EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
			EraseInitStruct.Banks         = FLASH_BANK_1;
			EraseInitStruct.Sector        = FLASH_SECTOR_0;
			EraseInitStruct.NbSectors     = 1;
			
			if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
			{
				return (1);
			}

			/* 逐字编程用户Flash区域 */

			Address = 0x08000000;
			Index =0;
			while (Address < 0x08000000+Bootloader_size())
			{
				if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, Address, (uint64_t)((uint32_t)BootloaderImage)+Index) == HAL_OK)
				{
					Address = Address + 32;
					Index = Index + 32;
				}
				else
				{
					return (1);
				}
			}	

			/* 锁定闪存以禁用闪存控制寄存器访问（建议保护闪存不受可能的意外操作）***/
			HAL_FLASH_Lock();	
		}			
		/*初始化Flash*/
		QSPI_Init();
	}
	base_adr = adr;
	
  return (0);
}


/*
 *  De-Initialize Flash Programming Functions
 *    Parameter:      fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

int UnInit (unsigned long fnc) {

  return (0);
}


/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */

int EraseChip (void) {

//  W25QXX_Erase_Chip();
  return (0);                                        /* Finished without Errors */
}


/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */

int EraseSector (unsigned long adr) {

	uint32_t status;
	status = QSPI_EraseSector(adr-base_adr);
	if(status)
  return (0);  
	else
  return (1); 	/* Finished without Errors */
}


/*
 *  Blank Check Checks if Memory is Blank
 *    Parameter:      adr:  Block Start Address
 *                    sz:   Block Size (in bytes)
 *                    pat:  Block Pattern
 *    Return Value:   0 - OK,  1 - Failed
 */

int BlankCheck (unsigned long adr, unsigned long sz, unsigned char pat) {

  return (1);                                        /* Always Force Erase */
}


/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */

int ProgramPage (unsigned long adr, unsigned long sz, unsigned char *buf) {
	
//	 unsigned char temp[8]={0xab,0Xcd,0X00,0X20,0Xad,0x00,0x00,0x90};
	uint32_t status;
	status = QSPI_WritePage(adr-base_adr,sz,buf);
	if(status)
  return (0);  
	else
  return (1); 	/* Finished without Errors */
//	QSPI_WritePage(0,8,temp);
//	return (0); 
}


/*  
 *  Verify Flash Contents
 *    Parameter:      adr:  Start Address
 *                    sz:   Size (in bytes)
 *                    buf:  Data
 *    Return Value:   (adr+sz) - OK, Failed Address
 */

/*
   Verify function is obsolete because all other function leave 
    the SPIFI in memory mode so a memory compare could be used.
 */
unsigned long Verify (unsigned long adr, unsigned long sz, unsigned char *buf) {

//  int i;
//	uint32_t status;
//  status = QSPI_ReadPage(adr-base_adr, sz,aux_buf);
//	if(status)
//	{
//		for (i = 0; i< sz; i++) 
//		{
//			if (aux_buf[i] != buf[i]) 
//				return (adr+i); // aux_buf[0];                 // Verification Failed (return address)
//		}
		return (adr+sz);                      // Done successfully
//	}
//	else
//	{
//		return (SystemCoreClock);
//	}
}
