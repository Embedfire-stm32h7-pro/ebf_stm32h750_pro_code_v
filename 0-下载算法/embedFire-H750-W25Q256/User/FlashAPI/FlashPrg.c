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

#include "FlashOS.H"        // FlashOS Structures
#include "stm32h7xx.h"
#include "bsp_qspi_flash.h"
#include "bsp_led.h"   

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;
 
typedef volatile uint32_t  vu32;
typedef volatile uint16_t vu16;
typedef volatile uint8_t  vu8; 

#define PAGE_SIZE            512

/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *                    fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

static void SystemClock_Config(void);
uint8_t aux_buf[PAGE_SIZE];
uint32_t base_adr;

uint32_t Address = 0, SECTORError = 0, Index = 0;
__IO uint64_t data64 = 0;

unsigned int  Bootloader_size(void);

extern const unsigned char BootloaderImage[];

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
		if(data64 != ((uint64_t*)BootloaderImage)[Index])
		{
			MemoryProgramStatus++;
		}
		Address +=8;
		Index++;
  }
	return MemoryProgramStatus;
}

//这个函数在keil下载后，在擦除、编程都会重新调用一次此函数，而且在调用此函数之前keil会复位单片机，相当于单片机跑起来的初始化功能要在擦除、编程时都要初始化一遍
int Init (unsigned long adr, unsigned long clk, unsigned long fnc) {

	if(fnc==1)
	{/* WORKAROUND for BL: Disable interrupts to avoid SPI/DMA interrupts enabled by BL */
		__disable_irq();

    /* 在下载算法中，可以不引入复杂的芯片特性 */
    /* 使能指令缓存 */
//		SCB_EnableICache();
		/* 使能数据缓存 */
//		SCB_EnableDCache();
    
    /* Init value with the default clock after reset. Needed by HAL drivers */
		SystemInit();
		
    HAL_Init();

    SystemClock_Config();

    LED_GPIO_Config();
    /* 检查BootLoader完整性 */
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
				if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, Address, ((uint32_t)BootloaderImage)+Index) == HAL_OK)
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
		QSPI_FLASH_Init();
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
  uint8_t res = QSPI_ERROR;
  res = BSP_QSPI_Erase_Chip();
  if(res != QSPI_OK)
    return (1);/* Finished with Errors */
  else
    return (0);/* Finished without Errors */
}


/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */

int EraseSector (unsigned long adr) {
  uint8_t res = QSPI_ERROR;
	res = BSP_QSPI_Erase_Block(adr-base_adr);
  if(res != QSPI_OK)
  {
    return (1);   
  }    /* Finished with Errors */
  else
    return (0);                                     /* Finished without Errors */
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

  uint8_t res = QSPI_ERROR;
	res = BSP_QSPI_Write(buf,adr-base_adr,sz);
  if(res != QSPI_OK)
    return (1);                                        /* Finished with Errors */
  else
    return (0);                                        /* Finished without Errors */
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

  int numWR;// 实际读取的数量
  int oriAddr=adr,oriSize=sz;// 记录原地址和大小
  uint8_t res = QSPI_ERROR;

  while(sz!=0)
  {
    // 如果传入的sz大于PAGE_SIZE，拆分读取，一次读PAGE_SIZE个数据，待读取数据为sz-=numWR个
    if(sz>PAGE_SIZE)
    {
      numWR=PAGE_SIZE;
      sz-=numWR;
    }
    else
    {
      numWR=sz;// 否则就是小于等于PAGE_SIZE，直接读取numWR个数据，剩余读取数据sz为0个
      sz=0;
    }
    
    res = BSP_QSPI_FastRead(aux_buf,adr-base_adr,numWR);
    if(res == QSPI_OK)
    {
      for (int i = 0; i< numWR; i++) 
      {
        if (aux_buf[i] != buf[i]) 
          return (adr+i);                 // Verification Failed (return address)
      }
      adr+=numWR;// 递增读取的地址
    }
    else
    {
      return (0x0badc0de);// failed
    }
  }

  return (oriAddr+oriSize);                      // Done successfully
}

static void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1);
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    while(1);
  }
}
