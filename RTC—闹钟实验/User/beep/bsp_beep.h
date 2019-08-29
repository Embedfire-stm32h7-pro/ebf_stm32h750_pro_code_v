#ifndef __BSP_BEEP_H
#define	__BSP_BEEP_H


#include "stm32h7xx.h"


/* 定义蜂鸣器连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的蜂鸣器引脚 */
#define BEEP_GPIO_PORT    	    GPIOI			              /* GPIO端口 */
#define BEEP_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOI_CLK_ENABLE()  /* GPIO端口时钟 */
#define BEEP_GPIO_PIN		  	GPIO_PIN_8			          /* 连接到蜂鸣器的GPIO */

/* 高电平时，蜂鸣器响 */
#define ON  GPIO_PIN_RESET
#define OFF GPIO_PIN_SET

/* 带参宏，可以像内联函数一样使用 */
#define BEEP(a)	HAL_GPIO_WritePin(BEEP_GPIO_PORT,BEEP_GPIO_PIN,a)
					
/* 直接操作寄存器的方法控制IO */
#define	digitalHi(p,i)				{p->BSRR=i;}			  //设置为高电平		
#define digitalLo(p,i)				{p->BSRR=i<<16;}				//输出低电平
#define digitalToggle(p,i)		{p->ODR ^=i;}			//输出反转状态


/* 定义控制IO的宏 */
#define BEEP_TOGGLE		digitalToggle(BEEP_GPIO_PORT,BEEP_GPIO_PIN)
#define BEEP_ON	        digitalHi(BEEP_GPIO_PORT,BEEP_GPIO_PIN)
#define BEEP_OFF		digitalLo(BEEP_GPIO_PORT,BEEP_GPIO_PIN)

void BEEP_GPIO_Config(void);
					
#endif /* __BEEP_H */
