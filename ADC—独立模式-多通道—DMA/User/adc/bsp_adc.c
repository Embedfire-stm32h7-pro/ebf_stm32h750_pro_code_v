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
#include "stm32h7xx.h"

extern float ADC_vol;

ADC_HandleTypeDef ADC1_Handle;
DMA_HandleTypeDef hdma_adc1;
__IO uint16_t ADC_ConvertedValue[4];


/**
  * @brief  ADC引脚配置函数
  * @param  无
  * @retval 无
  */  
static void ADC_GPIO_Mode_Config(void)
{
    /* 定义一个GPIO_InitTypeDef类型的结构体 */
    GPIO_InitTypeDef  GPIO_InitStruct;
    /* 使能ADC引脚的时钟 */
    RHEOSTAT_ADC_GPIO_CLK_ENABLE();
    //通道18——IO初始化
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG; 
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Pin = RHEOSTAT_ADC_PIN1; 
    /* 配置为模拟输入，不需要上拉电阻 */ 
    HAL_GPIO_Init(RHEOSTAT_ADC_GPIO_PORT, &GPIO_InitStruct);
    //通道19——IO初始化
    GPIO_InitStruct.Pin = RHEOSTAT_ADC_PIN2;
    HAL_GPIO_Init(RHEOSTAT_ADC_GPIO_PORT, &GPIO_InitStruct);
    //通道3——IO初始化
    GPIO_InitStruct.Pin = RHEOSTAT_ADC_PIN3;
    HAL_GPIO_Init(RHEOSTAT_ADC_GPIO_PORT, &GPIO_InitStruct);
    //通道7——IO初始化
    GPIO_InitStruct.Pin = RHEOSTAT_ADC_PIN4;
    HAL_GPIO_Init(RHEOSTAT_ADC_GPIO_PORT, &GPIO_InitStruct);  
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
    /*         PLL_P                = 25
	*/
    /*         PLL_Q                = 2     */
    /*         PLL_R                = 2     */
    /*     ADC_ker_clk         = 32000000   */
		RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    RCC_PeriphClkInit.PLL2.PLL2FRACN = 0;
    RCC_PeriphClkInit.PLL2.PLL2M = 5;
    RCC_PeriphClkInit.PLL2.PLL2N = 160;
    RCC_PeriphClkInit.PLL2.PLL2P = 16;
    RCC_PeriphClkInit.PLL2.PLL2Q = 2;
    RCC_PeriphClkInit.PLL2.PLL2R = 2;
    RCC_PeriphClkInit.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2;
    RCC_PeriphClkInit.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
    RCC_PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2; 
		HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);  
  
    /* 使能ADC1、2时钟 */
    RHEOSTAT_ADC1_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();
    
    hdma_adc1.Instance = DMA1_Stream1;
    hdma_adc1.Init.Request = DMA_REQUEST_ADC1;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
    hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if(HAL_DMA_Init(&hdma_adc1) != HAL_OK)
    {}
    __HAL_LINKDMA(&ADC1_Handle,DMA_Handle,hdma_adc1);    
      
    ADC1_Handle.Instance = RHEOSTAT_ADC1;
    //ADC时钟1分频
    ADC1_Handle.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
    //使能连续转换模式
    ADC1_Handle.Init.ContinuousConvMode = ENABLE;
    //数据存放在数据寄存器中
    ADC1_Handle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
    //关闭不连续转换模式
    ADC1_Handle.Init.DiscontinuousConvMode = DISABLE;
    //单次转换
    ADC1_Handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    //软件触发
    ADC1_Handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    //关闭低功耗自动等待
    ADC1_Handle.Init.LowPowerAutoWait = DISABLE;
    //数据溢出时，覆盖写入
    ADC1_Handle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
    //不使能过采样模式
    ADC1_Handle.Init.OversamplingMode = DISABLE;
    //分辨率为：16bit
    ADC1_Handle.Init.Resolution = ADC_RESOLUTION_16B;
    //不使能多通道扫描
    ADC1_Handle.Init.ScanConvMode = ENABLE;
    //扫描四个通道
    ADC1_Handle.Init.NbrOfConversion = 4;
    //初始化 ADC1
    HAL_ADC_Init(&ADC1_Handle);
          
    //使用通道18
    ADC_Config.Channel = RHEOSTAT_ADC_CHANNEL1;
    //转换顺序为1
    ADC_Config.Rank = ADC_REGULAR_RANK_1;
    //采样周期为64.5个周期
    ADC_Config.SamplingTime = ADC_SAMPLETIME_64CYCLES_5;
    //不使用差分输入的功能
    ADC_Config.SingleDiff = ADC_SINGLE_ENDED ;
    //配置ADC通道
    HAL_ADC_ConfigChannel(&ADC1_Handle, &ADC_Config);    

    //使用通道19
    ADC_Config.Channel = RHEOSTAT_ADC_CHANNEL2;
    //转换顺序为2
    ADC_Config.Rank = ADC_REGULAR_RANK_2;
    //配置ADC通道
    HAL_ADC_ConfigChannel(&ADC1_Handle, &ADC_Config);
    
    //使用通道3
    ADC_Config.Channel = RHEOSTAT_ADC_CHANNEL3;
    //转换顺序为1
    ADC_Config.Rank = ADC_REGULAR_RANK_3;
    //配置ADC通道
    HAL_ADC_ConfigChannel(&ADC1_Handle, &ADC_Config); 

    //使用通道7
    ADC_Config.Channel = RHEOSTAT_ADC_CHANNEL4;
    //转换顺序为1
    ADC_Config.Rank = ADC_REGULAR_RANK_4;
    //配置ADC通道
    HAL_ADC_ConfigChannel(&ADC1_Handle, &ADC_Config);
    
    //使能ADC1
    ADC_Enable(&ADC1_Handle);
    
    HAL_ADC_Start_DMA(&ADC1_Handle, (uint32_t *)ADC_ConvertedValue, 4);
    
}
/**
  * @brief  ADC中断优先级配置函数
  * @param  无
  * @retval 无
  */  
void Rheostat_ADC_NVIC_Config(void)
{
    HAL_NVIC_SetPriority(Rheostat_ADC12_IRQ, 0, 0);
    HAL_NVIC_EnableIRQ(Rheostat_ADC12_IRQ);
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
  
    HAL_ADC_Start(&ADC1_Handle);
}

/*********************************************END OF FILE**********************/


