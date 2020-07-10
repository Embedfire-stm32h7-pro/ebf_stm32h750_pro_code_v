#ifndef __CAN_H
#define	__CAN_H

#include "stm32h7xx.h"
#include <stdio.h>

extern FDCAN_HandleTypeDef Can_Handle;

#define CANx                   			FDCAN2
#define CAN_CLK_ENABLE()     				__HAL_RCC_FDCAN_CLK_ENABLE()
#define CAN_RX_IRQ				   				FDCAN2_IT0_IRQn
#define CAN_RX_IRQHandler		  			FDCAN2_IRQHandler

#define CAN_RX_PIN                 	GPIO_PIN_6
#define CAN_TX_PIN                 	GPIO_PIN_5
#define CAN_TX_GPIO_PORT           	GPIOB
#define CAN_RX_GPIO_PORT           	GPIOB
#define CAN_TX_GPIO_CLK_ENABLE()   	__GPIOB_CLK_ENABLE()
#define CAN_RX_GPIO_CLK_ENABLE()   	__GPIOB_CLK_ENABLE()
#define CAN_AF_PORT                	GPIO_AF9_FDCAN2 


/* Definition for FDCANx's NVIC IRQ and IRQ Handlers */
#define FDCANx_IT0_IRQn   FDCAN1_IT0_IRQn
#define FDCANx_IT1_IRQn   FDCAN1_IT1_IRQn
#define FDCANx_IRQHandler FDCAN1_IRQHandler


/*debug*/

#define CAN_DEBUG_ON         1
#define CAN_DEBUG_ARRAY_ON   1
#define CAN_DEBUG_FUNC_ON    1
   
   
// Log define
#define CAN_INFO(fmt,arg...)           printf("<<-CAN-INFO->> "fmt"\n",##arg)
#define CAN_ERROR(fmt,arg...)          printf("<<-CAN-ERROR->> "fmt"\n",##arg)
#define CAN_DEBUG(fmt,arg...)          do{\
                                         if(CAN_DEBUG_ON)\
                                         printf("<<-CAN-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                       }while(0)

#define CAN_DEBUG_ARRAY(array, num)    do{\
                                         int32_t i;\
                                         uint8_t* a = array;\
                                         if(CAN_DEBUG_ARRAY_ON)\
                                         {\
                                            printf("<<-CAN-DEBUG-ARRAY->>\n");\
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

#define CAN_DEBUG_FUNC()               do{\
                                         if(CAN_DEBUG_FUNC_ON)\
                                         printf("<<-CAN-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)






static void CAN_GPIO_Config(void);
static void CAN_NVIC_Config(void);
static void CAN_Mode_Config(void);
static void CAN_Filter_Config(void);
void CAN_Config(void);
void CAN_SetMsg(void);
void Init_RxMes(void);
#endif







