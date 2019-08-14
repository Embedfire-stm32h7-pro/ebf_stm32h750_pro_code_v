#ifndef __ADC_H
#define	__ADC_H

#include "stm32h7xx.h"


//引脚定义
#define RHEOSTAT_ADC_PIN                            GPIO_PIN_4
#define RHEOSTAT_ADC_GPIO_PORT                      GPIOA                     
#define RHEOSTAT_ADC_GPIO_CLK_ENABLE()              __GPIOA_CLK_ENABLE()

// ADC_MASTER序号宏定义
#define RHEOSTAT_ADC_MASTER                         ADC1
#define RHEOSTAT_ADC_MASTER_CLK_ENABLE()            __ADC12_CLK_ENABLE()
#define RHEOSTAT_ADC_MASTER_CHANNEL                 ADC_CHANNEL_18

// ADC_SLAVE序号宏定义
#define RHEOSTAT_ADC_SLAVE                          ADC2
#define RHEOSTAT_ADC_SLAVE_CLK_ENABLE()             __ADC12_CLK_ENABLE()
#define RHEOSTAT_ADC_SLAVE_CHANNEL                  ADC_CHANNEL_18

//DMA时钟使能
#define RHEOSTAT_ADC_DMA_CLK_ENABLE()               __HAL_RCC_DMA1_CLK_ENABLE();
#define RHEOSTAT_ADC_DMA_Base                       DMA1_Stream1
#define RHEOSTAT_ADC_DMA_Request                    DMA_REQUEST_ADC1
//DMA中断服务函数
#define RHEOSTAT_ADC_DMA_IRQHandler                 DMA1_Stream1_IRQHandler

#define Rheostat_ADC12_IRQ                          ADC_IRQn

void ADC_Init(void);

uint16_t ADC_GetValue(void);
void Rheostat_ADC_NVIC_Config(void);
#endif /* __ADC_H */
