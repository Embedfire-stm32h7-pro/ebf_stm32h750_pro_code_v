/**
  ******************************************************************************
  * @file    bsp_pvd.c
  * @author  fire
  * @version V1.0
  * @date    2017-xx-xx
  * @brief   pvd监控配置
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32F767 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./pvd/bsp_pvd.h" 

/**
  * @brief  配置PVD.
  * @param  None
  * @retval None
  */
void PVD_Config(void)
{
	PWR_PVDTypeDef sConfigPVD;
	
	/*使能 PWR 时钟 */
	HAL_PWR_EnablePVD();
	/* 配置 PVD 中断 */
	/*中断设置，抢占优先级0，子优先级为0*/
	HAL_NVIC_SetPriority(PVD_IRQn, 0 ,0);
	HAL_NVIC_EnableIRQ(PVD_IRQn);  

	/* 配置PVD级别5 (PVD检测电压的阈值为2.8V，
	   VDD电压低于2.8V时产生PVD中断，具体数据
	   可查询数据手册获知) 具体级别根据自己的
	   实际应用要求配置*/
	sConfigPVD.PVDLevel = PWR_PVDLEVEL_5;
	sConfigPVD.Mode = PWR_PVD_MODE_IT_RISING_FALLING;
	HAL_PWR_ConfigPVD(&sConfigPVD);
	/* 使能PVD输出 */
	HAL_PWR_EnablePVD();
}

/*********************************************END OF FILE**********************/
