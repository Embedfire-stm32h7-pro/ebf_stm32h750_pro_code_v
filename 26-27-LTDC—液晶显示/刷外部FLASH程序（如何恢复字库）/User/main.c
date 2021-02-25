/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   用V1.5.0版本库建的工程模板
  ******************************************************************
  * @attention
  *
  * 实验平台:野火  STM32H750开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************
  */  
#include "stm32h7xx.h"
#include "main.h"
#include "./led/bsp_led.h" 
#include "./usart/bsp_debug_usart.h"
#include "./sd_card/bsp_sdio_sd.h"
#include "./key/bsp_key.h" 
#include "./delay/core_delay.h"  
/* FatFs includes component */
#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"
#include "./flash/bsp_qspi_flash.h"
#include "RES_MGR.h"
/**
  ******************************************************************************
  *                              定义变量
  ******************************************************************************
  */
char SDPath[4]; /* SD逻辑驱动器路径 */
extern FATFS sd_fs;	
FRESULT res_sd;                /* 文件操作结果 */
extern char src_dir[];
uint8_t BSP_QSPI_Erase_Chip(void);
extern FATFS flash_fs;
extern Diskio_drvTypeDef  SD_Driver;
/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();

  //将Cache设置write-through方式
  //SCB->CACR|=1<<2;
}
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
uint8_t state = QSPI_ERROR;
int main(void)
{
    HAL_Init();
    /* 系统时钟初始化成480MHz */
    SystemClock_Config();
  
    CPU_CACHE_Enable();
    LED_GPIO_Config();
    LED_BLUE;	
    /* 初始化USART1 配置模式为 115200 8-N-1 */
    DEBUG_USART_Config();	
  
    Key_GPIO_Config();
    QSPI_FLASH_Init();
    QSPI_Set_WP_High();
    /*写状态寄存器*/
    /*对flash解锁，一般用不上*/
    QSPI_FLASH_WriteStatusReg(1,0X00);
    QSPI_FLASH_WriteStatusReg(2,0X00);
    QSPI_FLASH_WriteStatusReg(3,0X60);
    printf("\r\nFlash Status Reg1 is 0x%02X,\r\n", QSPI_FLASH_ReadStatusReg(1));	
    printf("\r\nFlash Status Reg2 is 0x%02X,\r\n", QSPI_FLASH_ReadStatusReg(2));
    printf("\r\nFlash Status Reg3 is 0x%02X,\r\n", QSPI_FLASH_ReadStatusReg(3));
    QSPI_Set_WP_TO_QSPI_IO();  
//    //FMC_Bank1_R->BTCR[0] |= (1 << 24);
    //链接驱动器，创建盘符
    FATFS_LinkDriver(&SD_Driver, SDPath);
    //在外部SD卡挂载文件系统，文件系统挂载时会对SD卡初始化
    res_sd = f_mount(&sd_fs,"0:",1);  
   
    if(res_sd != FR_OK)
    {
      printf("f_mount ERROR!请给开发板插入SD卡然后重新复位开发板!");
      LED_RED;
      while(1);
    }    
      
    printf("\r\n 按一次KEY1开始烧写字库并复制文件到FLASH。 \r\n"); 
    printf("\r\n 注意该操作会把FLASH的原内容会被删除！！ \r\n"); 
    
    while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==0);
    while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==1); 
    printf("\r\n 正在进行整片擦除，时间很长，请耐心等候...\r\n"); 
    
   // BSP_QSPI_Erase_Chip();
//     for(int i = 16; i < 32; i++)
//     {    
//       state = BSP_QSPI_Erase_Block(i*16);
//       if(state != QSPI_OK)
//         printf("擦除Block失败\n");
//       else
//         printf("OK\n");
//     }
    
    /* 生成烧录目录信息文件 */
    Make_Catalog(src_dir,0);

    /* 烧录 目录信息至FLASH*/
    Burn_Catalog();
     /* 根据 目录 烧录内容至FLASH*/
    Burn_Content();
    /* 校验烧录的内容 */
    Check_Resource();    
    /* 操作完成，停机 */
    while(1)
    {
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
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
