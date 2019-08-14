/**
  ******************************************************************************
  * @file    bsp_basic_tim.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   高级定时器互补输出
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 H750 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./tim/bsp_advance_tim.h"
#include "./led/bsp_led.h" 

TIM_HandleTypeDef TIM_Handle;
TIM_OC_InitTypeDef sConfig;
TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

__IO uint16_t ChannelPulse = 4999;
 /**
  * @brief  高级定时器 TIMx,x=[1,8]通道引脚初始化
  * @param  无
  * @retval 无
  */
static void TIMx_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    ADVANCE_TIM_CHx_CLK();
    ADVANCE_TIM_CHxN_CLK();
    ADVANCE_TIM_BKIN_CLK();
  
    GPIO_InitStruct.Pin = ADVANCE_TIM_CHx_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; 
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH ;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(ADVANCE_TIM_CHx_PORT, &GPIO_InitStruct);
  
    GPIO_InitStruct.Pin = ADVANCE_TIM_CHxN_PIN;
    HAL_GPIO_Init(ADVANCE_TIM_CHxN_PORT, &GPIO_InitStruct);
  
    GPIO_InitStruct.Pin = ADVANCE_TIM_BKIN_PIN;
    HAL_GPIO_Init(ADVANCE_TIM_BKIN_PORT, &GPIO_InitStruct);   
    
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
  * @brief  高级定时器 TIMx,x=[1,8]初始化函数
  * @param  无
  * @retval 无
  */
static void TIMx_Configuration(void)
{
    //使能TIMx的时钟
    ADVANCE_TIM_CLK_ENABLE();    
    //TIMx的外设寄存器基地址
    TIM_Handle.Instance = ADVANCE_TIM;
    //计数模式为向上计数，递增
    TIM_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    /* 累计 TIM_Period个后产生一个更新或者中断*/		
    //当定时器从0计数到9999，即为10000次，为一个定时周期
    TIM_Handle.Init.Period = 10000 - 1;
    //时钟不分频，即一个TIM_CLK时钟计1次
    TIM_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    //定时器时钟源TIMxCLK = 2 * PCLK1  
    //				PCLK1 = HCLK / 4 
    //				=> TIMxCLK=HCLK/2=SystemCoreClock/2=240MHz
    // 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=1000000Hz
    TIM_Handle.Init.Prescaler =  240 - 1;   
    // 初始化定时器TIM
    HAL_TIM_PWM_Init(&TIM_Handle);
  
    /* 配置TIM为互补输出模式 */
    //互补输出，PWM1模式
    //计数值CNT小于比较值CCR（sConfig.Pulse），通道x为有效电平
    //计数值CNT大于比较值CCR（sConfig.Pulse），通道x为无效电平
    sConfig.OCMode = TIM_OCMODE_PWM1;
    //CHx的有效电平为高电平
    sConfig.OCPolarity = TIM_OCNPOLARITY_LOW;
    //CHx在空闲状态下为低电平
    sConfig.OCIdleState = TIM_OCNIDLESTATE_SET;
    //CHxN在空闲状态下为高电平(必须与CHx相反)
    sConfig.OCNIdleState = TIM_OCIDLESTATE_RESET;
    //CHxN的有效电平为高电平
    sConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    //比较器CCR的值
    sConfig.Pulse = ChannelPulse;
    //初始化输出比较通道
    HAL_TIM_PWM_ConfigChannel(&TIM_Handle, &sConfig, ADVANCE_TIM_CH);
 
    //使能自动输出功能
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_ENABLE;
    //断路输入极性：低电平有效
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_LOW;
    //使能断路功能
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_ENABLE;
    //死区时间：11*Fdts
    sBreakDeadTimeConfig.DeadTime = 11;
    //对寄存器提供写保护，级别越高，可操作的寄存器位越少
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_1;
    //空闲模式下的断路状态，允许通道输出
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_ENABLE;
    //运行模式下的断路状态，允许通道输出
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_ENABLE;
    //配置TIM为死区，断路功能
    HAL_TIMEx_ConfigBreakDeadTime(&TIM_Handle, &sBreakDeadTimeConfig);
    //主动输出CHx使能
    HAL_TIM_PWM_Start(&TIM_Handle, ADVANCE_TIM_CH);
    //主动输出CHxN使能
    HAL_TIMEx_PWMN_Start(&TIM_Handle, ADVANCE_TIM_CH);
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
    //TIM外设初始化
    TIMx_Configuration();
  
}

/*********************************************END OF FILE**********************/

