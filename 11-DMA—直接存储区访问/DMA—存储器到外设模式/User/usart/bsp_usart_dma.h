#ifndef __DEBUG_USART_H
#define	__DEBUG_USART_H

#include "stm32h7xx.h"
#include <stdio.h>



//引脚定义
/*******************************************************/
#define DEBUG_USART                             USART1
#define DEBUG_USART_CLK_ENABLE()                __USART1_CLK_ENABLE();

#define DEBUG_USART_RX_GPIO_PORT                GPIOA
#define DEBUG_USART_RX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define DEBUG_USART_RX_PIN                      GPIO_PIN_10
#define DEBUG_USART_RX_AF                       GPIO_AF7_USART1


#define DEBUG_USART_TX_GPIO_PORT                GPIOA
#define DEBUG_USART_TX_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define DEBUG_USART_TX_PIN                      GPIO_PIN_9
#define DEBUG_USART_TX_AF                       GPIO_AF7_USART1

#define DEBUG_USART_IRQHandler                  USART1_IRQHandler
#define DEBUG_USART_IRQ                 		    USART1_IRQn
/************************************************************/
//DMA
#define SENDBUFF_SIZE                     		  5000	//发送的数据量
#define DEBUG_USART_DMA_CLK_ENABLE()      		  __DMA2_CLK_ENABLE()	
#define DEBUG_USART_DMA_REQUEST                 DMA_REQUEST_USART1_TX
#define DEBUG_USART_DMA_STREAM            		  DMA2_Stream7

//串口波特率
#define DEBUG_USART_BAUDRATE                    115200


void Usart_SendString(uint8_t *str);
void DEBUG_USART_Config(void);
void USART_DMA_Config(void);
//int fputc(int ch, FILE *f);
extern UART_HandleTypeDef UartHandle;
#endif /* __USART1_H */
