#ifndef __ADC_H
#define	__ADC_H

#include "stm32h7xx.h"


//引脚定义
#define RHEOSTAT_ADC_PIN                            GPIO_PIN_3                 
#define RHEOSTAT_ADC_GPIO_PORT                      GPIOC                     
#define RHEOSTAT_ADC_GPIO_CLK_ENABLE()              __GPIOC_CLK_ENABLE()

// ADC 序号宏定义
#define RHEOSTAT_ADC                                ADC3
#define RHEOSTAT_ADC_CLK_ENABLE()                   __ADC3_CLK_ENABLE()
#define RHEOSTAT_ADC_CHANNEL                        ADC_CHANNEL_1
//DMA时钟使能
#define RHEOSTAT_ADC_DMA_CLK_ENABLE()               __HAL_RCC_DMA1_CLK_ENABLE();
#define RHEOSTAT_ADC_DMA_Base                       DMA1_Stream1
#define RHEOSTAT_ADC_DMA_Request                    DMA_REQUEST_ADC3
//DMA中断服务函数
//#define RHEOSTAT_ADC_DMA_IRQHandler                 DMA1_Stream1_IRQHandler

#define Rheostat_ADC1_DMA_IRQ                       DMA1_Stream1_IRQn

void ADC_Init(void);

uint16_t ADC_GetValue(void);
void Rheostat_DMA_NVIC_Config(void);
#endif /* __ADC_H */
