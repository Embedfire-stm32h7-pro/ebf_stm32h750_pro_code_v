/**
  ******************************************************************************
  * @file    bsp_sdram.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   sdram应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 H750 开发板  
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./sdram/bsp_sdram.h"  
//static FMC_SDRAM_CommandTypeDef Command;
SDRAM_HandleTypeDef hsdram1;
#define sdramHandle hsdram1
/**
  * @brief  延迟一段时间
  * @param  延迟的时间长度
  * @retval None
  */
static void SDRAM_delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0; 
  for(index = (100000 * nCount); index != 0; index--)
  {
  }
}

/**
  * @brief  初始化控制SDRAM的IO
  * @param  无
  * @retval 无
  */
static void SDRAM_GPIO_Config(void)
{		
  //使能GPIO口时钟
  RCC->AHB4ENR |= 0x1FC; 

  //配置端口C、D、E、F、G、H、I相应的引脚为复用推挽输出
  GPIOC->MODER = 0xFFFFFFFE;
  GPIOC->OTYPER = 0;
  GPIOC->OSPEEDR = 0x00000003;
  GPIOC->PUPDR = 0x00000001;
  GPIOC->AFR[0] = 0x0000000C;
  
  GPIOD->MODER = 0xAFEAFFFA;
  GPIOD->OTYPER = 0;
  GPIOD->OSPEEDR = 0xF03F000F;
  GPIOD->PUPDR = 0x50150005;
  GPIOD->AFR[0] = 0x000000CC;  
  GPIOD->AFR[1] = 0xCC000CCC;

  GPIOE->MODER = 0xAAAABFFA;
  GPIOE->OTYPER = 0;
  GPIOE->OSPEEDR = 0xFFFFC00F;
  GPIOE->PUPDR = 0x55554005;
  GPIOE->AFR[0] = 0xC00000CC;  
  GPIOE->AFR[1] = 0xCCCCCCCC;
  
  GPIOF->MODER = 0xAABFFAAA;
  GPIOF->OTYPER = 0;
  GPIOF->OSPEEDR = 0xFFC00FFF;
  GPIOF->PUPDR = 0x55400555;
  GPIOF->AFR[0] = 0x00CCCCCC;
  GPIOF->AFR[1] = 0xCCCCC000;
  
  GPIOG->MODER = 0xBFFEFAEA;
  GPIOG->OTYPER = 0;
  GPIOG->OSPEEDR = 0xC0030F3F;
  GPIOG->PUPDR = 0x40010515;
  GPIOG->AFR[0] = 0x00CC0CCC;  
  GPIOG->AFR[1] = 0xC000000C;

  GPIOH->MODER = 0xAAAAAFFF;
  GPIOH->OTYPER = 0;
  GPIOH->OSPEEDR = 0xFFFFF000;
  GPIOH->PUPDR = 0x55555000;
  GPIOH->AFR[0] = 0xCC000000;  
  GPIOH->AFR[1] = 0xCCCCCCCC;  

  GPIOI->MODER = 0xFFEBAAAA;
  GPIOI->OTYPER = 0;
  GPIOI->OSPEEDR = 0x003CFFFF;
  GPIOI->PUPDR = 0x00145555;
  GPIOI->AFR[0] = 0xCCCCCCCC;  
  GPIOI->AFR[1] = 0x00000CC0;

}

/**
  * @brief  对SDRAM芯片进行初始化配置
  * @param  None. 
  * @retval None.
  */
static void SDRAM_InitSequence(void)
{


	/* Step 1 ----------------------------------------------------------------*/
	/* 配置命令：开启提供给SDRAM的时钟 */
  FMC_Bank5_6->SDCMR = 0x00000009;
	/* Step 2: 延时100us */ 
	SDRAM_delay(1);
	/* Step 3 ----------------------------------------------------------------*/
	/* 配置命令：对所有的bank预充电 */  
  FMC_Bank5_6->SDCMR = 0x0000000A;
	/* Step 4 ----------------------------------------------------------------*/
	/* 配置命令：自动刷新 */   
  FMC_Bank5_6->SDCMR = 0x000000EB;
	/* Step 5 ----------------------------------------------------------------*/
	/* 设置sdram寄存器配置 */
  FMC_Bank5_6->SDCMR = 0x0004600C;
	/* Step 6 ----------------------------------------------------------------*/
  /* 设置自刷新速率 */
  FMC_Bank5_6->SDRTR |= (1855<<1);
}


/**
  * @brief  初始化配置使用SDRAM的FMC及GPIO接口，
  *         本函数在SDRAM读写操作前需要被调用
  * @param  None
  * @retval None
  */
void SDRAM_Init(void)
{

  /* 配置FMC接口相关的 GPIO*/
  SDRAM_GPIO_Config();
  
  //使能HSE
  RCC->CR |= RCC_CR_HSEON;
  while(!(RCC->CR&(1<<17)));
  //选择HSE作为PLL时钟源
  RCC->PLLCKSELR |= (1 << 1);
  //使能PLL2R
  RCC->PLLCKSELR |= (25 << 12);//PLL2M
  RCC->PLLCFGR |= (1 << 21); 
  RCC->PLLCFGR &= ~(3 << 6);
  RCC->PLLCFGR &= ~(1 << 5);
  RCC->PLLCFGR &= ~(1 << 4);
  RCC->PLL2DIVR |= (265 << 0);//PLL2N
  RCC->PLL2DIVR |= (2 << 9);//PLL2P
  RCC->PLL2DIVR |= (2 << 16);//PLL2Q
  RCC->PLL2DIVR |= (2 << 24);//PLL2R

  //使能PLL2
  RCC->CR |= (1 << 26);
  while(!(RCC->CR&(1<<27)));
  RCC->D1CCIPR |= (2<<0); 
  
  (RCC->AHB3ENR |= (RCC_AHB3ENR_FMCEN));
  
  FMC_Bank5_6->SDCR[FMC_SDRAM_BANK1] = 0x00003AD0;
  FMC_Bank5_6->SDCR[FMC_SDRAM_BANK2] = 0x000001E9;
  
  FMC_Bank5_6->SDTR[FMC_SDRAM_BANK1] = 0x0F1F7FFF;
  FMC_Bank5_6->SDTR[FMC_SDRAM_BANK2] = 0x01010471;
  
  __FMC_ENABLE();                      
  /* FMC SDRAM 设备时序初始化 */
  SDRAM_InitSequence(); 
  
}



/**
  * @brief  以“字”为单位向sdram写入数据 
  * @param  pBuffer: 指向数据的指针 
  * @param  uwWriteAddress: 要写入的SDRAM内部地址
  * @param  uwBufferSize: 要写入数据大小
  * @retval None.
  */
void SDRAM_WriteBuffer(uint32_t* pBuffer, uint32_t uwWriteAddress, uint32_t uwBufferSize)
{
  __IO uint32_t write_pointer = (uint32_t)uwWriteAddress;

  /* 禁止写保护 */
  //HAL_SDRAM_WriteProtection_Disable(&hsdram1);
  FMC_Bank5_6->SDCR[1] &= ~FMC_SDRAM_WRITE_PROTECTION_ENABLE;
  /* 检查SDRAM标志，等待至SDRAM空闲 */ 
//  while(HAL_SDRAM_GetState(&hsdram1) != RESET)
//  {
//  }

  /* 循环写入数据 */
  for (; uwBufferSize != 0; uwBufferSize--) 
  {
    /* 发送数据到SDRAM */
    *(uint32_t *) (SDRAM_BANK_ADDR + write_pointer) = *pBuffer++;

    /* 地址自增*/
    write_pointer += 4;
  }
    
}

/**
  * @brief  从SDRAM中读取数据 
  * @param  pBuffer: 指向存储数据的buffer
  * @param  ReadAddress: 要读取数据的地十
  * @param  uwBufferSize: 要读取的数据大小
  * @retval None.
  */
void SDRAM_ReadBuffer(uint32_t* pBuffer, uint32_t uwReadAddress, uint32_t uwBufferSize)
{
  __IO uint32_t write_pointer = (uint32_t)uwReadAddress;
  
   
  /* 检查SDRAM标志，等待至SDRAM空闲 */  
//  while ( HAL_SDRAM_GetState(&hsdram1) != RESET)
//  {
//  }
  
  /*读取数据 */
  for(; uwBufferSize != 0x00; uwBufferSize--)
  {
   *pBuffer++ = *(__IO uint32_t *)(SDRAM_BANK_ADDR + write_pointer );
    
   /* 地址自增*/
    write_pointer += 4;
  } 
}


/**
  * @brief  测试SDRAM是否正常 
  * @param  None
  * @retval 正常返回1，异常返回0
  */
uint8_t SDRAM_Test(void)
{
  /*写入数据计数器*/
  uint32_t counter=0;
  
  /* 8位的数据 */
  uint8_t ubWritedata_8b = 0, ubReaddata_8b = 0;  
  
  /* 16位的数据 */
  uint16_t uhWritedata_16b = 0, uhReaddata_16b = 0; 
  
  SDRAM_INFO("正在检测SDRAM，以8位、16位的方式读写sdram...");


  /*按8位格式读写数据，并校验*/
  
  /* 把SDRAM数据全部重置为0 ，SDRAM_SIZE是以8位为单位的 */
  for (counter = 0x00; counter < SDRAM_SIZE; counter++)
  {
    *(__IO uint8_t*) (SDRAM_BANK_ADDR + counter) = (uint8_t)0x0;
  }
  
  /* 向整个SDRAM写入数据  8位 */
  for (counter = 0; counter < SDRAM_SIZE; counter++)
  {
    *(__IO uint8_t*) (SDRAM_BANK_ADDR + counter) = (uint8_t)(ubWritedata_8b + counter);
  }
  
  /* 读取 SDRAM 数据并检测*/
  for(counter = 0; counter<SDRAM_SIZE;counter++ )
  {
    ubReaddata_8b = *(__IO uint8_t*)(SDRAM_BANK_ADDR + counter);  //从该地址读出数据
    
    if(ubReaddata_8b != (uint8_t)(ubWritedata_8b + counter))      //检测数据，若不相等，跳出函数,返回检测失败结果。
    {
      SDRAM_ERROR("8位数据读写错误！出错位置：%d",counter);
      return 0;
    }
  }
	
  
  /*按16位格式读写数据，并检测*/
  
  /* 把SDRAM数据全部重置为0 */
  for (counter = 0x00; counter < SDRAM_SIZE/2; counter++)
  {
    *(__IO uint16_t*) (SDRAM_BANK_ADDR + 2*counter) = (uint16_t)0x00;
  }
  
  /* 向整个SDRAM写入数据  16位 */
  for (counter = 0; counter < SDRAM_SIZE/2; counter++)
  {
    *(__IO uint16_t*) (SDRAM_BANK_ADDR + 2*counter) = (uint16_t)(uhWritedata_16b + counter);
  }
  
    /* 读取 SDRAM 数据并检测*/
  for(counter = 0; counter<SDRAM_SIZE/2;counter++ )
  {
    uhReaddata_16b = *(__IO uint16_t*)(SDRAM_BANK_ADDR + 2*counter);  //从该地址读出数据
    
    if(uhReaddata_16b != (uint16_t)(uhWritedata_16b + counter))      //检测数据，若不相等，跳出函数,返回检测失败结果。
    {
      SDRAM_ERROR("16位数据读写错误！出错位置：%d",counter);

      return 0;
    }
  }

  
  SDRAM_INFO("SDRAM读写测试正常！"); 
  /*检测正常，return 1 */
  return 1;
  

}


/*********************************************END OF FILE**********************/

