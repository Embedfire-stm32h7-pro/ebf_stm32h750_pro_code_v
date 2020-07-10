/**
  ******************************************************************************
  * @file    bsp_beep.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   蜂鸣器函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32H743 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./beep/bsp_beep.h"   

 /**
  * @brief  初始化控制蜂鸣器的IO
  * @param  无
  * @retval 无
  */
void BEEP_GPIO_Config(void)
{		
    /*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;

    /*开启控制蜂鸣器的GPIO的端口时钟*/
    BEEP_GPIO_CLK_ENABLE();   

    /*选择要控制蜂鸣器的GPIO*/															   
    GPIO_InitStructure.Pin = BEEP_GPIO_PIN;	

    /*设置GPIO模式为通用推挽输出*/
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;   

    GPIO_InitStructure.Pull = GPIO_PULLDOWN;

    /*设置GPIO速率为50MHz */   
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; 

    /*调用库函数，初始化控制蜂鸣器的GPIO*/
    HAL_GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure);			 

    /*  关闭蜂鸣器*/
    HAL_GPIO_WritePin(BEEP_GPIO_PORT, BEEP_GPIO_PIN,GPIO_PIN_RESET);	 
}
/*********************************************END OF FILE**********************/
