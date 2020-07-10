#ifndef __STM32H743_FatFs_SD_H
#define __STM32H743_FatFs_SD_H

#include "stm32h7xx.h"
#include <stdio.h>

#include "diskio.h"
#include "integer.h"
#include "ff_gen_drv.h"

DSTATUS SD_initialize(BYTE lu);
DSTATUS SD_status(BYTE lu);
DRESULT SD_read(BYTE lun,//物理扇区，多个设备时用到(0...)
                                     BYTE *buff,//数据缓存区 
                                     DWORD sector, //扇区首地址
                                     UINT count);//扇区个数(1..128)

  
DRESULT SD_write(BYTE lun,//物理扇区，多个设备时用到(0...)
                                      const BYTE *buff,//数据缓存区 
                                      DWORD sector, //扇区首地址
                                      UINT count);//扇区个数(1..128)

DRESULT SD_ioctl(BYTE lun,BYTE cmd, void *buff);


#endif /* __STM32H743_FatFs_SD_H */
