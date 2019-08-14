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
  * 实验平台:野火 STM32 H750 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

#include "./ap3216c/ap3216c.h"
#include "./i2c/bsp_i2c.h"
#include "./delay/core_delay.h"   
 
#define PKG_USING_AP3216C

#ifdef PKG_USING_AP3216C


//System Register
#define AP3216C_SYS_CONFIGURATION_REG 0x00 //default
#define AP3216C_SYS_INT_STATUS_REG 0x01
#define AP3216C_SYS_INT_CLEAR_MANNER_REG 0x02
#define AP3216C_IR_DATA_L_REG 0x0A
#define AP3216C_IR_DATA_H_REG 0x0B
#define AP3216C_ALS_DATA_L_REG 0x0C
#define AP3216C_ALS_DATA_H_REG 0x0D
#define AP3216C_PS_DATA_L_REG 0x0E
#define AP3216C_PS_DATA_H_REG 0x0F

//ALS Register
#define AP3216C_ALS_CONFIGURATION_REG 0x10 //range 5:4,persist 3:0
#define AP3216C_ALS_CALIBRATION_REG 0x19
#define AP3216C_ALS_THRESHOLD_LOW_L_REG 0x1A  //bit 7:0
#define AP3216C_ALS_THRESHOLD_LOW_H_REG 0x1B  //bit 15:8
#define AP3216C_ALS_THRESHOLD_HIGH_L_REG 0x1C //bit 7:0
#define AP3216C_ALS_THRESHOLD_HIGH_H_REG 0x1D //bit 15:8

//PS Register
#define AP3216C_PS_CONFIGURATION_REG 0x20
#define AP3216C_PS_LED_DRIVER_REG 0x21
#define AP3216C_PS_INT_FORM_REG 0x22
#define AP3216C_PS_MEAN_TIME_REG 0x23
#define AP3216C_PS_LED_WAITING_TIME_REG 0x24
#define AP3216C_PS_CALIBRATION_L_REG 0x28
#define AP3216C_PS_CALIBRATION_H_REG 0x29
#define AP3216C_PS_THRESHOLD_LOW_L_REG 0x2A  //bit 1:0
#define AP3216C_PS_THRESHOLD_LOW_H_REG 0x2B  //bit 9:2
#define AP3216C_PS_THRESHOLD_HIGH_L_REG 0x2C //bit 1:0
#define AP3216C_PS_THRESHOLD_HIGH_H_REG 0x2D //bit 9:2

#define AP3216C_ADDR 0x1e /*0x3c=0x1e<<1*/ // AP3216C 的 7 位地址

#define AP3216C_ERROR 		I2C_ERROR
#define AP3216C_INFO 		I2C_INFO

/* 写寄存器的值 */
static void write_reg(uint8_t reg, uint8_t data)
{
  
  Sensors_I2C_WriteRegister(AP3216C_ADDR << 1, reg, 1, &data);
}

/* 读寄存器的值 */
static void read_regs(uint8_t reg, uint8_t len, uint8_t *buf)
{
  
  Sensors_I2C_ReadRegister(AP3216C_ADDR << 1, reg, len, buf);
}

/* 软件复位传感器 */
static void reset_sensor(void)
{
    write_reg(AP3216C_SYS_CONFIGURATION_REG, AP3216C_MODE_SW_RESET); //reset
}

/**
 * This function is convenient to getting data except including high and low data for this sensor.
 * note:after reading lower register first,reading higher add one.
 */
static uint32_t read_low_and_high(uint8_t reg, uint8_t len)
{
    uint32_t data;
    uint8_t buf = 0;

    read_regs( reg, len, &buf);        // 读低字节
    data = buf;
    read_regs( reg + 1, len, &buf);    // 读高字节
    data = data + (buf << len * 8);    // 合并数据

    return data;
}


/**
 * This function is only used to set threshold without filtering times
 *
 * @param cmd first register , and other cmd count by it.
 * @param threshold threshold and filtering times of als threshold
 */
static void set_als_threshold(ap3216c_cmd_t cmd, ap3216c_threshold_t threshold)
{
    uint8_t Resolution;
    double DB;

    /* 读光照强度的范围 */
    ap3216c_get_param(AP3216C_ALS_RANGE, &Resolution);

    if (Resolution == AP3216C_ALS_RANGE_20661)     // 光照强度范围 0 - 20661
    {
        DB = 0.35;    // 光照强度的分辨率
    }
    else if (Resolution == AP3216C_ALS_RANGE_5162)     // 光照强度范围 0 - 5162
    {
        DB = 0.0788;    // 光照强度的分辨率
    }
    else if (Resolution == AP3216C_ALS_RANGE_1291)     // 光照强度范围 0 - 1291
    {
        DB = 0.0197;    // 光照强度的分辨率
    }
    else if (Resolution == AP3216C_ALS_RANGE_323)     // 光照强度范围 0 - 323
    {
        DB = 0.0049;    // 光照强度的分辨率
    }
  
    threshold.min /= DB;    // 根据不同的分辨率来设置
    threshold.max /= DB;
      
    ap3216c_set_param(cmd, (threshold.min & 0xff));
    ap3216c_set_param((ap3216c_cmd_t)(cmd + 1), (threshold.min >> 8));
    ap3216c_set_param((ap3216c_cmd_t)(cmd + 2), (threshold.max & 0xff));
    ap3216c_set_param((ap3216c_cmd_t)(cmd + 3), threshold.max >> 8);
}

static void set_ps_threshold(ap3216c_cmd_t cmd, ap3216c_threshold_t threshold)
{
    if (threshold.min > 1020)    // 大于1020 时需要设置低字节的低两位
      ap3216c_set_param(cmd, (threshold.min - 1020 & 0x03));
    
    ap3216c_set_param((ap3216c_cmd_t)(cmd + 1), threshold.min/4);    // 设置高字节参数
    
    if (threshold.max > 1020)    // 大于1020 时需要设置低字节的低两位
      ap3216c_set_param((ap3216c_cmd_t)(cmd + 2), (threshold.max - 1020 & 0x03));
    
    ap3216c_set_param((ap3216c_cmd_t)(cmd + 3), threshold.max/4);    // 设置高字节参数
}

/**
 * This function reads status register by ap3216c sensor measurement
 *
 * @param no
 *
 * @return status register value.
 */

uint8_t ap3216c_get_IntStatus(void)
{
    uint8_t IntStatus;
    
    /* 读中断状态寄存器 */
  
    read_regs(AP3216C_SYS_INT_STATUS_REG, 1, &IntStatus);
    // IntStatus 第 0 位表示 ALS 中断，第 1 位表示 PS 中断。
    
    return IntStatus;    // 返回状态
}

static void ap3216c_int_init(void)
{ 
    ap3216c_threshold_t threshold;    // 设置报警值阈值结构体
  
    threshold.min = 10;      // 光照下限产生报警
    threshold.max = 1000;    // 光照上限产生报警
    threshold.noises_time = 1;    // ALS 达到阈值后持续 threshold.noises_time * 4 个周期后开始报警， threshold.noises_time 最大为； 15
    set_als_threshold(AP3216C_ALS_LOW_THRESHOLD_L, threshold);    // 设置 ALS 的报警值
  
  
    threshold.min = 200;    // PS 低于 200 产生报警
    threshold.max = 500;    // PS 高于 500 产生报警
    set_ps_threshold(AP3216C_PS_LOW_THRESHOLD_L, threshold);     // 设置 PS 的报警值
}


 /**
  * @brief  配置 中断输入引脚
  * @param  无
  * @retval 无
  */
void AP_INT_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 

    /*开启按键GPIO口的时钟*/
    AP_INT_GPIO_CLK_ENABLE();

    /* 选择按键1的引脚 */ 
    GPIO_InitStructure.Pin = AP_INT_GPIO_PIN;
    /* 设置引脚为输入模式 */ 
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;	    		
    /* 设置引脚不上拉也不下拉 */
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    /* 使用上面的结构体初始化按键 */
    HAL_GPIO_Init(AP_INT_GPIO_PORT, &GPIO_InitStructure);   
}

/**
 * This function initializes ap3216c registered device driver
 *
 * @param no
 *
 * @return the ap3216c device.
 */
void ap3216c_init(void)
{
  I2C_Mode_Config();

  /* 关闭所有功能 */
  write_reg(AP3216C_SYS_CONFIGURATION_REG, 0x00);
  /* reset ap3216c */
  reset_sensor();
  HAL_Delay(10);
  ap3216c_set_param(AP3216C_SYSTEM_MODE, AP3216C_MODE_ALS_AND_PS);
//  HAL_Delay(100); // delay at least  100ms

  /* 配置中断脚 和 中断数据	*/
  ap3216c_int_init();
  AP_INT_Config();
}

/**
 * This function reads light by ap3216c sensor measurement
 *
 * @param no
 *
 * @return the ambient light converted to float data.
 */
float ap3216c_read_ambient_light(void)
{
    float brightness = 0.0; // default error data
    uint32_t read_data;
    uint8_t temp;

    read_data = read_low_and_high(AP3216C_ALS_DATA_L_REG, 1);

    ap3216c_get_param(AP3216C_ALS_RANGE, &temp);
  
    if (temp == AP3216C_ALS_RANGE_20661)
    {
        brightness = 0.35 * read_data; //sensor ambient light converse to reality
    }
    else if (temp == AP3216C_ALS_RANGE_5162)
    {
        brightness = 0.0788 * read_data; //sensor ambient light converse to reality
    }
    else if (temp == AP3216C_ALS_RANGE_1291)
    {
        brightness = 0.0197 * read_data; //sensor ambient light converse to reality
    }
    else if (temp == AP3216C_ALS_RANGE_323)
    {
        brightness = 0.0049 * read_data; //sensor ambient light converse to reality
    }

    return brightness;
}

/**
 * This function reads proximity by ap3216c sensor measurement
 *
 * @param no
 *
 * @return the proximity data.
 */
uint16_t ap3216c_read_ps_data(void)
{
    uint16_t proximity = 0;

    uint32_t read_data;
    read_data = read_low_and_high(AP3216C_PS_DATA_L_REG, 1); //read two data

    if (1 == ((read_data >> 6) & 0x01 || (read_data >> 14) & 0x01))
    {
       return proximity = 55555;    // 红外过高（IR），PS无效 返回一个 55555 的无效数据
    }

    proximity = (read_data & 0x000f) + (((read_data >> 8) & 0x3f) << 4); //sensor proximity converse to reality

    proximity |= read_data & 0x8000;    // 取最高位，0 表示物体远离，1 表示物体靠近
    
    return proximity;    // proximity 后十位是数据位，最高位为状态位
}

/**
 * This function reads ir by ap3216c sensor measurement
 *
 * @param no
 *
 * @return the ir data.
 */
uint16_t ap3216c_read_ir_data(void)
{
    uint16_t proximity = 0;


    uint32_t read_data;
    read_data = read_low_and_high(AP3216C_IR_DATA_L_REG, 1); //read two data

    proximity = (read_data & 0x0003) + ((read_data >> 8) & 0xFF); //sensor proximity converse to reality

    return proximity;
}

/**
 * This function sets parameter of ap3216c sensor
 *
 * @param cmd the parameter cmd of device
 * @param value for setting value in cmd register
 *
 * @return the setting parameter status,RT_EOK reprensents setting successfully.
 */
void ap3216c_set_param(ap3216c_cmd_t cmd, uint8_t value)
{
    switch (cmd)
    {
      case AP3216C_SYSTEM_MODE:
      {
          /* default 000,power down */
          write_reg(AP3216C_SYS_CONFIGURATION_REG, value);

          break;
      }
      case AP3216C_INT_PARAM:
      {
          write_reg(AP3216C_SYS_INT_CLEAR_MANNER_REG, value);

          break;
      }

      case AP3216C_ALS_RANGE:
      {
          uint8_t args;
        
          read_regs( AP3216C_ALS_CONFIGURATION_REG, 1, &args);
          args &= 0xcf;
          args |= value << 4;
          write_reg( AP3216C_ALS_CONFIGURATION_REG, args);

          break;
      }
      case AP3216C_ALS_PERSIST:
      {
          uint8_t args = 0;

          read_regs(AP3216C_ALS_CONFIGURATION_REG, 1, &args);
          args &= 0xf0;
          args |= value;
          write_reg(AP3216C_ALS_CONFIGURATION_REG, args);

          break;
      }
      case AP3216C_ALS_LOW_THRESHOLD_L:
      {
          write_reg(AP3216C_ALS_THRESHOLD_LOW_L_REG, value);

          break;
      }
      case AP3216C_ALS_LOW_THRESHOLD_H:
      {
          write_reg(AP3216C_ALS_THRESHOLD_LOW_H_REG, value);

          break;
      }
      case AP3216C_ALS_HIGH_THRESHOLD_L:
      {
          write_reg(AP3216C_ALS_THRESHOLD_HIGH_L_REG, value);

          break;
      }
      case AP3216C_ALS_HIGH_THRESHOLD_H:
      {
          write_reg(AP3216C_ALS_THRESHOLD_HIGH_H_REG, value);

          break;
      }
      case AP3216C_PS_GAIN:
      {
          uint8_t args = 0;

          read_regs(AP3216C_PS_CONFIGURATION_REG, 1, &args);
          args &= 0xf3;
          args |= value;
          write_reg(AP3216C_PS_CONFIGURATION_REG, args);

          break;
      }
      case AP3216C_PS_PERSIST:
      {
          uint8_t args = 0;

          read_regs(AP3216C_PS_CONFIGURATION_REG, 1, &args);
          args &= 0xfc;
          args |= value;
          write_reg(AP3216C_PS_CONFIGURATION_REG, args);

          break;
      }
      case AP3216C_PS_LOW_THRESHOLD_L:
      {
          
          write_reg( AP3216C_PS_THRESHOLD_LOW_L_REG, value);

          break;
      }
      case AP3216C_PS_LOW_THRESHOLD_H:
      {
          write_reg( AP3216C_PS_THRESHOLD_LOW_H_REG, value);

          break;
      }
      case AP3216C_PS_HIGH_THRESHOLD_L:
      {
          write_reg( AP3216C_PS_THRESHOLD_HIGH_L_REG, value);

          break;
      }
      case AP3216C_PS_HIGH_THRESHOLD_H:
      {
          write_reg( AP3216C_PS_THRESHOLD_HIGH_H_REG, value);

          break;
      }

    default:
    {
         
    }
    }
}

/**
 * This function gets parameter of ap3216c sensor
 *
 * @param cmd the parameter cmd of device
 * @param value to get value in cmd register
 *
 * @return the getting parameter status,RT_EOK reprensents getting successfully.
 */
void ap3216c_get_param(ap3216c_cmd_t cmd, uint8_t *value)
{
    switch (cmd)
    {
    case AP3216C_SYSTEM_MODE:
    {
        read_regs( AP3216C_SYS_CONFIGURATION_REG, 1, value);

        break;
    }
    case AP3216C_INT_PARAM:
    {
        read_regs( AP3216C_SYS_INT_CLEAR_MANNER_REG, 1, value);

        break;
    }
    case AP3216C_ALS_RANGE:
    {
        uint8_t temp;

        read_regs( AP3216C_ALS_CONFIGURATION_REG, 1, value);
        temp = (*value & 0xff) >> 4;

        *value = temp;

        break;
    }
    case AP3216C_ALS_PERSIST:
    {
        uint8_t temp;

        read_regs( AP3216C_ALS_CONFIGURATION_REG, 1, value);
        temp = *value & 0x0f;

        *value = temp;

        break;
    }
    case AP3216C_ALS_LOW_THRESHOLD_L:
    {
        read_regs( AP3216C_ALS_THRESHOLD_LOW_L_REG, 1, value);

        break;
    }
    case AP3216C_ALS_LOW_THRESHOLD_H:
    {
        read_regs( AP3216C_ALS_THRESHOLD_LOW_H_REG, 1, value);

        break;
    }
    case AP3216C_ALS_HIGH_THRESHOLD_L:
    {
        read_regs( AP3216C_ALS_THRESHOLD_HIGH_L_REG, 1, value);

        break;
    }
    case AP3216C_ALS_HIGH_THRESHOLD_H:
    {
        read_regs( AP3216C_ALS_THRESHOLD_HIGH_H_REG, 1, value);

        break;
    }
    case AP3216C_PS_GAIN:
    {
        uint8_t temp;

        read_regs( AP3216C_PS_CONFIGURATION_REG, 1, &temp);

        *value = (temp & 0xc) >> 2;

        break;
    }
    case AP3216C_PS_PERSIST:
    {
        uint8_t temp;

        read_regs( AP3216C_PS_CONFIGURATION_REG, 1, &temp);

        *value = temp & 0x3;

        break;
    }
    case AP3216C_PS_LOW_THRESHOLD_L:
    {
        read_regs( AP3216C_PS_THRESHOLD_LOW_L_REG, 1, value);

        break;
    }
    case AP3216C_PS_LOW_THRESHOLD_H:
    {
        read_regs( AP3216C_PS_THRESHOLD_LOW_H_REG, 1, value);
        break;
    }
    case AP3216C_PS_HIGH_THRESHOLD_L:
    {
        read_regs( AP3216C_PS_THRESHOLD_HIGH_L_REG, 1, value);

        break;
    }
    case AP3216C_PS_HIGH_THRESHOLD_H:
    {
        read_regs( AP3216C_PS_THRESHOLD_HIGH_H_REG, 1, value);

        break;
    }

    default:
    {
         
    }
    }
}

#endif /* PKG_USING_AP3216C */
