/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   RTC驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32H743 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32h7xx.h"
#include "./RTC/bsp_rtc.h"
#include "./usart/bsp_usart.h"
#include "./lcd/bsp_lcd.h"
#include "./led/bsp_led.h"
#include "./beep/bsp_beep.h"

RTC_HandleTypeDef Rtc_Handle;
__IO uint8_t Alarmflag =0;
/**
  * @brief  设置时间和日期
  * @param  无
  * @retval 无
  */
void RTC_TimeAndDate_Set(void)
{
  RTC_DateTypeDef  RTC_DateStructure;
  RTC_TimeTypeDef  RTC_TimeStructure;
	// 初始化时间
	RTC_TimeStructure.TimeFormat = RTC_H12_AMorPM;
	RTC_TimeStructure.Hours = HOURS;        
	RTC_TimeStructure.Minutes = MINUTES;      
	RTC_TimeStructure.Seconds = SECONDS;    
  RTC_TimeStructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	RTC_TimeStructure.StoreOperation = RTC_STOREOPERATION_RESET;
  HAL_RTC_SetTime(&Rtc_Handle,&RTC_TimeStructure, RTC_FORMAT_BIN);
    // 初始化日期	
	RTC_DateStructure.WeekDay = WEEKDAY;       
	RTC_DateStructure.Date = DATE;         
	RTC_DateStructure.Month = MONTH;         
	RTC_DateStructure.Year = YEAR;        
  HAL_RTC_SetDate(&Rtc_Handle,&RTC_DateStructure, RTC_FORMAT_BIN);
    
  HAL_RTCEx_BKUPWrite(&Rtc_Handle,RTC_BKP_DRX, RTC_BKP_DATA);
}

/**
  * @brief  显示时间和日期
  * @param  无
  * @retval 无
  */
void RTC_TimeAndDate_Show(void)
{
	uint8_t Rtctmp=0;
	char LCDTemp[100];
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	
	
	while(1)
	{
		// 获取日历
    HAL_RTC_GetTime(&Rtc_Handle, &RTC_TimeStructure, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&Rtc_Handle, &RTC_DateStructure, RTC_FORMAT_BIN);
		
		// 每秒打印一次
  	if(Rtctmp != RTC_TimeStructure.Seconds)
    {
			// 打印日期
			printf("The Date :  Y:20%0.2d - M:%0.2d - D:%0.2d - W:%0.2d\r\n", 
			RTC_DateStructure.Year,
			RTC_DateStructure.Month, 
			RTC_DateStructure.Date,
			RTC_DateStructure.WeekDay);

			//液晶显示日期
			//先把要显示的数据用sprintf函数转换为字符串，然后才能用液晶显示函数显示
			sprintf(LCDTemp,"The Date :  Y:20%0.2d - M:%0.2d - D:%0.2d - W:%0.2d", 
			RTC_DateStructure.Year,
			RTC_DateStructure.Month, 
			RTC_DateStructure.Date,
			RTC_DateStructure.WeekDay);

			LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_BLACK);
			LCD_DisplayStringLine_EN_CH(8,(uint8_t *)LCDTemp); 

			// 打印时间
			printf("The Time :  %0.2d:%0.2d:%0.2d \r\n\r\n", 
			RTC_TimeStructure.Hours, 
			RTC_TimeStructure.Minutes, 
			RTC_TimeStructure.Seconds);

			//液晶显示时间
			sprintf(LCDTemp,"The Time :  %0.2d:%0.2d:%0.2d", 
			RTC_TimeStructure.Hours, 
			RTC_TimeStructure.Minutes, 
			RTC_TimeStructure.Seconds);

			LCD_DisplayStringLine_EN_CH(10,(uint8_t *)LCDTemp);
					
//			(void)RTC->DR;

			if(Alarmflag)
			{
				Alarmflag++;
				if(Alarmflag < 30)
				{
					LED2_TOGGLE;
					BEEP_TOGGLE;
				}
				else
					Alarmflag = 30;
			}
    }
    Rtctmp = RTC_TimeStructure.Seconds;
	}	
}

/**
  * @brief  RTC配置：选择RTC时钟源，设置RTC_CLK的分频系数
  * @param  无
  * @retval 无
  */
void RTC_CLK_Config(void)
{  
	RCC_OscInitTypeDef        RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

	// 配置RTC外设
	Rtc_Handle.Instance = RTC;

	/*使能 PWR 时钟*/
	__HAL_RCC_RTC_ENABLE();
	/* PWR_CR:DBF置1，使能RTC、RTC备份寄存器和备份SRAM的访问 */
	HAL_PWR_EnableBkUpAccess();

	#if defined (RTC_CLOCK_SOURCE_LSI) 
	/* 使用LSI作为RTC时钟源会有误差 
	 * 默认选择LSE作为RTC的时钟源
	 */
	/* 初始化LSI */ 
	RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);
	/* 选择LSI做为RTC的时钟源 */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

	#elif defined (RTC_CLOCK_SOURCE_LSE)
	/* 初始化LSE */ 
	RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);
	/* 选择LSE做为RTC的时钟源 */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

	/* Configures the External Low Speed oscillator (LSE) drive capability */
	__HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_HIGH);  

	#endif /* RTC_CLOCK_SOURCE_LSI */

	/* 使能RTC时钟 */
	__HAL_RCC_RTC_ENABLE(); 

	/* 等待 RTC APB 寄存器同步 */
	HAL_RTC_WaitForSynchro(&Rtc_Handle);

	/*=====================初始化同步/异步预分频器的值======================*/
	/* 驱动日历的时钟ck_spare = LSE/[(255+1)*(127+1)] = 1HZ */

	/* 设置异步预分频器的值 */
	Rtc_Handle.Init.AsynchPrediv = ASYNCHPREDIV;
	/* 设置同步预分频器的值 */
	Rtc_Handle.Init.SynchPrediv  = SYNCHPREDIV;	
	Rtc_Handle.Init.HourFormat   = RTC_H12_AMorPM; 
	/* 用RTC_InitStructure的内容初始化RTC寄存器 */
	if (HAL_RTC_Init(&Rtc_Handle) != HAL_OK)
	{
		printf("\n\r RTC 时钟初始化失败 \r\n");
	}	
}

/*
 *    要使能 RTC 闹钟中断，需按照以下顺序操作：
 * 1. 配置 NVIC 中的 RTC_Alarm IRQ 通道并将其使能。
 * 2. 配置 RTC 以生成 RTC 闹钟（闹钟 A 或闹钟 B）。
 *
 *
*/
void RTC_AlarmSet(void)
{
	RTC_AlarmTypeDef  RTC_AlarmStructure;

	/* RTC 闹钟中断配置 */
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);
	/* EXTI 配置 */
	HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 0);
	/* 使能RTC闹钟中断 */
	HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);

	/* 设置闹钟时间 */
	RTC_AlarmStructure.Alarm = RTC_Alarm_X;
	RTC_AlarmStructure.AlarmTime.TimeFormat = RTC_H12_AMorPM;
	RTC_AlarmStructure.AlarmTime.Hours   = ALARM_HOURS;
	RTC_AlarmStructure.AlarmTime.Minutes = ALARM_MINUTES;
	RTC_AlarmStructure.AlarmTime.Seconds = ALARM_SECONDS;
	RTC_AlarmStructure.AlarmMask = ALARM_MASK;
	RTC_AlarmStructure.AlarmDateWeekDaySel = ALARM_DATE_WEEKDAY_SEL;
	RTC_AlarmStructure.AlarmDateWeekDay = ALARM_DATE_WEEKDAY;  

	HAL_RTC_SetAlarm_IT(&Rtc_Handle, &RTC_AlarmStructure, RTC_FORMAT_BIN); 
}

/**
  * @brief  Alarm callback
  * @param  hrtc : RTC handle
  * @retval None
  */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	/* 闹钟时间到，蜂鸣器标志位置1 */
	Alarmflag = 1;
}
/**********************************END OF FILE*************************************/
