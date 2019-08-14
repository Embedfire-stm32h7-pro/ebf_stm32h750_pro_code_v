/**
  ******************************************************************************
  * @file    bsp_basic_tim.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   基本定时器定时范例
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 H750 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./tim/bsp_advance_tim.h"
#include "./led/bsp_led.h" 

TIM_HandleTypeDef TIM_PWMOUTNPUT_Handle;
TIM_HandleTypeDef TIM_PWMINPUT_Handle;



__IO uint16_t ChannelPulse = 5000;

__IO uint16_t IC2Value = 0;
__IO uint16_t IC1Value = 0;
__IO float DutyCycle = 0;
__IO float Frequency = 0;
 /**
  * @brief  高级定时器 TIMx,x=[1,8]通道引脚初始化
  * @param  无
  * @retval 无
  */
static void TIMx_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    //使能高级定时器的引脚时钟
    ADVANCE_TIM_CHx_CLK();
    //使能通用定时器的引脚时钟
    GENERAL_TIM_CHx_CLK();
  
   
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; 
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH ;
    
    GPIO_InitStruct.Pin = GENERAL_TIM_CHx_PIN;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GENERAL_TIM_CHx_PORT, &GPIO_InitStruct); 

    GPIO_InitStruct.Pin = ADVANCE_TIM_CHx_PIN;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(ADVANCE_TIM_CHx_PORT, &GPIO_InitStruct);
}  

/*
 * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
 * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
 * 另外三个成员是通用定时器和高级定时器才有.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         都有
 * TIM_CounterMode			 TIMx,x[6,7]没有，其他都有（基本定时器）
 * TIM_Period            都有
 * TIM_ClockDivision     TIMx,x[6,7]没有，其他都有(基本定时器)
 * TIM_RepetitionCounter TIMx,x[1,8]才有(高级定时器)
 *-----------------------------------------------------------------------------
 */
 
 /**
  * @brief  通用定时器TIMx初始化函数
  * @param  无
  * @retval 无
  */
static void TIMx_Output_Configuration(void)
{
    TIM_OC_InitTypeDef sConfig;
    //使能TIMx的时钟
    GENERAL_TIM_CLK_ENABLE();    
    //TIMx的外设寄存器基地址
    TIM_PWMOUTNPUT_Handle.Instance = GENERAL_TIM;
    //计数模式为向上计数，递增
    TIM_PWMOUTNPUT_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    /* 累计 TIM_Period个后产生一个更新或者中断*/		
    //当定时器从0计数到9999，即为10000次，为一个定时周期
    TIM_PWMOUTNPUT_Handle.Init.Period = 10000 - 1;
    //时钟不分频，即一个TIM_CLK时钟计1次
    TIM_PWMOUTNPUT_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    //定时器时钟源TIMxCLK = 2 * PCLK1  
    //				PCLK1 = HCLK / 4 
    //				=> TIMxCLK=HCLK/2=SystemCoreClock/2=200MHz
    // 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=1000000Hz
    TIM_PWMOUTNPUT_Handle.Init.Prescaler =  200 - 1;   
    // 初始化定时器TIM
    HAL_TIM_PWM_Init(&TIM_PWMOUTNPUT_Handle);
  
    /* 配置TIM为互补输出模式 */
    //互补输出，PWM1模式
    //计数值CNT小于比较值CCR（sConfig.Pulse），通道x为有效电平
    //计数值CNT大于比较值CCR（sConfig.Pulse），通道x为无效电平
    sConfig.OCMode = TIM_OCMODE_PWM1;
    //CHx的有效电平为高电平
    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    //CHx在空闲状态下为低电平
    sConfig.OCIdleState = TIM_OCNIDLESTATE_RESET;
    //CHxN在空闲状态下为高电平(必须与CHx相反)
    sConfig.OCNIdleState = TIM_OCIDLESTATE_SET;
    //CHxN的有效电平为高电平
    sConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    //比较器CCR的值
    sConfig.Pulse = ChannelPulse;
    //初始化输出比较通道
    HAL_TIM_OC_ConfigChannel(&TIM_PWMOUTNPUT_Handle, &sConfig, GENERAL_TIM_CH);
 
    //主动输出CHx使能
    HAL_TIM_OC_Start(&TIM_PWMOUTNPUT_Handle, GENERAL_TIM_CH);

}


/**
  * @brief  通用定时器TIMx初始化函数
  * @param  无
  * @retval 无
  */
static void TIMx_Input_Configuration(void)
{
  
    TIM_IC_InitTypeDef TIM_IC_Config; 
    TIM_SlaveConfigTypeDef TIM_SlaveConfig;
  
    //使能TIMx的时钟
    ADVANCE_TIM_CLK_ENABLE();    
    //TIMx的外设寄存器基地址
    TIM_PWMINPUT_Handle.Instance = ADVANCE_TIM;
    //计数模式为向上计数，递增
    TIM_PWMINPUT_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    /* 累计 TIM_Period个后产生一个更新或者中断*/		
    //当定时器从0计数到9999，即为10000次，为一个定时周期
    TIM_PWMINPUT_Handle.Init.Period = 10000;
    //时钟不分频，即一个TIM_CLK时钟计1次
    TIM_PWMINPUT_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    //定时器时钟源TIMxCLK = 2 * PCLK1  
    //				PCLK1 = HCLK / 4 
    //				=> TIMxCLK=HCLK/2=SystemCoreClock/2=200MHz
    // 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=100Hz
    TIM_PWMINPUT_Handle.Init.Prescaler =  200 - 1;   
    // 初始化定时器TIM
    HAL_TIM_PWM_Init(&TIM_PWMINPUT_Handle);    
    
    /* IC1捕获：上升沿触发TI1FP1 */
    TIM_IC_Config.ICPolarity = TIM_ICPOLARITY_RISING;
    TIM_IC_Config.ICFilter = 0;
    TIM_IC_Config.ICPrescaler = TIM_ICPSC_DIV1;
    TIM_IC_Config.ICSelection = TIM_ICSELECTION_DIRECTTI; 
    HAL_TIM_IC_ConfigChannel(&TIM_PWMINPUT_Handle, &TIM_IC_Config, ADVANCE_TIM_IC_CH1);
    /* IC2捕获：上升沿触发TI1FP2 */
    TIM_IC_Config.ICPolarity = TIM_ICPOLARITY_FALLING;
    TIM_IC_Config.ICFilter = 0;
    TIM_IC_Config.ICPrescaler = TIM_ICPSC_DIV1;
    TIM_IC_Config.ICSelection = TIM_ICSELECTION_INDIRECTTI; 
    HAL_TIM_IC_ConfigChannel(&TIM_PWMINPUT_Handle, &TIM_IC_Config, ADVANCE_TIM_IC_CH2);    
    /* 选择从模式: 复位模式 */
    TIM_SlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
    /* 选择定时器输入触发: TI1FP1 */
    TIM_SlaveConfig.InputTrigger = TIM_TS_TI1FP1;
    HAL_TIM_SlaveConfigSynchronization(&TIM_PWMINPUT_Handle,&TIM_SlaveConfig);
    
    HAL_TIM_IC_Start_IT(&TIM_PWMINPUT_Handle,ADVANCE_TIM_IC_CH1);
    HAL_TIM_IC_Start_IT(&TIM_PWMINPUT_Handle,ADVANCE_TIM_IC_CH2);
}


/**
  * @brief  TIM中断优先级配置函数
  * @param  无
  * @retval 无
  */  
static void ADVANCE_TIM_NVIC_Config(void)
{
    HAL_NVIC_SetPriority(ADVANCE_TIM_IRQ, 0, 0);
    HAL_NVIC_EnableIRQ(ADVANCE_TIM_IRQ);
}
/**
  * @brief  初始化基本定时器定时，1ms产生一次中断
  * @param  无
  * @retval 无
  */
void TIM_Advance_Init(void)
{
    //TIM通道引脚初始化
    TIMx_GPIO_Configuration();
    //高级定时器中断配置
    ADVANCE_TIM_NVIC_Config();
    //通用TIM外设初始化
    TIMx_Output_Configuration();
    //高级TIM外设初始化
    TIMx_Input_Configuration();
}

/**
  * @brief  输入捕获完成中断回调函数
  * @param  TIM_Handle : ADC句柄
  * @retval 无
  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
      /* 获取输入捕获值 */
      IC1Value = HAL_TIM_ReadCapturedValue(&TIM_PWMINPUT_Handle,TIM_CHANNEL_1);
      IC2Value = HAL_TIM_ReadCapturedValue(&TIM_PWMINPUT_Handle,TIM_CHANNEL_2);	
      if (IC1Value != 0)
      {
      /* 占空比计算 */
      DutyCycle = (float)((IC2Value+1) * 100) / (IC1Value+1);

      /* 频率计算 */
      Frequency = 200000000/200/(float)(IC1Value+1);
      
      }
      else
      {
      DutyCycle = 0;
      Frequency = 0;
      }

    }    
}
/*********************************************END OF FILE**********************/

