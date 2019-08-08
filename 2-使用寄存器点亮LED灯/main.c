
/*
	使用寄存器的方法点亮LED灯
  */
#include  "stm32h750xx.h" 


/**
  *   主函数
  */
int main(void)
{	
	/*开启 GPIOB 时钟，使用外设时都要先开启它的时钟*/
	RCC_AHB4ENR |= (1<<1);	
	
	/* LED 端口初始化 */
	
	/*GPIOB MODER0清空*/
	GPIOB_MODER  &= ~0x03;	
	/*PH10 MODER10 = 01b 输出模式*/
	GPIOB_MODER |= 1;
	
	/*GPIOB OTYPER0清空*/
	GPIOB_OTYPER &= ~1;
	/*PB0 OTYPER0 = 0b 推挽模式*/
	GPIOB_OTYPER |= 0;
	
	/*清空GPIOB OSPEEDR0*/
	GPIOB_OSPEEDR &= ~0x03;
	/*PB0 OSPEEDR0 = 0b 低速模式*/
	GPIOB_OSPEEDR |= 0;
	
	/*清空GPIOB PUPDR0*/
	GPIOB_PUPDR &= ~0x03;
	/*PB0 PUPDR0 = 01b 上拉模式*/
	GPIOB_PUPDR |= 1;
	
	/*PB0 BSRR寄存器的 BR0置1，使引脚输出低电平*/
	GPIOB_BSRR |= 1<<16;
	
	/*PH10 BSRR寄存器的 BS10置1，使引脚输出高电平*/
	//GPIOH_BSRR |= (1<<10);

	while(1);

}

// 函数为空，目的是为了骗过编译器不报错
void SystemInit(void)
{	
}






/*********************************************END OF FILE**********************/

