/**
  ******************************************************************
  * @file    bsp_adcd.c
  * @author  fire
  * @version V1.1
  * @date    2018-xx-xx
  * @brief   adc应用函数接口
  ******************************************************************
  * @attention
  *
  * 实验平台:野火 STM32H750开发板 
  * 公司    :http://www.embedfire.com
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */
#include "./adc/bsp_adc.h" 

extern float ADC_vol;

ADC_HandleTypeDef ADC_Handle;
__IO uint16_t ADC_ConvertedValue = 0;


/**
  * @brief  ADC引脚配置函数
  * @param  无
  * @retval 无
  */  
static void ADC_GPIO_Mode_Config(void)
{
  /* 定义一个GPIO_InitTypeDef类型的结构体 */
  GPIO_InitTypeDef  GPIO_InitStruct;
  /* 使能ADC3引脚的时钟 */
  RHEOSTAT_ADC_GPIO_CLK_ENABLE();
  __HAL_RCC_SYSCFG_CLK_ENABLE();
    
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG; 
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = RHEOSTAT_ADC_PIN;
  /* 配置为模拟输入，不需要上拉电阻 */ 
  HAL_GPIO_Init(RHEOSTAT_ADC_GPIO_PORT, &GPIO_InitStruct);
  
  /* H750XBH6的ADC3_CH1使用的是PC3_C，与PC3是两个不同的引脚，通过一个模拟开关连接，使用时需要切换 */
  /* PC3_C ------> ADC3_INP1  */
  HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PC3, SYSCFG_SWITCH_PC3_OPEN);  
}

/**
  * @brief  ADC工作模式配置函数
  * @param  无
  * @retval 无
  */  
static void ADC_Mode_Config(void)
{
	ADC_ChannelConfTypeDef ADC_Config;

	RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;  
	/*            配置ADC3时钟源             */
	/*    HSE Frequency(Hz)    = 25000000   */                                             
	/*         PLL_M                = 5     */
	/*         PLL_N                = 160   */
	/*         PLL_P                = 25    */
	/*         PLL_Q                = 2     */
	/*         PLL_R                = 2     */
	/*     ADC_ker_clk         = 32000000   */
	RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	RCC_PeriphClkInit.PLL2.PLL2FRACN = 0;
	RCC_PeriphClkInit.PLL2.PLL2M = 5;
	RCC_PeriphClkInit.PLL2.PLL2N = 160;
	RCC_PeriphClkInit.PLL2.PLL2P = 25;
	RCC_PeriphClkInit.PLL2.PLL2Q = 2;
	RCC_PeriphClkInit.PLL2.PLL2R = 2;
	RCC_PeriphClkInit.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2;
	RCC_PeriphClkInit.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
	RCC_PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2; 
	HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);  

	/* 使能ADC3时钟 */
	RHEOSTAT_ADC_CLK_ENABLE();  
		
	ADC_Handle.Instance = RHEOSTAT_ADC;
	//ADC时钟1分频
	ADC_Handle.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	//使能连续转换模式
	ADC_Handle.Init.ContinuousConvMode = ENABLE;
	//转换通道 1个
	ADC_Handle.Init.NbrOfConversion = 1;
	//数据存放在数据寄存器中
	ADC_Handle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
	//关闭不连续转换模式
	ADC_Handle.Init.DiscontinuousConvMode = DISABLE;
	// 非连续转换个数
	ADC_Handle.Init.NbrOfDiscConversion = 0;
	//数据右对齐	
	ADC_Handle.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
	
	//使能EOC标志位
	ADC_Handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	//软件触发
	ADC_Handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	//关闭低功耗自动等待
	ADC_Handle.Init.LowPowerAutoWait = DISABLE;
	//数据溢出时，覆盖写入
	ADC_Handle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
	//不使能过采样模式
	ADC_Handle.Init.OversamplingMode = DISABLE;
	//分辨率为：16bit
	ADC_Handle.Init.Resolution = ADC_RESOLUTION_16B;
	//不使能多通道扫描
	ADC_Handle.Init.ScanConvMode = DISABLE;
	//初始化 ADC
	HAL_ADC_Init(&ADC_Handle);
	
	//ADC校准
  HAL_ADCEx_Calibration_Start(&ADC_Handle,ADC_CALIB_OFFSET,ADC_SINGLE_ENDED); 

	//使用通道1
	ADC_Config.Channel = ADC_CHANNEL_1;
	//转换顺序为1
	ADC_Config.Rank = ADC_REGULAR_RANK_1 ;
	//采样周期为64.5个周期
	ADC_Config.SamplingTime = ADC_SAMPLETIME_64CYCLES_5;
	//不使用差分输入的功能
	ADC_Config.SingleDiff = ADC_SINGLE_ENDED ;
	//配置ADC通道
	HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);
	//使能ADC
	ADC_Enable(&ADC_Handle);
}
/**
  * @brief  ADC中断优先级配置函数
  * @param  无
  * @retval 无
  */  
static void Rheostat_ADC_NVIC_Config(void)
{
	HAL_NVIC_SetPriority(Rheostat_ADC_IRQ, 1, 1);
	HAL_NVIC_EnableIRQ(Rheostat_ADC_IRQ);
}

/**
  * @brief  ADC初始化函数
  * @param  无
  * @retval 无
  */
void ADC_Init(void)
{
	ADC_GPIO_Mode_Config();

	ADC_Mode_Config();
	//ADC中断优先级配置
	Rheostat_ADC_NVIC_Config();
	//使能ADC中断 
	HAL_ADC_Start_IT(&ADC_Handle);
	//软件触发ADC采样
	HAL_ADC_Start(&ADC_Handle);
}

/**
  * @brief  转换完成中断回调函数（非阻塞模式）
  * @param  AdcHandle : ADC句柄
  * @retval 无
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
  /* 获取结果 */
  ADC_ConvertedValue = HAL_ADC_GetValue(AdcHandle);  
}
/*********************************************END OF FILE**********************/


