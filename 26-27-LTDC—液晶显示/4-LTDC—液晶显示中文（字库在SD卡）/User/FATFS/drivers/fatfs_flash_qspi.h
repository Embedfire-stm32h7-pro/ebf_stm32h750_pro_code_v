#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "stm32h7xx.h"
#include <stdio.h>

#include "diskio.h"
#include "integer.h"
#include "ff_gen_drv.h"

/* Private typedef -----------------------------------------------------------*/
//#define  sFLASH_ID                       0xEF3015     //W25X16
//#define  sFLASH_ID                       0xEF4015	    //W25Q16
//#define  sFLASH_ID                       0XEF4017     //W25Q64
#define  sFLASH_ID                         0XEF4018     //W25Q128

/* QSPI Error codes */
#define QSPI_OK            ((uint8_t)0x00)
#define QSPI_ERROR         ((uint8_t)0x01)
#define QSPI_BUSY          ((uint8_t)0x02)
#define QSPI_NOT_SUPPORTED ((uint8_t)0x04)
#define QSPI_SUSPENDED     ((uint8_t)0x08)



/* W25Q256JV Micron memory */
/* Size of the flash */
#define QSPI_FLASH_SIZE            23     /* Address bus width to access whole memory space */
#define QSPI_PAGE_SIZE             256

/* QSPI Info */
typedef struct {
  uint32_t FlashSize;          /*!< Size of the flash */
  uint32_t EraseSectorSize;    /*!< Size of sectors for the erase operation */
  uint32_t EraseSectorsNumber; /*!< Number of sectors for the erase operation */
  uint32_t ProgPageSize;       /*!< Size of pages for the program operation */
  uint32_t ProgPagesNumber;    /*!< Number of pages for the program operation */
} QSPI_Info;

/* Private define ------------------------------------------------------------*/
/*命令定义-开头*******************************/
/** 
  * @brief  W25Q256JV Configuration  
  */  
#define W25Q256JV_FLASH_SIZE                  0x1000000 /* 128 MBits => 16MBytes */
#define W25Q256JV_SECTOR_SIZE                 0x10000   /* 256 sectors of 64KBytes */
#define W25Q256JV_SUBSECTOR_SIZE              0x1000    /* 4096 subsectors of 4kBytes */
#define W25Q256JV_PAGE_SIZE                   0x100     /* 65536 pages of 256 bytes */

#define W25Q256JV_DUMMY_CYCLES_READ           4
#define W25Q256JV_DUMMY_CYCLES_READ_QUAD      10

#define W25Q256JV_BULK_ERASE_MAX_TIME         250000
#define W25Q256JV_SECTOR_ERASE_MAX_TIME       3000
#define W25Q256JV_SUBSECTOR_ERASE_MAX_TIME    800

/** 
  * @brief  W25Q256JV Commands  
  */  
/* Reset Operations */
#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99

#define ENTER_QPI_MODE_CMD                   0x38
#define EXIT_QPI_MODE_CMD                    0xFF

/* Identification Operations */
#define READ_ID_CMD                          0x90
#define DUAL_READ_ID_CMD                     0x92
#define QUAD_READ_ID_CMD                     0x94
#define READ_JEDEC_ID_CMD                    0x9F

/* Read Operations */
#define READ_CMD                             0x03
#define FAST_READ_CMD                        0x0B
#define DUAL_OUT_FAST_READ_CMD               0x3B
#define DUAL_INOUT_FAST_READ_CMD             0xBB
#define QUAD_OUT_FAST_READ_CMD               0x6B
#define QUAD_INOUT_FAST_READ_CMD             0xEB

/* Write Operations */
#define WRITE_ENABLE_CMD                     0x06
#define WRITE_DISABLE_CMD                    0x04

/* Register Operations */
#define READ_STATUS_REG1_CMD                  0x05
#define READ_STATUS_REG2_CMD                  0x35
#define READ_STATUS_REG3_CMD                  0x15

#define WRITE_STATUS_REG1_CMD                 0x01
#define WRITE_STATUS_REG2_CMD                 0x31
#define WRITE_STATUS_REG3_CMD                 0x11


/* Program Operations */
#define PAGE_PROG_CMD                        0x02
#define QUAD_INPUT_PAGE_PROG_CMD             0x32


/* Erase Operations */
#define SECTOR_ERASE_CMD                     0x20
#define CHIP_ERASE_CMD                       0xC7

#define PROG_ERASE_RESUME_CMD                0x7A
#define PROG_ERASE_SUSPEND_CMD               0x75


/* Flag Status Register */
#define W25Q256JV_FSR_BUSY                    ((uint8_t)0x01)    /*!< busy */
#define W25Q256JV_FSR_WREN                    ((uint8_t)0x02)    /*!< write enable */
#define W25Q256JV_FSR_QE                      ((uint8_t)0x02)    /*!< quad enable */
/*命令定义-结尾*******************************/


/*QSPI接口定义-开头****************************/
#define QSPI_FLASH                         QUADSPI
#define QSPI_FLASH_CLK_ENABLE()            __HAL_RCC_QSPI_CLK_ENABLE()

#define QSPI_FLASH_CLK_PIN                 GPIO_PIN_2                  
#define QSPI_FLASH_CLK_GPIO_PORT           GPIOB                       
#define QSPI_FLASH_CLK_GPIO_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define QSPI_FLASH_CLK_GPIO_AF             GPIO_AF9_QUADSPI

#define QSPI_FLASH_BK1_IO0_PIN             GPIO_PIN_8                
#define QSPI_FLASH_BK1_IO0_PORT            GPIOF                   
#define QSPI_FLASH_BK1_IO0_CLK_ENABLE()    __HAL_RCC_GPIOF_CLK_ENABLE()
#define QSPI_FLASH_BK1_IO0_AF              GPIO_AF10_QUADSPI

#define QSPI_FLASH_BK1_IO1_PIN             GPIO_PIN_9                
#define QSPI_FLASH_BK1_IO1_PORT            GPIOF                      
#define QSPI_FLASH_BK1_IO1_CLK_ENABLE()    __HAL_RCC_GPIOF_CLK_ENABLE()
#define QSPI_FLASH_BK1_IO1_AF              GPIO_AF10_QUADSPI

#define QSPI_FLASH_BK1_IO2_PIN             GPIO_PIN_7                
#define QSPI_FLASH_BK1_IO2_PORT            GPIOF                      
#define QSPI_FLASH_BK1_IO2_CLK_ENABLE()    __HAL_RCC_GPIOF_CLK_ENABLE()
#define QSPI_FLASH_BK1_IO2_AF              GPIO_AF9_QUADSPI

#define QSPI_FLASH_BK1_IO3_PIN             GPIO_PIN_6                
#define QSPI_FLASH_BK1_IO3_PORT            GPIOF                      
#define QSPI_FLASH_BK1_IO3_CLK_ENABLE()    __HAL_RCC_GPIOF_CLK_ENABLE()
#define QSPI_FLASH_BK1_IO3_AF              GPIO_AF9_QUADSPI

#define QSPI_FLASH_CS_PIN                 GPIO_PIN_6               
#define QSPI_FLASH_CS_GPIO_PORT           GPIOB                   
#define QSPI_FLASH_CS_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()
#define QSPI_FLASH_CS_GPIO_AF             GPIO_AF10_QUADSPI



void QSPI_FLASH_Init(void);
uint8_t BSP_QSPI_Init(void);
uint8_t BSP_QSPI_Erase_Block(uint32_t BlockAddress);
uint8_t BSP_QSPI_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
uint8_t BSP_QSPI_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);


static uint8_t QSPI_ResetMemory          (void);
static uint8_t QSPI_WriteEnable          (void);
static uint8_t QSPI_AutoPollingMemReady  (uint32_t Timeout);

uint32_t QSPI_FLASH_ReadDeviceID(void);
uint32_t QSPI_FLASH_ReadID(void);
uint8_t BSP_QSPI_Erase_Chip(void);
uint8_t BSP_QSPI_GetStatus(void);

#undef  USB_FLASH_SPI3
#define USB_FLASH_SPI3   //使用SPI3控制读写外部SPI_FLASH芯片
#undef  USB_FLASH_SPI1


#define USB_SPI_TIMEOUT

//#define  sFLASH_ID                        0xEF3015     //W25X16
//#define  sFLASH_ID                        0xEF4015	   //W25Q16
//#define  sFLASH_ID                        0XEF4017     //W25Q64
#define  sFLASH_ID                          0XEF4018     //W25Q128

/* Private typedef -----------------------------------------------------------*/
//#define SPI_FLASH_PageSize                4096
#define SPI_FLASH_PageSize                  256
#define SPI_FLASH_PerWritePageSize          256

/* Private define ------------------------------------------------------------*/
#define W25X_WriteEnable		                0x06 
#define W25X_WriteDisable		                0x04 
#define W25X_ReadStatusReg		              0x05 
#define W25X_WriteStatusReg		              0x01 
#define W25X_ReadData			                  0x03 
#define W25X_FastReadData		                0x0B 
#define W25X_FastReadDual		                0x3B 
#define W25X_PageProgram		                0x02 
#define W25X_BlockErase			                0xD8 
#define W25X_SectorErase		                0x20 
#define W25X_ChipErase			                0xC7 
#define W25X_PowerDown			                0xB9 
#define W25X_ReleasePowerDown	              0xAB 
#define W25X_DeviceID			                  0xAB 
#define W25X_ManufactDeviceID   	          0x90 
#define W25X_JedecDeviceID		              0x9F 

#define WIP_Flag                            0x01  /* Write In Progress (WIP) flag */

#define Dummy_Byte                          0xFF



#ifdef USB_FLASH_SPI3
/*SPI接口*/
#define FLASH_SPI                           SPI5
#define FLASH_SPI_CLK                       RCC_APB2Periph_SPI5
#define FLASH_SPI_CLK_INIT                  RCC_APB2PeriphClockCmd

#define FLASH_SPI_SCK_PIN                   GPIO_Pin_7                  
#define FLASH_SPI_SCK_GPIO_PORT             GPIOF                       
#define FLASH_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOF
#define FLASH_SPI_SCK_PINSOURCE             GPIO_PinSource7
#define FLASH_SPI_SCK_AF                    GPIO_AF_SPI5

#define FLASH_SPI_MISO_PIN                  GPIO_Pin_8                
#define FLASH_SPI_MISO_GPIO_PORT            GPIOF                   
#define FLASH_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOF
#define FLASH_SPI_MISO_PINSOURCE            GPIO_PinSource8
#define FLASH_SPI_MISO_AF                   GPIO_AF_SPI5

#define FLASH_SPI_MOSI_PIN                  GPIO_Pin_9                
#define FLASH_SPI_MOSI_GPIO_PORT            GPIOF                    
#define FLASH_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOF
#define FLASH_SPI_MOSI_PINSOURCE            GPIO_PinSource9
#define FLASH_SPI_MOSI_AF                   GPIO_AF_SPI5

#endif  //end #ifdef USB_FLASH_SPI3 

#ifdef  USB_FLASH_SPI1
/*SPI接口*/
#define FLASH_SPI                           SPI5
#define FLASH_SPI_CLK                       RCC_APB2Periph_SPI5
#define FLASH_SPI_CLK_INIT                  RCC_APB2PeriphClockCmd

#define FLASH_SPI_SCK_PIN                   GPIO_Pin_7                  
#define FLASH_SPI_SCK_GPIO_PORT             GPIOF                       
#define FLASH_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOF
#define FLASH_SPI_SCK_PINSOURCE             GPIO_PinSource7
#define FLASH_SPI_SCK_AF                    GPIO_AF_SPI5

#define FLASH_SPI_MISO_PIN                  GPIO_Pin_8                
#define FLASH_SPI_MISO_GPIO_PORT            GPIOF                   
#define FLASH_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOF
#define FLASH_SPI_MISO_PINSOURCE            GPIO_PinSource8
#define FLASH_SPI_MISO_AF                   GPIO_AF_SPI5

#define FLASH_SPI_MOSI_PIN                  GPIO_Pin_9                
#define FLASH_SPI_MOSI_GPIO_PORT            GPIOF                    
#define FLASH_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOF
#define FLASH_SPI_MOSI_PINSOURCE            GPIO_PinSource9
#define FLASH_SPI_MOSI_AF                   GPIO_AF_SPI5


#endif  //end #ifdef USB_FLASH_SPI1 

#define FLASH_CS_PIN                        GPIO_Pin_6                 
#define FLASH_CS_GPIO_PORT                  GPIOF                      
#define FLASH_CS_GPIO_CLK                   RCC_AHB1Periph_GPIOF


#define SPI_FLASH_CS_LOW()      {FLASH_CS_GPIO_PORT->BSRRH=FLASH_CS_PIN;}				//输出低电平 GPIO_ResetBits(FLASH_CS_GPIO_PORT, FLASH_CS_PIN)
#define SPI_FLASH_CS_HIGH()     {FLASH_CS_GPIO_PORT->BSRRL=FLASH_CS_PIN;}			  //设置为高电平 GPIO_SetBits(FLASH_CS_GPIO_PORT, FLASH_CS_PIN)

/*等待超时时间*/
#ifdef  USB_SPI_TIMEOUT
#define SPIT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define SPIT_LONG_TIMEOUT         ((uint32_t)(10 * SPIT_FLAG_TIMEOUT))
#endif

/*信息输出*/
#define FLASH_DEBUG_ON         1
#define FLASH_DEBUG_FUNC_ON    0

#define FLASH_INFO(fmt,arg...)           printf("<<-FLASH-INFO->> "fmt"\n",##arg)
#define FLASH_ERROR(fmt,arg...)          printf("<<-FLASH-ERROR->> "fmt"\n",##arg)
#define FLASH_DEBUG(fmt,arg...)          do{\
                                          if(FLASH_DEBUG_ON)\
                                          printf("<<-FLASH-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

#define FLASH_DEBUG_FUNC()               do{\
                                         if(FLASH_DEBUG_FUNC_ON)\
                                         printf("<<-FLASH-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)



DSTATUS TM_FATFS_FLASH_SPI_disk_initialize(BYTE lun);
DSTATUS TM_FATFS_FLASH_SPI_disk_status(BYTE lun) ;
DRESULT TM_FATFS_FLASH_SPI_disk_ioctl(BYTE lun,BYTE cmd, void *buff) ;
DRESULT TM_FATFS_FLASH_SPI_disk_read(BYTE lun,BYTE *buff, DWORD sector, UINT count) ;
DRESULT TM_FATFS_FLASH_SPI_disk_write(BYTE lun,const BYTE *buff, DWORD sector, UINT count) ;

void SPI_FLASH_SectorErase(uint32_t SectorAddr);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);
uint32_t SPI_FLASH_ReadID(void);
uint32_t SPI_FLASH_ReadDeviceID(void);
void SPI_FLASH_StartReadSequence(uint32_t ReadAddr);
void SPI_Flash_PowerDown(void);
void SPI_Flash_WAKEUP(void);


uint8_t SPI_FLASH_ReadByte(void);
uint8_t SPI_FLASH_SendByte(uint8_t byte);
uint16_t SPI_FLASH_SendHalfWord(uint16_t HalfWord);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);

#endif /* __SPI_FLASH_H */

