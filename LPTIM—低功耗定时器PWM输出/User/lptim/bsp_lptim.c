/**
  ******************************************************************
  * @file    bsp_key.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   LPTIM-低功耗定时器PWM输出初始化配置
  ******************************************************************
  * @attention
  *
  * 实验平台:野火 STM32H750 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************
  */ 
#include "./lptim/bsp_lptim.h" 
LPTIM_HandleTypeDef LPTIM_Handle;
/**
  * @brief  LPTIM的引脚初始化
  * @param  无
  * @retval 无
  */  
void LPTIM_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /*开启LPTIM1引脚的时钟*/
    LPTIMx_OUT_GPIO_CLK_ENABLE();
    /*选择按键的引脚*/	
    GPIO_InitStructure.Pin = LPTIMx_OUT_PIN; 
    /*设置引脚为复用推捖模式*/
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP; 
    /*设置引脚不上拉也不下拉*/
    GPIO_InitStructure.Pull = GPIO_PULLUP;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; 	
    GPIO_InitStructure.Alternate = LPTIMx_OUT_AF;
    /*使用上面的结构体初始化按键*/
    HAL_GPIO_Init(LPTIMx_OUT_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  配置TIM输出PWM
  * @param  无
  * @retval 无
  */
void LPTIM_MODE_Config(void)
{

  RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInitStruct;  
  uint32_t PeriodValue;
	uint32_t PulseValue;
  RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LPTIM1;
  /* 选择LSI时钟作为LPTIM时钟源 */
#if (LPTIMx_CLK_Source == LSI)    
  RCC_PeriphCLKInitStruct.Lptim1ClockSelection = RCC_LPTIM1CLKSOURCE_LSI;
  /* 选择LSE时钟作为LPTIM时钟源 */
#elif (LPTIMx_CLK_Source == LSE)    
  RCC_PeriphCLKInitStruct.Lptim1ClockSelection = RCC_LPTIM1CLKSOURCE_LSE;
#endif
  HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);
  /*开启LPTIM1的时钟*/
  
  LPTIMx_CLK_ENABLE();   
	/* 定义定时器的句柄即确定定时器寄存器的基地址*/  
  LPTIM_Handle.Instance = LPTIMx_Instance;
  //选择内部时钟源LPTIM_CLK = LSE=32.768KHz   
  LPTIM_Handle.Init.Clock.Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC; 
	// 定时器时钟分频系数  
  LPTIM_Handle.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV1;
	// 定时器计数源，内部  
  LPTIM_Handle.Init.CounterSource = LPTIM_COUNTERSOURCE_INTERNAL;
	// 触发源，软件触发
	LPTIM_Handle.Init.Trigger.Source  = LPTIM_TRIGSOURCE_SOFTWARE;  
	// 定时器输出极性
	LPTIM_Handle.Init.OutputPolarity  = LPTIM_OUTPUTPOLARITY_HIGH;
  // 定时器更新方式
	LPTIM_Handle.Init.UpdateMode      = LPTIM_UPDATE_IMMEDIATE;
  
  HAL_LPTIM_Init(&LPTIM_Handle);
	/*PWM模式配置*/
	/*当定时器从0计数到99，即为100次，
  为一个定时周期PWM周期，32.768KHz/100=327.68Hz*/
	PeriodValue = 100-1;
	/*PWM脉冲为周期一半即50% */
	PulseValue = 50-1;
	HAL_LPTIM_PWM_Start(&LPTIM_Handle, PeriodValue, PulseValue);
}
/**
  * @brief  低功耗定时器在低功耗模式输出PWM
  * @param  无
  * @retval 无
  */
void LPTIM_PWM_OUT(void)
{
	LPTIM_GPIO_Config();	
	
	LPTIM_MODE_Config();
	/* 进入低功耗模式 */
  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
	/* 等待系统被按键KEY2唤醒，退出低功耗模式后停止输出PWM */
  HAL_LPTIM_PWM_Stop(&LPTIM_Handle);
}

/*********************************************END OF FILE**********************/
