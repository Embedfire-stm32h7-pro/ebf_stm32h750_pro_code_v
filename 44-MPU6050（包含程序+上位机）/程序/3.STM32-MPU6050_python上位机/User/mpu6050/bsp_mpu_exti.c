/**
  ******************************************************************************
  * @file    bsp_exti.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   陀螺仪数据中断配置
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32H750 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./mpu6050/bsp_mpu_exti.h"

 /**
  * @brief  配置中断口，并设置中断优先级
  * @param  无
  * @retval 无
  */
void EXTI_MPU_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 

    /*开启GPIO口的时钟*/
    MPU_INT_GPIO_CLK_ENABLE();

    /* 选择中断的引脚 */ 
    GPIO_InitStructure.Pin = MPU_INT_GPIO_PIN;
    /* 设置引脚为输入模式 */ 
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;	    		
    /* 设置引脚不上拉也不下拉 */
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    /* 使用上面的结构体初始化按键 */
    HAL_GPIO_Init(MPU_INT_GPIO_PORT, &GPIO_InitStructure); 
    /* 配置 EXTI 中断源 到key1 引脚、配置中断优先级*/
    HAL_NVIC_SetPriority(MPU_INT_EXTI_IRQ, 1, 1);
    /* 使能中断 */
    HAL_NVIC_EnableIRQ(MPU_INT_EXTI_IRQ);
}
/*********************************************END OF FILE**********************/
