/**
  ******************************************************************
  * @file    bsp_adcd.c
  * @author  fire
  * @version V1.1
  * @date    2019xx-xx
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
ADC_HandleTypeDef ADC_SLAVE_Handle;
DMA_HandleTypeDef hdma_adc;
ADC_MultiModeTypeDef ADC_multimode;
__IO uint32_t ADC_ConvertedValue = 0;


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
    
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG; 
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Pin = RHEOSTAT_ADC_PIN; 
    /* 配置为模拟输入，不需要上拉电阻 */ 
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
  
    /* 使能ADC时钟 */
    RHEOSTAT_ADC_MASTER_CLK_ENABLE();
    /* 使能DMA时钟 */
    RHEOSTAT_ADC_DMA_CLK_ENABLE();
    /* 使能ADC_SLAVE时钟 */
    RHEOSTAT_ADC_SLAVE_CLK_ENABLE();
    
    //选择DMA1的Stream1
    hdma_adc.Instance = RHEOSTAT_ADC_DMA_Base;
    //ADC1的DMA请求
    hdma_adc.Init.Request = RHEOSTAT_ADC_DMA_Request;
    //传输方向：外设-》内存
    hdma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY;
    //外设地址不自增
    hdma_adc.Init.PeriphInc = DMA_PINC_DISABLE;
    //内存地址不自增
    hdma_adc.Init.MemInc = DMA_PINC_DISABLE;
    //外设数据宽度：字
    hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    //内存数据宽度：字
    hdma_adc.Init.MemDataAlignment = DMA_PDATAALIGN_WORD;
    //DMA循环传输
    hdma_adc.Init.Mode = DMA_CIRCULAR;
    //DMA的软件优先级：低
    hdma_adc.Init.Priority = DMA_PRIORITY_LOW;
    //FIFO模式关闭
    hdma_adc.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    //DMA初始化
    HAL_DMA_Init(&hdma_adc);
    //hdma_adc和ADC_Handle.DMA_Handle链接
    __HAL_LINKDMA(&ADC_Handle,DMA_Handle,hdma_adc);     
    
    ADC_Handle.Instance = RHEOSTAT_ADC_MASTER;

    //ADC时钟1分频
    ADC_Handle.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
    //使能连续转换模式
    ADC_Handle.Init.ContinuousConvMode = ENABLE;
    //数据存放在数据寄存器中
    ADC_Handle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
    //关闭不连续转换模式
    ADC_Handle.Init.DiscontinuousConvMode = DISABLE;
    //单次转换
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
    //初始化 ADC_MASTER
    HAL_ADC_Init(&ADC_Handle);
		
		//ADC校准
    HAL_ADCEx_Calibration_Start(&ADC_Handle,ADC_CALIB_OFFSET,ADC_SINGLE_ENDED); 
		
    //初始化 ADC_SLAVE
    ADC_SLAVE_Handle.Instance = RHEOSTAT_ADC_SLAVE;
    ADC_SLAVE_Handle.Init = ADC_Handle.Init;
    HAL_ADC_Init(&ADC_SLAVE_Handle);
		
		//ADC校准
    HAL_ADCEx_Calibration_Start(&ADC_SLAVE_Handle,ADC_CALIB_OFFSET,ADC_SINGLE_ENDED); 
                  
    //使用通道18
    ADC_Config.Channel = RHEOSTAT_ADC_MASTER_CHANNEL;
    //转换顺序为1
    ADC_Config.Rank = ADC_REGULAR_RANK_1;
    //采样周期为64.5个周期
    ADC_Config.SamplingTime = ADC_SAMPLETIME_64CYCLES_5;
    //不使用差分输入的功能
    ADC_Config.SingleDiff = ADC_SINGLE_ENDED ;
    //设置ADC选择的偏移量
    ADC_Config.OffsetNumber = ADC_OFFSET_NONE;
    
    //配置ADC_MASTER通道
    HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config); 
    
    ADC_Config.Channel =RHEOSTAT_ADC_SLAVE_CHANNEL; 
    //配置ADC_SLAVE通道
    HAL_ADC_ConfigChannel(&ADC_SLAVE_Handle, &ADC_Config);
    
    //使能ADC1、2
    ADC_Enable(&ADC_Handle);
    ADC_Enable(&ADC_SLAVE_Handle);
    
    //数据格式
    ADC_multimode.DualModeData = ADC_DUALMODEDATAFORMAT_32_10_BITS;
    //双重ADC交替模式
    ADC_multimode.Mode = ADC_DUALMODE_INTERL;
    //ADC_MASTER和ADC_SLAVE采样间隔3个ADC时钟
    ADC_multimode.TwoSamplingDelay = ADC_TWOSAMPLINGDELAY_3CYCLES;
    //ADC双重模式配置初始化
    HAL_ADCEx_MultiModeConfigChannel(&ADC_Handle, &ADC_multimode);
    //使能DMA
    HAL_ADCEx_MultiModeStart_DMA(&ADC_Handle, (uint32_t*)&ADC_ConvertedValue, 1);
    
}
/**
  * @brief  ADC中断优先级配置函数
  * @param  无
  * @retval 无
  */  
void Rheostat_ADC_NVIC_Config(void)
{
    HAL_NVIC_SetPriority(Rheostat_ADC12_IRQ, 1, 0);
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
    
    //开始ADC转换，软件触发
    HAL_ADC_Start(&ADC_Handle);
    HAL_ADC_Start(&ADC_SLAVE_Handle);
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


