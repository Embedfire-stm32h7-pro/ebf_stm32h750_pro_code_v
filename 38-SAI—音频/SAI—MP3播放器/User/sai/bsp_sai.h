#ifndef _BSP_SAI_H
#define	_BSP_SAI_H


#include "stm32h7xx.h"
/**SAI1_A_Block_A GPIO Configuration    
  PE5     ------>       SAI1_SCK_A/WM8978_BCLK
  PE4     ------>       SAI1_FS_A/WM8978_LRC
  PE6     ------>       SAI1_SD_A/WM8978_DACDAT/ADCDAT
  PE2     ------>       SAI1_MCLK_A/WM8978_MCLK 
*/
#define SAI_LRC_PIN                  GPIO_PIN_4                 
#define SAI_LRC_PORT                 GPIOE                     
#define SAI_LRC_GPIO_CLK_ENABLE()    __GPIOE_CLK_ENABLE()
#define SAI_LRC_GPIO_AF              GPIO_AF6_SAI1

#define SAI_BCLK_PIN                  GPIO_PIN_5                 
#define SAI_BCLK_PORT                 GPIOE                     
#define SAI_BCLK_GPIO_CLK_ENABLE()    __GPIOE_CLK_ENABLE()
#define SAI_BCLK_GPIO_AF              GPIO_AF6_SAI1

#define SAI_SDA_PIN                  GPIO_PIN_6                 
#define SAI_SDA_PORT                 GPIOE                     
#define SAI_SDA_GPIO_CLK_ENABLE()    __GPIOE_CLK_ENABLE()
#define SAI_SDA_GPIO_AF              GPIO_AF6_SAI1

#define SAI_MCLK_PIN                  GPIO_PIN_2                 
#define SAI_MCLK_PORT                 GPIOE                     
#define SAI_MCLK_GPIO_CLK_ENABLE()    __GPIOE_CLK_ENABLE()
#define SAI_MCLK_GPIO_AF              GPIO_AF6_SAI1

#define SAIx_Block_A                  SAI1_Block_A
#define SAI_CLK_ENABLE()              __HAL_RCC_SAI1_CLK_ENABLE();


#define DMA_Instance                  DMA1_Stream2
#define DMA_IRQn                      DMA1_Stream2_IRQn
#define DMA_CLK_ENABLE()              __HAL_RCC_DMA1_CLK_ENABLE(); 
extern void (*SAI_DMA_TX_Callback)(void);		//I2S DMA TX回调函数指针  
void I2Sx_TX_DMA_STREAM_IRQFUN(void);
void SAI_GPIO_Config(void);
void SAI_Play_Stop(void);
void SAI_Play_Start(void);
void SAIxA_Tx_Config(const uint16_t _usStandard, const uint16_t _usWordLen, const uint32_t _usAudioFreq);
void SAIA_TX_DMA_Init(uint32_t buffer0,uint32_t buffer1,const uint32_t num);
#endif /* _BSP_SAI_H */


