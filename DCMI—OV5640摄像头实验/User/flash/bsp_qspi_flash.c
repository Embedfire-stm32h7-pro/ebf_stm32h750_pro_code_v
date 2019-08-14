 /**
  ******************************************************************************
  * @file    bsp_qspi_flash.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   qspi flash 底层应用函数bsp 
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火STM32 H743 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./flash/bsp_qspi_flash.h"

QSPI_HandleTypeDef QSPIHandle;
static uint8_t QSPI_Addr_Mode_Init(void);

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

#if USE_DUAL_ExtFLASH
	/* 使能 QSPI BANK2时钟 */
	QSPI_FLASH_BK2_IO0_CLK_ENABLE();
	QSPI_FLASH_BK2_IO1_CLK_ENABLE();
	QSPI_FLASH_BK2_IO2_CLK_ENABLE();
	QSPI_FLASH_BK2_IO3_CLK_ENABLE();
#endif
	
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

#if USE_DUAL_ExtFLASH
  /*!< 配置 QSPI_FLASH_BK2 引脚: IO0 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK2_IO0_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_BK2_IO0_AF;
	HAL_GPIO_Init(QSPI_FLASH_BK2_IO0_PORT, &GPIO_InitStruct);

	/*!< 配置 QSPI_FLASH_BK2 引脚: IO1 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK2_IO1_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_BK2_IO1_AF;
	HAL_GPIO_Init(QSPI_FLASH_BK2_IO1_PORT, &GPIO_InitStruct);

	/*!< 配置 QSPI_FLASH_BK2 引脚: IO2 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK2_IO2_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_BK2_IO2_AF;
	HAL_GPIO_Init(QSPI_FLASH_BK2_IO2_PORT, &GPIO_InitStruct);

	/*!< 配置 QSPI_FLASH_BK2 引脚: IO3 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK2_IO3_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_BK2_IO3_AF;
	HAL_GPIO_Init(QSPI_FLASH_BK2_IO3_PORT, &GPIO_InitStruct);
#endif
	
	/* QSPI_FLASH 模式配置 */
	QSPIHandle.Instance = QUADSPI;
	/*二分频，时钟为216/(1+1)=108MHz */
	QSPIHandle.Init.ClockPrescaler = 1;
	/*FIFO 阈值为 4 个字节*/
	QSPIHandle.Init.FifoThreshold = 32;
	/*采样移位半个周期*/
	QSPIHandle.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
#if USE_DUAL_ExtFLASH
	/*Flash大小为64M字节，2^26，所以取权值26-1=25*/
	QSPIHandle.Init.FlashSize = 25;
#else
  /*Flash大小为32M字节，2^25，所以取权值25-1=24*/
	QSPIHandle.Init.FlashSize = 24;
#endif
	/*片选高电平保持时间，至少50ns，对应周期数6*9.2ns =55.2ns*/
	QSPIHandle.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_6_CYCLE;
	/*时钟模式选择模式0，nCS为高电平（片选释放）时，CLK必须保持低电平*/
	QSPIHandle.Init.ClockMode = QSPI_CLOCK_MODE_0;
	/*根据硬件连接选择第一片Flash*/
	QSPIHandle.Init.FlashID = QSPI_FLASH_ID_1;
#if USE_DUAL_ExtFLASH
	/*使能双闪存模式*/
	QSPIHandle.Init.DualFlash= QSPI_DUALFLASH_ENABLE;
#endif
	HAL_QSPI_Init(&QSPIHandle);
	/*初始化QSPI接口*/
	BSP_QSPI_Init();
}

/**
  * @brief  配置QSPI为内存映射模式
  * @retval QSPI内存状态
  */
uint32_t QSPI_EnableMemoryMappedMode(void)
{
  QSPI_CommandTypeDef      s_command;
  QSPI_MemoryMappedTypeDef s_mem_mapped_cfg;

  /* 配置读指令 */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = QUAD_INOUT_FAST_READ_CMD_4BYTE;
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

  if (HAL_QSPI_MemoryMapped(&QSPIHandle, &s_command, &s_mem_mapped_cfg) != HAL_OK)
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
	uint8_t value = W25Q256JV_FSR_QE;
	
	/* QSPI存储器复位 */
	if (QSPI_ResetMemory() != QSPI_OK)
	{
		return QSPI_NOT_SUPPORTED;
	}
	
	/* 使能写操作 */
	if (QSPI_WriteEnable() != QSPI_OK)
	{
		return QSPI_ERROR;
	}	
	/* 设置四路使能的状态寄存器，使能四通道IO2和IO3引脚 */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = WRITE_STATUS_REG2_CMD;
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
	if (HAL_QSPI_Transmit(&QSPIHandle, &value, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}
	/* 自动轮询模式等待存储器就绪 */  
	if (QSPI_AutoPollingMemReady(W25Q256JV_SUBSECTOR_ERASE_MAX_TIME) != QSPI_OK)
	{
		return QSPI_ERROR;
	}
  
  /* 配置地址模式为 4 字节 */
  if (QSPI_Addr_Mode_Init() != QSPI_OK)   
  {
		return QSPI_ERROR;
	}
  
	return QSPI_OK;
}

/**
  * @brief  检查地址模式不是4字节地址，配置为4字节
  * @retval QSPI存储器状态
  */
static uint8_t QSPI_Addr_Mode_Init(void)
{
	QSPI_CommandTypeDef s_command;
	uint8_t reg;
	/* 初始化读取状态寄存器命令 */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = READ_STATUS_REG3_CMD;
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
	/* 接收数据 */
	if (HAL_QSPI_Receive(&QSPIHandle, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	} 
  
	/* 检查寄存器的值 */
	if((reg & W25Q256FV_FSR_4ByteAddrMode) == 1)    // 4字节模式
	{
    return QSPI_OK;
	}
  else    // 3字节模式
  {
  /* 配置进入 4 字节地址模式命令 */
  s_command.Instruction = Enter_4Byte_Addr_Mode_CMD;
  s_command.DataMode    = QSPI_DATA_NONE;

  /* 配置并发送命令 */
  if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }
  
  /* 自动轮询模式等待存储器就绪 */  
  if (QSPI_AutoPollingMemReady(W25Q256JV_SUBSECTOR_ERASE_MAX_TIME) != QSPI_OK)
  {
    return QSPI_ERROR;
  }
    
    return QSPI_OK;
  }
}

/**
  * @brief  从QSPI存储器中读取大量数据.
  * @param  pData: 指向要读取的数据的指针
  * @param  ReadAddr: 读取起始地址
  * @param  Size: 要读取的数据大小    
  * @retval QSPI存储器状态
  */
uint8_t BSP_QSPI_FastRead(uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
{
	QSPI_CommandTypeDef s_command;

	/* 初始化读命令 */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = QUAD_INOUT_FAST_READ_CMD_4BYTE;
	s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
	s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
	s_command.Address           = ReadAddr;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_4_LINES;
	s_command.DummyCycles       = 6;
	s_command.NbData            = Size;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	/* 配置命令 */
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* 接收数据 */
	if (HAL_QSPI_Receive(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

/**
  * @brief  从QSPI存储器中读取大量数据.
  * @note   改指令只能使用在50MHz一下，本配置下不好用
  * @param  pData: 指向要读取的数据的指针
  * @param  ReadAddr: 读取起始地址
  * @param  Size: 要读取的数据大小    
  * @retval QSPI存储器状态
  */
uint8_t BSP_QSPI_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
{
	QSPI_CommandTypeDef s_command;
	/* 初始化读命令 */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = READ_CMD_4BYTE;    //READ_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
	s_command.Address           = ReadAddr;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.DummyCycles       = 0;
	s_command.NbData            = Size;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	/* 配置命令 */
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* 接收数据 */
	if (HAL_QSPI_Receive(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

/**
  * @brief  将大量数据写入QSPI存储器
  * @param  pData: 指向要写入数据的指针
  * @param  WriteAddr: 写起始地址
  * @param  Size: 要写入的数据大小    
  * @retval QSPI存储器状态
  */
uint8_t BSP_QSPI_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size)
{
	QSPI_CommandTypeDef s_command;
	uint32_t end_addr, current_size, current_addr;
	/* 计算写入地址和页面末尾之间的大小 */
	current_addr = 0;

	while (current_addr <= WriteAddr)
	{
		current_addr += W25Q256JV_PAGE_SIZE;
	}
	current_size = current_addr - WriteAddr;

	/* 检查数据的大小是否小于页面中的剩余位置 */
	if (current_size > Size)
	{
		current_size = Size;
	}

	/* 初始化地址变量 */
	current_addr = WriteAddr;
	end_addr = WriteAddr + Size;

	/* 初始化程序命令 */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = EXT_QUAD_IN_FAST_PROG_CMD_4BYTE;
	s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_4_LINES;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	/* 逐页执行写入 */
	do
	{
		s_command.Address = current_addr;
		s_command.NbData  = current_size;

		/* 启用写操作 */
		if (QSPI_WriteEnable() != QSPI_OK)
		{
			return QSPI_ERROR;
		}

		/* 配置命令 */
		if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		{
			return QSPI_ERROR;
		}

		/* 传输数据 */
		if (HAL_QSPI_Transmit(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		{
			return QSPI_ERROR;
		}

		/* 配置自动轮询模式等待程序结束 */  
		if (QSPI_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
		{
			return QSPI_ERROR;
		}

		/* 更新下一页编程的地址和大小变量 */
		current_addr += current_size;
		pData += current_size;
		current_size = ((current_addr + W25Q256JV_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W25Q256JV_PAGE_SIZE;
	} while (current_addr < end_addr);
	return QSPI_OK;
}

/**
  * @brief  擦除QSPI存储器的指定扇区 
  * @param  SectorAddress: 需要擦除的扇区地址  
  * @retval QSPI存储器状态
  */
uint8_t BSP_QSPI_Erase_Sector(uint32_t SectorAddress)
{
	QSPI_CommandTypeDef s_command;
	/* 初始化擦除命令 */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = SECTOR_ERASE_CMD_4BYTE;
	s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
	s_command.Address           = SectorAddress;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_NONE;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	/* 启用写操作 */
	if (QSPI_WriteEnable() != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	/* 发送命令 */
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* 配置自动轮询模式等待擦除结束 */  
	if (QSPI_AutoPollingMemReady(W25Q256JV_SECTOR_ERASE_MAX_TIME) != QSPI_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

/**
  * @brief  擦除QSPI存储器的指定块，单块大小32k 
  * @param  BlockAddress: 需要擦除的块地址 
  * @retval QSPI存储器状态
  */
uint8_t BSP_QSPI_Erase_Block32(uint32_t BlockAddress)
{
	QSPI_CommandTypeDef s_command;
	/* 初始化擦除命令 */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = BLOCK32_ERASE_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
	s_command.Address           = BlockAddress;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_NONE;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	/* 启用写操作 */
	if (QSPI_WriteEnable() != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	/* 发送命令 */
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* 配置自动轮询模式等待擦除结束 */  
	if (QSPI_AutoPollingMemReady(W25Q256JV_SECTOR_ERASE_MAX_TIME) != QSPI_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

/**
  * @brief  擦除QSPI存储器的指定块，单块大小64k
  * @param  BlockAddress: 需要擦除的块地址
  * @retval QSPI存储器状态
  */
uint8_t BSP_QSPI_Erase_Block64(uint32_t BlockAddress)
{
	QSPI_CommandTypeDef s_command;
	/* 初始化擦除命令 */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = BLOCK64_ERASE_CMD_4BYTE;
	s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
	s_command.Address           = BlockAddress;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_NONE;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	/* 启用写操作 */
	if (QSPI_WriteEnable() != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	/* 发送命令 */
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* 配置自动轮询模式等待擦除结束 */  
	if (QSPI_AutoPollingMemReady(W25Q256JV_SECTOR_ERASE_MAX_TIME) != QSPI_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

/**
  * @brief  擦除整个QSPI存储器
  * @retval QSPI存储器状态
  */
uint8_t BSP_QSPI_Erase_Chip(void)
{
	QSPI_CommandTypeDef s_command;
	/* 初始化擦除命令 */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = CHIP_ERASE_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_NONE;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	/* 启用写操作 */
	if (QSPI_WriteEnable() != QSPI_OK)
	{
		return QSPI_ERROR;
	}
	/* 发送命令 */
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	} 
	/* 配置自动轮询模式等待擦除结束 */  
	if (QSPI_AutoPollingMemReady(W25Q256JV_BULK_ERASE_MAX_TIME) != QSPI_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

/**
  * @brief  读取QSPI存储器的当前状态
  * @retval QSPI存储器状态
  */
uint8_t BSP_QSPI_GetStatus(void)
{
	QSPI_CommandTypeDef s_command;
	uint8_t reg;
	/* 初始化读取状态寄存器命令 */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = READ_STATUS_REG1_CMD;
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
	/* 接收数据 */
	if (HAL_QSPI_Receive(&QSPIHandle, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	} 
	/* 检查寄存器的值 */
	if((reg & W25Q256JV_FSR_BUSY) != 0)
	{
		return QSPI_BUSY;
	}
	else
	{
		return QSPI_OK;
	}
}

/**
  * @brief  返回QSPI存储器的配置
  * @param  pInfo: 在配置结构上的指针  
  * @retval QSPI存储器状态
  */
uint8_t BSP_QSPI_GetInfo(QSPI_Info* pInfo)
{
	/* 配置存储器配置结构 */
	pInfo->FlashSize          = W25Q256JV_FLASH_SIZE;
	pInfo->EraseSectorSize    = W25Q256JV_SECTOR_SIZE;
	pInfo->EraseSectorsNumber = (W25Q256JV_FLASH_SIZE/W25Q256JV_SECTOR_SIZE);
	pInfo->ProgPageSize       = W25Q256JV_PAGE_SIZE;
	pInfo->ProgPagesNumber    = (W25Q256JV_FLASH_SIZE/W25Q256JV_PAGE_SIZE);
	return QSPI_OK;
}

/**
  * @brief  复位QSPI存储器。
  * @param  QSPIHandle: QSPI句柄
  * @retval 无
  */
static uint8_t QSPI_ResetMemory()
{
	QSPI_CommandTypeDef s_command;
	/* 初始化复位使能命令 */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = RESET_ENABLE_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_NONE;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	/* 发送命令 */
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* 发送复位存储器命令 */
	s_command.Instruction = RESET_MEMORY_CMD;
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* 配置自动轮询模式等待存储器就绪 */  
	if (QSPI_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
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
	QSPI_CommandTypeDef     s_command;
	QSPI_AutoPollingTypeDef s_config;
	/* 启用写操作 */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = WRITE_ENABLE_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_NONE;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* 配置自动轮询模式等待写启用 */  
	s_config.Match           = W25Q256JV_FSR_WREN;
	s_config.Mask            = W25Q256JV_FSR_WREN;
	s_config.MatchMode       = QSPI_MATCH_MODE_AND;
	s_config.StatusBytesSize = 1;
	s_config.Interval        = 0x10;
	s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

	s_command.Instruction    = READ_STATUS_REG1_CMD;
	s_command.DataMode       = QSPI_DATA_1_LINE;
	s_command.NbData         = 1;

	if (HAL_QSPI_AutoPolling(&QSPIHandle, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

/**
  * @brief  读取存储器的SR并等待EOP
  * @param  QSPIHandle: QSPI句柄
  * @param  Timeout 超时
  * @retval 无
  */
static uint8_t QSPI_AutoPollingMemReady(uint32_t Timeout)
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
	s_config.StatusBytesSize = 1;
	s_config.Interval        = 0x10;
	s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

	if (HAL_QSPI_AutoPolling(&QSPIHandle, &s_command, &s_config, Timeout) != HAL_OK)
	{
		return QSPI_ERROR;
	}
	return QSPI_OK;
}

 /**
  * @brief  读取FLASH ID
  * @param 	无
  * @retval FLASH ID
  */
uint32_t QSPI_FLASH_ReadID(void)
{
	QSPI_CommandTypeDef s_command;
	uint32_t Temp = 0;
	uint8_t pData[3];
	/* 读取JEDEC ID */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = READ_JEDEC_ID_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DummyCycles       = 0;
	s_command.NbData            = 3;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* 用户可以在这里添加一些代码来处理这个错误 */
		while(1)
		{
			
		}
	}
	if (HAL_QSPI_Receive(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* 用户可以在这里添加一些代码来处理这个错误 */
		while(1)
		{
			
		}
	}

	Temp = ( pData[2] | pData[1]<<8 )| ( pData[0]<<16 );

	return Temp;
}

 /**
  * @brief  读取FLASH Device ID
  * @param 	无
  * @retval FLASH Device ID
  */
uint32_t QSPI_FLASH_ReadDeviceID(void)
{  
	QSPI_CommandTypeDef s_command;
	uint32_t Temp = 0;
	uint8_t pData[3];
	/*##-2-读取设备ID测试    ###########################################*/
	/* 读取制造/设备 ID */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = READ_ID_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
	s_command.Address           = 0x000000;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.DummyCycles       = 0;
	s_command.NbData            = 2;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* 用户可以在这里添加一些代码来处理这个错误 */
		while(1)
		{
			
		}
	}
	if (HAL_QSPI_Receive(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* 用户可以在这里添加一些代码来处理这个错误 */
		while(1)
		{
			
		}
	}

	Temp = pData[1] |( pData[0]<<8 ) ;

	return Temp;
}

/**
  * @brief  读取ReadStatusReg
  * @param 	无
  * @retval ReadStatusReg
  */
uint32_t QSPI_FLASH_ReadStatusReg(uint8_t reg)
{  
	QSPI_CommandTypeDef s_command;
	uint32_t Temp = 0;
	uint8_t pData[10];

	/* 读取制造/设备 ID */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	if(reg == 1)
	s_command.Instruction       = READ_STATUS_REG1_CMD;
	else if(reg == 2)
	s_command.Instruction       = READ_STATUS_REG2_CMD;
	else if(reg == 3)
	s_command.Instruction       = READ_STATUS_REG3_CMD;
	
	s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
	s_command.Address           = 0x000000;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.DummyCycles       = 0;
	s_command.NbData            = 1;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* 用户可以在这里添加一些代码来处理这个错误 */
		while(1)
		{
			
		}
	}
	if (HAL_QSPI_Receive(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* 用户可以在这里添加一些代码来处理这个错误 */
		while(1)
		{
			
		}
	}

	//单flash时读取的字节数为pData[0]，pData[1]
	//Temp = pData[0] |( pData[1]<<8 ) ;
	
	//双flash时读取的字节数为pData[0]，pData[2]
	//Temp = pData[1] |( pData[0]<<8 ) ;
		Temp = pData[0] ;
	return Temp;
}

 /**
  * @brief  读取ReadStatusReg
  * @param 	无
  * @retval ReadStatusReg
  */
uint32_t QSPI_FLASH_WriteStatusReg(uint8_t reg,uint8_t regvalue)
{  
	QSPI_CommandTypeDef s_command;
	
	/* 使能写操作 */
	if (QSPI_WriteEnable() != QSPI_OK)
	{
		return QSPI_ERROR;
	}	
	/* 读取制造/设备 ID */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	if(reg == 1)
	s_command.Instruction       = WRITE_STATUS_REG1_CMD;
	else if(reg == 2)
	s_command.Instruction       = WRITE_STATUS_REG2_CMD;
	else if(reg == 3)
	s_command.Instruction       = WRITE_STATUS_REG3_CMD;
	
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.AddressSize       = QSPI_ADDRESS_8_BITS;
	s_command.Address           = 0x000000;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.DummyCycles       = 0;
	s_command.NbData            = 1;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* 用户可以在这里添加一些代码来处理这个错误 */
		while(1)
		{
			
		}
	}
	if (HAL_QSPI_Transmit(&QSPIHandle, &regvalue, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		printf("something wrong ....\r\n");
		/* 用户可以在这里添加一些代码来处理这个错误 */
		while(1)
		{
			
		}
	}
	/* 自动轮询模式等待存储器就绪 */  
	if (QSPI_AutoPollingMemReady(W25Q256JV_SUBSECTOR_ERASE_MAX_TIME) != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

void QSPI_Set_WP_High(void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef  GPIO_InitStruct;
	__GPIOF_CLK_ENABLE();
	/*选择要控制的GPIO引脚*/															   
	GPIO_InitStruct.Pin = GPIO_PIN_7;	
	/*设置引脚的输出类型为推挽输出*/
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  
	/*设置引脚为上拉模式*/
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	/*设置引脚速率为高速 */   
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; 
	/*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);	
	
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_7,1);
}

void QSPI_Set_WP_TO_QSPI_IO(void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef  GPIO_InitStruct;
	QSPI_FLASH_BK1_IO2_CLK_ENABLE();

	//设置引脚
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	/*!< 配置 QSPI_FLASH 引脚: IO2 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK1_IO2_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_BK1_IO2_AF;
	HAL_GPIO_Init(QSPI_FLASH_BK1_IO2_PORT, &GPIO_InitStruct);
}

/*********************************************END OF FILE**********************/
