/**
  ******************************************************************
  * @file    bsp_mpu.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   配置MPU区域属性和大小
  ******************************************************************
  * @attention
  *
  * 实验平台:野火 STM32H750开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************
  */ 
#include "./mpu/bsp_mpu.h" 

/**
  * @brief  配置MPU区域属性和大小寄存器值
	* @param  Region  					MPU保护区域，取值范围（0—7）
	* @param  DisableExec  			指令访问禁用位，0=允许指令访问，1=禁止指令访问。
	* @param  AccessPermission	数据访问权限，取值范围（MPU_NO_ACCESS—MPU_PRIV_RO_URO）
	* @param  TypeExtField 			键入扩展字段，允许您配置内存访问类型，例如强排序，外设。
	*														可以取值（0，1，2）
  * @param  Address 					MPU保护区域基地址，特别注意配置的Address需要被Size整除
  * @param  Size 							MPU保护区域大小,可以取值（MPU_1KB，MPU_4KB ...MPU_512MB）
	* @param  IsShareable 			保护的存储空间是否可以共享，1=允许共享，0=禁止共享。
  * @param  IsCacheable 			保护的存储空间是否可以缓存，1=允许缓存，0=禁止缓存。
  * @param  IsBufferable 			使能Cache之后，策略是write-through还是write-back(bufferable)
	*												    1=允许缓冲，即回写（write-back），0=禁止缓冲，即直写（write-through）。
  * @retval None
  */
void BSP_MPU_ConfigRegion(	uint8_t		Region,
														uint8_t  	DisableExec,
														uint8_t 	AccessPermission,
														uint8_t 	TypeExtField, 
														uint32_t 	Address, 
														uint32_t 	Size,
														uint8_t 	IsShareable, 
														uint8_t  	IsCacheable, 
														uint8_t  	IsBufferable	)
{
  MPU_Region_InitTypeDef MPU_InitStruct;
  
  /* 禁用MPU */
  HAL_MPU_Disable();

  /* 配置MPU属性*/
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;											
  MPU_InitStruct.BaseAddress = Address;									//区域基地址。
  MPU_InitStruct.Size = Size;														//要配置的区域的区域大小。
  MPU_InitStruct.AccessPermission = AccessPermission;		//数据访问权限允许您配置用户和特权模式的读/写访问权限。
  MPU_InitStruct.IsBufferable = IsBufferable;						//区域是可缓冲的，即使用回写缓存。 可缓存但不可缓冲的区域使用直写策略。
  MPU_InitStruct.IsCacheable = IsCacheable;							//区域是否可缓存的，即其值是否可以保存在缓存中。
  MPU_InitStruct.IsShareable = IsShareable;							//区域是否可以在多个总线主控器之间共享。
  MPU_InitStruct.Number = Region;												//区域保护。
  MPU_InitStruct.TypeExtField = TypeExtField;						//键入扩展字段，允许您配置内存访问类型。
  MPU_InitStruct.SubRegionDisable = 0x00;								//子区域禁用字段。
  MPU_InitStruct.DisableExec = DisableExec;							//指令访问禁用位。

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* 启用MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

void Board_MPU_Config(uint8_t	Region ,uint8_t Mode,uint32_t Address,uint32_t Size)
{
	switch(Mode)
	{
		case MPU_Normal_WB:
				 /* 设置内存为Normal类型,禁用共享, 回写模式不带写入读取分配*/
				 BSP_MPU_ConfigRegion(Region,0,MPU_FULL_ACCESS,0,Address,Size,0,1,1);	
				 break;

		case MPU_Normal_WBWARA:
				 /* 设置内存为Normal类型,禁用共享, 回写模式带写入读取分配*/
				 BSP_MPU_ConfigRegion(Region,0,MPU_FULL_ACCESS,1,Address,Size,0,1,1);	
				 break;
		
		case MPU_Normal_WT:
				 /* 设置内存为Normal类型,禁用共享, 直写模式*/
				 BSP_MPU_ConfigRegion(Region,0,MPU_FULL_ACCESS,0,Address,Size,0,1,0);	
				 break;		
		
		case MPU_Normal_NonCache:
				 /* 设置内存为Normal类型,禁用共享，禁用缓存模式*/
				 BSP_MPU_ConfigRegion(Region,0,MPU_FULL_ACCESS,1,Address,Size,0,0,0);	
				 break;
		
		case MPU_Device_NonCache:
				 /*设置内存为Device类型,共享设置无效，禁止共享，禁用缓存模式*/
				 BSP_MPU_ConfigRegion(Region,0,MPU_FULL_ACCESS,2,Address,Size,0,0,0);	
				 break;
	}
	
}
