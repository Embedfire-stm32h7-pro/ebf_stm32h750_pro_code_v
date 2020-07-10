

/*片上外设基地址  */

#define PERIPH_BASE           ((unsigned int)0x40000000)                          

/*总线基地址 */

#define D3_AHB1PERIPH_BASE    (PERIPH_BASE + 0x18020000)	


/*GPIO外设基地址*/

#define GPIOB_BASE            (D3_AHB1PERIPH_BASE + 0x0400)





/* GPIOB寄存器地址,强制转换成指针 */

#define GPIOB_MODER				*(unsigned int*)(GPIOB_BASE+0x00)

#define GPIOB_OTYPER			*(unsigned int*)(GPIOB_BASE+0x04)

#define GPIOB_OSPEEDR			*(unsigned int*)(GPIOB_BASE+0x08)

#define GPIOB_PUPDR				*(unsigned int*)(GPIOB_BASE+0x0C)

#define GPIOB_IDR					*(unsigned int*)(GPIOB_BASE+0x10)

#define GPIOB_ODR					*(unsigned int*)(GPIOB_BASE+0x14)

#define GPIOB_BSRR				*(unsigned int*)(GPIOB_BASE+0x18)

#define GPIOB_LCKR				*(unsigned int*)(GPIOB_BASE+0x1C)

#define GPIOB_AFRL				*(unsigned int*)(GPIOB_BASE+0x20)

#define GPIOB_AFRB				*(unsigned int*)(GPIOB_BASE+0x24)



/*RCC外设基地址*/

#define RCC_BASE          (D3_AHB1PERIPH_BASE + 0x4400)



/*RCC的AHB1时钟使能寄存器地址,强制转换成指针*/

#define RCC_AHB4ENR				*(unsigned int*)(RCC_BASE+0xE0)

	



