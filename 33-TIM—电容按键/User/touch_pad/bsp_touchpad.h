#ifndef __TOUCHPAD_H_
#define	__TOUCHPAD_H_

#include "stm32h7xx.h"
 
/******************** TPAD 引脚配置参数定义 **************************/
#define TPAD_TIMx                         TIM2
#define TPAD_TIM_CLK_ENABLE()       		  __TIM2_CLK_ENABLE()
#define TPAD_TIM_Channel_X                TIM_CHANNEL_1

//定时器的通道引脚初始化
#define TPAD_TIM_GPIO_CLK_ENABLE()         __GPIOA_CLK_ENABLE()
#define TPAD_TIM_CH_PORT                  GPIOA
#define TPAD_TIM_CH_PIN                   GPIO_PIN_5
#define TPAD_TIMx_AF                      GPIO_AF1_TIM2

/************************** TPAD 函数声明********************************/
uint8_t TPAD_Init(void);
uint8_t TPAD_Scan(uint8_t mode);
 

#endif /* __TOUCHPAD_H_ */

