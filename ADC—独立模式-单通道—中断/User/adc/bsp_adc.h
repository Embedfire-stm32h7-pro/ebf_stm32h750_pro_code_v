#ifndef __ADC_H
#define	__ADC_H

#include "stm32h7xx.h"


//引脚定义
#define RHEOSTAT_ADC_PIN                            GPIO_PIN_3                
#define RHEOSTAT_ADC_GPIO_PORT                      GPIOC                     
#define RHEOSTAT_ADC_GPIO_CLK_ENABLE()              __GPIOC_CLK_ENABLE()

// ADC 序号宏定义
#define RHEOSTAT_ADC                        ADC3
#define RHEOSTAT_ADC_CLK_ENABLE()           __ADC3_CLK_ENABLE()
#define RHEOSTAT_ADC_CHANNEL                ADC_CHANNEL_1

#define Rheostat_ADC_IRQ                    ADC3_IRQn

void ADC_Init(void);

uint16_t ADC_GetValue(void);
#endif /* __ADC_H */
