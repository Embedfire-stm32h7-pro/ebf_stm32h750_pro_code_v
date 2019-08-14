/**
  ******************************************************************************
  * @file    ap3216.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   AP3216C 驱动文件
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 H743 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

#include "./ap3216c/ap3216c.h"
#include "./usart/bsp_debug_usart.h"
#include "./delay/core_delay.h" 
#include "./i2c/bsp_i2c.h"


#define AP3216C_ERROR 		I2C_ERROR
#define AP3216C_INFO 		I2C_INFO
/**
  * @brief   写数据到AP3216C寄存器
  * @param   reg_add:寄存器地址
	* @param	 reg_data:要写入的数据
  * @retval  
  */
void AP3216C_WriteReg(uint8_t reg_add,uint8_t reg_dat)
{
	Sensors_I2C_WriteRegister(AP3216C_ADDRESS,reg_add,1,&reg_dat);
}

/**
  * @brief   从AP3216C寄存器读取数据
  * @param   reg_add:寄存器地址
	* @param	 Read：存储数据的缓冲区
	* @param	 num：要读取的数据量
  * @retval  
  */
void AP3216C_ReadData(uint8_t reg_add,unsigned char* Read,uint8_t num)
{
	Sensors_I2C_ReadRegister(AP3216C_ADDRESS,reg_add,num,Read);
}


/**
  * @brief   初始化AP3216C芯片
  * @param   
  * @retval  
  */
void AP3216C_Init(void)
{
  AP3216C_WriteReg(AP3216C_SYS_CONFIG, 0x00);//关闭所有功能
  AP3216C_WriteReg(AP3216C_SYS_CONFIG, AP3216C_SW_RST_BIT);//复位
  HAL_Delay(10);//复位后一定要延时10ms，否则会出错
  AP3216C_WriteReg(AP3216C_SYS_CONFIG, AP3216C_ALS_PS_IR_ACTIVE_BIT);//开启所有功能
}

/**
  * @brief   读取AP3216C的环境光传感器数据
  * @param   
  * @retval  
  */
void AP3216CReadALS(uint16_t *alsData)
{
  uint8_t buf[2];
  AP3216C_ReadData(AP3216C_ALS_DATA_LOW, buf, 2);
  *alsData = (buf[1] << 8) | buf[0];
}

/**
  * @brief   读取AP3216C的接近传感器数据
  * @param   
  * @retval  
  */
void AP3216CReadPS(uint16_t *psData)
{
  uint8_t buf[2];
  AP3216C_ReadData(AP3216C_PS_DATA_LOW, buf, 2);
  buf[0] = buf[0] & 0x0F; // PS Data LOW 4 bits
  buf[1] = buf[1] & 0x3F; // PS Data HIGH 6 bits
  *psData = (buf[1] << 4) | buf[0];
}

/**
  * @brief   读取AP3216C的红外光传感器数据
  * @param   
  * @retval  
  */
void AP3216CReadIR(uint16_t *irData)
{
  uint8_t buf[2];
  AP3216C_ReadData(AP3216C_IR_DATA_LOW, buf, 2);
  buf[0] = buf[0] & 0x03; // IR Data LOW 2 bits
  *irData = (buf[1] << 2) | buf[0];
}
