
/*片上外设基地址  */

#define PERIPH_BASE           ((unsigned int)0x40000000)                          

/*总线基地址 */

#define D3_AHB1PERIPH_BASE    (PERIPH_BASE + 0x18020000)	

/*RCC外设基地址*/

#define RCC_BASE          (D3_AHB1PERIPH_BASE + 0x4400)


	


//寄存器的值常常是芯片外设自动更改的，即使CPU没有执行程序，也有可能发生变化

//编译器有可能会对没有执行程序的变量进行优化



//volatile表示易变的变量，防止编译器优化，

#define     __IO    volatile

typedef unsigned int uint32_t;

typedef unsigned short uint16_t;

typedef unsigned char  uint8_t;

/* GPIO寄存器列表 */
typedef struct

{
	__IO	uint32_t MODER;    /*GPIO模式寄存器					地址偏移: 0x00      */

	__IO	uint32_t OTYPER;   /*GPIO输出类型寄存器				地址偏移: 0x04      */

	__IO	uint32_t OSPEEDR;  /*GPIO输出速度寄存器				地址偏移: 0x08      */

	__IO	uint32_t PUPDR;    /*GPIO上拉/下拉寄存器				地址偏移: 0x0C      		*/

	__IO	uint32_t IDR;      /*GPIO输入数据寄存器				地址偏移: 0x10      		*/

	__IO	uint32_t ODR;      /*GPIO输出数据寄存器				地址偏移: 0x14      		*/

	__IO	uint16_t BSRRL;    /*GPIO置位/复位寄存器（低16位） 				地址偏移: 0x18 	*/
	
	__IO	uint16_t BSRRH;    /*GPIO置位/复位寄存器（高16位） 				地址偏移: 0x1A 	*/

	__IO	uint32_t LCKR;     /*GPIO配置锁定寄存器				地址偏移: 0x1C      */

	__IO	uint32_t AFR[2];   /*GPIO复用功能配置寄存器			地址偏移: 0x20-0x24 		*/

} GPIO_TypeDef;
/*RCC寄存器列表*/

typedef struct

{
	__IO uint32_t CR;             /*!< RCC时钟控制寄存器，地址偏移量：0x00  */
	__IO uint32_t ICSCR;          /*!< RCC内部时钟源校准寄存器，地址偏移量：0x04  */
	__IO uint32_t CRRCR;          /*!< 时钟恢复RC寄存器，地址偏移量：0x08  */
	uint32_t     RESERVED0;       /*!< 保留，地址偏移：0x0C  */
	__IO uint32_t CFGR;           /*!< RCC时钟配置寄存器，地址偏移量：0x10  */
	uint32_t     RESERVED1;       /*!< 保留，地址偏移量：0x14  */
	__IO uint32_t D1CFGR;         /*!< RCC域1配置寄存器，地址偏移量：0x18 */
	__IO uint32_t D2CFGR;         /*!< RCC域2配置寄存器，地址偏移量：0x1C */
	__IO uint32_t D3CFGR;         /*!< RCC域3配置寄存器，地址偏移量：0x20  */
	uint32_t     RESERVED2;       /*!< 保留，地址偏移量：0x24  */
	__IO uint32_t PLLCKSELR;      /*!< RCC PLL时钟源选择寄存器，地址偏移量：0x28  */
	__IO uint32_t PLLCFGR;        /*!< RCC PLL配置寄存器，地址偏移量：0x2C  */
	__IO uint32_t PLL1DIVR;       /*!< RCC PLL1分频器配置寄存器，地址偏移：0x30  */
	__IO uint32_t PLL1FRACR;      /*!< RCC PLL1小数分频器配置寄存器，地址偏移量：0x34  */
	__IO uint32_t PLL2DIVR;       /*!< RCC PLL2分频器配置寄存器，地址偏移：0x38  */
	__IO uint32_t PLL2FRACR;      /*!< RCC PLL2小数分频器配置寄存器，地址偏移量：0x3C  */
	__IO uint32_t PLL3DIVR;       /*!< RCC PLL3分频器配置寄存器，地址偏移量：0x40  */
	__IO uint32_t PLL3FRACR;      /*!< RCC PLL3小数分频器配置寄存器，地址偏移量：0x44  */
	uint32_t      RESERVED3;      /*!< 保留，地址偏移量：0x48  */
	__IO uint32_t  D1CCIPR;       /*!< RCC域1内核时钟配置寄存器地址偏移量：0x4C  */
	__IO uint32_t  D2CCIP1R;      /*!< RCC域2内核时钟配置寄存器地址偏移量：0x50  */
	__IO uint32_t  D2CCIP2R;      /*!< RCC域2内核时钟配置寄存器地址偏移量：0x54  */
	__IO uint32_t  D3CCIPR;       /*!< RCC域3内核时钟配置寄存器地址偏移量：0x58  */
	uint32_t      RESERVED4;      /*!< 保留，地址偏移：0x5C  */
	__IO uint32_t  CIER;          /*!< RCC时钟源中断使能寄存器地址偏移量：0x60  */
	__IO uint32_t  CIFR;          /*!< RCC时钟源中断标志寄存器地址偏移量：0x64  */
	__IO uint32_t  CICR;          /*!< RCC时钟源中断清除寄存器地址偏移量：0x68  */
	uint32_t     RESERVED5;       /*!< 保留，地址偏移：0x6C  */
	__IO uint32_t  BDCR;          /*!< RCC Vswitch备份域控制寄存器，地址偏移量：0x70  */
	__IO uint32_t  CSR;           /*!< RCC时钟控制和状态寄存器，地址偏移：0x74  */
	uint32_t     RESERVED6;       /*!< 保留，地址偏移量：0x78  */
	__IO uint32_t AHB3RSTR;       /*!< RCC AHB3外设复位寄存器，地址偏移：0x7C  */
	__IO uint32_t AHB1RSTR;       /*!< RCC AHB1外设复位寄存器，地址偏移量：0x80  */
	__IO uint32_t AHB2RSTR;       /*!< RCC AHB2外设复位寄存器，地址偏移量：0x84  */
	__IO uint32_t AHB4RSTR;       /*!< RCC AHB4外设复位寄存器，地址偏移量：0x88  */
	__IO uint32_t APB3RSTR;       /*!< RCC APB3外设复位寄存器，地址偏移量：0x8C  */
	__IO uint32_t APB1LRSTR;      /*!< RCC APB1外设复位寄存器低位，地址偏移：0x90  */
	__IO uint32_t APB1HRSTR;      /*!< RCC APB1外设复位寄存器高位，地址偏移：0x94  */
	__IO uint32_t APB2RSTR;       /*!< RCC APB2外设复位寄存器，地址偏移量：0x98  */
	__IO uint32_t APB4RSTR;       /*!< RCC APB4外设复位寄存器，地址偏移：0x9C  */
	__IO uint32_t GCR;            /*!< RCC RCC全局控制寄存器，地址偏移量：0xA0  */
	uint32_t     RESERVED7;       /*!< 保留，地址偏移量：0xA4  */ 
	__IO uint32_t D3AMR;          /*!< RCC域3自主模式寄存器，地址偏移量：0xA8  */
	uint32_t     RESERVED8[9];    /*!< 保留，0xAC-0xCC 地址偏移量：0xAC  */
	__IO uint32_t RSR;            /*!< RCC复位状态寄存器，地址偏移：0xD0  */
	__IO uint32_t AHB3ENR;        /*!< RCC AHB3外设时钟寄存器，地址偏移量：0xD4  */
	__IO uint32_t AHB1ENR;        /*!< RCC AHB1外设时钟寄存器，地址偏移量：0xD8  */
	__IO uint32_t AHB2ENR;        /*!< RCC AHB2外设时钟寄存器，地址偏移量：0xDC  */
	__IO uint32_t AHB4ENR;        /*!< RCC AHB4外设时钟寄存器，地址偏移量：0xE0  */
	__IO uint32_t APB3ENR;        /*!< RCC APB3外设时钟寄存器，地址偏移量：0xE4  */
	__IO uint32_t APB1LENR;       /*!< RCC APB1外设时钟低字寄存器，地址偏移：0xE8  */
	__IO uint32_t APB1HENR;       /*!< RCC APB1外设时钟高字寄存器，地址偏移：0xEC  */
	__IO uint32_t APB2ENR;        /*!< RCC APB2外设时钟寄存器，地址偏移量：0xF0 */
	__IO uint32_t APB4ENR;        /*!< RCC APB4外设时钟寄存器，地址偏移量：0xF4  */
	uint32_t      RESERVED9;      /*!< 保留，地址偏移量：0xF8  */
	__IO uint32_t AHB3LPENR;      /*!< RCC AHB3外设睡眠时钟寄存器，地址偏移量：0xFC  */
	__IO uint32_t AHB1LPENR;      /*!< RCC AHB1外设睡眠时钟寄存器，地址偏移量：0x100 */
	__IO uint32_t AHB2LPENR;      /*!< RCC AHB2外设睡眠时钟寄存器，地址偏移量：0x104 */
	__IO uint32_t AHB4LPENR;      /*!< RCC AHB4外设睡眠时钟寄存器，地址偏移量：0x108 */
	__IO uint32_t APB3LPENR;      /*!< RCC APB3外设睡眠时钟寄存器，地址偏移量：0x10C */
	__IO uint32_t APB1LLPENR;     /*!< RCC APB1外设睡眠时钟低位字寄存器，地址偏移量：0x110 */
	__IO uint32_t APB1HLPENR;     /*!< RCC APB1外设休眠时钟高字寄存器，地址偏移量：0x114 */
	__IO uint32_t APB2LPENR;      /*!< RCC APB2外设睡眠时钟寄存器，地址偏移量：0x118 */
	__IO uint32_t APB4LPENR;      /*!< RCC APB4外设睡眠时钟寄存器，地址偏移量：0x11C */
	uint32_t     RESERVED10[4];   /*!< 保留，0x120-0x12C地址偏移量：0x120 */
} RCC_TypeDef;
/******************************************************************************/
/*                                                                            */
/*                            General Purpose I/O                             */
/*                                                                            */
/******************************************************************************/
/******************  Bits definition for GPIO_MODER register  *****************/
#define GPIO_MODER_MODER0                    0x00000003U
#define GPIO_MODER_MODER0_0                  0x00000001U
#define GPIO_MODER_MODER0_1                  0x00000002U
#define GPIO_MODER_MODER1                    0x0000000CU
#define GPIO_MODER_MODER1_0                  0x00000004U
#define GPIO_MODER_MODER1_1                  0x00000008U
#define GPIO_MODER_MODER2                    0x00000030U
#define GPIO_MODER_MODER2_0                  0x00000010U
#define GPIO_MODER_MODER2_1                  0x00000020U
#define GPIO_MODER_MODER3                    0x000000C0U
#define GPIO_MODER_MODER3_0                  0x00000040U
#define GPIO_MODER_MODER3_1                  0x00000080U
#define GPIO_MODER_MODER4                    0x00000300U
#define GPIO_MODER_MODER4_0                  0x00000100U
#define GPIO_MODER_MODER4_1                  0x00000200U
#define GPIO_MODER_MODER5                    0x00000C00U
#define GPIO_MODER_MODER5_0                  0x00000400U
#define GPIO_MODER_MODER5_1                  0x00000800U
#define GPIO_MODER_MODER6                    0x00003000U
#define GPIO_MODER_MODER6_0                  0x00001000U
#define GPIO_MODER_MODER6_1                  0x00002000U
#define GPIO_MODER_MODER7                    0x0000C000U
#define GPIO_MODER_MODER7_0                  0x00004000U
#define GPIO_MODER_MODER7_1                  0x00008000U
#define GPIO_MODER_MODER8                    0x00030000U
#define GPIO_MODER_MODER8_0                  0x00010000U
#define GPIO_MODER_MODER8_1                  0x00020000U
#define GPIO_MODER_MODER9                    0x000C0000U
#define GPIO_MODER_MODER9_0                  0x00040000U
#define GPIO_MODER_MODER9_1                  0x00080000U
#define GPIO_MODER_MODER10                   0x00300000U
#define GPIO_MODER_MODER10_0                 0x00100000U
#define GPIO_MODER_MODER10_1                 0x00200000U
#define GPIO_MODER_MODER11                   0x00C00000U
#define GPIO_MODER_MODER11_0                 0x00400000U
#define GPIO_MODER_MODER11_1                 0x00800000U
#define GPIO_MODER_MODER12                   0x03000000U
#define GPIO_MODER_MODER12_0                 0x01000000U
#define GPIO_MODER_MODER12_1                 0x02000000U
#define GPIO_MODER_MODER13                   0x0C000000U
#define GPIO_MODER_MODER13_0                 0x04000000U
#define GPIO_MODER_MODER13_1                 0x08000000U
#define GPIO_MODER_MODER14                   0x30000000U
#define GPIO_MODER_MODER14_0                 0x10000000U
#define GPIO_MODER_MODER14_1                 0x20000000U
#define GPIO_MODER_MODER15                   0xC0000000U
#define GPIO_MODER_MODER15_0                 0x40000000U
#define GPIO_MODER_MODER15_1                 0x80000000U

/******************  Bits definition for GPIO_OTYPER register  ****************/
#define GPIO_OTYPER_OT_0                     0x00000001U
#define GPIO_OTYPER_OT_1                     0x00000002U
#define GPIO_OTYPER_OT_2                     0x00000004U
#define GPIO_OTYPER_OT_3                     0x00000008U
#define GPIO_OTYPER_OT_4                     0x00000010U
#define GPIO_OTYPER_OT_5                     0x00000020U
#define GPIO_OTYPER_OT_6                     0x00000040U
#define GPIO_OTYPER_OT_7                     0x00000080U
#define GPIO_OTYPER_OT_8                     0x00000100U
#define GPIO_OTYPER_OT_9                     0x00000200U
#define GPIO_OTYPER_OT_10                    0x00000400U
#define GPIO_OTYPER_OT_11                    0x00000800U
#define GPIO_OTYPER_OT_12                    0x00001000U
#define GPIO_OTYPER_OT_13                    0x00002000U
#define GPIO_OTYPER_OT_14                    0x00004000U
#define GPIO_OTYPER_OT_15                    0x00008000U

/******************  Bits definition for GPIO_OSPEEDR register  ***************/
#define GPIO_OSPEEDER_OSPEEDR0               0x00000003U
#define GPIO_OSPEEDER_OSPEEDR0_0             0x00000001U
#define GPIO_OSPEEDER_OSPEEDR0_1             0x00000002U
#define GPIO_OSPEEDER_OSPEEDR1               0x0000000CU
#define GPIO_OSPEEDER_OSPEEDR1_0             0x00000004U
#define GPIO_OSPEEDER_OSPEEDR1_1             0x00000008U
#define GPIO_OSPEEDER_OSPEEDR2               0x00000030U
#define GPIO_OSPEEDER_OSPEEDR2_0             0x00000010U
#define GPIO_OSPEEDER_OSPEEDR2_1             0x00000020U
#define GPIO_OSPEEDER_OSPEEDR3               0x000000C0U
#define GPIO_OSPEEDER_OSPEEDR3_0             0x00000040U
#define GPIO_OSPEEDER_OSPEEDR3_1             0x00000080U
#define GPIO_OSPEEDER_OSPEEDR4               0x00000300U
#define GPIO_OSPEEDER_OSPEEDR4_0             0x00000100U
#define GPIO_OSPEEDER_OSPEEDR4_1             0x00000200U
#define GPIO_OSPEEDER_OSPEEDR5               0x00000C00U
#define GPIO_OSPEEDER_OSPEEDR5_0             0x00000400U
#define GPIO_OSPEEDER_OSPEEDR5_1             0x00000800U
#define GPIO_OSPEEDER_OSPEEDR6               0x00003000U
#define GPIO_OSPEEDER_OSPEEDR6_0             0x00001000U
#define GPIO_OSPEEDER_OSPEEDR6_1             0x00002000U
#define GPIO_OSPEEDER_OSPEEDR7               0x0000C000U
#define GPIO_OSPEEDER_OSPEEDR7_0             0x00004000U
#define GPIO_OSPEEDER_OSPEEDR7_1             0x00008000U
#define GPIO_OSPEEDER_OSPEEDR8               0x00030000U
#define GPIO_OSPEEDER_OSPEEDR8_0             0x00010000U
#define GPIO_OSPEEDER_OSPEEDR8_1             0x00020000U
#define GPIO_OSPEEDER_OSPEEDR9               0x000C0000U
#define GPIO_OSPEEDER_OSPEEDR9_0             0x00040000U
#define GPIO_OSPEEDER_OSPEEDR9_1             0x00080000U
#define GPIO_OSPEEDER_OSPEEDR10              0x00300000U
#define GPIO_OSPEEDER_OSPEEDR10_0            0x00100000U
#define GPIO_OSPEEDER_OSPEEDR10_1            0x00200000U
#define GPIO_OSPEEDER_OSPEEDR11              0x00C00000U
#define GPIO_OSPEEDER_OSPEEDR11_0            0x00400000U
#define GPIO_OSPEEDER_OSPEEDR11_1            0x00800000U
#define GPIO_OSPEEDER_OSPEEDR12              0x03000000U
#define GPIO_OSPEEDER_OSPEEDR12_0            0x01000000U
#define GPIO_OSPEEDER_OSPEEDR12_1            0x02000000U
#define GPIO_OSPEEDER_OSPEEDR13              0x0C000000U
#define GPIO_OSPEEDER_OSPEEDR13_0            0x04000000U
#define GPIO_OSPEEDER_OSPEEDR13_1            0x08000000U
#define GPIO_OSPEEDER_OSPEEDR14              0x30000000U
#define GPIO_OSPEEDER_OSPEEDR14_0            0x10000000U
#define GPIO_OSPEEDER_OSPEEDR14_1            0x20000000U
#define GPIO_OSPEEDER_OSPEEDR15              0xC0000000U
#define GPIO_OSPEEDER_OSPEEDR15_0            0x40000000U
#define GPIO_OSPEEDER_OSPEEDR15_1            0x80000000U

/******************  Bits definition for GPIO_PUPDR register  *****************/
#define GPIO_PUPDR_PUPDR0                    0x00000003U
#define GPIO_PUPDR_PUPDR0_0                  0x00000001U
#define GPIO_PUPDR_PUPDR0_1                  0x00000002U
#define GPIO_PUPDR_PUPDR1                    0x0000000CU
#define GPIO_PUPDR_PUPDR1_0                  0x00000004U
#define GPIO_PUPDR_PUPDR1_1                  0x00000008U
#define GPIO_PUPDR_PUPDR2                    0x00000030U
#define GPIO_PUPDR_PUPDR2_0                  0x00000010U
#define GPIO_PUPDR_PUPDR2_1                  0x00000020U
#define GPIO_PUPDR_PUPDR3                    0x000000C0U
#define GPIO_PUPDR_PUPDR3_0                  0x00000040U
#define GPIO_PUPDR_PUPDR3_1                  0x00000080U
#define GPIO_PUPDR_PUPDR4                    0x00000300U
#define GPIO_PUPDR_PUPDR4_0                  0x00000100U
#define GPIO_PUPDR_PUPDR4_1                  0x00000200U
#define GPIO_PUPDR_PUPDR5                    0x00000C00U
#define GPIO_PUPDR_PUPDR5_0                  0x00000400U
#define GPIO_PUPDR_PUPDR5_1                  0x00000800U
#define GPIO_PUPDR_PUPDR6                    0x00003000U
#define GPIO_PUPDR_PUPDR6_0                  0x00001000U
#define GPIO_PUPDR_PUPDR6_1                  0x00002000U
#define GPIO_PUPDR_PUPDR7                    0x0000C000U
#define GPIO_PUPDR_PUPDR7_0                  0x00004000U
#define GPIO_PUPDR_PUPDR7_1                  0x00008000U
#define GPIO_PUPDR_PUPDR8                    0x00030000U
#define GPIO_PUPDR_PUPDR8_0                  0x00010000U
#define GPIO_PUPDR_PUPDR8_1                  0x00020000U
#define GPIO_PUPDR_PUPDR9                    0x000C0000U
#define GPIO_PUPDR_PUPDR9_0                  0x00040000U
#define GPIO_PUPDR_PUPDR9_1                  0x00080000U
#define GPIO_PUPDR_PUPDR10                   0x00300000U
#define GPIO_PUPDR_PUPDR10_0                 0x00100000U
#define GPIO_PUPDR_PUPDR10_1                 0x00200000U
#define GPIO_PUPDR_PUPDR11                   0x00C00000U
#define GPIO_PUPDR_PUPDR11_0                 0x00400000U
#define GPIO_PUPDR_PUPDR11_1                 0x00800000U
#define GPIO_PUPDR_PUPDR12                   0x03000000U
#define GPIO_PUPDR_PUPDR12_0                 0x01000000U
#define GPIO_PUPDR_PUPDR12_1                 0x02000000U
#define GPIO_PUPDR_PUPDR13                   0x0C000000U
#define GPIO_PUPDR_PUPDR13_0                 0x04000000U
#define GPIO_PUPDR_PUPDR13_1                 0x08000000U
#define GPIO_PUPDR_PUPDR14                   0x30000000U
#define GPIO_PUPDR_PUPDR14_0                 0x10000000U
#define GPIO_PUPDR_PUPDR14_1                 0x20000000U
#define GPIO_PUPDR_PUPDR15                   0xC0000000U
#define GPIO_PUPDR_PUPDR15_0                 0x40000000U
#define GPIO_PUPDR_PUPDR15_1                 0x80000000U

/******************  Bits definition for GPIO_IDR register  *******************/
#define GPIO_IDR_IDR_0                       0x00000001U
#define GPIO_IDR_IDR_1                       0x00000002U
#define GPIO_IDR_IDR_2                       0x00000004U
#define GPIO_IDR_IDR_3                       0x00000008U
#define GPIO_IDR_IDR_4                       0x00000010U
#define GPIO_IDR_IDR_5                       0x00000020U
#define GPIO_IDR_IDR_6                       0x00000040U
#define GPIO_IDR_IDR_7                       0x00000080U
#define GPIO_IDR_IDR_8                       0x00000100U
#define GPIO_IDR_IDR_9                       0x00000200U
#define GPIO_IDR_IDR_10                      0x00000400U
#define GPIO_IDR_IDR_11                      0x00000800U
#define GPIO_IDR_IDR_12                      0x00001000U
#define GPIO_IDR_IDR_13                      0x00002000U
#define GPIO_IDR_IDR_14                      0x00004000U
#define GPIO_IDR_IDR_15                      0x00008000U

/******************  Bits definition for GPIO_ODR register  *******************/
#define GPIO_ODR_ODR_0                       0x00000001U
#define GPIO_ODR_ODR_1                       0x00000002U
#define GPIO_ODR_ODR_2                       0x00000004U
#define GPIO_ODR_ODR_3                       0x00000008U
#define GPIO_ODR_ODR_4                       0x00000010U
#define GPIO_ODR_ODR_5                       0x00000020U
#define GPIO_ODR_ODR_6                       0x00000040U
#define GPIO_ODR_ODR_7                       0x00000080U
#define GPIO_ODR_ODR_8                       0x00000100U
#define GPIO_ODR_ODR_9                       0x00000200U
#define GPIO_ODR_ODR_10                      0x00000400U
#define GPIO_ODR_ODR_11                      0x00000800U
#define GPIO_ODR_ODR_12                      0x00001000U
#define GPIO_ODR_ODR_13                      0x00002000U
#define GPIO_ODR_ODR_14                      0x00004000U
#define GPIO_ODR_ODR_15                      0x00008000U

/******************  Bits definition for GPIO_BSRR register  ******************/
#define GPIO_BSRR_BS_0                       0x00000001U
#define GPIO_BSRR_BS_1                       0x00000002U
#define GPIO_BSRR_BS_2                       0x00000004U
#define GPIO_BSRR_BS_3                       0x00000008U
#define GPIO_BSRR_BS_4                       0x00000010U
#define GPIO_BSRR_BS_5                       0x00000020U
#define GPIO_BSRR_BS_6                       0x00000040U
#define GPIO_BSRR_BS_7                       0x00000080U
#define GPIO_BSRR_BS_8                       0x00000100U
#define GPIO_BSRR_BS_9                       0x00000200U
#define GPIO_BSRR_BS_10                      0x00000400U
#define GPIO_BSRR_BS_11                      0x00000800U
#define GPIO_BSRR_BS_12                      0x00001000U
#define GPIO_BSRR_BS_13                      0x00002000U
#define GPIO_BSRR_BS_14                      0x00004000U
#define GPIO_BSRR_BS_15                      0x00008000U
#define GPIO_BSRR_BR_0                       0x00010000U
#define GPIO_BSRR_BR_1                       0x00020000U
#define GPIO_BSRR_BR_2                       0x00040000U
#define GPIO_BSRR_BR_3                       0x00080000U
#define GPIO_BSRR_BR_4                       0x00100000U
#define GPIO_BSRR_BR_5                       0x00200000U
#define GPIO_BSRR_BR_6                       0x00400000U
#define GPIO_BSRR_BR_7                       0x00800000U
#define GPIO_BSRR_BR_8                       0x01000000U
#define GPIO_BSRR_BR_9                       0x02000000U
#define GPIO_BSRR_BR_10                      0x04000000U
#define GPIO_BSRR_BR_11                      0x08000000U
#define GPIO_BSRR_BR_12                      0x10000000U
#define GPIO_BSRR_BR_13                      0x20000000U
#define GPIO_BSRR_BR_14                      0x40000000U
#define GPIO_BSRR_BR_15                      0x80000000U

/****************** Bit definition for GPIO_LCKR register *********************/
#define GPIO_LCKR_LCK0                       0x00000001U
#define GPIO_LCKR_LCK1                       0x00000002U
#define GPIO_LCKR_LCK2                       0x00000004U
#define GPIO_LCKR_LCK3                       0x00000008U
#define GPIO_LCKR_LCK4                       0x00000010U
#define GPIO_LCKR_LCK5                       0x00000020U
#define GPIO_LCKR_LCK6                       0x00000040U
#define GPIO_LCKR_LCK7                       0x00000080U
#define GPIO_LCKR_LCK8                       0x00000100U
#define GPIO_LCKR_LCK9                       0x00000200U
#define GPIO_LCKR_LCK10                      0x00000400U
#define GPIO_LCKR_LCK11                      0x00000800U
#define GPIO_LCKR_LCK12                      0x00001000U
#define GPIO_LCKR_LCK13                      0x00002000U
#define GPIO_LCKR_LCK14                      0x00004000U
#define GPIO_LCKR_LCK15                      0x00008000U
#define GPIO_LCKR_LCKK                       0x00010000U



/*GPIO外设基地址*/

#define GPIOA_BASE            (D3_AHB1PERIPH_BASE + 0x0000U)
#define GPIOB_BASE            (D3_AHB1PERIPH_BASE + 0x0400U)
#define GPIOC_BASE            (D3_AHB1PERIPH_BASE + 0x0800U)
#define GPIOD_BASE            (D3_AHB1PERIPH_BASE + 0x0C00U)
#define GPIOE_BASE            (D3_AHB1PERIPH_BASE + 0x1000U)
#define GPIOF_BASE            (D3_AHB1PERIPH_BASE + 0x1400U)
#define GPIOG_BASE            (D3_AHB1PERIPH_BASE + 0x1800U)
#define GPIOH_BASE            (D3_AHB1PERIPH_BASE + 0x1C00U)
#define GPIOI_BASE            (D3_AHB1PERIPH_BASE + 0x2000U)
#define GPIOJ_BASE            (D3_AHB1PERIPH_BASE + 0x2400U)
#define GPIOK_BASE            (D3_AHB1PERIPH_BASE + 0x2800U)

/*定义GPIOA-K 寄存器结构体指针*/
#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB               ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC               ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD               ((GPIO_TypeDef *) GPIOD_BASE)
#define GPIOE               ((GPIO_TypeDef *) GPIOE_BASE)
#define GPIOF               ((GPIO_TypeDef *) GPIOF_BASE)
#define GPIOG               ((GPIO_TypeDef *) GPIOG_BASE)
#define GPIOH               ((GPIO_TypeDef *) GPIOH_BASE)
#define GPIOI               ((GPIO_TypeDef *) GPIOI_BASE)
#define GPIOJ               ((GPIO_TypeDef *) GPIOJ_BASE)
#define GPIOK               ((GPIO_TypeDef *) GPIOK_BASE)

/*定义RCC外设 寄存器结构体指针*/
#define RCC                 ((RCC_TypeDef *) RCC_BASE)


