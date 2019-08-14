/**
  ******************************************************************************
  * @file    bsp_exti.c
  * @author  fire
  * @version V1.0
  * @date    2017-xx-xx
  * @brief   I/O线中断应用bsp
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32H743 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./key/bsp_exti.h"
#include "./led/bsp_led.h" 
#include "./usart/bsp_usart.h"

 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Configuration(void)
{
	/* 配置NVIC为优先级组1 */
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_1);	
	/*按键1中断设置，抢占优先级0，子优先级为0*/
	HAL_NVIC_SetPriority(KEY1_INT_EXTI_IRQ, 1 ,1);
	HAL_NVIC_EnableIRQ(KEY1_INT_EXTI_IRQ);	
	/*按键2中断设置，抢占优先级0，子优先级为0*/
	HAL_NVIC_SetPriority(KEY2_INT_EXTI_IRQ, 1 ,1);
	HAL_NVIC_EnableIRQ(KEY2_INT_EXTI_IRQ);
}

 /**
  * @brief  配置 KEY1和KEY2为中断，并设置中断优先级
  * @param  无
  * @retval 无
  */
void EXTI_Key_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
  
	/*开启按键GPIO口的时钟*/
	KEY1_INT_GPIO_CLK_ENABLE();
	KEY2_INT_GPIO_CLK_ENABLE();
   
	/* 配置 NVIC */
	NVIC_Configuration();  
	/*选择按键的引脚*/	
	GPIO_InitStructure.Pin = KEY1_INT_GPIO_PIN; 
	/*设置引脚为上升沿中断模式*/
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING; 
	/*设置引脚不上拉也不下拉*/
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	/*使用上面的结构体初始化按键*/
	HAL_GPIO_Init(KEY1_INT_GPIO_PORT, &GPIO_InitStructure);
	HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PA0, SYSCFG_SWITCH_PA0_CLOSE);

	/*选择按键2的引脚*/	
	GPIO_InitStructure.Pin = KEY2_INT_GPIO_PIN; 
	/*设置引脚为上升沿中断模式*/
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING; 
	/*设置引脚不上拉也不下拉*/
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	/*使用上面的结构体初始化按键*/
	HAL_GPIO_Init(KEY2_INT_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	LED_BLUE;
	if(GPIO_Pin==KEY1_INT_GPIO_PIN)
		printf("\r\n KEY1 按键中断唤醒 \r\n");
	else if(GPIO_Pin==KEY2_INT_GPIO_PIN)
		printf("\r\n KEY2 按键中断唤醒 \r\n"); 
	else
		{}
}

/*********************************************END OF FILE**********************/
