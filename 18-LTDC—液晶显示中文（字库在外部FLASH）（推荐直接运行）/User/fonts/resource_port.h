#ifndef __GUI_RESOURCE_PORT_H
#define	__GUI_RESOURCE_PORT_H

#include "stm32h7xx.h"

#ifdef __cplusplus
extern "C" {
#endif  

typedef int         BOOL;

/* 资源所在的基地址 */
#define GUI_RES_BASE             (16*1024*1024)

/* 存储在FLASH中的资源目录大小 */
#define GUI_CATALOG_SIZE         (8*1024)
  
/* 目录信息类型 */
typedef struct 
{
	char 	name[40];  /* 资源的名字 */
	uint32_t  	size;      /* 资源的大小 */ 
	uint32_t 	offset;    /* 资源相对于基地址的偏移 */
}CatalogTypeDef;


/* 用于标记是否有资源文件无法找到 */
extern BOOL res_not_found_flag;

BOOL RES_DevRead(uint8_t *buf,uint32_t addr,uint32_t size);
int RES_DevEraseSector(uint32_t addr);
void RES_DevTest(void);

int RES_GetOffset(const char *res_name);
int RES_GetInfo_AbsAddr(const char *res_name, CatalogTypeDef *dir);

#ifdef	__cplusplus
}
#endif
#endif /* __GUI_RESOURCE_PORT_H */
