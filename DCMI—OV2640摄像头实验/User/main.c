/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   LTDC—液晶显示中文
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
#include "./usart/bsp_usart.h"
#include "./sdram/bsp_sdram.h" 
#include "./lcd/bsp_lcd.h"
#include "string.h"
#include "./i2c/bsp_i2c.h"
#include "./camera/bsp_ov2640.h"

#include "./delay/core_delay.h"  

/*简单任务管理*/
#define TASK_ENABLE 0
#define NumOfTask 3

__IO uint8_t* qspi_addr = (__IO uint8_t*)(0x90000000);

extern uint32_t Task_Delay[NumOfTask];
uint32_t Task_Delay[NumOfTask]={0};
uint8_t dispBuf[100];
uint8_t fps=0;
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{  
  OV2640_IDTypeDef OV2640_Camera_ID; 		
	/* 系统时钟初始化成400MHz */
	SystemClock_Config();
	/* LED 端口初始化 */
	LED_GPIO_Config();
	/* 配置串口1为：115200 8-N-1 */
	UARTx_Config();	
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0x0E);
	printf("\r\n 欢迎使用野火  STM32 H750 开发板。\r\n");		 
	printf("\r\n野火STM32H750 OV2640摄像头测试例程\r\n");
	/*蓝灯亮，表示正在读写SDRAM测试*/
	//LED_BLUE;
	/* LCD 端口初始化 */ 
	LCD_Init();
	/* LCD 第一层初始化 */ 
	LCD_LayerInit(0, LCD_FB_START_ADDRESS,RGB565);
	/* LCD 第二层初始化 */ 
	LCD_LayerInit(1, LCD_FB_START_ADDRESS+(LCD_GetXSize()*LCD_GetYSize()*4),ARGB8888);
	/* 使能LCD，包括开背光 */ 
	LCD_DisplayOn(); 

	/* 选择LCD第一层 */
	LCD_SelectLayer(0);

	/* 第一层清屏，显示全蓝色 */ 
	LCD_Clear(LCD_COLOR_BLUE);  

	/* 选择LCD第二层 */
	LCD_SelectLayer(1);

	/* 第二层清屏，显示透明 */ 
	LCD_Clear(TRANSPARENCY);

	/* 配置第一和第二层的透明度,0位完全透明，255为完全不透明*/
	LCD_SetTransparency(0, 255);
	LCD_SetTransparency(1, 255);
	
	LCD_SetColors(LCD_COLOR_WHITE,TRANSPARENCY);
	LCD_DisplayStringLine_EN_CH(1,(uint8_t* )" 模式:UXGA 800x480");
	CAMERA_DEBUG("STM32H750 DCMI 驱动OV2640例程");
		
	//初始化 I2C
	I2CMaster_Init(); 
	HAL_Delay(100);
	OV2640_HW_Init();
	/* 读取摄像头芯片ID，确定摄像头正常连接 */
	OV2640_ReadID(&OV2640_Camera_ID);

	if(OV2640_Camera_ID.PIDH  == 0x26)
	{
		CAMERA_DEBUG("%x%x",OV2640_Camera_ID.PIDH ,OV2640_Camera_ID.PIDL);
	}
	else
	{
		LCD_SetColors(LCD_COLOR_WHITE,TRANSPARENCY);
		LCD_DisplayStringLine_EN_CH(8,(uint8_t*) "         没有检测到OV2640，请重新检查连接。");
		CAMERA_DEBUG("没有检测到OV2640摄像头，请重新检查连接。");
		while(1);  
	}
  /* 配置摄像头输出像素格式 */
	OV2640_UXGAConfig();
  /* 初始化摄像头，捕获并显示图像 */
	OV2640_Init();
	//重置
  fps =0;
	Task_Delay[0]=1000;
	
	while(1)
	{
		if(Task_Delay[0]==0)
		{
			LCD_SelectLayer(1);       
			LCD_SetColors(LCD_COLOR_WHITE,TRANSPARENCY);
			sprintf((char*)dispBuf, " 帧率:%d FPS", fps/1);
			LCD_ClearLine(2);
			/*输出帧率*/
			LCD_DisplayStringLine_EN_CH(2,dispBuf);		
      fps =0;
			Task_Delay[0]=1000; //此值每1ms会减1，减到0才可以重新进来这里
		}		
	}
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
  RCC_OscInitStruct.PLL.PLLQ = 2;
 
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
