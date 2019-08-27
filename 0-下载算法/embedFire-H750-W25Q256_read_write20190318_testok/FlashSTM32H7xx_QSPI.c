/*************************************************************************
 *
 *   Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2018
 *
 *    File name   : FlashSTM32H7xx_QSPI.c
 *    Description : Flash Loader For Serial NOR flash
 *
 *    History :
 *    1. Date        : July, 2018
 *       Author      : Atanas Uzunov
 *       Description : Initial QSPI flashloader for Micron MT25TL01GHBB8ESF
 *
 *
 *    $Revision: 39 $
 **************************************************************************/

//#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
#include "stm32h7xx_hal.h"
uint32_t QSPI_EnterQPI(void);
#define RESULT_OK 		0
#define RESULT_ERROR  0xff

/*QSPI接口定义-开头****************************/
#define QSPI_FLASH                         QUADSPI
#define QSPI_FLASH_CLK_ENABLE()            __QSPI_CLK_ENABLE()

#define QSPI_FLASH_CLK_PIN                 GPIO_PIN_2                  
#define QSPI_FLASH_CLK_GPIO_PORT           GPIOB                       
#define QSPI_FLASH_CLK_GPIO_ENABLE()       __GPIOB_CLK_ENABLE()
#define QSPI_FLASH_CLK_GPIO_AF             GPIO_AF9_QUADSPI

#define QSPI_FLASH_BK1_IO0_PIN             GPIO_PIN_8                
#define QSPI_FLASH_BK1_IO0_PORT            GPIOF                   
#define QSPI_FLASH_BK1_IO0_CLK_ENABLE()    __GPIOF_CLK_ENABLE()
#define QSPI_FLASH_BK1_IO0_AF              GPIO_AF10_QUADSPI

#define QSPI_FLASH_BK1_IO1_PIN             GPIO_PIN_9                
#define QSPI_FLASH_BK1_IO1_PORT            GPIOF                      
#define QSPI_FLASH_BK1_IO1_CLK_ENABLE()    __GPIOF_CLK_ENABLE()
#define QSPI_FLASH_BK1_IO1_AF              GPIO_AF10_QUADSPI

#define QSPI_FLASH_BK1_IO2_PIN             GPIO_PIN_7                
#define QSPI_FLASH_BK1_IO2_PORT            GPIOF                      
#define QSPI_FLASH_BK1_IO2_CLK_ENABLE()    __GPIOF_CLK_ENABLE()
#define QSPI_FLASH_BK1_IO2_AF              GPIO_AF9_QUADSPI

#define QSPI_FLASH_BK1_IO3_PIN             GPIO_PIN_6                
#define QSPI_FLASH_BK1_IO3_PORT            GPIOF                      
#define QSPI_FLASH_BK1_IO3_CLK_ENABLE()    __GPIOF_CLK_ENABLE()
#define QSPI_FLASH_BK1_IO3_AF              GPIO_AF9_QUADSPI

#define QSPI_FLASH_CS_PIN                 GPIO_PIN_6              
#define QSPI_FLASH_CS_GPIO_PORT           GPIOG                   
#define QSPI_FLASH_CS_GPIO_CLK_ENABLE()   __GPIOG_CLK_ENABLE()
#define QSPI_FLASH_CS_GPIO_AF             GPIO_AF10_QUADSPI

/*QSPI BANK2接口定义-开头****************************/
#define QSPI_FLASH_BK2_IO0_PIN             GPIO_PIN_2                
#define QSPI_FLASH_BK2_IO0_PORT            GPIOH                   
#define QSPI_FLASH_BK2_IO0_CLK_ENABLE()    __GPIOH_CLK_ENABLE()
#define QSPI_FLASH_BK2_IO0_AF              GPIO_AF9_QUADSPI

#define QSPI_FLASH_BK2_IO1_PIN             GPIO_PIN_3                
#define QSPI_FLASH_BK2_IO1_PORT            GPIOH                      
#define QSPI_FLASH_BK2_IO1_CLK_ENABLE()    __GPIOH_CLK_ENABLE()
#define QSPI_FLASH_BK2_IO1_AF              GPIO_AF9_QUADSPI

#define QSPI_FLASH_BK2_IO2_PIN             GPIO_PIN_9                
#define QSPI_FLASH_BK2_IO2_PORT            GPIOG                      
#define QSPI_FLASH_BK2_IO2_CLK_ENABLE()    __GPIOG_CLK_ENABLE()
#define QSPI_FLASH_BK2_IO2_AF              GPIO_AF9_QUADSPI

#define QSPI_FLASH_BK2_IO3_PIN             GPIO_PIN_14                
#define QSPI_FLASH_BK2_IO3_PORT            GPIOG                      
#define QSPI_FLASH_BK2_IO3_CLK_ENABLE()    __GPIOG_CLK_ENABLE()
#define QSPI_FLASH_BK2_IO3_AF              GPIO_AF9_QUADSPI


/* The flash loader framework API declarations */
//#include "flash_loader.h"
//#include "flash_loader_extra.h"

/** default settings **/
#define QSPI_FLASH_SIZE            25
#define QSPI_PAGE_SIZE            (256)
#define QSPI_ADDRESS_WIDTH         QSPI_ADDRESS_32_BITS

#define QUAD_IN_FAST_PROG_CMD      0x32
#define QUAD_IN_FAST_READ_CMD      0xEB
#define READ_FLAG_STATUS_REG_CMD   0x05
#define SECTOR_ERASE_CMD           0x21
#define WRITE_ENABLE_CMD           0x06
#define READ_STATUS_REG_CMD        0x05

#define WRITE_STATUS_REG2_CMD      0x31
#define READ_STATUS_REG2_CMD       0x35

#define W25Q256JVEM_QE						 0x0202
#define ENTER_4BYTE_ADDR_MODE_CMD  0xB7
#define ENTER_QPI_MODE_CMD  			 0x38
/** external functions **/
/* 寄存器操作 */
#define READ_STATUS_REG1_CMD                  0x05
#define READ_STATUS_REG2_CMD                  0x35
#define READ_STATUS_REG3_CMD                  0x15

#define WRITE_STATUS_REG1_CMD                 0x01
#define WRITE_STATUS_REG2_CMD                 0x31
#define WRITE_STATUS_REG3_CMD                 0x11

/** external data **/

uint32_t QSPI_Init(void);
uint32_t QSPI_WritePage(unsigned long adr, unsigned long sz, unsigned char *buf);
uint32_t QSPI_ReadPage(unsigned long adr, unsigned long sz, unsigned char *buf);
uint32_t QSPI_EraseSector(uint32_t adr);
uint32_t QSPI_WriteEnable(void);
uint32_t QSPI_SetQE(void);

uint32_t QSPI_FLASH_ReadStatusReg(uint8_t reg);
uint32_t QSPI_FLASH_WriteStatusReg(uint8_t reg,uint8_t regvalue);
void QSPI_Set_WP_High(void);
void QSPI_Set_WP_TO_QSPI_IO(void);
//uint32_t SystemCoreClock;
/** private data **/
QSPI_HandleTypeDef QSPIHandle;

/** public functions **/

///*************************************************************************
// * Function Name: FlashInit
// * Parameters: Flash Base Address
// *
// * Return: RESULT_OK
// *         RESULT_ERROR
// *
// * Description: Init QSPI flash driver.
// *************************************************************************/

//uint32_t FlashInit(void *base_of_flash, uint32_t image_size,
//                   uint32_t link_address, uint32_t flags)
//{
//  /* WORKAROUND for BL: Disable interrupts to avoid SPI/DMA interrupts enabled by BL */
//  __disable_irq();

//  /* Init value with the default clock after reset. Needed by HAL drivers */
////  SystemCoreClock = 400000000;

//  if(QSPI_Init())
//    return RESULT_OK;
//  else
//    return RESULT_ERROR;
//}

/*************************************************************************
 * Function Name: FlashWrite
 * Parameters: block base address, offset in block, data size, ram buffer
 *             pointer
 * Return: 0
 *
 * Description. Writes data to QSPI flash
 *************************************************************************/
uint32_t FlashWrite(void *block_start,
                    uint32_t offset_into_block,
                    uint32_t count,
                    char const *buffer)
{
uint32_t size = 0;
/* Set destination address */
uint32_t dest = (uint32_t)block_start + offset_into_block;
/* Set source address */
uint8_t * src = (uint8_t*)buffer;

  while(size < count)
  {
    /* Write one page */
    if(!QSPI_WritePage(dest, QSPI_PAGE_SIZE, src))
    {
      return RESULT_ERROR;
    }

    size += (QSPI_PAGE_SIZE);
    dest += (QSPI_PAGE_SIZE);
    src += (QSPI_PAGE_SIZE);
  }
  return RESULT_OK;
}

/*************************************************************************
 * Function Name: FlashErase
 * Parameters:  Block Address, Block Size
 *
 * Return: 0
 *
 * Description: Erase block
 *************************************************************************/
uint32_t FlashErase(void *block_start,
                    uint32_t block_size)
{
  if(QSPI_EraseSector((uint32_t)block_start))
    return RESULT_OK;
  else
    return RESULT_ERROR;
}

/**
  * @brief Initialize the QSPI MSP.
  * @param hqspi: QSPI handle
  * @retval None
  */
void HAL_QSPI_MspInit(QSPI_HandleTypeDef *hqspi)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* Enable DMA clock */
  __HAL_RCC_MDMA_CLK_ENABLE();
	/* 使能 QSPI 及 GPIO 时钟 */
	QSPI_FLASH_CLK_ENABLE();
	QSPI_FLASH_CLK_GPIO_ENABLE();
	QSPI_FLASH_BK1_IO0_CLK_ENABLE();
	QSPI_FLASH_BK1_IO1_CLK_ENABLE();
	QSPI_FLASH_BK1_IO2_CLK_ENABLE();
	QSPI_FLASH_BK1_IO3_CLK_ENABLE();
	QSPI_FLASH_CS_GPIO_CLK_ENABLE();

	  /* 使能 QSPI BANK2时钟 */
	QSPI_FLASH_BK2_IO0_CLK_ENABLE();
	QSPI_FLASH_BK2_IO1_CLK_ENABLE();
	QSPI_FLASH_BK2_IO2_CLK_ENABLE();
	QSPI_FLASH_BK2_IO3_CLK_ENABLE();
	
	//设置引脚
	/*!< 配置 QSPI_FLASH 引脚: CLK */
	GPIO_InitStruct.Pin = QSPI_FLASH_CLK_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = QSPI_FLASH_CLK_GPIO_AF;

	HAL_GPIO_Init(QSPI_FLASH_CLK_GPIO_PORT, &GPIO_InitStruct);

	/*!< 配置 QSPI_FLASH 引脚: IO0 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK1_IO0_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_BK1_IO0_AF;
	HAL_GPIO_Init(QSPI_FLASH_BK1_IO0_PORT, &GPIO_InitStruct);

	/*!< 配置 QSPI_FLASH 引脚: IO1 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK1_IO1_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_BK1_IO1_AF;
	HAL_GPIO_Init(QSPI_FLASH_BK1_IO1_PORT, &GPIO_InitStruct);

	/*!< 配置 QSPI_FLASH 引脚: IO2 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK1_IO2_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_BK1_IO2_AF;
	HAL_GPIO_Init(QSPI_FLASH_BK1_IO2_PORT, &GPIO_InitStruct);

	/*!< 配置 QSPI_FLASH 引脚: IO3 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK1_IO3_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_BK1_IO3_AF;
	HAL_GPIO_Init(QSPI_FLASH_BK1_IO3_PORT, &GPIO_InitStruct);

	/*!< 配置 SPI_FLASH_SPI 引脚: NCS */
	GPIO_InitStruct.Pin = QSPI_FLASH_CS_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_CS_GPIO_AF;
	HAL_GPIO_Init(QSPI_FLASH_CS_GPIO_PORT, &GPIO_InitStruct);

//	/*!< 配置 QSPI_FLASH_BK2 引脚: IO0 */
//	GPIO_InitStruct.Pin = QSPI_FLASH_BK2_IO0_PIN;
//	GPIO_InitStruct.Alternate = QSPI_FLASH_BK2_IO0_AF;
//	HAL_GPIO_Init(QSPI_FLASH_BK2_IO0_PORT, &GPIO_InitStruct);

//	/*!< 配置 QSPI_FLASH_BK2 引脚: IO1 */
//	GPIO_InitStruct.Pin = QSPI_FLASH_BK2_IO1_PIN;
//	GPIO_InitStruct.Alternate = QSPI_FLASH_BK2_IO1_AF;
//	HAL_GPIO_Init(QSPI_FLASH_BK2_IO1_PORT, &GPIO_InitStruct);

//	/*!< 配置 QSPI_FLASH_BK2 引脚: IO2 */
//	GPIO_InitStruct.Pin = QSPI_FLASH_BK2_IO2_PIN;
//	GPIO_InitStruct.Alternate = QSPI_FLASH_BK2_IO2_AF;
//	HAL_GPIO_Init(QSPI_FLASH_BK2_IO2_PORT, &GPIO_InitStruct);

//	/*!< 配置 QSPI_FLASH_BK2 引脚: IO3 */
//	GPIO_InitStruct.Pin = QSPI_FLASH_BK2_IO3_PIN;
//	GPIO_InitStruct.Alternate = QSPI_FLASH_BK2_IO3_AF;
//	HAL_GPIO_Init(QSPI_FLASH_BK2_IO3_PORT, &GPIO_InitStruct);
}
uint32_t QSPI_Init(void)
{
QSPI_CommandTypeDef sCommand;

  /* Zero Init structs */
  memset(&QSPIHandle,0,sizeof(QSPIHandle));
  memset(&sCommand,0,sizeof(sCommand));

  __QSPI_CLK_ENABLE();

    /* Initialize QuadSPI ------------------------------------------------------ */
  QSPIHandle.Instance = QUADSPI;
  HAL_QSPI_DeInit(&QSPIHandle) ;

  /* Using default clock source HSI = 64MHz */
  /* ClockPrescaler set to 0, so QSPI clock = 64MHz */
  QSPIHandle.Init.ClockPrescaler     = 1;
  QSPIHandle.Init.FifoThreshold      = 1;
  QSPIHandle.Init.SampleShifting     = QSPI_SAMPLE_SHIFTING_NONE;
  QSPIHandle.Init.FlashSize          = QSPI_FLASH_SIZE;
  QSPIHandle.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_3_CYCLE;
  QSPIHandle.Init.ClockMode          = QSPI_CLOCK_MODE_3;
  QSPIHandle.Init.DualFlash          = QSPI_DUALFLASH_DISABLE;
  QSPIHandle.Init.FlashID            = QSPI_FLASH_ID_1;

  if (HAL_QSPI_Init(&QSPIHandle) != HAL_OK)
  {
    return 0;
  }
	QSPI_Set_WP_High();
//	/*写状态寄存器*/
	QSPI_FLASH_WriteStatusReg(1,0X00);
	QSPI_FLASH_WriteStatusReg(2,0X02);
	QSPI_FLASH_WriteStatusReg(3,0X62);
	QSPI_Set_WP_TO_QSPI_IO();	
  /* Reset Memory */

  /* Reset memory config, Cmd in 1 line */
  /* Send RESET ENABLE command (0x66) to be able to reset the memory registers */
  while(QSPIHandle.Instance->SR & QSPI_FLAG_BUSY);  /* Wait for busy flag to be cleared */
  QSPIHandle.Instance->CCR = 0x2166;
  QSPIHandle.Instance->AR = 0;
  QSPIHandle.Instance->ABR = 0;
  QSPIHandle.Instance->DLR = 0;
  __DSB();

  /* Send RESET command (0x99) to reset the memory registers */
  while(QSPIHandle.Instance->SR & QSPI_FLAG_BUSY);  /* Wait for busy flag to be cleared */
  QSPIHandle.Instance->CCR = 0x2199;
  QSPIHandle.Instance->AR = 0;
  QSPIHandle.Instance->ABR = 0;
  QSPIHandle.Instance->DLR = 0;
  __DSB();

  /* Reset memory config, Cmd in 2 lines*/
  /* Send RESET ENABLE command (0x66) to be able to reset the memory registers */
  while(QSPIHandle.Instance->SR & QSPI_FLAG_BUSY);  /* Wait for busy flag to be cleared */
  QSPIHandle.Instance->CCR = 0x2266;
  QSPIHandle.Instance->AR = 0;
  QSPIHandle.Instance->ABR = 0;
  QSPIHandle.Instance->DLR = 0;
  __DSB();

   /* Send RESET command (0x99) to reset the memory registers */
  while(QSPIHandle.Instance->SR & QSPI_FLAG_BUSY);  /* Wait for busy flag to be cleared */
  QSPIHandle.Instance->CCR = 0x2299;
  QSPIHandle.Instance->AR = 0;
  QSPIHandle.Instance->ABR = 0;
  QSPIHandle.Instance->DLR = 0;
  __DSB();

    /* Reset memory config, Cmd in 4 lines*/
  /* Send RESET ENABLE command (0x66) to be able to reset the memory registers */
  while(QSPIHandle.Instance->SR & QSPI_FLAG_BUSY);  /* Wait for busy flag to be cleared */
  QSPIHandle.Instance->CCR = 0x2366;
  QSPIHandle.Instance->AR = 0;
  QSPIHandle.Instance->ABR = 0;
  QSPIHandle.Instance->DLR = 0;
  __DSB();

   /* Send RESET command (0x99) to reset the memory registers */
  while(QSPIHandle.Instance->SR & QSPI_FLAG_BUSY);  /* Wait for busy flag to be cleared */
  QSPIHandle.Instance->CCR = 0x2399;
  QSPIHandle.Instance->AR = 0;
  QSPIHandle.Instance->ABR = 0;
  QSPIHandle.Instance->DLR = 0;
  __DSB();

  /* Enable 32-Bit address mode */
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.AddressSize       = QSPI_ADDRESS_32_BITS;
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.AlternateBytesSize= QSPI_ALTERNATE_BYTES_8_BITS;
  sCommand.DataMode          = QSPI_DATA_NONE;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
  sCommand.DummyCycles       = 0;
  sCommand.Instruction       = ENTER_4BYTE_ADDR_MODE_CMD;
  QSPIHandle.State           = HAL_QSPI_STATE_READY;
  HAL_QSPI_Command(&QSPIHandle, &sCommand, 0x10);
  QSPI_SetQE();
	
  return 1;
}

uint32_t QSPI_WritePage(unsigned long adr, unsigned long sz, unsigned char *buf)
{
QSPI_CommandTypeDef       sCommand;
QSPI_AutoPollingTypeDef   sConfig;

  if (sz == 0)
    return 0;

  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.AddressSize       = QSPI_ADDRESS_WIDTH;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Enable write operations ------------------------------------------- */
  if (!QSPI_WriteEnable())
    return 0;

  /* 4- Writing Sequence ---------------------------------------------- */
  QSPIHandle.Init.FifoThreshold = 4;
  if (HAL_QSPI_Init(&QSPIHandle) != HAL_OK)
  {
    return 0;
  }

  sCommand.Instruction = QUAD_IN_FAST_PROG_CMD;
  sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
  sCommand.AddressSize = QSPI_ADDRESS_WIDTH;
  sCommand.Address     = adr;
  sCommand.DataMode    = QSPI_DATA_4_LINES;
  sCommand.NbData      = sz;
  sCommand.DummyCycles = 0;

  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return 0;
  }

  if (HAL_QSPI_Transmit(&QSPIHandle, buf, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return 0;
  }

  /* Reconfigure QUADSPI to automatic polling mode to wait for end of program */
  sConfig.Match           = 0;
  sConfig.Mask            = 0x0101;
  sConfig.StatusBytesSize = 2;

  sCommand.Instruction    = READ_FLAG_STATUS_REG_CMD;
  sCommand.AddressMode    = QSPI_ADDRESS_NONE;
  sCommand.DataMode       = QSPI_DATA_1_LINE;

  if (HAL_QSPI_AutoPolling(&QSPIHandle, &sCommand, &sConfig, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return 0;
  }

  return 1;
}

uint32_t QSPI_ReadPage(unsigned long adr, unsigned long sz, unsigned char *buf)
{
QSPI_CommandTypeDef       sCommand;
QSPI_AutoPollingTypeDef   sConfig;

  if (sz == 0)
    return 0;

  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.AddressSize       = QSPI_ADDRESS_WIDTH;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Enable write operations ------------------------------------------- */
  if (!QSPI_WriteEnable())
    return 0;

  /* 4- Writing Sequence ---------------------------------------------- */
  QSPIHandle.Init.FifoThreshold = 4;
  if (HAL_QSPI_Init(&QSPIHandle) != HAL_OK)
  {
    return 0;
  }

  sCommand.Instruction = QUAD_IN_FAST_READ_CMD;
  sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
  sCommand.AddressSize = QSPI_ADDRESS_WIDTH;
  sCommand.Address     = adr;
  sCommand.DataMode    = QSPI_DATA_4_LINES;
  sCommand.NbData      = sz;
  sCommand.DummyCycles = 6;

  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return 0;
  }

  if (HAL_QSPI_Receive(&QSPIHandle, buf, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return 0;
  }

  /* Reconfigure QUADSPI to automatic polling mode to wait for end of program */
  sConfig.Match           = 0;
  sConfig.Mask            = 0x0101;
  sConfig.StatusBytesSize = 2;

  sCommand.Instruction    = READ_FLAG_STATUS_REG_CMD;
  sCommand.AddressMode    = QSPI_ADDRESS_NONE;
  sCommand.DataMode       = QSPI_DATA_1_LINE;

  if (HAL_QSPI_AutoPolling(&QSPIHandle, &sCommand, &sConfig, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return 0;
  }

  return 1;
}

uint32_t QSPI_EraseSector(uint32_t adr)
{
QSPI_CommandTypeDef       sCommand;
QSPI_AutoPollingTypeDef   sConfig;

  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.AddressSize       = QSPI_ADDRESS_WIDTH;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Enable write operations ------------------------------------------- */
  if (!QSPI_WriteEnable())
  {
    return 0;
  }

  /* Erasing Sequence -------------------------------------------------- */
  sCommand.Instruction = SECTOR_ERASE_CMD;
  sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
  sCommand.Address     = adr;
  sCommand.DataMode    = QSPI_DATA_NONE;
  sCommand.DummyCycles = 0;

  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, 0x100) != HAL_OK)
  {
    return 0;
  }

  /* Reconfigure Quadspi to automatic polling mode to wait for end of erase */
  sConfig.Match           = 0;
  sConfig.Mask            = 0x0101;
  sConfig.StatusBytesSize = 2;

  sCommand.Instruction    = READ_FLAG_STATUS_REG_CMD;
  sCommand.AddressMode    = QSPI_ADDRESS_NONE;
  sCommand.DataMode       = QSPI_DATA_1_LINE;

  if (HAL_QSPI_AutoPolling(&QSPIHandle, &sCommand, &sConfig, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return 0;
  }

  return 1;
}


uint32_t QSPI_SetQE(void)
{
	QSPI_CommandTypeDef       sCommand;
	QSPI_AutoPollingTypeDef   sConfig;
	uint8_t Reg2value =W25Q256JVEM_QE&0xff;
	
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.AddressSize       = QSPI_ADDRESS_WIDTH;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Enable write operations ------------------------------------------- */
  if (!QSPI_WriteEnable())
  {
    return 0;
  }
  /* Enable QE operations ------------------------------------------ */
  sCommand.Instruction       = WRITE_STATUS_REG2_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;
  sCommand.DataMode          = QSPI_DATA_1_LINE;
  sCommand.DummyCycles 			 = 0;
  sCommand.NbData      			 = 1;

  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, 0x100) != HAL_OK)
  {
    return 0;
  }
  if (HAL_QSPI_Transmit(&QSPIHandle, &Reg2value, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return 0;
  }
//  /* Reconfigure Quadspi to automatic polling mode to wait for end of erase */
//  sConfig.Match           = W25Q256JVEM_QE;
//  sConfig.Mask            = W25Q256JVEM_QE;
//  sConfig.StatusBytesSize = 2;

//  sCommand.Instruction    = READ_STATUS_REG2_CMD;
//  sCommand.AddressMode    = QSPI_ADDRESS_NONE;
//  sCommand.DataMode       = QSPI_DATA_1_LINE;

//  if (HAL_QSPI_AutoPolling(&QSPIHandle, &sCommand, &sConfig, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
//  {
//    return 0;
//  }
//	QSPI_EnterQPI();
  return 1;
}
uint32_t QSPI_EnterQPI(void)
{
QSPI_CommandTypeDef     sCommand;
QSPI_AutoPollingTypeDef sConfig;

  /* Enable write operations ------------------------------------------ */
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction       = ENTER_QPI_MODE_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode          = QSPI_DATA_NONE;
  sCommand.DummyCycles       = 0;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return 0;
  }

//  sConfig.Match           = 0;
//  sConfig.Mask            = 0x0101;
//  sConfig.StatusBytesSize = 2;

//  sCommand.Instruction    = READ_FLAG_STATUS_REG_CMD;
//  sCommand.AddressMode    = QSPI_ADDRESS_NONE;
//  sCommand.DataMode       = QSPI_DATA_1_LINE;

//  if (HAL_QSPI_AutoPolling(&QSPIHandle, &sCommand, &sConfig, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
//  {
//    return 0;
//  }
  return 1;
}
uint32_t QSPI_WriteEnable(void)
{
QSPI_CommandTypeDef     sCommand;
QSPI_AutoPollingTypeDef sConfig;

  /* Enable write operations ------------------------------------------ */
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction       = WRITE_ENABLE_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode          = QSPI_DATA_NONE;
  sCommand.DummyCycles       = 0;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return 0;
  }
	
  sConfig.Match           = 0;
  sConfig.Mask            = 0x0101;
  sConfig.StatusBytesSize = 2;

  sCommand.Instruction    = READ_FLAG_STATUS_REG_CMD;
  sCommand.AddressMode    = QSPI_ADDRESS_NONE;
  sCommand.DataMode       = QSPI_DATA_1_LINE;

  if (HAL_QSPI_AutoPolling(&QSPIHandle, &sCommand, &sConfig, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return 0;
  }
  return 1;
}
 /**
  * @brief  读取ReadStatusReg
  * @param 	无
  * @retval ReadStatusReg
  */
uint32_t QSPI_FLASH_ReadStatusReg(uint8_t reg)
{  
	QSPI_CommandTypeDef s_command;
	uint32_t Temp = 0;
	uint8_t pData[10];
		  /* Zero Init structs */
  memset(&s_command,0,sizeof(s_command));	
	/* 读取制造/设备 ID */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	if(reg == 1)
	s_command.Instruction       = READ_STATUS_REG1_CMD;
	else if(reg == 2)
	s_command.Instruction       = READ_STATUS_REG2_CMD;
	else if(reg == 3)
	s_command.Instruction       = READ_STATUS_REG3_CMD;
	
	s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
	s_command.Address           = 0x000000;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.DummyCycles       = 0;
	s_command.NbData            = 2;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* 用户可以在这里添加一些代码来处理这个错误 */
		while(1)
		{
			
		}
	}
	if (HAL_QSPI_Receive(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* 用户可以在这里添加一些代码来处理这个错误 */
		while(1)
		{
			
		}
	}

	//单flash时读取的字节数为pData[0]，pData[1]
	//Temp = pData[0] |( pData[1]<<8 ) ;
	
	//双flash时读取的字节数为pData[0]，pData[2]
	Temp = pData[1] |( pData[0]<<8 ) ;

	return Temp;
}

 /**
  * @brief  读取ReadStatusReg
  * @param 	无
  * @retval ReadStatusReg
  */
uint32_t QSPI_FLASH_WriteStatusReg(uint8_t reg,uint8_t regvalue)
{  
	QSPI_CommandTypeDef s_command;
	
	QSPI_AutoPollingTypeDef sConfig;
		  /* Zero Init structs */
  memset(&s_command,0,sizeof(s_command));	
	
  /* Enable write operations ------------------------------------------- */
  if (!QSPI_WriteEnable())
    return 0;
	/* 读取制造/设备 ID */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	if(reg == 1)
	s_command.Instruction       = WRITE_STATUS_REG1_CMD;
	else if(reg == 2)
	s_command.Instruction       = WRITE_STATUS_REG2_CMD;
	else if(reg == 3)
	s_command.Instruction       = WRITE_STATUS_REG3_CMD;
	
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.AddressSize       = QSPI_ADDRESS_8_BITS;
	s_command.Address           = 0x000000;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.DummyCycles       = 0;
	s_command.NbData            = 1;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* 用户可以在这里添加一些代码来处理这个错误 */
		while(1)
		{
			
		}
	}
	if (HAL_QSPI_Transmit(&QSPIHandle, &regvalue, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* 用户可以在这里添加一些代码来处理这个错误 */
		while(1)
		{
			
		}
	}
  /* Reconfigure QUADSPI to automatic polling mode to wait for end of program */
  sConfig.Match           = 0;
  sConfig.Mask            = 0x0101;
  sConfig.StatusBytesSize = 2;

  s_command.Instruction    = READ_FLAG_STATUS_REG_CMD;
  s_command.AddressMode    = QSPI_ADDRESS_NONE;
  s_command.DataMode       = QSPI_DATA_1_LINE;

  if (HAL_QSPI_AutoPolling(&QSPIHandle, &s_command, &sConfig, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return 0;
  }

	return 1;
}

void QSPI_Set_WP_High(void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef  GPIO_InitStruct;
	QSPI_FLASH_BK1_IO2_CLK_ENABLE();
	QSPI_FLASH_BK2_IO2_CLK_ENABLE();
	/*选择要控制的GPIO引脚*/															   
	GPIO_InitStruct.Pin = QSPI_FLASH_BK1_IO2_PIN;	
	/*设置引脚的输出类型为推挽输出*/
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  
	/*设置引脚为上拉模式*/
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	/*设置引脚速率为高速 */   
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; 
	/*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
	HAL_GPIO_Init(QSPI_FLASH_BK1_IO2_PORT, &GPIO_InitStruct);		
	/*选择要控制的GPIO引脚*/															   
	GPIO_InitStruct.Pin = QSPI_FLASH_BK2_IO2_PIN;	
	/*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
	HAL_GPIO_Init(QSPI_FLASH_BK2_IO2_PORT, &GPIO_InitStruct);		
	HAL_GPIO_WritePin(QSPI_FLASH_BK1_IO2_PORT,QSPI_FLASH_BK1_IO2_PIN,GPIO_PIN_SET);
	HAL_GPIO_WritePin(QSPI_FLASH_BK2_IO2_PORT,QSPI_FLASH_BK2_IO2_PIN,GPIO_PIN_SET);
}
void QSPI_Set_WP_TO_QSPI_IO(void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef  GPIO_InitStruct;
	QSPI_FLASH_BK1_IO2_CLK_ENABLE();
	QSPI_FLASH_BK2_IO2_CLK_ENABLE();
	//设置引脚
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	/*!< 配置 QSPI_FLASH 引脚: IO2 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK1_IO2_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_BK1_IO2_AF;
	HAL_GPIO_Init(QSPI_FLASH_BK1_IO2_PORT, &GPIO_InitStruct);
	
		/*!< 配置 QSPI_FLASH 引脚: IO2 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK2_IO2_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_BK2_IO2_AF;
	HAL_GPIO_Init(QSPI_FLASH_BK2_IO2_PORT, &GPIO_InitStruct);
}
