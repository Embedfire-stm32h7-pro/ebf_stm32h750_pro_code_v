/**
  ******************************************************************************
  * @file    bsp_i2c.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   I2C 配置文件
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 H750 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

#include "./i2c/bsp_i2c.h"
#include "./usart/bsp_debug_usart.h"

/**
  * @brief  I2C I/O配置
  * @param  无
  * @retval 无
  */
static void I2C_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;  
  
  /*使能引脚时钟*/
  AP3216C_I2C_SCL_GPIO_CLK_ENABLE();
  AP3216C_I2C_SDA_GPIO_CLK_ENABLE(); 
    /*配置SCL引脚 */   
  GPIO_InitStructure.Pin = AP3216C_I2C_SCL_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStructure.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init(AP3216C_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

  /*配置SDA引脚 */
  GPIO_InitStructure.Pin = AP3216C_I2C_SDA_PIN;
  HAL_GPIO_Init(AP3216C_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

}



/**
  * @brief  I2C 外设(GT91xx)初始化
  * @param  无
  * @retval 无
  */
void I2C_Init(void)
{
  I2C_GPIO_Config(); 
}



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
	uint16_t i;

	/*　
	 	下面的时间是通过逻辑分析仪测试得到的。
    工作条件：CPU主频216MHz ，MDK编译环境，1级优化
      
		循环次数为200时，SCL频率 = 100KHz  
	 	循环次数为30时，SCL频率 = 400KHz 
  */
	for (i = 0; i < 200; i++);
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
   if(NumByteToRead == 1)
    {
			i2c_NAck();	/* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
      
      /* 发送I2C总线停止信号 */
      i2c_Stop();
    }
    
   *pBuffer = i2c_ReadByte();
    
    /* 读指针自增 */
    pBuffer++; 
      
    /*计数器自减 */
    NumByteToRead--;
    
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

/**
  * @brief   写数据到AP3216C寄存器
  * @param   reg_add:寄存器地址
	* @param reg_data:要写入的数据
  * @retval  
  */
uint32_t Sensor_write(uint8_t reg_add,uint8_t reg_dat)
{ 
	/*　第0步：发停止信号，启动内部写操作　*/
	i2c_Stop();
	/* 第1步：发起I2C总线启动信号 */
	i2c_Start();

	/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
	i2c_SendByte(AP3216C_ADDR | I2C_DIR_WR);	/* 此处是写指令 */

	/* 第3步：发送一个时钟，判断器件是否正确应答 */
	if (i2c_WaitAck() != 0)
	{
	  goto cmd_fail;	/* EEPROM器件写超时 */
	}
	/* 第4步：开始写入寄存器地址 */
	i2c_SendByte(reg_add);

	/* 第5步：检查ACK */
	if (i2c_WaitAck() != 0)
	{
		goto cmd_fail;	/* 器件无应答 */
	}
	/* 第5步：开始写入数据 */
	i2c_SendByte(reg_dat);

	/* 第5步：检查ACK */
	if (i2c_WaitAck() != 0)
	{
		goto cmd_fail;	/* 器件无应答 */
	}	
	
	/* 命令执行成功，发送I2C总线停止信号 */
	i2c_Stop();
	return 0;

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	i2c_Stop();
	return 1;
}

/**
  * @brief   写数据到AP3216C寄存器
  * @param   reg_add:寄存器地址
	* @param reg_data:要写入的数据
  * @retval  
  */
uint32_t Sensor_Read(uint8_t reg_add,unsigned char* Read,uint8_t num)
{ 
	/* 第0步：发停止信号，启动内部写操作　*/
	i2c_Stop();
	/* 第1步：发起I2C总线启动信号 */
	i2c_Start();
	/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
	i2c_SendByte(AP3216C_ADDR | I2C_DIR_WR);	/* 此处是写指令 */
	/* 第3步：发送一个时钟，判断器件是否正确应答 */
	if (i2c_WaitAck() != 0)
	{
	  goto cmd_fail;	/* EEPROM器件写超时 */
	}
	/* 第4步：开始写入寄存器地址 */
	i2c_SendByte(reg_add);
	/* 第5步：检查ACK */
	if (i2c_WaitAck() != 0)
	{
		goto cmd_fail;	/* 器件无应答 */
	}
    /* 发送I2C总线停止信号 */
    i2c_Stop();
	
    /* 第6步：发起I2C总线启动信号 */
	i2c_Start();
	
	/* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
	i2c_SendByte(AP3216C_ADDR | I2C_DIR_RD);	/* 此处是读指令 */

	/* 第8步：检查ACK */
	if (i2c_WaitAck() != 0)
	{
		goto cmd_fail;	/* 器件无应答 */
	}
	while(num) 
   {

		*Read = i2c_ReadByte();
    
		/* 读指针自增 */
		Read++; 
      
		if(num == 1)
		{
			i2c_NAck();	/* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
		}
		else
		{
			i2c_Ack();	/* 中间字节读完后，CPU产生ACK信号(驱动SDA = 0) */  
		}				
		/*计数器自减 */
		num--;
  }

	/* 发送I2C总线停止信号 */
	i2c_Stop();
	return 0;	/* 执行成功 */

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	i2c_Stop();
	return 1;
}
/*********************************************END OF FILE**********************/
