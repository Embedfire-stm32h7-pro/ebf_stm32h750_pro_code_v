/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   野火H743 PWR—停止模式
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 H743 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "main.h"  
#include "stm32h7xx.h"
#include "./led/bsp_led.h" 
#include "./usart/bsp_usart.h"
#include "./key/bsp_exti.h" 
#include "./delay/core_delay.h"  

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void) 
{
	uint32_t SYSCLK_Frequency=0; 
	uint32_t HCLK_Frequency=0;
	uint32_t PCLK1_Frequency=0;
	uint32_t PCLK2_Frequency=0;
	uint32_t SYSCLK_Source=0;
	
	/* 初始化系统时钟为480MHz */
	SystemClock_Config();
	/* 初始化LED */
	LED_GPIO_Config();	
	/* 初始化调试串口，一般为串口1 */
	UARTx_Config();	
	/* 初始化按键为中断模式，按下中断后会进入中断服务函数	*/
	EXTI_Key_Config(); 
	
	printf("\r\n 欢迎使用野火 STM32H743 开发板。\r\n");
    printf("\r\n 野火H743 停止模式例程\r\n");
	
	printf("\r\n 实验说明：\r\n");

	printf("\r\n 1.本程序中，绿灯表示STM32正常运行，红灯表示睡眠状态，蓝灯表示刚从停止状态被唤醒\r\n");
	printf("\r\n 2.程序运行一段时间后自动进入停止状态，在停止状态下，可使用KEY1或KEY2唤醒\r\n");
	printf("\r\n 3.本实验执行这样一个循环：\r\n ------》亮绿灯(正常运行)->亮红灯(停止模式)->按KEY1或KEY2唤醒->亮蓝灯(刚被唤醒)-----》\r\n");
	printf("\r\n 4.在停止状态下，DAP下载器无法给STM32下载程序，\r\n 可按KEY1、KEY2唤醒后下载，\r\n 或按复位键使芯片处于复位状态，然后在电脑上点击下载按钮，再释放复位按键，即可下载\r\n");

	while(1)
	{	
		/*********执行任务***************************/
		printf("\r\n STM32正常运行，亮绿灯\r\n");

		LED_GREEN;	
		HAL_Delay(2000);	
		
		/*****任务执行完毕，进入睡眠降低功耗***********/
		printf("\r\n 进入停止模式，亮红灯,按KEY1或KEY2按键可唤醒\r\n");
		//使用红灯指示，进入睡眠状态
		LED_RED;
		//暂停滴答时钟，防止通过滴答时钟中断唤醒
		HAL_SuspendTick();
		/*设置停止模式时，FLASH进入掉电状态*/
		HAL_PWREx_EnableFlashPowerDown();
		/* 进入停止模式，设置电压调节器为低功耗模式，等待中断唤醒 */
		HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON,PWR_STOPENTRY_WFI);
		//等待中断唤醒  K1或K2按键中断	
		/***被唤醒，亮蓝灯指示***/
		LED_BLUE;
		//根据时钟寄存器的值更新SystemCoreClock变量
		SystemCoreClockUpdate();
		//获取唤醒后的时钟状态	
		SYSCLK_Frequency = HAL_RCC_GetSysClockFreq(); 
		HCLK_Frequency   = HAL_RCC_GetHCLKFreq();
		PCLK1_Frequency  = HAL_RCC_GetPCLK1Freq();
		PCLK2_Frequency  = HAL_RCC_GetPCLK2Freq();
		SYSCLK_Source    = __HAL_RCC_GET_SYSCLK_SOURCE();
		//由于系统直接使用HSI时钟，影响串口波特率，需要重新初始化串口
		UARTx_HSI_Config();
		printf("\r\n刚唤醒的时钟状态：\r\n");
		printf(" SYSCLK频率:%d,\r\n HCLK频率:%d,\r\n PCLK1频率:%d,\r\n PCLK2频率:%d,\r\n 时钟源:%d (0表示HSI，8表示PLLCLK)\n", 
		SYSCLK_Frequency,HCLK_Frequency,PCLK1_Frequency,PCLK2_Frequency,SYSCLK_Source);		  
		
		/* 从停止模式唤醒后配置系统时钟:启用HSE、PLL*/
		/* 选择PLL作为系统时钟源(HSE和PLL在停止模式下被禁用)*/
		SYSCLKConfig_STOP();
		//被唤醒后，恢复滴答时钟			
		HAL_ResumeTick();
		//获取重新配置后的时钟状态
		SYSCLK_Frequency = HAL_RCC_GetSysClockFreq(); 
		HCLK_Frequency   = HAL_RCC_GetHCLKFreq();
		PCLK1_Frequency  = HAL_RCC_GetPCLK1Freq();
		PCLK2_Frequency  = HAL_RCC_GetPCLK2Freq();
		SYSCLK_Source    = __HAL_RCC_GET_SYSCLK_SOURCE();
		
		//重新配置时钟源后始终状态
		printf("\r\n重新配置后的时钟状态：\r\n");
		printf(" SYSCLK频率:%d,\r\n HCLK频率:%d,\r\n PCLK1频率:%d,\r\n PCLK2频率:%d,\r\n 时钟源:%d (0表示HSI，8表示PLLCLK)\n", 
		SYSCLK_Frequency,HCLK_Frequency,PCLK1_Frequency,PCLK2_Frequency,SYSCLK_Source);

		HAL_Delay(2000);	
			
		printf("\r\n 已退出停止模式\r\n");
		//继续执行while循环
	}
}

/**
  * @brief  System Clock 配置
  *         system Clock 配置如下: 
	*            System Clock source  = PLL (HSE)
	*            SYSCLK(Hz)           = 400000000 (CPU Clock)
	*            HCLK(Hz)             = 200000000 (AXI and AHBs Clock)
	*            AHB Prescaler        = 2
	*            D1 APB3 Prescaler    = 2 (APB3 Clock  120MHz)
	*            D2 APB1 Prescaler    = 2 (APB1 Clock  120MHz)
	*            D2 APB2 Prescaler    = 2 (APB2 Clock  120MHz)
	*            D3 APB4 Prescaler    = 2 (APB4 Clock  120MHz)
	*            HSE Frequency(Hz)    = 25000000
	*            PLL_M                = 5
	*            PLL_N                = 192
	*            PLL_P                = 2
	*            PLL_Q                = 2
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
  /** 初始化CPU、AHB和、APB总线时钟
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
  }
  /** 初始化CPU、AHB和、APB总线时钟
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
  }
}


/**
  * @brief  从停止模式唤醒后配置系统时钟:启用HSE、PLL并选择PLL作为系统时钟源。
  * @param  无
  * @retval 无
  */
static void SYSCLKConfig_STOP(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	uint32_t pFLatency = 0;
	

  /* 配置主内稳压器输出电压 */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);
	
  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
		
	/* 根据内部RCC寄存器获取振荡器配置 */
	HAL_RCC_GetOscConfig(&RCC_OscInitStruct);

	/* 从停止模式唤醒后重新配置系统时钟: 启用HSE和PLL */
	RCC_OscInitStruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState        = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState    = RCC_PLL_ON;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
  	while(1) { ; }
	}

	/* 根据内部RCC寄存器获取时钟配置 */
	HAL_RCC_GetClockConfig(&RCC_ClkInitStruct, &pFLatency);

	/* 选择 PLL 作为系统时钟源, 并配置 HCLK、PCLK1 和 PCLK2时钟分频系数 */
	RCC_ClkInitStruct.ClockType     = RCC_CLOCKTYPE_SYSCLK;
	RCC_ClkInitStruct.SYSCLKSource  = RCC_SYSCLKSOURCE_PLLCLK;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, pFLatency) != HAL_OK)
	{
	 while(1) { ; }
	}
}
/*********************************************END OF FILE**********************/

