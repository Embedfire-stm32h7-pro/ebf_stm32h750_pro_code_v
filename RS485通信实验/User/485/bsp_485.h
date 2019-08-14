#ifndef __485_H
#define	__485_H

#include "stm32h7xx.h"
#include <stdio.h>



#define _485_USART                             USART2
#define _485_USART_CLK_ENABLE()                __USART2_CLK_ENABLE();
#define _485_USART_BAUDRATE                    115200

#define RCC_PERIPHCLK_485_USART                RCC_PERIPHCLK_USART2
#define RCC_485_USARTCLKSOURCE_SYSCLK          RCC_USART2CLKSOURCE_SYSCLK

#define _485_USART_RX_GPIO_PORT                GPIOD
#define _485_USART_RX_GPIO_CLK_ENABLE()        __GPIOD_CLK_ENABLE()
#define _485_USART_RX_PIN                      GPIO_PIN_6
#define _485_USART_RX_AF                       GPIO_AF7_USART2

#define _485_USART_TX_GPIO_PORT                GPIOD
#define _485_USART_TX_GPIO_CLK_ENABLE()        __GPIOD_CLK_ENABLE()
#define _485_USART_TX_PIN                      GPIO_PIN_5
#define _485_USART_TX_AF                       GPIO_AF7_USART2


#define _485_RE_GPIO_PORT					             GPIOD
#define _485_RE_GPIO_CLK_ENABLE()              __GPIOD_CLK_ENABLE()
#define _485_RE_PIN							               GPIO_PIN_4 

#define _485_INT_IRQ                 		       USART2_IRQn
#define bsp_485_IRQHandler                     USART2_IRQHandler


// 不精确的延时
static void _485_delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
} 

/*控制收发引脚*/
//进入接收模式,必须要有延时等待485处理完数据
#define _485_RX_EN()			_485_delay(1000);\
		HAL_GPIO_WritePin(_485_RE_GPIO_PORT,_485_RE_PIN,GPIO_PIN_RESET); _485_delay(1000);
//进入发送模式,必须要有延时等待485处理完数据
#define _485_TX_EN()			_485_delay(1000);\
HAL_GPIO_WritePin(_485_RE_GPIO_PORT,_485_RE_PIN,GPIO_PIN_SET); _485_delay(1000);



/*debug*/

#define _485_DEBUG_ON         1
#define _485_DEBUG_ARRAY_ON   1
#define _485_DEBUG_FUNC_ON    1
   
   
// Log define
#define _485_INFO(fmt,arg...)           printf("<<-_485-INFO->> "fmt"\n",##arg)
#define _485_ERROR(fmt,arg...)          printf("<<-_485-ERROR->> "fmt"\n",##arg)
#define _485_DEBUG(fmt,arg...)          do{\
																					 if(_485_DEBUG_ON)\
																					 printf("<<-_485-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
																				 }while(0)

#define _485_DEBUG_ARRAY(array, num)    do{\
                                         int32_t i;\
                                         uint8_t* a = array;\
                                         if(_485_DEBUG_ARRAY_ON)\
                                         {\
                                            printf("<<-_485-DEBUG-ARRAY->>\n");\
                                            for (i = 0; i < (num); i++)\
                                            {\
                                                printf("%02x   ", (a)[i]);\
                                                if ((i + 1 ) %10 == 0)\
                                                {\
                                                    printf("\n");\
                                                }\
                                            }\
                                            printf("\n");\
                                        }\
                                       }while(0)

#define _485_DEBUG_FUNC()               do{\
                                         if(_485_DEBUG_FUNC_ON)\
                                         printf("<<-_485-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)


void _485_Config(void);
void _485_SendByte(  uint8_t ch );
void _485_SendStr_length( uint8_t *str,uint32_t strlen );
void _485_SendString(  uint8_t *str);



void bsp_485_IRQHandler(void);
char *get_rebuff(uint16_t *len);
void clean_rebuff(void);
#endif /* __485_H */
