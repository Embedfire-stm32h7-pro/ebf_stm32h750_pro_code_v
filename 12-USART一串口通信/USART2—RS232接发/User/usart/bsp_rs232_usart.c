/**
  ******************************************************************************
  * @file    bsp_RS232_USART.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   使用串口2，重定向c库printf函数到usart端口，中断接收模式
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 H750 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./usart/bsp_rs232_usart.h"

UART_HandleTypeDef UartHandle;
extern uint8_t ucTemp;  
 /**
  * @brief  RS232_USART GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */  
void RS232_USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;
        
    RS232_USART_RX_GPIO_CLK_ENABLE();
    RS232_USART_TX_GPIO_CLK_ENABLE();
    
    /* 配置串口2时钟源*/
    RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_UARTx;
    RCC_PeriphClkInit.Usart234578ClockSelection = RCC_UARTxCLKSOURCE_SYSCLK;
    HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);
    /* 使能 UART 时钟 */
    RS232_USART_CLK_ENABLE();

    /**USART2 GPIO Configuration    
    PB13    ------> UART5_TX
    PB12    ------> UART5_RX 
    */
    /* 配置Tx引脚为复用功能  */
    GPIO_InitStruct.Pin = RS232_USART_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = RS232_USART_TX_AF;
    HAL_GPIO_Init(RS232_USART_TX_GPIO_PORT, &GPIO_InitStruct);
    
    /* 配置Rx引脚为复用功能 */
    GPIO_InitStruct.Pin = RS232_USART_RX_PIN;
    GPIO_InitStruct.Alternate = RS232_USART_RX_AF;
    HAL_GPIO_Init(RS232_USART_RX_GPIO_PORT, &GPIO_InitStruct); 
    
    /* 配置串RS232_USART 模式 */
    UartHandle.Instance = RS232_USART;
    UartHandle.Init.BaudRate = 115200;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits = UART_STOPBITS_1;
    UartHandle.Init.Parity = UART_PARITY_NONE;
    UartHandle.Init.Mode = UART_MODE_TX_RX;
//    UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//    UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
//    UartHandle.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
//    UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    HAL_UART_Init(&UartHandle);

    /*串口中断初始化 */
    HAL_NVIC_SetPriority(RS232_USART_IRQ, 0, 0);
    HAL_NVIC_EnableIRQ(RS232_USART_IRQ);
    /*配置串口接收中断 */
    __HAL_UART_ENABLE_IT(&UartHandle,UART_IT_RXNE);  
}


/*****************  发送字符串 **********************/
void Usart_SendString(uint8_t *str)
{
	unsigned int k=0;
  do 
  {
      HAL_UART_Transmit( &UartHandle,(uint8_t *)(str + k) ,1,1000);
      k++;
  } while(*(str + k)!='\0');
  
}
///重定向c库函数printf到串口RS232_USART，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
	/* 发送一个字节数据到串口RS232_USART */
	HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 1000);	
	
	return (ch);
}

///重定向c库函数scanf到串口RS232_USART，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
		
	int ch;
	HAL_UART_Receive(&UartHandle, (uint8_t *)&ch, 1, 1000);	
	return (ch);
}
/*********************************************END OF FILE**********************/
