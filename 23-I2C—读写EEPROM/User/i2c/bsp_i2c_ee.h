#ifndef __I2C_EE_H
#define	__I2C_EE_H

#include "stm32h7xx.h"

/* AT24C01/02每页有8个字节 */
//#define EEPROM_PAGESIZE           8
#define EEPROM_PAGESIZE 	   8
/* AT24C04/08A/16A每页有16个字节 */
//#define EEPROM_PAGESIZE           16			

#define EEPROM_I2C_FORCE_RESET()      __HAL_RCC_I2C4_FORCE_RESET()
#define EEPROM_I2C_RELEASE_RESET()    __HAL_RCC_I2C4_RELEASE_RESET()
/* I2C interrupt requests */
#define EEPROM_I2C_EV_IRQn            I2C4_EV_IRQn
#define EEPROM_I2C_ER_IRQn            I2C4_ER_IRQn
#define EEPROM_I2C_EV_IRQHandler	    I2C4_EV_IRQHandler
#define EEPROM_I2C_ER_IRQHandler	    I2C4_ER_IRQHandler

/* 这个地址只要与STM32外挂的I2C器件地址不一样即可 */
#define I2C_OWN_ADDRESS7      0X0A   

/*I2C接口*/
#define EEPROM_I2C                          I2C4
#define EEPROM_I2C_CLK_ENABLE()             __HAL_RCC_I2C4_CLK_ENABLE()
//#define RCC_PERIPHCLK_I2Cx               	  RCC_PERIPHCLK_I2C4

#define EEPROM_I2C_SCL_PIN                  GPIO_PIN_8                 
#define EEPROM_I2C_SCL_GPIO_PORT            GPIOB                       
#define EEPROM_I2C_SCL_GPIO_CLK_ENABLE()    __GPIOB_CLK_ENABLE()
#define EEPROM_I2C_SCL_AF                   GPIO_AF6_I2C4

#define EEPROM_I2C_SDA_PIN                  GPIO_PIN_9                 
#define EEPROM_I2C_SDA_GPIO_PORT            GPIOB                       
#define EEPROM_I2C_SDA_GPIO_CLK_ENABLE()    __GPIOB_CLK_ENABLE()
#define EEPROM_I2C_SDA_AF                   GPIO_AF6_I2C4



/*等待超时时间*/
#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

#define I2Cx_TIMEOUT_MAX                300
/* Maximum number of trials for HAL_I2C_IsDeviceReady() function */
#define EEPROM_MAX_TRIALS               300

/*信息输出*/
#define EEPROM_DEBUG_ON         0

#define EEPROM_INFO(fmt,arg...)           printf("<<-EEPROM-INFO->> "fmt"\n",##arg)
#define EEPROM_ERROR(fmt,arg...)          printf("<<-EEPROM-ERROR->> "fmt"\n",##arg)
#define EEPROM_DEBUG(fmt,arg...)          do{\
                                          if(EEPROM_DEBUG_ON)\
                                          printf("<<-EEPROM-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

/* 
 * AT24C02 2kb = 2048bit = 2048/8 B = 256 B
 * 32 pages of 8 bytes each
 *
 * Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0XA0
 * 1 0 1 0 0  0  0  1 = 0XA1 
 */

/* EEPROM Addresses defines */
#define EEPROM_Block0_ADDRESS 0xA0 /* E2 = 0 */
#define EEPROM_Block1_ADDRESS 0xA2 /* E2 = 0 */
#define EEPROM_Block2_ADDRESS 0xA4 /* E2 = 0 */
#define EEPROM_Block3_ADDRESS 0xA6 /* E2 = 0 */

#define EEPROM_ADDRESS        0xA0

void I2C_EE_Init(void);
										  
void I2C_EE_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
uint32_t I2C_EE_ByteWrite(uint8_t* pBuffer, uint8_t WriteAddr);
uint32_t I2C_EE_PageWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite);
uint32_t I2C_EE_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);

extern I2C_HandleTypeDef  I2C_Handle; 
#endif /* __I2C_EE_H */
