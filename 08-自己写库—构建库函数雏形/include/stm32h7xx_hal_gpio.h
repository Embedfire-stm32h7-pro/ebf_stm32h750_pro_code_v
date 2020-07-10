

#include "stm32h7xx.h"


/*GPIO引脚号定义*/
#define GPIO_PIN_0                 ((uint16_t)0x0001)  /*!< 选择Pin0 (1<<0) */
#define GPIO_PIN_1                 ((uint16_t)0x0002)  /*!< 选择Pin1 (1<<1)*/
#define GPIO_PIN_2                 ((uint16_t)0x0004)  /*!< 选择Pin2 (1<<2)*/
#define GPIO_PIN_3                 ((uint16_t)0x0008)  /*!< 选择Pin3 (1<<3)*/
#define GPIO_PIN_4                 ((uint16_t)0x0010)  /*!< 选择Pin4 */
#define GPIO_PIN_5                 ((uint16_t)0x0020)  /*!< 选择Pin5 */
#define GPIO_PIN_6                 ((uint16_t)0x0040)  /*!< 选择Pin6 */
#define GPIO_PIN_7                 ((uint16_t)0x0080)  /*!< 选择Pin7 */
#define GPIO_PIN_8                 ((uint16_t)0x0100)  /*!< 选择Pin8 */
#define GPIO_PIN_9                 ((uint16_t)0x0200)  /*!< 选择Pin9 */
#define GPIO_PIN_10                ((uint16_t)0x0400)  /*!< 选择Pin10 */
#define GPIO_PIN_11                ((uint16_t)0x0800)  /*!< 选择Pin11 */
#define GPIO_PIN_12                ((uint16_t)0x1000)  /*!< 选择Pin12 */
#define GPIO_PIN_13                ((uint16_t)0x2000)  /*!< 选择Pin13 */
#define GPIO_PIN_14                ((uint16_t)0x4000)  /*!< 选择Pin14 */
#define GPIO_PIN_15                ((uint16_t)0x8000)  /*!< 选择Pin15 */
#define GPIO_PIN_All               ((uint16_t)0xFFFF)  /*!< 选择全部引脚 */




#define  GPIO_MODE_INPUT           ((uint32_t)0x00000000U)   /*!< 浮空输入*/
#define  GPIO_MODE_OUTPUT_PP       ((uint32_t)0x00000001U)   /*!< 推挽输出 */
#define  GPIO_MODE_OUTPUT_OD       ((uint32_t)0x00000011U)   /*!< 开漏输出 */
#define  GPIO_MODE_AF_PP           ((uint32_t)0x00000002U)   /*!< 推挽复用输出*/
#define  GPIO_MODE_AF_OD           ((uint32_t)0x00000012U)   /*!< 开漏复用输出*/

#define  GPIO_MODE_ANALOG          ((uint32_t)0x00000003U)   /*!< 模拟模式*/
    
#define  GPIO_SPEED_FREQ_LOW         ((uint32_t)0x00000000U)  /*!< 低速*/
#define  GPIO_SPEED_FREQ_MEDIUM      ((uint32_t)0x00000001U)  /*!< 中速*/
#define  GPIO_SPEED_FREQ_HIGH        ((uint32_t)0x00000002U)  /*!< 快速*/
#define  GPIO_SPEED_FREQ_VERY_HIGH   ((uint32_t)0x00000003U)  /*!< 高速*/

#define  GPIO_NOPULL        ((uint32_t)0x00000000U)   /*!< 无上下拉  */
#define  GPIO_PULLUP        ((uint32_t)0x00000001U)   /*!< 上拉   */
#define  GPIO_PULLDOWN      ((uint32_t)0x00000002U)   /*!< 下拉   */

/** 
  * @brief GPIO初始化结构体类型定义  
  */ 
typedef struct
{
  uint32_t Pin;       /*指定要配置的GPIO引脚 */

  uint32_t Mode;      /*指定所选引脚的工作模式*/

  uint32_t Pull;      /*指定所选引脚的上拉或下拉激活 */

  uint32_t Speed;     /*指定所选引脚的速度 */

  uint32_t Alternate;  /*要连接到所选引脚的外设*/ 
}GPIO_InitTypeDef;

/** 
  * @brief  GPIO Bit SET and Bit RESET enumeration 
  */
typedef enum
{
  GPIO_PIN_RESET = 0,
  GPIO_PIN_SET
}GPIO_PinState;

#define GPIO_MODE             ((uint32_t)0x00000003U)
#define GPIO_OUTPUT_TYPE      ((uint32_t)0x00000010U)

#define GPIO_GET_INDEX(__GPIOx__)   (uint8_t)(((__GPIOx__) == (GPIOA))? 0U :\
                                              ((__GPIOx__) == (GPIOB))? 1U :\
                                              ((__GPIOx__) == (GPIOC))? 2U :\
                                              ((__GPIOx__) == (GPIOD))? 3U :\
                                              ((__GPIOx__) == (GPIOE))? 4U :\
                                              ((__GPIOx__) == (GPIOF))? 5U :\
                                              ((__GPIOx__) == (GPIOG))? 6U :\
                                              ((__GPIOx__) == (GPIOH))? 7U :\
                                              ((__GPIOx__) == (GPIOI))? 8U :\
                                              ((__GPIOx__) == (GPIOJ))? 9U : 10U)

void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
void HAL_GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);


