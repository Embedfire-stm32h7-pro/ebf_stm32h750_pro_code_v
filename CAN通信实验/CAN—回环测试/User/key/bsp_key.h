#ifndef __KEY_H
#define	__KEY_H

#include "stm32h7xx.h"
#include "main.h"
//引脚定义
/*******************************************************/
#define KEY1_PIN                  GPIO_PIN_0                 
#define KEY1_GPIO_PORT            GPIOA                      
#define KEY1_GPIO_CLK_ENABLE()    __GPIOA_CLK_ENABLE()

#define KEY2_PIN                  GPIO_PIN_13                 
#define KEY2_GPIO_PORT            GPIOC                      
#define KEY2_GPIO_CLK_ENABLE()    __GPIOC_CLK_ENABLE()
/*******************************************************/

 /** 按键按下标置宏
	* 按键按下为高电平，设置 KEY_ON=1， KEY_OFF=0
	* 若按键按下为低电平，把宏设置成KEY_ON=0 ，KEY_OFF=1 即可
	*/
#define KEY_ON	1
#define KEY_OFF	0

void Key_GPIO_Config(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);

#endif /* __LED_H */

