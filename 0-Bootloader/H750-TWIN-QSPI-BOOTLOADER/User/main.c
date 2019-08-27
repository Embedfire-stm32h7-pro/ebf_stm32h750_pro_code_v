/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   USART—USART1接发例程
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
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"
#include "./flash/bsp_qspi_flash.h" 
#include "./sdram/bsp_sdram.h"  

typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;
/* 获取缓冲区的长度 */
#define TxBufferSize1   (countof(TxBuffer1) - 1)
#define RxBufferSize1   (countof(TxBuffer1) - 1)
#define countof(a)      (sizeof(a) / sizeof(*(a)))
#define  BufferSize (countof(Tx_Buffer)-1)

#define  FLASH_WriteAddress     0x2000
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress

   
/* 发送缓冲区初始化 */
uint8_t Tx_Buffer[256] = "感谢您选用野火stm32开发板\r\nhttp://firestm32.taobao.com";
uint8_t Rx_Buffer[BufferSize];

//读取的ID存储位置
__IO uint32_t DeviceID = 0;
__IO uint32_t FlashID = 0;
__IO TestStatus TransferStatus1 = FAILED;

// 函数原型声明
//void Delay(__IO uint32_t nCount);
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
extern __IO uint8_t QSPIStatusReady;
extern __IO uint8_t QSPITxOK;
extern __IO uint8_t QSPIRxOK;
extern __IO uint8_t QSPICmdOK;

/* Private typedef -----------------------------------------------------------*/
typedef  void (*pFunction)(void);
pFunction JumpToApplication;

/* QSPI is used to emulate SPI-NOR*/
#define APPLICATION_ADDRESS            QSPI_BASE
/**
  * @brief  使能CPU L1-Cache.
  * @param  无
  * @retval 无
  */
static void CPU_CACHE_Enable(void)
{
  /* 使能I-Cache */
  SCB_EnableICache();

  /* 使能D-Cache */
  SCB_EnableDCache();
}

/**
  * @brief  禁用CPU L1-Cache.
  * @param  无
  * @retval 无
  */
static void CPU_CACHE_Disable(void)
{
  /* 禁止I-Cache */
  SCB_DisableICache();

  /* 禁止D-Cache */
  SCB_DisableDCache();
}

//#if (DATA_AREA == USE_EXTERNAL_SDRAM) || (CODE_AREA == USE_EXTERNAL_SDRAM)
static void MPU_Config (void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;

  /* 禁止MPU */
  HAL_MPU_Disable();

  /* 配置SDRAM的MPU */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0xD0000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_64MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER7;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.SubRegionDisable = 0x00;
//#if (DATA_AREA == USE_EXTERNAL_SDRAM)
//  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
//#else
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
//#endif
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* 使能MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}
//#endif
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{

	/* 使能CPU Cache */
	CPU_CACHE_Enable();
	/* STM32H7xx HAL库初始化：
		- 配置Systick以每1毫秒生成一个中断
		- 将NVIC组优先级设置为4
		- 全局MSP（MCU支持包）初始化 */
  HAL_Init();	
	/* 系统时钟初始化成480MHz */
	SystemClock_Config();
//	LED_GPIO_Config();
//#if (DATA_AREA == USE_EXTERNAL_SDRAM) || (CODE_AREA == USE_EXTERNAL_SDRAM)
	/* 配置MPU以允许非对齐访问 */
//	MPU_Config();
//#endif
	/* 配置串口1为：115200 8-N-1 */
	DEBUG_USART_Config();
	SDRAM_Init();
  //SDRAM_Test();

	printf("欢迎使用野火开发板 \r\n");
	printf("STM32H7启动引导程序运行中。。。\r\n");
	/* 32M串行flash W25Q256初始化 */
	QSPI_FLASH_Init();


  /* Disable CPU L1 cache before jumping to the QSPI code execution */
  CPU_CACHE_Disable();
  /* Disable Systick interrupt */
  SysTick->CTRL = 0;
  /* Initialize user application's Stack Pointer & Jump to user application */
  JumpToApplication = (pFunction) (*(__IO uint32_t*) (APPLICATION_ADDRESS + 4));
  __set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
  JumpToApplication();

  /* We should never get here as execution is now on user application */
  while(1)
  {
		
  }
}
/*
 * 函数名：Buffercmp
 * 描述  ：比较两个缓冲区中的数据是否相等
 * 输入  ：-pBuffer1     src缓冲区指针
 *         -pBuffer2     dst缓冲区指针
 *         -BufferLength 缓冲区长度
 * 输出  ：无
 * 返回  ：-PASSED pBuffer1 等于   pBuffer2
 *         -FAILED pBuffer1 不同于 pBuffer2
 */
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }
  return PASSED;
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
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;
  
  /*使能供电配置更新 */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /* 当器件的时钟频率低于最大系统频率时，电压调节可以优化功耗，
		 关于系统频率的电压调节值的更新可以参考产品数据手册。  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
 
  /* 启用HSE振荡器并使用HSE作为源激活PLL */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
	RCC_OscInitStruct.PLL.PLLFRACN = 0;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
 
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {

    while(1) { ; }
  }
  
	/* 选择PLL作为系统时钟源并配置总线时钟分频器 */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK  | \
																 RCC_CLOCKTYPE_HCLK    | \
																 RCC_CLOCKTYPE_D1PCLK1 | \
																 RCC_CLOCKTYPE_PCLK1   | \
                                 RCC_CLOCKTYPE_PCLK2   | \
																 RCC_CLOCKTYPE_D3PCLK1);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;  
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2; 
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2; 
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2; 
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
}

/****************************END OF FILE***************************/
