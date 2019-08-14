#ifndef __I2C_H
#define	__I2C_H

#include "stm32h7xx.h"
		
/*设定的AP3216C IIC设备地址*/
#define AP3216C_ADDR              0x3C

#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

/*I2C引脚*/
#define AP3216C_I2C_SCL_PIN                  GPIO_PIN_8                 
#define AP3216C_I2C_SCL_GPIO_PORT            GPIOB                       
#define AP3216C_I2C_SCL_GPIO_CLK_ENABLE()    __GPIOB_CLK_ENABLE()

#define AP3216C_I2C_SDA_PIN                  GPIO_PIN_9                 
#define AP3216C_I2C_SDA_GPIO_PORT            GPIOB                    
#define AP3216C_I2C_SDA_GPIO_CLK_ENABLE()    __GPIOB_CLK_ENABLE()

//软件IIC使用的宏
#define I2C_SCL_1()  HAL_GPIO_WritePin(AP3216C_I2C_SCL_GPIO_PORT, AP3216C_I2C_SCL_PIN,GPIO_PIN_SET)		/* SCL = 1 */
#define I2C_SCL_0()  HAL_GPIO_WritePin(AP3216C_I2C_SCL_GPIO_PORT, AP3216C_I2C_SCL_PIN,GPIO_PIN_RESET)		/* SCL = 0 */

#define I2C_SDA_1()  HAL_GPIO_WritePin(AP3216C_I2C_SDA_GPIO_PORT, AP3216C_I2C_SDA_PIN,GPIO_PIN_SET)		/* SDA = 1 */
#define I2C_SDA_0()  HAL_GPIO_WritePin(AP3216C_I2C_SDA_GPIO_PORT, AP3216C_I2C_SDA_PIN,GPIO_PIN_RESET)		/* SDA = 0 */

#define I2C_SDA_READ()  HAL_GPIO_ReadPin(AP3216C_I2C_SDA_GPIO_PORT, AP3216C_I2C_SDA_PIN)	/* 读SDA口线状态 */

/*信息输出*/
#define I2C_DEBUG_ON         1
#define I2C_DEBUG_FUNC_ON    0

#define I2C_INFO(fmt,arg...)           printf("<<-I2C-INFO->> "fmt"\n",##arg)
#define I2C_ERROR(fmt,arg...)          printf("<<-I2C-ERROR->> "fmt"\n",##arg)
#define I2C_DEBUG(fmt,arg...)          do{\
                                          if(I2C_DEBUG_ON)\
                                          printf("<<-I2C-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

#define I2C_DEBUG_FUNC()               do{\
                                         if(I2C_DEBUG_FUNC_ON)\
                                         printf("<<-I2C-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)

//函数接口
void I2C_Init(void);
uint32_t I2C_WriteBytes(uint8_t ClientAddr,uint8_t* pBuffer,  uint8_t NumByteToWrite);
uint32_t I2C_ReadBytes(uint8_t ClientAddr,uint8_t* pBuffer, uint16_t NumByteToRead);
uint32_t Sensor_write(uint8_t reg_add,uint8_t reg_dat);
uint32_t Sensor_Read(uint8_t reg_add,unsigned char* Read,uint8_t num); 

#endif /* __BSP_I2C */
