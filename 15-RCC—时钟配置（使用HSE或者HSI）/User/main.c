/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   使用按键在线切换系统时钟源
  ******************************************************************
  * @attention
  *
  * 实验平台:野火 STM32H750开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************
  */  
#include "stm32h7xx.h"
#include "./led/bsp_led.h"
#include "./rcc/bsp_clkconfig.h"
#include "./key/bsp_key.h" 
#include "./delay/core_delay.h" 
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	/* 系统时钟初始化成480MHz */
	SystemClock_Config();
	// LED 端口初始化 
	LED_GPIO_Config();	
	/*初始化按键*/
	Key_GPIO_Config();		
  /* 在MCO2引脚（PC.09）上输出SYSCLK / 4 */
  HAL_RCC_MCOConfig(RCC_MCO2, RCC_MCO2SOURCE_SYSCLK, RCC_MCODIV_4);

	while (1)
	{
		/* 检查是否按下了KEY2按钮来切换时钟配置 */
    if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON  )
		{
      
      //切换不同的时钟源,系统时钟为400MHz。
			SwitchSystemClock();

		} 		

    /* LED闪烁 */
    LED3_TOGGLE;
    HAL_Delay(100);
	}
}

/****************************END OF FILE***************************/
