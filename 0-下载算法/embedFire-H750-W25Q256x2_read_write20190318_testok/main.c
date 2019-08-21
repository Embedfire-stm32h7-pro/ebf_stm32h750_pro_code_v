/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   QSPI FLASH基本读写例程
  ******************************************************************
  * @attention
  *
  * 实验平台:野火 STM32H743开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************
  */  
#include "stm32h7xx.h"
#include "main.h"
#include "bsp_led.h"
#include "bsp_debug_usart.h"

typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;
/* 获取缓冲区的长度 */
#define TxBufferSize1   (countof(TxBuffer1) - 1)
#define RxBufferSize1   (countof(TxBuffer1) - 1)
#define countof(a)      (sizeof(a) / sizeof(*(a)))
#define  BufferSize (countof(Tx_Buffer)-1)

#define  FLASH_WriteAddress     0
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress

   
/* 发送缓冲区初始化 */
volatile uint8_t Tx_Buffer[256] = "感谢您选用野火stm32开发板\r\nhttp://firestm32.taobao.com";
volatile uint8_t Rx_Buffer[BufferSize];

//读取的ID存储位置
__IO uint32_t DeviceID = 0;
__IO uint32_t FlashID = 0;
__IO TestStatus TransferStatus1 = FAILED;

// 函数原型声明
void Delay(__IO uint32_t nCount);

extern uint32_t QSPI_WritePage(unsigned long adr, unsigned long sz, unsigned char *buf);
extern uint32_t QSPI_ReadPage(unsigned long adr, unsigned long sz, unsigned char *buf);
extern uint32_t QSPI_EraseSector(uint32_t adr);

TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	int i=0;
	/* 使能指令缓存 */
	SCB_EnableICache();
  /* 使能数据缓存 */
  SCB_EnableDCache();
	/* 系统时钟初始化成400MHz */
//	SystemClock_Config();
//	/* LED 端口初始化 */
//	LED_GPIO_Config();	
//	LED_BLUE;
	
	/* 配置串口1为：115200 8-N-1 */
	DEBUG_USART_Config();
  
	printf("\r\n这是一个64M串行flash(W25Q256)实验(QSPI驱动) \r\n");
	
	/* 64M串行flash W25Q256初始化 */
	QSPI_Init();

//	/* 获取 Flash Device ID */
//	DeviceID = QSPI_FLASH_ReadDeviceID();
//	
//	Delay( 200 );
//	
//	/* 获取 SPI Flash ID */
//	FlashID = QSPI_FLASH_ReadID();
//	
//	printf("\r\nFlashID is 0x%X,  Manufacturer Device ID is 0x%X\r\n", FlashID, DeviceID);
//	
//	/* 检验 SPI Flash ID */
//	if (FlashID == sFLASH_ID) 
	{	
			/* Flash进入四字节地址模式 */
//		QSPI_EnterFourBytesAddress();
//		printf("\r\n检测到QSPI FLASH W25Q256 !\r\n");
		/* 擦除将要写入的 QSPI FLASH 扇区，FLASH写入前要先擦除 */
//		for(i=0;i<32;i++)
//		BSP_QSPI_Erase_Block(i*0x1000);
//		FlashErase(FLASH_WriteAddress,NULL);	
  	QSPI_EraseSector((uint32_t)FLASH_WriteAddress);		
//		Delay(1000);
		for(i=0;i<256;i++)
		Rx_Buffer[i]=0;
//		QSPI_FLASH_Init();
		/* 将擦除后的数据读出来放到接收缓冲区中 */
		QSPI_ReadPage(FLASH_WriteAddress, BufferSize,Rx_Buffer);
		printf("\r\n擦除后数据为：\r\n");
		for(i=0;i<256;i++)
		printf("0x%x ", Rx_Buffer[i]);
//		QSPI_FLASH_Init();
		printf("\r\n写入的数据为：\r\n");
		for(i=0;i<256;i++)
		{	
			Tx_Buffer[i]=0xab;
			printf("0x%x ", Tx_Buffer[i]);
		}
		
		/* 将发送缓冲区的数据写到flash中 */
		QSPI_WritePage(FLASH_WriteAddress, BufferSize,Tx_Buffer);
//		FlashWrite(FLASH_WriteAddress,0,256,Tx_Buffer);
//		printf("\r\n写入的数据为：\r\n%s", Tx_Buffer);
//		QSPI_FLASH_Init();
		/* 将刚刚写入的数据读出来放到接收缓冲区中 */
		QSPI_ReadPage(FLASH_ReadAddress, BufferSize,Rx_Buffer);
//		printf("\r\n读出的数据为：\r\n%s", Rx_Buffer);
		printf("\r\n读出的数据为：\r\n");
		for(i=0;i<256;i++)
		printf("0x%x ", Rx_Buffer[i]);		
		/* 检查写入的数据与读出的数据是否相等 */
		TransferStatus1 = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize);
		
		if( PASSED == TransferStatus1 )
		{    
//			LED_GREEN;
			printf("\r\n64M串行flash(W25Q256)测试成功!\n\r");
		}
		else
		{        
//			LED_RED;
			printf("\r\n64M串行flash(W25Q256)测试失败!\n\r");
		}
	}// if (FlashID == sFLASH_ID)
//	else
//	{    
//		LED_RED;
//		printf("\r\n获取不到 W25Q256 ID!\n\r");
//	}
	
	while(1);  
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
void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}
/**
  * @brief  System Clock 配置
  *         system Clock 配置如下: 
	*            System Clock source  = PLL (HSE)
	*            SYSCLK(Hz)           = 400000000 (CPU Clock)
	*            HCLK(Hz)             = 200000000 (AXI and AHBs Clock)
	*            AHB Prescaler        = 2
	*            D1 APB3 Prescaler    = 2 (APB3 Clock  100MHz)
	*            D2 APB1 Prescaler    = 2 (APB1 Clock  100MHz)
	*            D2 APB2 Prescaler    = 2 (APB2 Clock  100MHz)
	*            D3 APB4 Prescaler    = 2 (APB4 Clock  100MHz)
	*            HSE Frequency(Hz)    = 25000000
	*            PLL_M                = 5
	*            PLL_N                = 160
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
  MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);

  /* 当器件的时钟频率低于最大系统频率时，电压调节可以优化功耗，
		 关于系统频率的电压调节值的更新可以参考产品数据手册。  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
 
  /* 启用HSE振荡器并使用HSE作为源激活PLL */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 160;
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
