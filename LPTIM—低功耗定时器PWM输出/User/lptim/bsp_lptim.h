#ifndef _BSP_LPTIM_H
#define	_BSP_LPTIM_H

#include "stm32h7xx.h"

#define LSE                               1
#define LSI                               2
//选择内部时钟的来源
#define LPTIMx_CLK_Source                 LSI 

//引脚定义
/*******************************************************/
#define LPTIMx_CLK_ENABLE()               __LPTIM1_CLK_ENABLE()  
#define LPTIMx_Instance                   LPTIM1


#define LPTIMx_OUT_GPIO_CLK_ENABLE()      __GPIOD_CLK_ENABLE()
#define LPTIMx_OUT_GPIO_PORT              GPIOD 
#define LPTIMx_OUT_PIN            		    GPIO_PIN_13                 
#define LPTIMx_OUT_AF	                    GPIO_AF1_LPTIM1                 

void 	LPTIM_PWM_OUT(void);
#endif /* _BSP_LPTIM_H */

