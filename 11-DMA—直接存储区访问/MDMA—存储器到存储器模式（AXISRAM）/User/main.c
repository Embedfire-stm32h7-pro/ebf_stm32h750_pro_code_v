/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   DMA实验
  ******************************************************************
  * @attention
  *
  * 实验平台:野火 STM32H750 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************
  */  
#include "stm32h7xx.h"
#include "main.h"
#include "./led/bsp_led.h" 
#include "./delay/core_delay.h"   
/* 相关宏定义，使用存储器到存储器传输必须使用DMA2 */
MDMA_HandleTypeDef     MDMA_Handle;

#define MDMA_INSTANCE            MDMA_Channel0
#define DMA_STREAM_CLOCK()       __HAL_RCC_MDMA_CLK_ENABLE();

#define BUFFER_SIZE              32


__IO uint32_t TransferCompleted = 0;

/* 定义aSRC_Const_Buffer数组作为DMA传输数据源
  const关键字将aSRC_Const_Buffer数组变量定义为常量类型 */
static const uint32_t SRC_Const_Buffer[BUFFER_SIZE] =
{
  0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10,
  0x11121314, 0x15161718, 0x191A1B1C, 0x1D1E1F20,
  0x21222324, 0x25262728, 0x292A2B2C, 0x2D2E2F30,
  0x31323334, 0x35363738, 0x393A3B3C, 0x3D3E3F40,
  0x41424344, 0x45464748, 0x494A4B4C, 0x4D4E4F50,
  0x51525354, 0x55565758, 0x595A5B5C, 0x5D5E5F60,
  0x61626364, 0x65666768, 0x696A6B6C, 0x6D6E6F70,
  0x71727374, 0x75767778, 0x797A7B7C, 0x7D7E7F80
};

/* 定义DMA传输目标存储器AXISRAM */
__attribute__((at(0x24004000))) uint32_t DESTBuffer[BUFFER_SIZE];
                                                                       
static void MDMA_Config(void);                                    
static void Delay(__IO uint32_t nCount);
static void SystemClock_Config(void); 
uint8_t Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength); 
static void MDMA_TransferCompleteCallback(MDMA_HandleTypeDef *hmdma);         
        

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	/* 定义存放比较结果变量 */
	uint8_t TransferStatus;
	/* 系统时钟初始化成480 MHz */
	SystemClock_Config();
  
    /* 默认不配置 MPU，若需要更高性能，当配置 MPU 后，使用 
   DMA 时需注意 Cache 与 内存内容一致性的问题，
   具体注意事项请参考配套教程的 MPU 配置相关章节 */
//  Board_MPU_Config(0, MPU_Normal_WT, 0xD0000000, MPU_32MB);
//  Board_MPU_Config(1, MPU_Normal_WT, 0x24000000, MPU_512KB);
  
  SCB_EnableICache();    // 使能指令 Cache
//  SCB_EnableDCache();    // 使能数据 Cache
  
	/* LED 端口初始化 */
	LED_GPIO_Config();
	/* 设置RGB彩色灯为紫色 */
	LED_PURPLE;  

	/* 简单延时函数 */
	Delay(0xFFFFFF);  

	/* DMA传输配置 */
	MDMA_Config(); 
  
	/* 等待DMA传输完成 */
	while(TransferCompleted == 0)
	{

	}   

	/* 比较源数据与传输后数据 */
	TransferStatus=Buffercmp(SRC_Const_Buffer, DESTBuffer, BUFFER_SIZE);

	/* 判断源数据与传输后数据比较结果*/
	if(TransferStatus==0)  
	{
		/* 源数据与传输后数据不相等时RGB彩色灯显示红色 */
		LED_RED;
	}
	else
	{ 
		/* 源数据与传输后数据相等时RGB彩色灯显示蓝色 */
		LED_BLUE;
	}

	while (1)
	{		
	}
}

/* 简单的延时函数 */
static void Delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}

/**
  * DMA传输配置
  */
static void MDMA_Config(void)
{
	HAL_StatusTypeDef DMA_status = HAL_ERROR; 
	/* 使能MDMA时钟 */
	DMA_STREAM_CLOCK();
  //MDMA外设寄存器基地址——通道0
	MDMA_Handle.Instance = MDMA_INSTANCE;
  //软件触发MDMA采集
  MDMA_Handle.Init.Request              = MDMA_REQUEST_SW;
  
  MDMA_Handle.Init.TransferTriggerMode  = MDMA_FULL_TRANSFER;
  //MDMA的软件优先级：低   
  MDMA_Handle.Init.Priority             = MDMA_PRIORITY_HIGH;
  
  MDMA_Handle.Init.Endianness           = MDMA_LITTLE_ENDIANNESS_PRESERVE;
  //源地址地址自增，长度为字
  MDMA_Handle.Init.SourceInc            = MDMA_SRC_INC_WORD;
  //目标地址地址自增，长度为字节
  MDMA_Handle.Init.DestinationInc       = MDMA_DEST_INC_BYTE;
  //源地址数据宽度：字  
  MDMA_Handle.Init.SourceDataSize       = MDMA_SRC_DATASIZE_WORD;
  //目标地址数据宽度：字节
  MDMA_Handle.Init.DestDataSize         = MDMA_DEST_DATASIZE_BYTE;
  //数据对齐格式：右对齐（小端模式）
  MDMA_Handle.Init.DataAlignment        = MDMA_DATAALIGN_PACKENABLE;  
  //源地址地址不自增  
  MDMA_Handle.Init.SourceBurst          = MDMA_SOURCE_BURST_SINGLE;
  //目标地址地址不自增  
  MDMA_Handle.Init.DestBurst            = MDMA_DEST_BURST_SINGLE;
  
  MDMA_Handle.Init.BufferTransferLength = 4;
  MDMA_Handle.Init.SourceBlockAddressOffset  = 0;
  MDMA_Handle.Init.DestBlockAddressOffset    = 0;

  
  HAL_MDMA_Init(&MDMA_Handle);
  
  HAL_MDMA_RegisterCallback(&MDMA_Handle, HAL_MDMA_XFER_CPLT_CB_ID, MDMA_TransferCompleteCallback);

  /* Set Interrupt Group Priority */
  HAL_NVIC_SetPriority(MDMA_IRQn, 0, 0);
  
  /* Enable the MDMA channel global Interrupt */
  HAL_NVIC_EnableIRQ(MDMA_IRQn);
  

  DMA_status = HAL_MDMA_Start_IT(&MDMA_Handle, (uint32_t)&SRC_Const_Buffer, 
                                 (uint32_t)&DESTBuffer, 
                                 (BUFFER_SIZE * 4), 
                                 1);	/* 判断DMA状态 */
	if (DMA_status != HAL_OK)
	{
		/* DMA出错就让程序运行下面循环：RGB彩色灯闪烁 */
		while (1)
		{      
			LED_RED;
			Delay(0xFFFFFF);
			LED_RGBOFF;
			Delay(0xFFFFFF);
		}
	} 
}

/**
  * 判断指定长度的两个数据源是否完全相等，
  * 如果完全相等返回1，只要其中一对数据不相等返回0
  */
uint8_t Buffercmp(const uint32_t* pBuffer, 
                  uint32_t* pBuffer1, uint16_t BufferLength)
{
	/* 数据长度递减 */
	while(BufferLength--)
	{
		/* 判断两个数据源是否对应相等 */
		if(*pBuffer != *pBuffer1)
		{
		  /* 对应数据源不相等马上退出函数，并返回0 */
		  return 0;
		}
		/* 递增两个数据源的地址指针 */
		pBuffer++;
		pBuffer1++;
	}
	/* 完成判断并且对应数据相对 */
	return 1;  
}
/**
  * @brief  MDMA conversion complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
static void MDMA_TransferCompleteCallback(MDMA_HandleTypeDef *hmdma)
{
  TransferCompleted = 1;
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
