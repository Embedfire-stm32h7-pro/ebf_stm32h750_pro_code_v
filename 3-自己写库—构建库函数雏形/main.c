
/*
	使用寄存器的方法点亮LED灯
  */
#include "stm32h7xx_hal_gpio.h"

void Delay( uint32_t nCount);
/**
  *   主函数
  */
int main(void)
{	
	GPIO_InitTypeDef InitStruct;
	
	/*开启 GPIOB 时钟，使用外设时都要先开启它的时钟*/
	RCC->AHB4ENR |= (1<<1);

	/* LED 端口初始化 */
	
	/*初始化PB0引脚*/
	/*选择要控制的GPIO引脚*/															   
	InitStruct.Pin = GPIO_PIN_0;
	/*设置引脚的输出类型为推挽输出*/
	InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	/*设置引脚为上拉模式*/
	InitStruct.Pull = GPIO_PULLUP;
	/*设置引脚速率为低速模式 */   
	InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	/*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
	HAL_GPIO_Init(GPIOB, &InitStruct);	

	/*使引脚输出低电平,点亮LED1*/
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);

	/*延时一段时间*/
	Delay(0xFFFFFF);	
	
	/*使引脚输出高电平，关闭LED1*/
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
	
	/*初始化PB1引脚*/
	InitStruct.Pin = GPIO_PIN_1;
	HAL_GPIO_Init(GPIOB,&InitStruct);
	
	/*使引脚输出低电平，点亮LED2*/
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);

	while(1);

}

//简单的延时函数，让cpu执行无意义指令，消耗时间
//具体延时时间难以计算，以后我们可使用定时器精确延时
void Delay( uint32_t nCount)	 
{
	for(; nCount != 0; nCount--);
}
// 函数为空，目的是为了骗过编译器不报错
void SystemInit(void)
{	
}






/*********************************************END OF FILE**********************/

