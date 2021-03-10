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
#define RESULT_OK 		0
#define RESULT_ERROR  0xff

/** public functions **/

/*************************************************************************
 * Function Name: FlashInit
 * Parameters: Flash Base Address
 *
 * Return: RESULT_OK
 *         RESULT_ERROR
 *
 * Description: Init QSPI flash driver.
 *************************************************************************/

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
