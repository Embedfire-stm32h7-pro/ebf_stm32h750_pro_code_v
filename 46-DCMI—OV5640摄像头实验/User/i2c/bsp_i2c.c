/**
  ******************************************************************************
  * @file    i2c.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   i2c 总线驱动，方便与各种I2C传感器通讯。
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 H750 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "./i2c/bsp_i2c.h"
#include "./delay/core_delay.h"  
  
I2C_HandleTypeDef I2C_Handle;					
/*******************************  Function ************************************/

/**
  * @brief  初始化I2C总线，使用I2C前需要调用
  * @param  无
  * @retval 无
  */
void I2CMaster_Init(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 使能I2Cx时钟 */
	SENSORS_I2C_RCC_CLK_ENABLE();

	/* 使能I2C GPIO 时钟 */
	SENSORS_I2C_SCL_GPIO_CLK_ENABLE();
	SENSORS_I2C_SDA_GPIO_CLK_ENABLE();

	/* 配置I2Cx引脚: SCL ----------------------------------------*/
	GPIO_InitStructure.Pin =  SENSORS_I2C_SCL_GPIO_PIN; 
	GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pull= GPIO_NOPULL;
	GPIO_InitStructure.Alternate=SENSORS_I2C_AF; 
	HAL_GPIO_Init(SENSORS_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

	/* 配置I2Cx引脚: SDA ----------------------------------------*/
	GPIO_InitStructure.Pin = SENSORS_I2C_SDA_GPIO_PIN;  
	HAL_GPIO_Init(SENSORS_I2C_SDA_GPIO_PORT, &GPIO_InitStructure); 
	
	if(HAL_I2C_GetState(&I2C_Handle) == HAL_I2C_STATE_RESET)
	{	
		/* 强制复位I2C外设时钟 */  
		SENSORS_I2C_FORCE_RESET(); 

		/* 释放I2C外设时钟复位 */  
		SENSORS_I2C_RELEASE_RESET(); 
		
		/* I2C 配置 */
		I2C_Handle.Instance = SENSORS_I2C;
		I2C_Handle.Init.Timing           = 0x40604E73;//100KHz
		I2C_Handle.Init.OwnAddress1      = 0;
		I2C_Handle.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
		I2C_Handle.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
		I2C_Handle.Init.OwnAddress2      = 0;
		I2C_Handle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
		I2C_Handle.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
		I2C_Handle.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;

		/* 初始化I2C */
		HAL_I2C_Init(&I2C_Handle);	
		/* 使能模拟滤波器 */
		HAL_I2CEx_AnalogFilter_Config(&I2C_Handle, I2C_ANALOGFILTER_ENABLE); 
    
    
	}
  Delay(100);
}
/**
  * @brief  Manages error callback by re-initializing I2C.
  * @param  Addr: I2C Address
  * @retval None
  */
static void I2Cx_Error(void)
{
	/* 恢复I2C寄存器为默认值 */
	HAL_I2C_DeInit(&I2C_Handle); 
	/* 重新初始化I2C外设 */
	I2CMaster_Init();
}
/**
  * @brief  写一字节数据到OV2640寄存器
  * @param  Addr: OV2640 的寄存器地址
  * @param  Data: 要写入的数据
  * @retval 返回0表示写入正常，0xFF表示错误
  */
uint8_t OV5640_WriteReg(uint16_t Addr, uint8_t Data)
{
//    I2Cx_WriteMultiple(&I2C_Handle, OV2640_DEVICE_WRITE_ADDRESS, (uint16_t)Addr, I2C_MEMADD_SIZE_8BIT,(uint8_t*)&Data, 1);
  HAL_StatusTypeDef status = HAL_OK;
  
  
  status = HAL_I2C_Mem_Write(&I2C_Handle, OV5640_DEVICE_ADDRESS, (uint16_t)Addr, I2C_MEMADD_SIZE_16BIT, (uint8_t*)&Data, 1, 1000);
  
  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Re-Initiaize the I2C Bus */
    I2Cx_Error();
  }
  return status;
}

/**
  * @brief  从OV2640寄存器中读取一个字节的数据
  * @param  Addr: 寄存器地址
  * @retval 返回读取得的数据
  */
uint8_t OV5640_ReadReg(uint16_t Addr)
{
    uint8_t Data = 0;
//    I2Cx_ReadMultiple(&I2C_Handle, OV2640_DEVICE_READ_ADDRESS, Addr, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&Data, 1);
    
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_I2C_Mem_Read(&I2C_Handle, OV5640_DEVICE_ADDRESS, (uint16_t)Addr, I2C_MEMADD_SIZE_16BIT, (uint8_t*)&Data, 1, 1000);

  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* I2C error occurred */
    I2Cx_Error();
  }
  /* return the read data */
    return Data;
}

/**
  * @brief  将固件写入到OV5640片内MCU
  * @param  Addr: OV5640 的MCU基地址0x8000
  * @param  Data: 要写入的数据
  * @retval 返回0表示写入正常，0xFF表示错误
  */
uint8_t OV5640_WriteFW(uint8_t *pBuffer ,uint16_t BufferSize)
{
  uint16_t Addr=0x8000;
  HAL_StatusTypeDef status = HAL_OK;
  
  status = HAL_I2C_Mem_Write(&I2C_Handle, OV5640_DEVICE_ADDRESS, (uint16_t)Addr, I2C_MEMADD_SIZE_16BIT, pBuffer, BufferSize, 1000);
  
  /* 检查通信状态 */
  if(status != HAL_OK)
  {
    /* 发生错误重新初始化I2C */
    I2Cx_Error();
  }
  return status;
}



