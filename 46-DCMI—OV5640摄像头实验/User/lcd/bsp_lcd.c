/**
  ******************************************************************************
  * @file    bsp_lcd.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   LCD应用函数接口，支持ARGB888/RGB888/RGB565/ARGB4444/ARGB1555 (不含中文显示)
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 H750 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "./lcd/bsp_lcd.h"
#include "./fonts//font24.c"
#include "./fonts//font20.c"
#include "./fonts//font16.c"
#include "./fonts//font12.c"
#include "./fonts//font8.c"
#include "./flash/bsp_qspi_flash.h"
#include "./delay/core_delay.h"  

#include <string.h>
#include <stdlib.h>

#define POLY_X(Z)              ((int32_t)((Points + Z)->X))
#define POLY_Y(Z)              ((int32_t)((Points + Z)->Y)) 

#define ABS(X)  ((X) > 0 ? (X) : -(X))

#if USE_ExtFlash_Single
extern __IO uint8_t* qspi_addr;
#endif

static LTDC_HandleTypeDef  Ltdc_Handler;
static DMA2D_HandleTypeDef Dma2d_Handler;

/* Default LCD configuration with LCD Layer 1 */
uint32_t            ActiveLayer = 0;
LCD_DrawPropTypeDef DrawProp[MAX_LAYER_NUMBER];
/**
 * @brief  Initializes the LCD.
 * @param  None
 * @retval None
 */


/*根据液晶数据手册的参数配置*/
#define HBP  46		//HSYNC后的无效像素
#define VBP  23		//VSYNC后的无效行数

#define HSW   1		//HSYNC宽度
#define VSW   1		//VSYNC宽度

#define HFP  22		//HSYNC前的无效像素
#define VFP  22		//VSYNC前的无效行数

static void DrawChar(uint16_t Xpos, uint16_t Ypos, const uint8_t *c);
static void FillTriangle(uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3);
static void LL_FillBuffer(uint32_t LayerIndex, void *pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex);
static void LL_ConvertLineToARGB8888(void * pSrc, void *pDst, uint32_t xSize, uint32_t ColorMode);
 /**
  * @brief  初始化控制LCD的IO
  * @param  无
  * @retval 无
  */
static void LCD_GPIO_Config(void)
{ 
	GPIO_InitTypeDef GPIO_InitStruct;
  
  /* 使能LCD使用到的引脚时钟 */
                          //红色数据线
  LTDC_R0_GPIO_CLK_ENABLE();LTDC_R1_GPIO_CLK_ENABLE();LTDC_R2_GPIO_CLK_ENABLE();\
  LTDC_R3_GPIO_CLK_ENABLE();LTDC_R4_GPIO_CLK_ENABLE();LTDC_R5_GPIO_CLK_ENABLE();\
  LTDC_R6_GPIO_CLK_ENABLE();LTDC_R7_GPIO_CLK_ENABLE();LTDC_G0_GPIO_CLK_ENABLE();\
  LTDC_G1_GPIO_CLK_ENABLE();LTDC_G2_GPIO_CLK_ENABLE();LTDC_G3_GPIO_CLK_ENABLE();\
  LTDC_G3_GPIO_CLK_ENABLE();LTDC_G5_GPIO_CLK_ENABLE();LTDC_G6_GPIO_CLK_ENABLE();\
  LTDC_G7_GPIO_CLK_ENABLE();LTDC_B0_GPIO_CLK_ENABLE();LTDC_B1_GPIO_CLK_ENABLE();\
  LTDC_B2_GPIO_CLK_ENABLE();LTDC_B3_GPIO_CLK_ENABLE();LTDC_B4_GPIO_CLK_ENABLE();\
  LTDC_B5_GPIO_CLK_ENABLE();LTDC_B6_GPIO_CLK_ENABLE();LTDC_B7_GPIO_CLK_ENABLE();\
  LTDC_CLK_GPIO_CLK_ENABLE();LTDC_HSYNC_GPIO_CLK_ENABLE();LTDC_VSYNC_GPIO_CLK_ENABLE();\
  LTDC_DE_GPIO_CLK_ENABLE();LTDC_BL_GPIO_CLK_ENABLE();
/* GPIO配置 */

 /* 红色数据线 */                        
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  
  GPIO_InitStruct.Pin =   LTDC_R0_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R0_AF;
  HAL_GPIO_Init(LTDC_R0_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_R1_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R1_AF;
  HAL_GPIO_Init(LTDC_R1_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_R2_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R2_AF;
  HAL_GPIO_Init(LTDC_R2_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_R3_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R3_AF;
  HAL_GPIO_Init(LTDC_R3_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_R4_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R4_AF;
  HAL_GPIO_Init(LTDC_R4_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_R5_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R5_AF;
  HAL_GPIO_Init(LTDC_R5_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_R6_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R6_AF;
  HAL_GPIO_Init(LTDC_R6_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_R7_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R7_AF;
  HAL_GPIO_Init(LTDC_R7_GPIO_PORT, &GPIO_InitStruct);
  
  //绿色数据线
  GPIO_InitStruct.Pin =   LTDC_G0_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_G0_AF;
  HAL_GPIO_Init(LTDC_G0_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_G1_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_G1_AF;
  HAL_GPIO_Init(LTDC_G1_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_G2_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G2_AF;
  HAL_GPIO_Init(LTDC_G2_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_G3_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G3_AF;
  HAL_GPIO_Init(LTDC_G3_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_G4_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G4_AF;
  HAL_GPIO_Init(LTDC_G4_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_G5_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G5_AF;
  HAL_GPIO_Init(LTDC_G5_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_G6_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G6_AF;
  HAL_GPIO_Init(LTDC_G6_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_G7_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G7_AF;
  HAL_GPIO_Init(LTDC_G7_GPIO_PORT, &GPIO_InitStruct);
  
  //蓝色数据线
  GPIO_InitStruct.Pin =   LTDC_B0_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B0_AF;
  HAL_GPIO_Init(LTDC_B0_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_B1_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B1_AF;
  HAL_GPIO_Init(LTDC_B1_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_B2_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B2_AF;
  HAL_GPIO_Init(LTDC_B2_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_B3_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B3_AF;
  HAL_GPIO_Init(LTDC_B3_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_B4_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B4_AF;
  HAL_GPIO_Init(LTDC_B4_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_B5_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B5_AF;
  HAL_GPIO_Init(LTDC_B5_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_B6_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B6_AF;
  HAL_GPIO_Init(LTDC_B6_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_B7_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B7_AF;
  HAL_GPIO_Init(LTDC_B7_GPIO_PORT, &GPIO_InitStruct);
  
  //控制信号线
  GPIO_InitStruct.Pin = LTDC_CLK_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_CLK_AF;
  HAL_GPIO_Init(LTDC_CLK_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = LTDC_HSYNC_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_HSYNC_AF;
  HAL_GPIO_Init(LTDC_HSYNC_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = LTDC_VSYNC_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_VSYNC_AF;
  HAL_GPIO_Init(LTDC_VSYNC_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = LTDC_DE_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_DE_AF;
  HAL_GPIO_Init(LTDC_DE_GPIO_PORT, &GPIO_InitStruct);
  
  //背光BL
  GPIO_InitStruct.Pin = LTDC_BL_GPIO_PIN;                             
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;

  HAL_GPIO_Init(LTDC_BL_GPIO_PORT, &GPIO_InitStruct);
  
}
/**
  * @brief  时钟配置
  * @retval 无
  */
void LCD_ClockConfig(void)
{
  static RCC_PeriphCLKInitTypeDef  periph_clk_init_struct;

	/* 液晶屏时钟配置 */
	/* 5寸屏的典型PCLK为27 MHz，因此PLL3R配置为提供此时钟 */ 
	/* AMPIRE640480 LCD clock configuration */
	/* PLL3_VCO Input = HSE_VALUE/PLL3M = 5 Mhz */
	/* PLL3_VCO Output = PLL3_VCO Input * PLL3N = 800 Mhz */
	/* PLLLCDCLK = PLL3_VCO Output/PLL3R = 800/40 = 20Mhz */
	/* LTDC clock frequency = PLLLCDCLK = 20 Mhz */    
	periph_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
	periph_clk_init_struct.PLL3.PLL3M = 25;    
	periph_clk_init_struct.PLL3.PLL3N = 150;
	periph_clk_init_struct.PLL3.PLL3P = 2;
	periph_clk_init_struct.PLL3.PLL3Q = 2;
	periph_clk_init_struct.PLL3.PLL3R = 10;  
	HAL_RCCEx_PeriphCLKConfig(&periph_clk_init_struct);  
}
void LCD_Init(void)
{ 
    /* 使能LTDC时钟 */
    __HAL_RCC_LTDC_CLK_ENABLE();
    /* 使能DMA2D时钟 */
    __HAL_RCC_DMA2D_CLK_ENABLE();
	  /* 液晶屏时钟配置 */
		LCD_ClockConfig();
    /* 初始化LCD引脚 */
    LCD_GPIO_Config();
    /* 初始化SDRAM 用作LCD 显存*/
    SDRAM_Init();
    /* 配置LTDC参数 */
    Ltdc_Handler.Instance = LTDC;  
    /* 配置行同步信号宽度(HSW-1) */
    Ltdc_Handler.Init.HorizontalSync =HSW-1;
    /* 配置垂直同步信号宽度(VSW-1) */
    Ltdc_Handler.Init.VerticalSync = VSW-1;
    /* 配置(HSW+HBP-1) */
    Ltdc_Handler.Init.AccumulatedHBP = HSW+HBP-1;
    /* 配置(VSW+VBP-1) */
    Ltdc_Handler.Init.AccumulatedVBP = VSW+VBP-1;
    /* 配置(HSW+HBP+有效像素宽度-1) */
    Ltdc_Handler.Init.AccumulatedActiveW = HSW+HBP+LCD_PIXEL_WIDTH-1;
    /* 配置(VSW+VBP+有效像素高度-1) */
    Ltdc_Handler.Init.AccumulatedActiveH = VSW+VBP+LCD_PIXEL_HEIGHT-1;
    /* 配置总宽度(HSW+HBP+有效像素宽度+HFP-1) */
    Ltdc_Handler.Init.TotalWidth =HSW+ HBP+LCD_PIXEL_WIDTH + HFP-1; 
    /* 配置总高度(VSW+VBP+有效像素高度+VFP-1) */
    Ltdc_Handler.Init.TotalHeigh =VSW+ VBP+LCD_PIXEL_HEIGHT + VFP-1;

    /* 初始化LCD的像素宽度和高度 */
    Ltdc_Handler.LayerCfg->ImageWidth  = LCD_PIXEL_WIDTH;
    Ltdc_Handler.LayerCfg->ImageHeight = LCD_PIXEL_HEIGHT;
    /* 设置LCD背景层的颜色，默认黑色 */
    Ltdc_Handler.Init.Backcolor.Red = 0;
    Ltdc_Handler.Init.Backcolor.Green = 0;
    Ltdc_Handler.Init.Backcolor.Blue = 0;
    /* 极性配置 */
    /* 初始化行同步极性，低电平有效 */
    Ltdc_Handler.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    /* 初始化场同步极性，低电平有效 */
    Ltdc_Handler.Init.VSPolarity = LTDC_VSPOLARITY_AL;
    /* 初始化数据有效极性，低电平有效 */
    Ltdc_Handler.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    /* 初始化行像素时钟极性，同输入时钟 */
    Ltdc_Handler.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
    HAL_LTDC_Init(&Ltdc_Handler);
    /* 初始化字体 */
    LCD_SetFont(&LCD_DEFAULT_FONT);
}

/**
  * @brief  获取LCD当前有效层X轴的大小
  * @retval X轴的大小
  */
uint32_t LCD_GetXSize(void)
{
  return Ltdc_Handler.LayerCfg[ActiveLayer].ImageWidth;
}

/**
  * @brief  获取LCD当前有效层Y轴的大小
  * @retval Y轴的大小
  */
uint32_t LCD_GetYSize(void)
{
  return Ltdc_Handler.LayerCfg[ActiveLayer].ImageHeight;
}

/**
  * @brief  设置LCD当前有效层Y轴的大小
  * @param  imageWidthPixels：图像宽度像素个数
  * @retval 无
  */
void LCD_SetXSize(uint32_t imageWidthPixels)
{
  Ltdc_Handler.LayerCfg[ActiveLayer].ImageWidth = imageWidthPixels;
}

/**
  * @brief  设置LCD当前有效层Y轴的大小
  * @param  imageHeightPixels：图像高度像素个数
  * @retval None
  */
void LCD_SetYSize(uint32_t imageHeightPixels)
{
  Ltdc_Handler.LayerCfg[ActiveLayer].ImageHeight = imageHeightPixels;
}

/**
  * @brief  初始化LCD层 
  * @param  LayerIndex:  前景层(层1)或者背景层(层0)
  * @param  FB_Address:  每一层显存的首地址
  * @param  PixelFormat: 层的像素格式
  * @retval 无
  */
void LCD_LayerInit(uint16_t LayerIndex, uint32_t FB_Address,uint32_t PixelFormat)
{     
  LTDC_LayerCfgTypeDef  layer_cfg;

  /* 层初始化 */
  layer_cfg.WindowX0 = 0;				//窗口起始位置X坐标
  layer_cfg.WindowX1 = LCD_GetXSize();	//窗口结束位置X坐标
  layer_cfg.WindowY0 = 0;				//窗口起始位置Y坐标
  layer_cfg.WindowY1 = LCD_GetYSize();  //窗口结束位置Y坐标
  layer_cfg.PixelFormat = PixelFormat;	//像素格式
  layer_cfg.FBStartAdress = FB_Address; //层显存首地址
  layer_cfg.Alpha = 255;				//用于混合的透明度常量，范围（0—255）0为完全透明
  layer_cfg.Alpha0 = 0;					//默认透明度常量，范围（0—255）0为完全透明
  layer_cfg.Backcolor.Blue = 0;			//层背景颜色蓝色分量
  layer_cfg.Backcolor.Green = 0;		//层背景颜色绿色分量
  layer_cfg.Backcolor.Red = 0;			//层背景颜色红色分量
  layer_cfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;//层混合系数1
  layer_cfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;//层混合系数2
  layer_cfg.ImageWidth = LCD_GetXSize();//设置图像宽度
  layer_cfg.ImageHeight = LCD_GetYSize();//设置图像高度
  
  HAL_LTDC_ConfigLayer(&Ltdc_Handler, &layer_cfg, LayerIndex); //设置选中的层参数

  DrawProp[LayerIndex].BackColor = LCD_COLOR_WHITE;//设置层的字体颜色
  DrawProp[LayerIndex].pFont     = &LCD_DEFAULT_FONT;//设置层的字体类型
  DrawProp[LayerIndex].TextColor = LCD_COLOR_BLACK; //设置层的字体背景颜色
  
  __HAL_LTDC_RELOAD_CONFIG(&Ltdc_Handler);//重载层的配置参数
}
/**
  * @brief  选择LCD层
  * @param  LayerIndex: 前景层(层1)或者背景层(层0)
  * @retval 无
  */
void LCD_SelectLayer(uint32_t LayerIndex)
{
  ActiveLayer = LayerIndex;
} 

/**
  * @brief  设置LCD层的可视化
  * @param  LayerIndex: 前景层(层1)或者背景层(层0)
  * @param  State: 禁能或者使能
  * @retval 无
  */
void LCD_SetLayerVisible(uint32_t LayerIndex, FunctionalState State)
{
  if(State == ENABLE)
  {
    __HAL_LTDC_LAYER_ENABLE(&Ltdc_Handler, LayerIndex);
  }
  else
  {
    __HAL_LTDC_LAYER_DISABLE(&Ltdc_Handler, LayerIndex);
  }
  __HAL_LTDC_RELOAD_CONFIG(&Ltdc_Handler);
} 

/**
  * @brief  设置LCD的透明度常量
  * @param  LayerIndex: 前景层(层1)或者背景层(层0)
  * @param  Transparency: 透明度，范围（0—255）0为完全透明
  * @retval 无
  */
void LCD_SetTransparency(uint32_t LayerIndex, uint8_t Transparency)
{    
  HAL_LTDC_SetAlpha(&Ltdc_Handler, Transparency, LayerIndex);
}

/**
  * @brief  设置LCD缓冲帧的首地址 
  * @param  LayerIndex: 前景层(层1)或者背景层(层0)
  * @param  Address: LCD缓冲帧的首地址     
  * @retval 无
  */
void LCD_SetLayerAddress(uint32_t LayerIndex, uint32_t Address)
{
  HAL_LTDC_SetAddress(&Ltdc_Handler, Address, LayerIndex);
}

/**
  * @brief  设置显示窗口
  * @param  LayerIndex: 前景层(层1)或者背景层(层0)
  * @param  Xpos: LCD 	X轴起始位置
  * @param  Ypos: LCD 	Y轴起始位置
  * @param  Width: LCD  窗口大小
  * @param  Height: LCD 窗口大小  
  * @retval None
  */
void LCD_SetLayerWindow(uint16_t LayerIndex, uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  /* 重新设置窗口大小 */
  HAL_LTDC_SetWindowSize(&Ltdc_Handler, Width, Height, LayerIndex);
  
  /* 重新设置窗口的起始位置 */
  HAL_LTDC_SetWindowPosition(&Ltdc_Handler, Xpos, Ypos, LayerIndex); 
}


/**
  * @brief  设置LCD当前层文字颜色
  * @param  Color: 文字颜色
  * @retval 无
  */
void LCD_SetTextColor(uint32_t Color)
{
  DrawProp[ActiveLayer].TextColor = Color;
}

/**
  * @brief  获取LCD当前层文字颜色
  * @retval 文字颜色
  */
uint32_t LCD_GetTextColor(void)
{
  return DrawProp[ActiveLayer].TextColor;
}

/**
  * @brief  设置LCD当前层的文字背景颜色
  * @param  Color: 文字背景颜色
  * @retval 无
  */
void LCD_SetBackColor(uint32_t Color)
{
  DrawProp[ActiveLayer].BackColor = Color;
}

/**
  * @brief  获取LCD当前层的文字背景颜色
  * @retval 文字背景颜色
  */
uint32_t LCD_GetBackColor(void)
{
  return DrawProp[ActiveLayer].BackColor;
}

/**
 * @brief  设置LCD文字的颜色和背景的颜色
 * @param  TextColor: 指定文字颜色
 * @param  BackColor: 指定背景颜色
 * @retval 无
 */
void LCD_SetColors(uint32_t TextColor, uint32_t BackColor)
{
     LCD_SetTextColor (TextColor);
     LCD_SetBackColor (BackColor);
}
/**
  * @brief  设置LCD当前层显示的字体
  * @param  fonts: 字体类型
  * @retval None
  */
void LCD_SetFont(sFONT *fonts)
{
  DrawProp[ActiveLayer].pFont = fonts;
}

/**
  * @brief  获取LCD当前层显示的字体
  * @retval 字体类型
  */
sFONT *LCD_GetFont(void)
{
  return DrawProp[ActiveLayer].pFont;
}

/**
  * @brief  读LCD的像素值
  * @param  Xpos: X 轴坐标位置
  * @param  Ypos: Y 轴坐标位置
  * @retval RGB像素值
  */
uint32_t LCD_ReadPixel(uint16_t Xpos, uint16_t Ypos)
{
  uint32_t ret = 0;
  
  if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    /* 从SDRAM显存中读取颜色数据 */
    ret = *(__IO uint32_t*) (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*LCD_GetXSize() + Xpos)));
  }
  else if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  {
    /* 从SDRAM显存中读取颜色数据 */
    ret  = (*(__IO uint8_t*) (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (3*(Ypos*LCD_GetXSize() + Xpos))+2) & 0x00FFFFFF);
	ret |= (*(__IO uint8_t*) (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (3*(Ypos*LCD_GetXSize() + Xpos))+1) & 0x00FFFFFF);
	ret |= (*(__IO uint8_t*) (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (3*(Ypos*LCD_GetXSize() + Xpos))) & 0x00FFFFFF);
  }
  else if((Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565) || \
          (Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
          (Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_AL88))  
  {
    /* 从SDRAM显存中读取颜色数据 */
    ret = *(__IO uint16_t*) (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*LCD_GetXSize() + Xpos)));    
  }
  else
  {
    /* 从SDRAM显存中读取颜色数据 */
    ret = *(__IO uint8_t*) (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*LCD_GetXSize() + Xpos)));    
  }
  
  return ret;
}

/**
  * @brief  LCD当前层清屏
  * @param  Color: 背景颜色
  * @retval None
  */
void LCD_Clear(uint32_t Color)
{ 
  /* 清屏 */ 
  LL_FillBuffer(ActiveLayer, (uint32_t *)(Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress), LCD_GetXSize(), LCD_GetYSize(), 0, Color);
}

/**
  * @brief  清除一行
  * @param  Line: 行
  * @retval None
  */
void LCD_ClearLine(uint32_t Line)
{
  uint32_t color_backup = DrawProp[ActiveLayer].TextColor;
  DrawProp[ActiveLayer].TextColor = DrawProp[ActiveLayer].BackColor;
  
  /* 画一个跟背景色一致的矩形完成清除 */
  LCD_FillRect(0, (Line * DrawProp[ActiveLayer].pFont->Height), LCD_GetXSize(), DrawProp[ActiveLayer].pFont->Height);
  
  DrawProp[ActiveLayer].TextColor = color_backup;
  LCD_SetTextColor(DrawProp[ActiveLayer].TextColor);  
	HAL_Delay(1);
}

/**
  * @brief  显示一个字符
  * @param  Xpos: X轴起始坐标
  * @param  Ypos: Y轴起始坐标
  * @param  Ascii: 字符 ascii 码,范围（ 0x20 —0x7E ）
  * @retval 无
  */
void LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii)
{
    DrawChar(Xpos, Ypos, &DrawProp[ActiveLayer].pFont->table[(Ascii-' ') *\
    DrawProp[ActiveLayer].pFont->Height * ((DrawProp[ActiveLayer].pFont->Width + 7) / 8)]);
}

/**
  * @brief  显示字符串
  * @param  Xpos: X轴起始坐标
  * @param  Ypos: Y轴起始坐标 
  * @param  Text: 字符串指针
  * @param  Mode: 显示对齐方式，可以是CENTER_MODE、RIGHT_MODE、LEFT_MODE
  * @retval None
  */
void LCD_DisplayStringAt(uint16_t Xpos, uint16_t Ypos, uint8_t *Text, Text_AlignModeTypdef Mode)
{
  uint16_t ref_column = 1, i = 0;
  uint32_t size = 0, xsize = 0; 
  uint8_t  *ptr = Text;
  
  /* 获取字符串大小 */
  while (*ptr++) size ++ ;
  
  /* 每一行可以显示字符的数量 */
  xsize = (LCD_GetXSize()/DrawProp[ActiveLayer].pFont->Width);
  
  switch (Mode)
  {
  case CENTER_MODE:
    {
      ref_column = Xpos + ((xsize - size)* DrawProp[ActiveLayer].pFont->Width) / 2;
      break;
    }
  case LEFT_MODE:
    {
      ref_column = Xpos;
      break;
    }
  case RIGHT_MODE:
    {
      ref_column = - Xpos + ((xsize - size)*DrawProp[ActiveLayer].pFont->Width);
      break;
    }    
  default:
    {
      ref_column = Xpos;
      break;
    }
  }
  
  /*检查起始行是否在显示范围内 */
  if ((ref_column < 1) || (ref_column >= 0x8000))
  {
    ref_column = 1;
  }

  /* 使用字符显示函数显示每一个字符*/
  while ((*Text != 0) & (((LCD_GetXSize() - (i*DrawProp[ActiveLayer].pFont->Width)) & 0xFFFF)\
			>= DrawProp[ActiveLayer].pFont->Width))
  {
    /* 显示一个字符 */
    LCD_DisplayChar(ref_column, Ypos, *Text);
    /* 根据字体大小计算下一个偏移位置 */
    ref_column += DrawProp[ActiveLayer].pFont->Width;
    /* 指针指向下一个字符 */
    Text++;
    i++;
  }  
}

/**
  * @brief  在指定行显示字符串(最多60个)
  * @param  Line: 显示的行
  * @param  ptr: 字符串指针
  * @retval 无
  */
void LCD_DisplayStringLine(uint16_t Line, uint8_t *ptr)
{  
  LCD_DisplayStringAt(0, LINE(Line), ptr, LEFT_MODE);
}
/**
 * @brief  在显示器上显示一个中文字符
 * @param  usX ：在特定扫描方向下字符的起始X坐标
 * @param  usY ：在特定扫描方向下字符的起始Y坐标
 * @param  usChar ：要显示的中文字符（国标码）
 * @retval 无
 */ 
static void LCD_DispChar_CH (uint16_t Xpos, uint16_t Ypos, uint16_t Text)
{
  uint32_t i = 0, j = 0;
  uint16_t height, width;
  uint8_t  offset;
  uint8_t  *pchar;
  uint8_t  Buffer[HEIGHT_CH_CHAR*3];
  uint32_t line;
	
  GetGBKCode (Buffer, Text );
  
  height = 	HEIGHT_CH_CHAR;//取字模数据//获取正在使用字体高度
  width  =  WIDTH_CH_CHAR; //获取正在使用字体宽度
  
  offset =  8 *((width + 7)/8) -  width ;//计算字符的每一行像素的偏移值，实际存储大小-字体宽度
  
  for(i = 0; i < height; i++)//遍历字体高度绘点
  {
    pchar = ((uint8_t *)Buffer + (width + 7)/8 * i);//计算字符的每一行像素的偏移地址
    
    switch(((width + 7)/8))//根据字体宽度来提取不同字体的实际像素值
    {
      
    case 1:
      line =  pchar[0];      //提取字体宽度小于8的字符的像素值
      break;
      
    case 2:
      line =  (pchar[0]<< 8) | pchar[1]; //提取字体宽度大于8小于16的字符的像素值     
      break;
      
    case 3:
    default:
      line =  (pchar[0]<< 16) | (pchar[1]<< 8) | pchar[2]; //提取字体宽度大于16小于24的字符的像素值     
      break;
    } 
    
    for (j = 0; j < width; j++)//遍历字体宽度绘点
    {
      if(line & (1 << (width- j + offset- 1))) //根据每一行的像素值及偏移位置按照当前字体颜色进行绘点
      {
        LCD_DrawPixel((Xpos + j), Ypos, DrawProp[ActiveLayer].TextColor);
      }
      else//如果这一行没有字体像素则按照背景颜色绘点
      {
        LCD_DrawPixel((Xpos + j), Ypos, DrawProp[ActiveLayer].BackColor);
      } 
    }
    Ypos++;
  }
}

/**
  * @brief  显示一行字符，若超出液晶宽度，不自动换行。
			中英混显时，请把英文字体设置为Font24格式
  * @param  Line: 要显示的行编号LINE(0) - LINE(N)
  * @param  *ptr: 要显示的字符串指针
  * @retval None
  */
void LCD_DisplayStringLine_EN_CH(uint16_t Line, uint8_t *ptr)
{  
  uint16_t refcolumn = 0;
  /* 判断显示位置不能超出液晶的边界 */
  while ((refcolumn < LCD_PIXEL_WIDTH) && ((*ptr != 0) & (((refcolumn + DrawProp[ActiveLayer].pFont->Width) & 0xFFFF) >= DrawProp[ActiveLayer].pFont->Width)))
  {
	/* 使用LCD显示一个字符 */
	if ( * ptr <= 126 )	           	//英文字符
	{
				
		LCD_DisplayChar(refcolumn, LINE(Line), *ptr);
		/* 根据字体偏移显示的位置 */
		refcolumn += DrawProp[ActiveLayer].pFont->Width;
		/* 指向字符串中的下一个字符 */
		ptr++;
	}
	
	else	                            //汉字字符
	{	
		uint16_t usCh;
		
		/*一个汉字两字节*/
		usCh = * ( uint16_t * ) ptr;	
		/*交换编码顺序*/
		usCh = ( usCh << 8 ) + ( usCh >> 8 );		
		
		/*显示汉字*/
		LCD_DispChar_CH ( refcolumn,LINE(Line) , usCh );
		/*显示位置偏移*/
		refcolumn += WIDTH_CH_CHAR;
		/* 指向字符串中的下一个字符 */
		ptr += 2; 		
    }		
  }
}
/**
 * @brief  在显示器上显示中英文字符串,超出液晶宽度时会自动换行。
					 中英文混显示时，请把英文字体设置为Font16x24格式
 * @param  Line ：行(也可理解为y坐标)
 * @param  Column ：列（也可理解为x坐标）
 * @param  pStr ：要显示的字符串的首地址
 * @retval 无
 */
void LCD_DispString_EN_CH( uint16_t Line, uint16_t Column, const uint8_t * pStr )
{
      /* 判断显示位置不能超出液晶的边界 */
  while ((Column < LCD_PIXEL_WIDTH) && ((*pStr != 0) & (((Column + DrawProp[ActiveLayer].pFont->Width) & 0xFFFF) >= DrawProp[ActiveLayer].pFont->Width)))
  {
		/* 使用LCD显示一个字符 */
		if ( * pStr <= 126 )	           	//英文字符
		{
					
			LCD_DisplayChar(Column, Line, *pStr);
			/* 根据字体偏移显示的位置 */
			Column += DrawProp[ActiveLayer].pFont->Width;
			/* 指向字符串中的下一个字符 */
			pStr++;
		}
		
		else	                            //汉字字符
		{	
			uint16_t usCh;
			
			/*一个汉字两字节*/
			usCh = * ( uint16_t * ) pStr;	
			/*交换编码顺序*/
			usCh = ( usCh << 8 ) + ( usCh >> 8 );		
			
			/*显示汉字*/
			LCD_DispChar_CH (Column,Line, usCh );
			/*显示位置偏移*/
			Column += WIDTH_CH_CHAR;
			/* 指向字符串中的下一个字符 */
			pStr += 2; 		
	}
  }
}
/**
  * @brief  绘制水平线
  * @param  Xpos: X轴起始坐标
  * @param  Ypos: Y轴起始坐标
  * @param  Length: 线的长度
  * @retval 无
  */
void LCD_DrawHLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint32_t  Xaddress = 0;

  if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    Xaddress = (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress) + 4*(LCD_GetXSize()*Ypos + Xpos);
  }
  else if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  {
    Xaddress = (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress) + 3*(LCD_GetXSize()*Ypos + Xpos);
  }
  else if((Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565) || \
          (Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
          (Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_AL88))  
  {
    Xaddress = (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress) + 2*(LCD_GetXSize()*Ypos + Xpos);   
  }
  else
  {
    Xaddress = (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress) + 2*(LCD_GetXSize()*Ypos + Xpos);   
  }	
  /* 填充数据 */
  LL_FillBuffer(ActiveLayer, (uint32_t *)Xaddress, Length, 1, 0, DrawProp[ActiveLayer].TextColor);
}

/**
  * @brief  绘制垂直线
  * @param  Xpos: X轴起始坐标
  * @param  Ypos: Y轴起始坐标
  * @param  Length: 线的长度
  * @retval 无
  */
void LCD_DrawVLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint32_t  Xaddress = 0;
  
  if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    Xaddress = (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress) + 4*(LCD_GetXSize()*Ypos + Xpos);
  }
  else if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  {
    Xaddress = (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress) + 3*(LCD_GetXSize()*Ypos + Xpos);
  }
  else if((Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565) || \
          (Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
          (Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_AL88))  
  {
    Xaddress = (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress) + 2*(LCD_GetXSize()*Ypos + Xpos);   
  }
  else
  {
    Xaddress = (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress) + 2*(LCD_GetXSize()*Ypos + Xpos);   
  }	
  
  /* 填充数据 */
  LL_FillBuffer(ActiveLayer, (uint32_t *)Xaddress, 1, Length, (LCD_GetXSize() - 1), DrawProp[ActiveLayer].TextColor);
}

/**
  * @brief  指定两点画一条线
  * @param  x1: 第一点X轴坐标
  * @param  y1: 第一点Y轴坐标
  * @param  x2: 第二点X轴坐标
  * @param  y2: 第二点Y轴坐标
  * @retval 无
  */
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, num_add = 0, num_pixels = 0, 
  curpixel = 0;
  
  deltax = ABS(x2 - x1);        /* 求x轴的绝对值 */
  deltay = ABS(y2 - y1);        /* 求y轴的绝对值 */
  x = x1;                       /* 第一个像素的x坐标起始值 */
  y = y1;                       /* 第一个像素的y坐标起始值 */
  
  if (x2 >= x1)                 /* x坐标值为递增 */
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          /* x坐标值为递减 */
  {
    xinc1 = -1;
    xinc2 = -1;
  }
  
  if (y2 >= y1)                 /* y坐标值为递增 */
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          /* y坐标值为递减 */
  {
    yinc1 = -1;
    yinc2 = -1;
  }
  
  if (deltax >= deltay)         /* 每个 y 坐标值至少有一个x坐标值*/
  {
    xinc1 = 0;                  /* 当分子大于或等于分母时不要改变 x */
    yinc2 = 0;                  /* 不要为每次迭代更改 y */
    den = deltax;
    num = deltax / 2;
    num_add = deltay;
    num_pixels = deltax;         /* x比y多的值 */
  }
  else                          /* 每个 x 坐标值至少有一个y坐标值 */
  {
    xinc2 = 0;                  /* 不要为每次迭代更改 x */
    yinc1 = 0;                  /* 当分子大于或等于分母时不要改变 y */
    den = deltay;
    num = deltay / 2;
    num_add = deltax;
    num_pixels = deltay;         /* y比x多的值 */
  }
  
  for (curpixel = 0; curpixel <= num_pixels; curpixel++)
  {
    LCD_DrawPixel(x, y, DrawProp[ActiveLayer].TextColor);   /* 绘制当前像素点 */
    num += num_add;                            /* 在分数的基础上增加分子 */
    if (num >= den)                           /* 检查分子大于或等于分母 */
    {
      num -= den;                             /* 计算新的分子值 */
      x += xinc1;                             /* x值递增 */
      y += yinc1;                             /* y值递增 */
    }
    x += xinc2;                               /* y值递增 */
    y += yinc2;                               /* y值递增 */
  }
}

/**
  * @brief  绘制一个矩形
  * @param  Xpos:   X轴坐标
  * @param  Ypos:   Y轴坐标
  * @param  Width:  矩形宽度  
  * @param  Height: 矩形高度
  * @retval 无
  */
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  /* 绘制水平线 */
  LCD_DrawHLine(Xpos, Ypos, Width);
  LCD_DrawHLine(Xpos, (Ypos+ Height), Width);
  
  /* 绘制垂直线 */
  LCD_DrawVLine(Xpos, Ypos, Height);
  LCD_DrawVLine((Xpos + Width), Ypos, Height);
}

/**
  * @brief  绘制一个圆形
  * @param  Xpos:   X轴坐标
  * @param  Ypos:   Y轴坐标
  * @param  Radius: 圆的半径
  * @retval 无
  */
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t   decision;    /* 决策变量 */ 
  uint32_t  current_x;   /* 当前x坐标值 */
  uint32_t  current_y;   /* 当前y坐标值 */
  
  decision = 3 - (Radius << 1);
  current_x = 0;
  current_y = Radius;
  
  while (current_x <= current_y)
  {
    LCD_DrawPixel((Xpos + current_x), (Ypos - current_y), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos - current_x), (Ypos - current_y), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos + current_y), (Ypos - current_x), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos - current_y), (Ypos - current_x), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos + current_x), (Ypos + current_y), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos - current_x), (Ypos + current_y), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos + current_y), (Ypos + current_x), DrawProp[ActiveLayer].TextColor);
    
    LCD_DrawPixel((Xpos - current_y), (Ypos + current_x), DrawProp[ActiveLayer].TextColor);
    
    if (decision < 0)
    { 
      decision += (current_x << 2) + 6;
    }
    else
    {
      decision += ((current_x - current_y) << 2) + 10;
      current_y--;
    }
    current_x++;
  } 
}

/**
  * @brief  绘制一条折线
  * @param  Points: 指向点数组的指针
  * @param  PointCount: 点数
  * @retval 无
  */
void LCD_DrawPolygon(pPoint Points, uint16_t PointCount)
{
  int16_t x = 0, y = 0;
  
  if(PointCount < 2)
  {
    return;
  }
  
  LCD_DrawLine(Points->X, Points->Y, (Points+PointCount-1)->X, (Points+PointCount-1)->Y);
  
  while(--PointCount)
  {
    x = Points->X;
    y = Points->Y;
    Points++;
    LCD_DrawLine(x, y, Points->X, Points->Y);
  }
}

/**
  * @brief  绘制一个椭圆
  * @param  Xpos:   X轴坐标
  * @param  Ypos:   Y轴坐标
  * @param  XRadius: 椭圆X轴半径
  * @param  YRadius: 椭圆Y轴半径
  * @retval 无
  */
void LCD_DrawEllipse(int Xpos, int Ypos, int XRadius, int YRadius)
{
  int x = 0, y = -YRadius, err = 2-2*XRadius, e2;
  float k = 0, rad1 = 0, rad2 = 0;
  
  rad1 = XRadius;
  rad2 = YRadius;
  
  k = (float)(rad2/rad1);  
  
  do { 
    LCD_DrawPixel((Xpos-(uint16_t)(x/k)), (Ypos+y), DrawProp[ActiveLayer].TextColor);
    LCD_DrawPixel((Xpos+(uint16_t)(x/k)), (Ypos+y), DrawProp[ActiveLayer].TextColor);
    LCD_DrawPixel((Xpos+(uint16_t)(x/k)), (Ypos-y), DrawProp[ActiveLayer].TextColor);
    LCD_DrawPixel((Xpos-(uint16_t)(x/k)), (Ypos-y), DrawProp[ActiveLayer].TextColor);      
    
    e2 = err;
    if (e2 <= x) {
      err += ++x*2+1;
      if (-y == x && e2 <= y) e2 = 0;
    }
    if (e2 > y) err += ++y*2+1;     
  }
  while (y <= 0);
}

/**
  * @brief  绘制一个点
  * @param  Xpos:   X轴坐标
  * @param  Ypos:   Y轴坐标
  * @param  RGB_Code: 像素颜色值
  * @retval 无
  */
void LCD_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint32_t RGB_Code)
{

  if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    *(__IO uint32_t*) (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (4*(Ypos*LCD_GetXSize() + Xpos))) = RGB_Code;
  }
  else if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  {
    *(__IO uint8_t*) (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (3*(Ypos*LCD_GetXSize() + Xpos))+2) = 0xFF&(RGB_Code>>16);
		*(__IO uint8_t*) (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (3*(Ypos*LCD_GetXSize() + Xpos))+1) = 0xFF&(RGB_Code>>8);
		*(__IO uint8_t*) (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (3*(Ypos*LCD_GetXSize() + Xpos))) = 0xFF&RGB_Code;
  }
  else if((Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565) || \
          (Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
          (Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_AL88))  
  {
    *(__IO uint16_t*) (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*LCD_GetXSize() + Xpos))) = (uint16_t)RGB_Code;   
  }
  else
  {
    *(__IO uint16_t*) (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*LCD_GetXSize() + Xpos))) = (uint16_t)RGB_Code;   
  }

}

/**
  * @brief  绘制一幅从内部flash加载的ARGB888(32 bits per pixel)格式的图片
  * @param  Xpos: Bmp在液晶上X 轴坐标
  * @param  Ypos: Bmp在液晶上Y 轴坐标
  * @param  pbmp: 指针指向存在内部flash的Bmp图片的首地址
  * @retval 无
  */
void LCD_DrawBitmap(uint32_t Xpos, uint32_t Ypos, uint8_t *pbmp)
{
  uint32_t index = 0, width = 0, height = 0, bit_pixel = 0;
  uint32_t address;
  uint32_t input_color_mode = 0;
  
  /* 获取位图数据的基地址 */
  index = *(__IO uint16_t *) (pbmp + 10);
  index |= (*(__IO uint16_t *) (pbmp + 12)) << 16;
  
  /* 获取位图宽度 */
  width = *(uint16_t *) (pbmp + 18);
  width |= (*(uint16_t *) (pbmp + 20)) << 16;
  
  /* 获取位图高度 */
  height = *(uint16_t *) (pbmp + 22);
  height |= (*(uint16_t *) (pbmp + 24)) << 16; 
  
  /* 获取像素个数 */
  bit_pixel = *(uint16_t *) (pbmp + 28);   
  
  /* 设定地址 */
  address = Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress + (((LCD_GetXSize()*Ypos) + Xpos)*(4));
  
  /*判断层输入像素格式 */    
  if ((bit_pixel/8) == 4)
  {
    input_color_mode = DMA2D_INPUT_ARGB8888;
  }
  else if ((bit_pixel/8) == 2)
  {
    input_color_mode = DMA2D_INPUT_RGB565;   
  }
  else 
  {
    input_color_mode = DMA2D_INPUT_RGB888;
  }
  
  /* 绕过位图的帧头 */
  pbmp += (index + (width * (height - 1) * (bit_pixel/8)));  
  
  /* 将图片转换为 ARGB8888 像素格式 */
  for(index=0; index < height; index++)
  {
    /* 像素格式转换 */
    LL_ConvertLineToARGB8888((uint32_t *)pbmp, (uint32_t *)address, width, input_color_mode);
    
    /* 递增源和目标缓冲区 */
    address+=  (LCD_GetXSize()*4);
    pbmp -= width*(bit_pixel/8);
  } 
}

/**
  * @brief  填充一个实心矩形
  * @param  Xpos: X坐标值
  * @param  Ypos: Y坐标值
  * @param  Width:  矩形宽度 
  * @param  Height: 矩形高度
  * @retval 无
  */
void LCD_FillRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  uint32_t  x_address = 0;
  
  /* 设置文字颜色 */
  LCD_SetTextColor(DrawProp[ActiveLayer].TextColor);
  
  /* 设置矩形开始地址 */
    if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    x_address = (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress) + 4*(LCD_GetXSize()*Ypos + Xpos);
  }
  else if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  {
    x_address = (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress) + 3*(LCD_GetXSize()*Ypos + Xpos);
  }
  else if((Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565) || \
          (Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
          (Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_AL88))  
  {
    x_address = (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress) + 2*(LCD_GetXSize()*Ypos + Xpos);   
  }
  else
  {
    x_address = (Ltdc_Handler.LayerCfg[ActiveLayer].FBStartAdress) + 2*(LCD_GetXSize()*Ypos + Xpos);
  }	
  /* 填充矩形 */
  LL_FillBuffer(ActiveLayer, (uint32_t *)x_address, Width, Height, (LCD_GetXSize() - Width), DrawProp[ActiveLayer].TextColor);
}

/**
  * @brief  填充一个实心圆
  * @param  Xpos: X坐标值
  * @param  Ypos: Y坐标值
  * @param  Radius: 圆的半径
  * @retval 无
  */
void LCD_FillCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t   decision;    /* 决策变量 */ 
  uint32_t  current_x;   /* 当前x坐标值 */
  uint32_t  current_y;   /* 当前y坐标值 */
  
  decision = 3 - (Radius << 1);
  
  current_x = 0;
  current_y = Radius;
  
  LCD_SetTextColor(DrawProp[ActiveLayer].TextColor);
  
  while (current_x <= current_y)
  {
    if(current_y > 0) 
    {
      LCD_DrawHLine(Xpos - current_y, Ypos + current_x, 2*current_y);
      LCD_DrawHLine(Xpos - current_y, Ypos - current_x, 2*current_y);
    }
    
    if(current_x > 0) 
    {
      LCD_DrawHLine(Xpos - current_x, Ypos - current_y, 2*current_x);
      LCD_DrawHLine(Xpos - current_x, Ypos + current_y, 2*current_x);
    }
    if (decision < 0)
    { 
      decision += (current_x << 2) + 6;
    }
    else
    {
      decision += ((current_x - current_y) << 2) + 10;
      current_y--;
    }
    current_x++;
  }
  
  LCD_SetTextColor(DrawProp[ActiveLayer].TextColor);
  LCD_DrawCircle(Xpos, Ypos, Radius);
}

/**
  * @brief  绘制多边形
  * @param  Points: 指向点数组的指针
  * @param  PointCount: 点数
  * @retval 无
  */
void LCD_FillPolygon(pPoint Points, uint16_t PointCount)
{
  int16_t X = 0, Y = 0, X2 = 0, Y2 = 0, X_center = 0, Y_center = 0, X_first = 0, Y_first = 0, pixelX = 0, pixelY = 0, counter = 0;
  uint16_t  image_left = 0, image_right = 0, image_top = 0, image_bottom = 0;
  
  image_left = image_right = Points->X;
  image_top= image_bottom = Points->Y;
  
  for(counter = 1; counter < PointCount; counter++)
  {
    pixelX = POLY_X(counter);
    if(pixelX < image_left)
    {
      image_left = pixelX;
    }
    if(pixelX > image_right)
    {
      image_right = pixelX;
    }
    
    pixelY = POLY_Y(counter);
    if(pixelY < image_top)
    { 
      image_top = pixelY;
    }
    if(pixelY > image_bottom)
    {
      image_bottom = pixelY;
    }
  }  
  
  if(PointCount < 2)
  {
    return;
  }
  
  X_center = (image_left + image_right)/2;
  Y_center = (image_bottom + image_top)/2;
  
  X_first = Points->X;
  Y_first = Points->Y;
  
  while(--PointCount)
  {
    X = Points->X;
    Y = Points->Y;
    Points++;
    X2 = Points->X;
    Y2 = Points->Y;    
    
    FillTriangle(X, X2, X_center, Y, Y2, Y_center);
    FillTriangle(X, X_center, X2, Y, Y_center, Y2);
    FillTriangle(X_center, X2, X, Y_center, Y2, Y);   
  }
  
  FillTriangle(X_first, X2, X_center, Y_first, Y2, Y_center);
  FillTriangle(X_first, X_center, X2, Y_first, Y_center, Y2);
  FillTriangle(X_center, X2, X_first, Y_center, Y2, Y_first);   
}

/**
  * @brief  填充一个实心椭圆
  * @param  Xpos:   X轴坐标
  * @param  Ypos:   Y轴坐标
  * @param  XRadius: 椭圆X轴半径
  * @param  YRadius: 椭圆Y轴半径
  * @retval 无
  */
void LCD_FillEllipse(int Xpos, int Ypos, int XRadius, int YRadius)
{
  int x = 0, y = -YRadius, err = 2-2*XRadius, e2;
  float k = 0, rad1 = 0, rad2 = 0;
  
  rad1 = XRadius;
  rad2 = YRadius;
  
  k = (float)(rad2/rad1);
  
  do 
  {       
    LCD_DrawHLine((Xpos-(uint16_t)(x/k)), (Ypos+y), (2*(uint16_t)(x/k) + 1));
    LCD_DrawHLine((Xpos-(uint16_t)(x/k)), (Ypos-y), (2*(uint16_t)(x/k) + 1));
    
    e2 = err;
    if (e2 <= x) 
    {
      err += ++x*2+1;
      if (-y == x && e2 <= y) e2 = 0;
    }
    if (e2 > y) err += ++y*2+1;
  }
  while (y <= 0);
}

/**
  * @brief  使能显示
  * @retval 无
  */
void LCD_DisplayOn(void)
{
  /* 开显示 */
  __HAL_LTDC_ENABLE(&Ltdc_Handler);
//  HAL_GPIO_WritePin(LTDC_DISP_GPIO_PORT, LTDC_DISP_GPIO_PIN, GPIO_PIN_SET);/* LCD_DISP使能*/
  HAL_GPIO_WritePin(LTDC_BL_GPIO_PORT, LTDC_BL_GPIO_PIN, GPIO_PIN_SET);  /* 开背光*/
}

/**
  * @brief  禁能显示
  * @retval 无
  */
void LCD_DisplayOff(void)
{
  /* 关显示 */
  __HAL_LTDC_DISABLE(&Ltdc_Handler);
//  HAL_GPIO_WritePin(LTDC_DISP_GPIO_PORT, LTDC_DISP_GPIO_PIN, GPIO_PIN_RESET); /* LCD_DISP禁能*/
  HAL_GPIO_WritePin(LTDC_BL_GPIO_PORT, LTDC_BL_GPIO_PIN, GPIO_PIN_RESET);/*关背光*/
}


/*******************************************************************************
                            局部函数
*******************************************************************************/

/**
  * @brief  显示一个字符
  * @param  Xpos: 显示字符的行位置
  * @param  Ypos: 列起始位置
  * @param  c: 指向字体数据的指针
  * @retval 无
  */
static void DrawChar(uint16_t Xpos, uint16_t Ypos, const uint8_t *c)
{
  uint32_t i = 0, j = 0;
  uint16_t height, width;
  uint8_t  offset;
  uint8_t  *pchar;
  uint32_t line;
  
  height = DrawProp[ActiveLayer].pFont->Height;//获取正在使用字体高度
  width  = DrawProp[ActiveLayer].pFont->Width; //获取正在使用字体宽度
  
  offset =  8 *((width + 7)/8) -  width ;//计算字符的每一行像素的偏移值，实际存储大小-字体宽度
  
  for(i = 0; i < height; i++)//遍历字体高度绘点
  {
    pchar = ((uint8_t *)c + (width + 7)/8 * i);//计算字符的每一行像素的偏移地址
    
    switch(((width + 7)/8))//根据字体宽度来提取不同字体的实际像素值
    {
      
    case 1:
      line =  pchar[0];      //提取字体宽度小于8的字符的像素值
      break;
      
    case 2:
      line =  (pchar[0]<< 8) | pchar[1]; //提取字体宽度大于8小于16的字符的像素值     
      break;
      
    case 3:
    default:
      line =  (pchar[0]<< 16) | (pchar[1]<< 8) | pchar[2]; //提取字体宽度大于16小于24的字符的像素值     
      break;
    } 
    
    for (j = 0; j < width; j++)//遍历字体宽度绘点
    {
      if(line & (1 << (width- j + offset- 1))) //根据每一行的像素值及偏移位置按照当前字体颜色进行绘点
      {
        LCD_DrawPixel((Xpos + j), Ypos, DrawProp[ActiveLayer].TextColor);
      }
      else//如果这一行没有字体像素则按照背景颜色绘点
      {
        LCD_DrawPixel((Xpos + j), Ypos, DrawProp[ActiveLayer].BackColor);
      } 
    }
    Ypos++;
  }
}

/**
  * @brief  填充三角形（基于三点）
  * @param  x1: 第一点的X坐标值
  * @param  y1: 第一点的Y坐标值
  * @param  x2: 第二点的X坐标值
  * @param  y2: 第二点的Y坐标值
  * @param  x3: 第三点的X坐标值
  * @param  y3: 第三点的Y坐标值
  * @retval 无
  */
static void FillTriangle(uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3)
{ 
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, num_add = 0, num_pixels = 0,
  curpixel = 0;
  
  deltax = ABS(x2 - x1);        /* 求x轴的绝对值 */
  deltay = ABS(y2 - y1);        /* 求y轴的绝对值 */
  x = x1;                       /* 第一个像素的x坐标起始值 */
  y = y1;                       /* 第一个像素的y坐标起始值 */
  
  if (x2 >= x1)                 /* x坐标值为递增*/
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          /* x坐标值为递减 */
  {
    xinc1 = -1;
    xinc2 = -1;
  }
  
  if (y2 >= y1)                 /* y坐标值为递增*/
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          /* y坐标值为递减 */
  {
    yinc1 = -1;
    yinc2 = -1;
  }
  
  if (deltax >= deltay)         /* 每个 y 坐标值至少有一个x坐标值*/
  {
    xinc1 = 0;                  /* 当分子大于或等于分母时不要改变 x */
    yinc2 = 0;                  /* 不要为每次迭代更改 y */
    den = deltax;
    num = deltax / 2;
    num_add = deltay;
    num_pixels = deltax;         /* x比y多的值 */
  }
  else                          /* 每个 x 坐标值至少有一个y坐标值 */
  {
    xinc2 = 0;                  /* 不要为每次迭代更改 x */
    yinc1 = 0;                  /* 当分子大于或等于分母时不要改变 y */
    den = deltay;
    num = deltay / 2;
    num_add = deltax;
    num_pixels = deltay;         /* y比x多的值 */
  }
  
  for (curpixel = 0; curpixel <= num_pixels; curpixel++)
  {
    LCD_DrawLine(x, y, x3, y3);
    
    num += num_add;              /* 在分数的基础上增加分子 */
    if (num >= den)             /* 判断分子是否大于或等于分母 */
    {
      num -= den;               /* 计算新的分子值 */
      x += xinc1;               /* x值递增 */
      y += yinc1;               /* y值递增 */
    }
    x += xinc2;                 /* x值递增 */
    y += yinc2;                 /* y值递增 */
  } 
}

/**
  * @brief  填充一个缓冲区
  * @param  LayerIndex: 当前层
  * @param  pDst: 指向目标缓冲区指针
  * @param  xSize: 缓冲区宽度
  * @param  ySize: 缓冲区高度
  * @param  OffLine: 偏移量
  * @param  ColorIndex: 当前颜色
  * @retval None
  */
static void LL_FillBuffer(uint32_t LayerIndex, void *pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex) 
{

  Dma2d_Handler.Init.Mode         = DMA2D_R2M;
  if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
  { 
    Dma2d_Handler.Init.ColorMode    = DMA2D_OUTPUT_RGB565;
  }
  else if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  { 
    Dma2d_Handler.Init.ColorMode    = DMA2D_OUTPUT_ARGB8888;
  }
  else if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  { 
    Dma2d_Handler.Init.ColorMode    = DMA2D_OUTPUT_RGB888;
  }
  else if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB1555)
  { 
    Dma2d_Handler.Init.ColorMode    = DMA2D_OUTPUT_ARGB1555;
  }
  else if(Ltdc_Handler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444)
  { 
    Dma2d_Handler.Init.ColorMode    = DMA2D_OUTPUT_ARGB4444;
  }
  Dma2d_Handler.Init.OutputOffset = OffLine;      
  
  Dma2d_Handler.Instance = DMA2D;
  
  /* DMA2D 初始化 */
  if(HAL_DMA2D_Init(&Dma2d_Handler) == HAL_OK) 
  {
    if(HAL_DMA2D_ConfigLayer(&Dma2d_Handler, LayerIndex) == HAL_OK) 
    {
      if (HAL_DMA2D_Start(&Dma2d_Handler, ColorIndex, (uint32_t)pDst, xSize, ySize) == HAL_OK)
      {
        /* DMA轮询传输 */  
        HAL_DMA2D_PollForTransfer(&Dma2d_Handler, 10);
      }
    }
  } 
}

/**
  * @brief  转换一行为ARGB8888像素格式
  * @param  pSrc: 指向源缓冲区的指针
  * @param  pDst: 输出颜色
  * @param  xSize: 缓冲区宽度
  * @param  ColorMode: 输入颜色模式   
  * @retval 无
  */
static void LL_ConvertLineToARGB8888(void *pSrc, void *pDst, uint32_t xSize, uint32_t ColorMode)
{    
  /* 配置DMA2D模式,颜色模式和输出偏移 */
  Dma2d_Handler.Init.Mode         = DMA2D_M2M_PFC;
  Dma2d_Handler.Init.ColorMode    = DMA2D_OUTPUT_ARGB8888;
  Dma2d_Handler.Init.OutputOffset = 0;     
  
  /* Foreground Configuration */
  Dma2d_Handler.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  Dma2d_Handler.LayerCfg[1].InputAlpha = 0xFF;
  Dma2d_Handler.LayerCfg[1].InputColorMode = ColorMode;
  Dma2d_Handler.LayerCfg[1].InputOffset = 0;
  
  Dma2d_Handler.Instance = DMA2D; 
  
  /* DMA2D 初始化 */
  if(HAL_DMA2D_Init(&Dma2d_Handler) == HAL_OK) 
  {
    if(HAL_DMA2D_ConfigLayer(&Dma2d_Handler, 1) == HAL_OK) 
    {
      if (HAL_DMA2D_Start(&Dma2d_Handler, (uint32_t)pSrc, (uint32_t)pDst, xSize, 1) == HAL_OK)
      {
        /* DMA轮询传输*/  
        HAL_DMA2D_PollForTransfer(&Dma2d_Handler, 10);
      }
    }
  } 
}
#if GBKCODE_FLASH

/*使用FLASH字模*/

//中文字库存储在FLASH的起始地址 ：
//GBKCODE_START_ADDRESS 在fonts.h文件定义
/**
  * @brief  获取FLASH中文显示字库数据
	* @param  pBuffer:存储字库矩阵的缓冲区
	* @param  c ： 要获取的文字
  * @retval None.
  */
int GetGBKCode_from_EXFlash( uint8_t * pBuffer, uint16_t c)
{ 
	unsigned char High8bit,Low8bit;
	unsigned int pos;
	int offset, GBKCODE_START_ADDRESS;
	
	static uint8_t everRead=0;

	High8bit= c >> 8;     /* 取高8位数据 */
	Low8bit= c & 0x00FF;  /* 取低8位数据 */		

#if USE_ExtFlash_Single
	offset = GetResOffset("GB2312_H2424.FON");
	if(offset == -1)
		printf("无法在FLASH中找到字库文件\r\n");
	else
		GBKCODE_START_ADDRESS = offset + RESOURCE_BASE_ADDR;
	
	/*GB2312 公式*/
	pos = ((High8bit-0xa1)*94+Low8bit-0xa1)*24*24/8;
	memcpy(pBuffer, (uint8_t *)qspi_addr+GBKCODE_START_ADDRESS+pos, 24*24/8);//读取字库数据  
	
	return 0; 
#else
	/*GB2312 公式*/
	pos = ((High8bit-0xa1)*94+Low8bit-0xa1)*24*24/8;
	BSP_QSPI_FastRead(pBuffer,GBKCODE_START_ADDRESS+pos,24*24/8); //读取字库数据  
	
	return 0;  
#endif
}

/**
  * @brief  从FLASH中的目录查找相应的资源位置
  * @param  res_base 目录在FLASH中的基地址
  * @param  res_name[in] 要查找的资源名字
  * @retval -1表示找不到，其余值表示资源在FLASH中的基地址
  */
int GetResOffset(const char *res_name)
{
  
	int i,len;
	CatalogTypeDef dir;
	uint8_t CATALOG_BUF[CATALOG_SIZE];

	len =strlen(res_name);
	for(i=0;i<CATALOG_SIZE;i+=sizeof(CatalogTypeDef))
	{
#if USE_ExtFlash_Single
		memcpy((uint8_t*)&dir, (uint8_t *)qspi_addr+RESOURCE_BASE_ADDR+i, sizeof(CatalogTypeDef));    
#else
		BSP_QSPI_FastRead((uint8_t*)&dir, RESOURCE_BASE_ADDR+i, sizeof(CatalogTypeDef));
#endif
		if(strncasecmp(dir.name,res_name,len)==0)
		{
			return dir.offset;
		}
	}
	return -1;
}

#else

/*使用SD字模*/

static FIL fnew;													/* file objects */
static FATFS fs;													/* Work area (file system object) for logical drives */
static FRESULT res_sd; 
static UINT br;            					/* File R/W count */

//字库文件存储位置，fonts.h中的宏：
//#define GBKCODE_FILE_NAME			"0:/Font/GB2312_H2424.FON"

/**
  * @brief  获取SD卡中文显示字库数据
	* @param  pBuffer:存储字库矩阵的缓冲区
	* @param  c ： 要获取的文字
  * @retval None.
  */
int GetGBKCode_from_sd ( uint8_t * pBuffer, uint16_t c)
{ 
    unsigned char High8bit,Low8bit;
    unsigned int pos;
		
		static uint8_t everRead = 0;
	
    High8bit= c >> 8;     /* 取高8位数据 */
    Low8bit= c & 0x00FF;  /* 取低8位数据 */
		
    pos = ((High8bit-0xa1)*94+Low8bit-0xa1)*24*24/8;
	
	/*第一次使用，挂载文件系统，初始化sd*/
	if(everRead == 0)
	{
		res_sd = f_mount(&fs,"0:",1);
		everRead = 1;

	}
		
    res_sd = f_open(&fnew , GBKCODE_FILE_NAME, FA_OPEN_EXISTING | FA_READ);
    
    if ( res_sd == FR_OK ) 
    {
        f_lseek (&fnew, pos);		//指针偏移
        res_sd = f_read( &fnew, pBuffer, 24*24/8, &br );		 //24*24大小的汉字 其字模 占用24*24/8个字节
        
        f_close(&fnew);
        
        return 0;  
    }    
    else
        return -1;    
}

#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
