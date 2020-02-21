/**
  ******************************************************************************
  * @file    bsp_i2c_ee.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   电容触摸屏的专用iic驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 H750 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./touch/bsp_i2c_touch.h"
#include "./touch/bsp_touch_gtxx.h"
#include "./usart/bsp_debug_usart.h"
#include "./delay/core_delay.h"   


/* STM32 I2C 快速模式 */
#define I2C_Speed              400000

/* 这个地址只要与STM32外挂的I2C器件地址不一样即可 */
#define I2C_OWN_ADDRESS7      0x0A

I2C_HandleTypeDef  I2C_Handle; 

static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
 /**
  * @brief  配置 PB7 为线中断口，并设置中断优先级
  * @param  无
  * @retval 无
  */
void I2C_GTP_IRQEnable(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 

    /*开启按键GPIO口的时钟*/
    GTP_INT_GPIO_CLK_ENABLE();

    /* 选择中断引脚 */ 
    GPIO_InitStructure.Pin = GTP_INT_GPIO_PIN;
    /* 设置引脚为输入模式 */ 
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;	    		
    /* 设置引脚不上拉也不下拉 */
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    /* 使用上面的结构体初始化按键 */
    HAL_GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_InitStructure); 
    /* 配置中断优先级 */
    HAL_NVIC_SetPriority(GTP_INT_EXTI_IRQ, 1, 1);
    /* 使能中断 */
    HAL_NVIC_EnableIRQ(GTP_INT_EXTI_IRQ);

}
/**
  * @brief  关闭触摸屏中断
  * @param  无
  * @retval 无
  */
void I2C_GTP_IRQDisable(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 

    /*开启按键GPIO口的时钟*/
    GTP_INT_GPIO_CLK_ENABLE();

    /* 选择中断引脚 */ 
    GPIO_InitStructure.Pin = GTP_INT_GPIO_PIN;
    /* 设置引脚为输入模式 */ 
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;	    		
    /* 设置引脚不上拉也不下拉 */
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    /* 使用上面的结构体初始化按键 */
    HAL_GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_InitStructure); 
    /* 配置中断优先级 */
    HAL_NVIC_SetPriority(GTP_INT_EXTI_IRQ, 1, 1);
    /* 使能中断 */
    HAL_NVIC_DisableIRQ(GTP_INT_EXTI_IRQ);

}

/**
  * @brief  触摸屏 I/O配置
  * @param  无
  * @retval 无
  */
static void I2C_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;  
   
  /*使能I2C时钟 */
  GTP_I2C_CLK_ENABLE();
  
  /*使能触摸屏使用的引脚的时钟*/
  GTP_I2C_SCL_GPIO_CLK_ENABLE();
  GTP_I2C_SDA_GPIO_CLK_ENABLE(); 
  GTP_INT_GPIO_CLK_ENABLE();
  GTP_RST_GPIO_CLK_ENABLE();

#if !(SOFT_IIC)   //使用硬件IIC 
    
  /*配置SDA引脚 */  

  GPIO_InitStructure.Pin = GTP_I2C_SCL_PIN;

  GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;

  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;

  GPIO_InitStructure.Pull  = GPIO_NOPULL;

  GPIO_InitStructure.Alternate = GTP_I2C_SCL_AF;

  HAL_GPIO_Init(GTP_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

  /*配置SCL引脚 */
  GPIO_InitStructure.Pin = GTP_I2C_SDA_PIN;
  HAL_GPIO_Init(GTP_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);

#else  //使用软件IIC
    /*配置SCL引脚 */   
  GPIO_InitStructure.Pin = GTP_I2C_SCL_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStructure.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init(GTP_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

  /*配置SDA引脚 */
  GPIO_InitStructure.Pin = GTP_I2C_SDA_PIN;
  HAL_GPIO_Init(GTP_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);
#endif
 
  /*配置RST引脚，下拉推挽输出 */   
  GPIO_InitStructure.Pin = GTP_RST_GPIO_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStructure.Pull  = GPIO_PULLDOWN;
  HAL_GPIO_Init(GTP_RST_GPIO_PORT, &GPIO_InitStructure);
  
  /*配置 INT引脚，下拉推挽输出，方便初始化 */   
  GPIO_InitStructure.Pin =  GTP_INT_GPIO_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	//设置为下拉，方便初始化
  GPIO_InitStructure.Pull  = GPIO_PULLDOWN;
  HAL_GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_InitStructure);
}


/**
  * @brief  对GT91xx芯片进行复位
  * @param  无
  * @retval 无
  */
void I2C_ResetChip(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*配置 INT引脚，下拉推挽输出，方便初始化 */   
	GPIO_InitStructure.Pin = GTP_INT_GPIO_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pull  = GPIO_PULLDOWN;       //设置为下拉，方便初始化
	HAL_GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_InitStructure);

	/*初始化GT9157,rst为高电平，int为低电平，则gt9157的设备地址被配置为0xBA*/

	/*复位为低电平，为初始化做准备*/
	HAL_GPIO_WritePin (GTP_RST_GPIO_PORT,GTP_RST_GPIO_PIN,GPIO_PIN_RESET);
	CPU_TS_Tmr_Delay_US(10);

	/*拉高一段时间，进行初始化*/
	HAL_GPIO_WritePin (GTP_RST_GPIO_PORT,GTP_RST_GPIO_PIN,GPIO_PIN_SET);
	HAL_Delay(20);

	  /*初始化GT9157,rst为高电平，int为低电平，则gt9157的设备地址被配置为0xBA*/

//	/*复位为低电平，为初始化做准备*/
//	HAL_GPIO_WritePin (GTP_RST_GPIO_PORT,GTP_RST_GPIO_PIN,GPIO_PIN_RESET);
//	Delay(0x0FFFF);

//	/*拉高一段时间，进行初始化*/
//	HAL_GPIO_WritePin (GTP_RST_GPIO_PORT,GTP_RST_GPIO_PIN,GPIO_PIN_SET);
//	Delay(0x0FFFF);

	/*把INT引脚设置为浮空输入模式，以便接收触摸中断信号*/
	GPIO_InitStructure.Pin = GTP_INT_GPIO_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pull  = GPIO_NOPULL;
	HAL_GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  I2C 工作模式配置
  * @param  无
  * @retval 无
  */
static void I2C_Mode_Config(void)
{
    /* I2C 配置 */

    I2C_Handle.Instance = GTP_I2C;

    I2C_Handle.Init.Timing           = 0x90913232;//50KHz

    I2C_Handle.Init.OwnAddress1      = 0;

    I2C_Handle.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;

    I2C_Handle.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;

    I2C_Handle.Init.OwnAddress2      = 0;

    I2C_Handle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;

    I2C_Handle.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;

    I2C_Handle.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;

    /* Init the I2C */

    HAL_I2C_Init(&I2C_Handle);	
    HAL_I2CEx_AnalogFilter_Config(&I2C_Handle, I2C_ANALOGFILTER_ENABLE); 
}


/**
  * @brief  I2C 外设(GT91xx)初始化
  * @param  无
  * @retval 无
  */
void I2C_Touch_Init(void)
{
  I2C_GPIO_Config(); 
 
#if !(SOFT_IIC) //硬件IIC模式 
    I2C_Mode_Config();
#endif
  
  I2C_ResetChip();
  GTP_IRQ_Disable();
}


#if !(SOFT_IIC)   //使用硬件IIC     


__IO uint32_t  I2CTimeout = I2CT_LONG_TIMEOUT;   

/**
  * @brief   使用IIC读取数据
  * @param   
  * 	@arg ClientAddr:从设备地址
  *		@arg pBuffer:存放由从机读取的数据的缓冲区指针
  *		@arg NumByteToRead:读取的数据长度
  * @retval  无
  */
uint32_t I2C_ReadBytes(uint8_t ClientAddr,uint8_t* pBuffer, uint16_t NumByteToRead)
{
  HAL_I2C_Master_Receive(&I2C_Handle,ClientAddr,pBuffer,NumByteToRead,1000);    
  return 0;
}


/**
  * @brief   使用IIC写入数据
  * @param   
  * 	@arg ClientAddr:从设备地址
  *		@arg pBuffer:缓冲区指针
  *     @arg NumByteToWrite:写的字节数
  * @retval  无
  */
uint32_t I2C_WriteBytes(uint8_t ClientAddr,uint8_t* pBuffer,  uint8_t NumByteToWrite)
{
  HAL_I2C_Master_Transmit(&I2C_Handle,ClientAddr,pBuffer,NumByteToWrite,1000);
  return 0;  

}

#else //使用软件IIC

/*
*********************************************************************************************************
*	函 数 名: i2c_Delay
*	功能说明: I2C总线位延迟，最快400KHz
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void i2c_Delay(void)
{
	uint8_t i;

	/*　
	 	下面的时间是通过逻辑分析仪测试得到的。
    工作条件：CPU主频180MHz ，MDK编译环境，1级优化
      
		循环次数为50时，SCL频率 = 333KHz 
		循环次数为30时，SCL频率 = 533KHz，  
	 	循环次数为20时，SCL频率 = 727KHz， 
  */
	for (i = 0; i < 10*8; i++);
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Start
*	功能说明: CPU发起I2C总线启动信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Start(void)
{
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
	I2C_SDA_1();
	I2C_SCL_1();
	i2c_Delay();
	I2C_SDA_0();
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Start
*	功能说明: CPU发起I2C总线停止信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Stop(void)
{
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	I2C_SDA_0();
	I2C_SCL_1();
	i2c_Delay();
	I2C_SDA_1();
}

/*
*********************************************************************************************************
*	函 数 名: i2c_SendByte
*	功能说明: CPU向I2C总线设备发送8bit数据
*	形    参：_ucByte ： 等待发送的字节
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;

	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{		
		if (_ucByte & 0x80)
		{
			I2C_SDA_1();
		}
		else
		{
			I2C_SDA_0();
		}
		i2c_Delay();
		I2C_SCL_1();
		i2c_Delay();	
		I2C_SCL_0();
		if (i == 7)
		{
			 I2C_SDA_1(); // 释放总线
		}
		_ucByte <<= 1;	/* 左移一个bit */
		i2c_Delay();
	}
}

/*
*********************************************************************************************************
*	函 数 名: i2c_ReadByte
*	功能说明: CPU从I2C总线设备读取8bit数据
*	形    参：无
*	返 回 值: 读到的数据
*********************************************************************************************************
*/
uint8_t i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;

	/* 读到第1个bit为数据的bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		I2C_SCL_1();
		i2c_Delay();
		if (I2C_SDA_READ())
		{
			value++;
		}
		I2C_SCL_0();
		i2c_Delay();
	}
	return value;
}

/*
*********************************************************************************************************
*	函 数 名: i2c_WaitAck
*	功能说明: CPU产生一个时钟，并读取器件的ACK应答信号
*	形    参：无
*	返 回 值: 返回0表示正确应答，1表示无器件响应
*********************************************************************************************************
*/
uint8_t i2c_WaitAck(void)
{
	uint8_t re;

	I2C_SDA_1();	/* CPU释放SDA总线 */
	i2c_Delay();
	I2C_SCL_1();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	i2c_Delay();
	if (I2C_SDA_READ())	/* CPU读取SDA口线状态 */
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	I2C_SCL_0();
	i2c_Delay();
	return re;
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Ack
*	功能说明: CPU产生一个ACK信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Ack(void)
{
	I2C_SDA_0();	/* CPU驱动SDA = 0 */
	i2c_Delay();
	I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();
	I2C_SDA_1();	/* CPU释放SDA总线 */
}

/*
*********************************************************************************************************
*	函 数 名: i2c_NAck
*	功能说明: CPU产生1个NACK信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_NAck(void)
{
	I2C_SDA_1();	/* CPU驱动SDA = 1 */
	i2c_Delay();
	I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	I2C_SCL_0();
	i2c_Delay();	
}



#define I2C_DIR_WR	0		/* 写控制bit */
#define I2C_DIR_RD	1		/* 读控制bit */

/**
  * @brief   使用IIC读取数据
  * @param   
  * 	@arg ClientAddr:从设备地址
  *		@arg pBuffer:存放由从机读取的数据的缓冲区指针
  *		@arg NumByteToRead:读取的数据长度
  * @retval  无
  */
uint32_t I2C_ReadBytes(uint8_t ClientAddr,uint8_t* pBuffer, uint16_t NumByteToRead)
{
	
	/* 第1步：发起I2C总线启动信号 */
	i2c_Start();
	
	/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
	i2c_SendByte(ClientAddr | I2C_DIR_RD);	/* 此处是读指令 */
	
	/* 第3步：等待ACK */
	if (i2c_WaitAck() != 0)
	{
		goto cmd_fail;	/* 器件无应答 */
	}

	while(NumByteToRead) 
  {

    
   *pBuffer = i2c_ReadByte();
    
    /* 读指针自增 */
    pBuffer++; 
      
    /*计数器自减 */
    NumByteToRead--;
    
    if(NumByteToRead == 0)
			i2c_NAck();	/* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
    else
      i2c_Ack();	/* 中间字节读完后，CPU产生ACK信号(驱动SDA = 0) */  
  }

	/* 发送I2C总线停止信号 */
	i2c_Stop();
	return 0;	/* 执行成功 */

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	i2c_Stop();
	return 1;
}

/**
  * @brief   使用IIC写入数据
  * @param   
  * 	@arg ClientAddr:从设备地址
  *		@arg pBuffer:缓冲区指针
  *     @arg NumByteToWrite:写的字节数
  * @retval  无
  */
uint32_t I2C_WriteBytes(uint8_t ClientAddr,uint8_t* pBuffer,  uint8_t NumByteToWrite)
{
	uint16_t m;	

  /*　第0步：发停止信号，启动内部写操作　*/
  i2c_Stop();
  
  /* 通过检查器件应答的方式，判断内部写操作是否完成, 一般小于 10ms 			
    CLK频率为200KHz时，查询次数为30次左右
  */
  for (m = 0; m < 1000; m++)
  {				
    /* 第1步：发起I2C总线启动信号 */
    i2c_Start();
    
    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    i2c_SendByte(ClientAddr | I2C_DIR_WR);	/* 此处是写指令 */
    
    /* 第3步：发送一个时钟，判断器件是否正确应答 */
    if (i2c_WaitAck() == 0)
    {
      break;
    }
  }
  if (m  == 1000)
  {
    goto cmd_fail;	/* EEPROM器件写超时 */
  }	
	
  while(NumByteToWrite--)
  {
  /* 第4步：开始写入数据 */
  i2c_SendByte(*pBuffer);

  /* 第5步：检查ACK */
  if (i2c_WaitAck() != 0)
  {
    goto cmd_fail;	/* 器件无应答 */
  }
  
      pBuffer++;	/* 地址增1 */		
  }
	
	/* 命令执行成功，发送I2C总线停止信号 */
	i2c_Stop();
	return 0;

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	i2c_Stop();
	return 1;
}

#endif


/*********************************************END OF FILE**********************/
