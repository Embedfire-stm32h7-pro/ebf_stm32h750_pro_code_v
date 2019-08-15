#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "stm32h7xx.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
//#define  sFLASH_ID                       0xEF3015     //W25X16
//#define  sFLASH_ID                       0xEF4015	    //W25Q16
//#define  sFLASH_ID                       0XEF4017     //W25Q64
//#define  sFLASH_ID                         0XEF4018     //W25Q256
#define  sFLASH_ID                         0XEF4019     //W25Q256

/* QSPI Error codes */
#define QSPI_OK            ((uint8_t)0x00)
#define QSPI_ERROR         ((uint8_t)0x01)
#define QSPI_BUSY          ((uint8_t)0x02)
#define QSPI_NOT_SUPPORTED ((uint8_t)0x04)
#define QSPI_SUSPENDED     ((uint8_t)0x08)


/* W25Q256JV Micron memory */
/* Size of the flash */
#define QSPI_FLASH_SIZE            26     /* 地址总线宽度访问整个内存空间 */
#define QSPI_PAGE_SIZE             256

/* QSPI Info */
typedef struct {
  uint32_t FlashSize;          /*!< 闪存大小 */
  uint32_t EraseSectorSize;    /*!< 擦除操作的扇区大小 */
  uint32_t EraseSectorsNumber; /*!< 擦除操作的扇区数 */
  uint32_t ProgPageSize;       /*!< 编程操作的页面大小 */
  uint32_t ProgPagesNumber;    /*!< 编程操作的页面数 */
} QSPI_Info;

/* Private define ------------------------------------------------------------*/
/*命令定义-开头*******************************/
/** 
  * @brief  W25Q256JV配置 
  */  
#define W25Q256JV_FLASH_SIZE                  0x4000000 /* 256X2 MBits => 64MBytes */
#define W25Q256JV_SECTOR_SIZE                 0x10000   /* 256 sectors of 64KBytes */
#define W25Q256JV_SUBSECTOR_SIZE              0x1000    /* 4096 subsectors of 4kBytes */
#define W25Q256JV_PAGE_SIZE                   0x100     /* 65536 pages of 256 bytes */

#define W25Q256JV_DUMMY_CYCLES_READ           4
#define W25Q256JV_DUMMY_CYCLES_READ_QUAD      10

#define W25Q256JV_BULK_ERASE_MAX_TIME         250000
#define W25Q256JV_SECTOR_ERASE_MAX_TIME       3000
#define W25Q256JV_SUBSECTOR_ERASE_MAX_TIME    800

/** 
  * @brief  W25Q256JV 指令  
  */  
/* 复位操作 */
#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99

#define ENTER_QPI_MODE_CMD                   0x38
#define EXIT_QPI_MODE_CMD                    0xFF

/* 识别操作 */
#define READ_ID_CMD                          0x90
#define DUAL_READ_ID_CMD                     0x92
#define QUAD_READ_ID_CMD                     0x94
#define READ_JEDEC_ID_CMD                    0x9F

/* 读操作 */
#define READ_CMD                             0x03
//#define FAST_READ_CMD                        0x0B
//#define DUAL_OUT_FAST_READ_CMD               0x3B
//#define DUAL_INOUT_FAST_READ_CMD             0xBB
#define QUAD_OUT_FAST_READ_CMD               0x6B
#define QUAD_INOUT_FAST_READ_CMD             0xEB
#define QUAD_INOUT_4BYTE_FAST_READ_CMD       0xEC

/* 写操作 */
#define WRITE_ENABLE_CMD                     0x06
#define WRITE_DISABLE_CMD                    0x04

/* 寄存器操作 */
#define READ_STATUS_REG1_CMD                  0x05
#define READ_STATUS_REG2_CMD                  0x35
#define READ_STATUS_REG3_CMD                  0x15

#define WRITE_STATUS_REG1_CMD                 0x01
#define WRITE_STATUS_REG2_CMD                 0x31
#define WRITE_STATUS_REG3_CMD                 0x11


/* 编程操作 */
#define PAGE_PROG_CMD                        0x12
#define QUAD_INPUT_PAGE_PROG_CMD             0x32
//#define EXT_QUAD_IN_FAST_PROG_CMD            0x12

/* 擦除操作 */
#define SECTOR_ERASE_CMD                     0x21
#define CHIP_ERASE_CMD                       0xC7

//#define PROG_ERASE_RESUME_CMD                0x7A
//#define PROG_ERASE_SUSPEND_CMD               0x75

#define ENTER_4_BYTE_ADDR_MODE_CMD					 0xB7
#define EXIT_4_BYTE_ADDR_MODE_CMD					 	 0xE9

/* 状态寄存器标志 */
#define W25Q256JV_FSR_BUSY                    ((uint16_t)0x0101)    /*!< busy */
#define W25Q256JV_FSR_WREN                    ((uint16_t)0x0202)    /*!< write enable */
#define W25Q256JV_FSR_QE                      ((uint8_t)0x06)    /*!< quad enable */
/*命令定义-结尾*******************************/


/*QSPI接口定义-开头****************************/
#define QSPI_FLASH                         QUADSPI
#define QSPI_FLASH_CLK_ENABLE()            __QSPI_CLK_ENABLE()

#define QSPI_MDMA_CLK_ENABLE()         		__HAL_RCC_MDMA_CLK_ENABLE()

#define QSPI_FORCE_RESET()             		__HAL_RCC_QSPI_FORCE_RESET()
#define QSPI_RELEASE_RESET()           		__HAL_RCC_QSPI_RELEASE_RESET()

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


/* Definition for QSPI Pins */
#define QSPI_CS_PIN                    GPIO_PIN_6
#define QSPI_CS_GPIO_PORT              GPIOG
#define QSPI_CLK_PIN                   GPIO_PIN_2
#define QSPI_CLK_GPIO_PORT             GPIOB
#define QSPI_BK1_D0_PIN                GPIO_PIN_8
#define QSPI_BK1_D0_GPIO_PORT          GPIOF
#define QSPI_BK1_D1_PIN                GPIO_PIN_9
#define QSPI_BK1_D1_GPIO_PORT          GPIOF
#define QSPI_BK1_D2_PIN                GPIO_PIN_7
#define QSPI_BK1_D2_GPIO_PORT          GPIOF
#define QSPI_BK1_D3_PIN                GPIO_PIN_6
#define QSPI_BK1_D3_GPIO_PORT          GPIOF

#define QSPI_BK2_D0_PIN                GPIO_PIN_2
#define QSPI_BK2_D0_GPIO_PORT          GPIOH
#define QSPI_BK2_D1_PIN                GPIO_PIN_3
#define QSPI_BK2_D1_GPIO_PORT          GPIOH
#define QSPI_BK2_D2_PIN                GPIO_PIN_9
#define QSPI_BK2_D2_GPIO_PORT          GPIOG
#define QSPI_BK2_D3_PIN                GPIO_PIN_14
#define QSPI_BK2_D3_GPIO_PORT          GPIOG


void QSPI_FLASH_Init(void);
uint8_t BSP_QSPI_Init(void);
uint8_t QSPI_EnterFourBytesAddress(void);
uint8_t BSP_QSPI_Erase_Block(uint32_t BlockAddress);
uint8_t BSP_QSPI_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
uint8_t BSP_QSPI_FastRead(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
uint8_t BSP_QSPI_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
uint8_t BSP_QSPI_PageWrite(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
//static uint8_t QSPI_ResetMemory          (void);
static uint8_t QSPI_WriteEnable          (void);
static uint8_t QSPI_AutoPollingMemReady  (void);
void WaitQSPIReady(uint8_t flag);
uint32_t QSPI_FLASH_ReadDeviceID(void);
uint32_t QSPI_FLASH_ReadID(void);
static void Error_Handler(void);
#endif /* __SPI_FLASH_H */

