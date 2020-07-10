/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   双重ADC 单通道 交替采集
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
#include "main.h"
#include "./usart/bsp_debug_usart.h"
#include "./adc/bsp_adc.h"


extern __IO uint32_t ADC_ConvertedValue;
__IO uint16_t ADC_ConvertedValueLocal[2];
float ADC_vol[2];
//简单的延时函数
static void Delay(__IO uint32_t nCount)	 
{
	for(; nCount != 0; nCount--);
}
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
  
	/* 系统时钟初始化成480MHz */
	SystemClock_Config();
  
  /* 默认不配置 MPU，若需要更高性能，当配置 MPU 后，使用 
   DMA 时需注意 Cache 与 内存内容一致性的问题，
   具体注意事项请参考配套教程的 MPU 配置相关章节 */
//  Board_MPU_Config(0, MPU_Normal_WT, 0xD0000000, MPU_32MB);
//  Board_MPU_Config(1, MPU_Normal_WT, 0x24000000, MPU_512KB);
  
  SCB_EnableICache();    // 使能指令 Cache
//  SCB_EnableDCache();    // 使能数据 Cache

	/* 配置串口1为：115200 8-N-1 */
	DEBUG_USART_Config();
  
  /* ADC初始化子程序 */ 
  ADC_Init();
  
  while(1)
	{	
      HAL_Delay(100);
    //双ADC交替采样：ADC_MASTER的采样值存放在低16位；
    //              ADC_SLAVE的采样值存放在高16位；
    ADC_ConvertedValueLocal[0] = (uint16_t)ADC_ConvertedValue;
    ADC_ConvertedValueLocal[1] = (uint16_t)((ADC_ConvertedValue&0xFFFF0000)>>16);
    
    ADC_vol[0] =(float)((uint16_t)ADC_ConvertedValueLocal[0]*3.3/65536); 
    ADC_vol[1] =(float)((uint16_t)ADC_ConvertedValueLocal[1]*3.3/65536);    
    
    printf("\r\n The current AD value = 0x%08X \r\n", ADC_ConvertedValueLocal[0]); 
    printf("\r\n The current AD value = 0x%08X \r\n", ADC_ConvertedValueLocal[1]);     
    //读取转换的AD值
    printf("\r\n The current ADC1 value = %f V \r\n",ADC_vol[0]); 
    printf("\r\n The current ADC2 value = %f V \r\n",ADC_vol[1]); 

	}  
}

/**
  * @brief  System Clock 配置
  *         system Clock 配置如下: 
	*            System Clock source  = PLL (HSE)
	*            SYSCLK(Hz)           = 480000000 (CPU Clock)
	*            HCLK(Hz)             = 240000000 (AXI and AHBs Clock)
	*            AHB Prescaler        = 2
	*            D1 APB3 Prescaler    = 2 (APB3 Clock  120MHz)
	*            D2 APB1 Prescaler    = 2 (APB1 Clock  120MHz)
	*            D2 APB2 Prescaler    = 2 (APB2 Clock  120MHz)
	*            D3 APB4 Prescaler    = 2 (APB4 Clock  120MHz)
	*            HSE Frequency(Hz)    = 25000000
	*            PLL_M                = 5
	*            PLL_N                = 192
	*            PLL_P                = 2
	*            PLL_Q                = 4
	*            PLL_R                = 2
	*            VDD(V)               = 3.3
	*            Flash Latency(WS)    = 4
  * @param  None
  * @retval None
  */
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** 启用电源配置更新
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** 配置主内稳压器输出电压
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** 初始化CPU、AHB和APB总线时钟
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
		while(1);
  }
  /** 初始化CPU、AHB和APB总线时钟
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
		while(1);
  }
}
/****************************END OF FILE***************************/
