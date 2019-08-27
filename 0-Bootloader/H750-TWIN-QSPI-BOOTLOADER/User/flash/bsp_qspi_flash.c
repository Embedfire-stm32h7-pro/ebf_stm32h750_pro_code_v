 /**
  ******************************************************************************
  * @file    bsp_qspi_flash.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   qspi flash 底层应用函数bsp 
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火STM32 H750 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./flash/bsp_qspi_flash.h"

QSPI_HandleTypeDef QSPIHandle;

/**
  * @brief  QSPI_FLASH引脚初始化
  * @param  无
  * @retval 无
  */  
void QSPI_FLASH_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* 使能 QSPI 及 GPIO 时钟 */
	QSPI_FLASH_CLK_ENABLE();
	QSPI_FLASH_CLK_GPIO_ENABLE();
	QSPI_FLASH_BK1_IO0_CLK_ENABLE();
	QSPI_FLASH_BK1_IO1_CLK_ENABLE();
	QSPI_FLASH_BK1_IO2_CLK_ENABLE();
	QSPI_FLASH_BK1_IO3_CLK_ENABLE();
	QSPI_FLASH_CS_GPIO_CLK_ENABLE();

	
	//设置引脚
	/*!< 配置 QSPI_FLASH 引脚: CLK */
	GPIO_InitStruct.Pin = QSPI_FLASH_CLK_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = QSPI_FLASH_CLK_GPIO_AF;

	HAL_GPIO_Init(QSPI_FLASH_CLK_GPIO_PORT, &GPIO_InitStruct);

	/*!< 配置 QSPI_FLASH 引脚: IO0 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK1_IO0_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_BK1_IO0_AF;
	HAL_GPIO_Init(QSPI_FLASH_BK1_IO0_PORT, &GPIO_InitStruct);

	/*!< 配置 QSPI_FLASH 引脚: IO1 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK1_IO1_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_BK1_IO1_AF;
	HAL_GPIO_Init(QSPI_FLASH_BK1_IO1_PORT, &GPIO_InitStruct);

	/*!< 配置 QSPI_FLASH 引脚: IO2 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK1_IO2_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_BK1_IO2_AF;
	HAL_GPIO_Init(QSPI_FLASH_BK1_IO2_PORT, &GPIO_InitStruct);

	/*!< 配置 QSPI_FLASH 引脚: IO3 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK1_IO3_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_BK1_IO3_AF;
	HAL_GPIO_Init(QSPI_FLASH_BK1_IO3_PORT, &GPIO_InitStruct);

	/*!< 配置 SPI_FLASH_SPI 引脚: NCS */
	GPIO_InitStruct.Pin = QSPI_FLASH_CS_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_CS_GPIO_AF;
	HAL_GPIO_Init(QSPI_FLASH_CS_GPIO_PORT, &GPIO_InitStruct);
	
	HAL_QSPI_DeInit(&QSPIHandle);
	/* QSPI_FLASH 模式配置 */
	QSPIHandle.Instance = QUADSPI;
	/*二分频，时钟为240/(1+1)=120MHz */
	QSPIHandle.Init.ClockPrescaler = 1;
	/*FIFO 阈值为 4 个字节*/
	QSPIHandle.Init.FifoThreshold = 1;
	/*采样移位半个周期*/
	QSPIHandle.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
	/*Flash大小32M字节，2^25，所以取权值25-1=24*/
	QSPIHandle.Init.FlashSize = 24;
	/*片选高电平保持时间，至少50ns，对应周期数6*9.2ns =55.2ns*/
	QSPIHandle.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_6_CYCLE;
	/*时钟模式选择模式0，nCS为高电平（片选释放）时，CLK必须保持低电平*/
	QSPIHandle.Init.ClockMode = QSPI_CLOCK_MODE_0;
	/*根据硬件连接选择第一片Flash*/
	QSPIHandle.Init.FlashID = QSPI_FLASH_ID_1;
  /*使能双闪存模式*/
	QSPIHandle.Init.DualFlash= QSPI_DUALFLASH_DISABLE;
	HAL_QSPI_Init(&QSPIHandle);
	/*初始化QSPI接口*/
	BSP_QSPI_Init();
}

/**
  * @brief  配置QSPI为内存映射模式
  * @retval QSPI内存状态
  */
static uint32_t QSPI_EnableMemoryMappedMode(QSPI_HandleTypeDef *hqspi)
{
  QSPI_CommandTypeDef      s_command;
  QSPI_MemoryMappedTypeDef s_mem_mapped_cfg;

  /* 配置读指令 */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = QUAD_INOUT_4BYTE_FAST_READ_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
  s_command.DataMode          = QSPI_DATA_4_LINES;
  s_command.DummyCycles       = 6;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* 配置内存映射模式 */
  s_mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
  s_mem_mapped_cfg.TimeOutPeriod     = 0;

  if (HAL_QSPI_MemoryMapped(hqspi, &s_command, &s_mem_mapped_cfg) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  return QSPI_OK;
}

/**
  * @brief  This function reset the QSPI memory.
  * @param  hqspi: QSPI handle
  * @retval None
  */
static uint32_t QSPI_ResetMemory(QSPI_HandleTypeDef *hqspi)
{
	QSPI_CommandTypeDef s_command;
	/* 使能写操作 */
	if (QSPI_WriteEnable() != QSPI_OK)
	{
		return QSPI_ERROR;
	}	
  /* 初始化复位使能命令 */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction = RESET_MEMORY_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* 发送复位flash命令 */
  if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* 轮询等待flash就绪 */
  if (QSPI_AutoPollingMemReady() != QSPI_OK)
  {
    return QSPI_ERROR;
  }
  return QSPI_OK;
}
/**
  * @brief  初始化QSPI存储器
  * @retval QSPI存储器状态
  */
uint8_t BSP_QSPI_Init(void)
{
	QSPI_CommandTypeDef s_command;
	uint8_t value = 0X06;
	/* 使能写操作 */
	if (QSPI_WriteEnable() != QSPI_OK)
	{
		return QSPI_ERROR;
	}	
	/* 设置四路使能的状态寄存器，使能四通道IO2和IO3引脚 */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = WRITE_STATUS_REG1_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.DummyCycles       = 0;
	s_command.NbData            = 1;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
	/* 配置命令 */
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}
	/* 传输数据 */
	if (HAL_QSPI_Transmit(&QSPIHandle, &value,HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}
	/* 自动轮询模式等待存储器就绪 */  
	if (QSPI_AutoPollingMemReady() != QSPI_OK)
	{
		return QSPI_ERROR;
	}
  /* QSPI memory reset */
  if (QSPI_ResetMemory(&QSPIHandle) != QSPI_OK)
  {
    return QSPI_ERROR;
  }
	QSPI_EnterFourBytesAddress();
	/* Enable MemoryMapped mode */
  if( QSPI_EnableMemoryMappedMode(&QSPIHandle) != QSPI_OK )
  {
    return QSPI_ERROR;
  }
	return QSPI_OK;
}

/**
  * @brief  设置QSPI存储器为4-byte地址模式
  * @param  无
  * @retval 返回状态
  */
uint8_t QSPI_EnterFourBytesAddress(void)
{
  QSPI_CommandTypeDef s_command;

  /* Initialize the command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = ENTER_4_BYTE_ADDR_MODE_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* 使能写操作 */
  QSPI_WriteEnable();
  
  /* 传输命令 */
  if (HAL_QSPI_Command(&QSPIHandle, &s_command,HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

	/* 自动轮询模式等待存储器就绪 */  
	if (QSPI_AutoPollingMemReady() != QSPI_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;

}
/**
  * @brief  发送写入使能，等待它有效.
  * @param  QSPIHandle: QSPI句柄
  * @retval 无
  */
static uint8_t QSPI_WriteEnable()
{
	QSPI_CommandTypeDef     sCommand;
	QSPI_AutoPollingTypeDef s_config;
	/* 启用写操作 */
	sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	sCommand.Instruction       = WRITE_ENABLE_CMD;
	sCommand.AddressMode       = QSPI_ADDRESS_NONE;
	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCommand.DataMode          = QSPI_DATA_NONE;
	sCommand.DummyCycles       = 0;
	sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
	sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
	if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* 配置自动轮询模式等待写启用 */  
	s_config.Match           = W25Q256JV_FSR_WREN;
	s_config.Mask            = W25Q256JV_FSR_WREN;
	s_config.MatchMode       = QSPI_MATCH_MODE_AND;
	s_config.StatusBytesSize = 2;
	s_config.Interval        = 0x10;
	s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

	sCommand.Instruction    = READ_STATUS_REG1_CMD;
	sCommand.DataMode       = QSPI_DATA_1_LINE;
	sCommand.NbData         = 1;

	if (HAL_QSPI_AutoPolling(&QSPIHandle, &sCommand, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

/**
  * @brief  读取存储器的SR并等待EOP
  * @param  无
  * @retval 无
  */
static uint8_t QSPI_AutoPollingMemReady(void)
{
	QSPI_CommandTypeDef     s_command;
	QSPI_AutoPollingTypeDef s_config;
	/* 配置自动轮询模式等待存储器准备就绪 */  
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = READ_STATUS_REG1_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	s_config.Match           = 0x00;
	s_config.Mask            = W25Q256JV_FSR_BUSY;
	s_config.MatchMode       = QSPI_MATCH_MODE_AND;
	s_config.StatusBytesSize = 2;
	s_config.Interval        = 0x10;
	s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

	if (HAL_QSPI_AutoPolling(&QSPIHandle, &s_command, &s_config,HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}
/*********************************************END OF FILE**********************/
