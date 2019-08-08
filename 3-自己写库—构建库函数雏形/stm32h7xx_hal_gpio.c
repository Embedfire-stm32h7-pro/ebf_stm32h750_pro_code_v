
#include "stm32h7xx_hal_gpio.h"


/**
  * 函数功能：设置引脚电平
  * 参数说明：GPIOx: 该参数为GPIO_TypeDef类型的指针，指向GPIO端口的地址
  *			  GPIO_Pin: 选择要设置的GPIO端口引脚，可输入宏GPIO_Pin_0-15，
  *					    表示GPIOx端口的0-15号引脚
  * 			PinState: 设置所选引脚的电平
  *            @arg GPIO_PIN_RESET:设置低电平
  *            @arg GPIO_PIN_SET: 设置高电平
  * 返回值： 无
  */
void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
	/*设置GPIOx端口BSRR寄存器的低16位对应第GPIO_Pin位，使其输出高电平*/
	/*设置GPIOx端口BSRR寄存器的高16位对应第GPIO_Pin位，使其输出低电平*/
	/*因为BSRR寄存器写0不影响，GPIO_Pin只是对应位为1，其它位均为0，所以可以直接赋值*/

  if(PinState != GPIO_PIN_RESET)
  {
    GPIOx->BSRRL = GPIO_Pin;
  }
  else
  {
    GPIOx->BSRRH = GPIO_Pin ;
  }
}

/**
  *函数功能：初始化引脚模式
  *参数说明：GPIOx，该参数为GPIO_TypeDef类型的指针，指向GPIO端口的地址
  * 			  GPIO_InitTypeDef:GPIO_InitTypeDef结构体指针，指向初始化变量
  */
void HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init)
{
  uint32_t position = 0x00;
  uint32_t ioposition = 0x00;
  uint32_t iocurrent = 0x00;
  uint32_t temp = 0x00;

  /* Configure the port pins */
  for(position = 0; position < 16; position++)
  {
	/*以下运算是为了通过 GPIO_InitStruct->GPIO_Pin 算出引脚号0-15*/
	/*经过运算后pos的pinpos位为1，其余为0，与GPIO_Pin_x宏对应。pinpos变量每次循环加1，*/
    ioposition = ((uint32_t)0x01) << position;
    /* pos与GPIO_InitStruct->Pin做 & 运算，若运算结果currentpin == pos，
	则表示GPIO_InitStruct->Pin的pinpos位也为1，
	从而可知pinpos就是GPIO_InitStruct->Pin对应的引脚号：0-15*/
    iocurrent = (uint32_t)(GPIO_Init->Pin) & ioposition;

    if(iocurrent == ioposition)
    {
      /*--------------------- GPIO Mode Configuration ------------------------*/
      /* 在复用功能模式选择的情况下 */
      if((GPIO_Init->Mode == GPIO_MODE_AF_PP) || (GPIO_Init->Mode == GPIO_MODE_AF_OD))
      {
        /* 配置与当前IO映射的备用功能 */
        temp = GPIOx->AFR[position >> 3];
        temp &= ~((uint32_t)0xF << ((uint32_t)(position & (uint32_t)0x07) * 4)) ;
        temp |= ((uint32_t)(GPIO_Init->Alternate) << (((uint32_t)position & (uint32_t)0x07) * 4));
        GPIOx->AFR[position >> 3] = temp;
      }

      /* 配置IO方向模式（输入，输出，复用或模拟） */
      temp = GPIOx->MODER;
      temp &= ~(GPIO_MODER_MODER0 << (position * 2));
      temp |= ((GPIO_Init->Mode & GPIO_MODE) << (position * 2));
      GPIOx->MODER = temp;

      /* 在输出或复用功能模式选择的情况下 */
      if((GPIO_Init->Mode == GPIO_MODE_OUTPUT_PP) || (GPIO_Init->Mode == GPIO_MODE_AF_PP) ||
         (GPIO_Init->Mode == GPIO_MODE_OUTPUT_OD) || (GPIO_Init->Mode == GPIO_MODE_AF_OD))
      {

        /* 配置速度参数 */
        temp = GPIOx->OSPEEDR; 
        temp &= ~(GPIO_OSPEEDER_OSPEEDR0 << (position * 2));
        temp |= (GPIO_Init->Speed << (position * 2));
        GPIOx->OSPEEDR = temp;

        /* 配置IO输出类型 */
        temp = GPIOx->OTYPER;
        temp &= ~(GPIO_OTYPER_OT_0 << position) ;
        temp |= (((GPIO_Init->Mode & GPIO_OUTPUT_TYPE) >> 4) << position);
        GPIOx->OTYPER = temp;
      }

      /* 激活当前IO的上拉或下拉电阻 */
      temp = GPIOx->PUPDR;
      temp &= ~(GPIO_PUPDR_PUPDR0 << (position * 2));
      temp |= ((GPIO_Init->Pull) << (position * 2));
      GPIOx->PUPDR = temp;
    }
  }
}

/*********************************************END OF FILE**********************/

