/**
  ******************************************************************************
  * @file    qr_decoder_user.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   二维码识别
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板  
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

#include "qr_decoder_user.h"
#include "stdlib.h"
#include "string.h"
#include "./lcd/bsp_lcd.h"
/*扫描窗口参数*/
 #define  Frame_width           ((uint16_t)320)//扫描窗口边长（正方形）
 
 /*扫描框线条参数*/
 #define  Frame_line_length     ((uint16_t)30) //扫描框线条长度
 #define  Frame_line_size       ((uint16_t)3)  //扫描框线条宽度
 /**
  * @brief  打开一个QR扫描窗口
  * @param  None
  * @retval None
  */
 void LCD_Open_QR_Window(void)
 {
     //选择第二层
     LCD_SelectLayer(1);
     //填充搞一个透明矩形，用于显示第一层的二维码
     LCD_SetTextColor (TRANSPARENCY);
     LCD_FillRect((LCD_GetXSize()-Frame_width)/2,
                  (LCD_GetYSize()-Frame_width)/2,Frame_width,Frame_width);
     //绘制一个红色矩形框
     LCD_SetColors(LCD_COLOR_RED,TRANSPARENCY);
     LCD_DrawRect((LCD_GetXSize()-Frame_width)/2,
                  (LCD_GetYSize()-Frame_width)/2,Frame_width,Frame_width);
     //设置四个直角符号的颜色
     LCD_SetColors(LCD_COLOR_GREEN,TRANSPARENCY);
     //绘制左上角直角符号
     //水平方向
     LCD_FillRect((LCD_GetXSize()-Frame_width)/2,
     (LCD_GetYSize()-Frame_width)/2,Frame_line_length,Frame_line_size);
     //垂直方向
     LCD_FillRect((LCD_GetXSize()-Frame_width)/2,
     (LCD_GetYSize()-Frame_width)/2,Frame_line_size,Frame_line_length);
     //绘制右上角直角符号
     //水平方向
     LCD_FillRect((LCD_GetXSize()+Frame_width)/2-Frame_line_length-(Frame_line_size-1),
    (LCD_GetYSize()-Frame_width)/2,Frame_line_length,Frame_line_size);
     //垂直方向
     LCD_FillRect((LCD_GetXSize()+Frame_width)/2-(Frame_line_size-1),
     (LCD_GetYSize()-Frame_width)/2,Frame_line_size,Frame_line_length);
     //绘制左下角直角符号
     //水平方向
     LCD_FillRect((LCD_GetXSize()-Frame_width)/2,(LCD_GetYSize()+Frame_width)/2-
                  (Frame_line_size-1),Frame_line_length,Frame_line_size);
     //垂直方向
     LCD_FillRect((LCD_GetXSize()-Frame_width)/2,(LCD_GetYSize()+Frame_width)/2-
  Frame_line_length-(Frame_line_size-1),Frame_line_size,Frame_line_length);           
     //绘制右下角直角符号
     //水平方向
     LCD_FillRect((LCD_GetXSize()+Frame_width)/2-Frame_line_length-(Frame_line_size-1),
 (LCD_GetYSize()+Frame_width)/2-(Frame_line_size-1),Frame_line_length+Frame_line_size,Frame_line_size);
     //垂直方向
     LCD_FillRect((LCD_GetXSize()+Frame_width)/2-(Frame_line_size-1),
(LCD_GetYSize()+Frame_width)/2-Frame_line_length-(Frame_line_size-1),Frame_line_size,Frame_line_length);
 }
 /**
  * @brief  在扫描框里循环显示扫描线条.
  * @param  None
  * @retval None
  */
 void LCD_Line_Scan(void)
 {
     static uint16_t pos=100;
     /* 选择LCD第二层 */
     LCD_SelectLayer(1);
     //设置图形颜色为透明
     LCD_SetTextColor(TRANSPARENCY);
     //画一条透明颜色的矩形，即清除上一次绘制的矩形
     LCD_FillRect((LCD_PIXEL_WIDTH-Frame_width+20*Frame_line_size)/2,
                  pos,Frame_width-20*Frame_line_size,Frame_line_size);
     //改变线条位置
     pos=pos+Frame_line_size;
     //判断线条是否越界
     if (pos>=((LCD_PIXEL_HEIGHT+Frame_width)/2-5*Frame_line_size)) {
         pos = (LCD_PIXEL_HEIGHT-Frame_width)/2+5*Frame_line_size;
     }
     //设置图形颜色为红色
     LCD_SetTextColor(LCD_COLOR_RED);
     LCD_FillRect((LCD_PIXEL_WIDTH-Frame_width+20*Frame_line_size)/2,
                  pos,Frame_width-20*Frame_line_size,Frame_line_size);
 } 
