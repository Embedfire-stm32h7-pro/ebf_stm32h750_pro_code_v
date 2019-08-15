#ifndef _PALETTE_H
#define _PALETTE_H

#include "stm32h7xx.h"
#include "./lcd/bsp_lcd.h"
 

#define COLOR_BLOCK_WIDTH   lcd_param[cur_lcd].m_palette_btn_width
#define COLOR_BLOCK_HEIGHT  lcd_param[cur_lcd].m_palette_btn_height

#define BUTTON_NUM 18
#define PALETTE_START_Y   0
#define PALETTE_END_Y     LCD_PIXEL_HEIGHT

#if 1     //按钮栏在左边
  #define BUTTON_START_X      0
  #define PALETTE_START_X   COLOR_BLOCK_WIDTH*2+1
  #define PALETTE_END_X     LCD_PIXEL_WIDTH

#else     //按钮栏在右边，(存在触摸按键时也会的bug仅用于测试触摸屏左边界)
  #define BUTTON_START_X      LCD_PIXEL_WIDTH-2*COLOR_BLOCK_WIDTH
  #define PALETTE_START_X   0
  #define PALETTE_END_X     LCD_PIXEL_WIDTH-2*COLOR_BLOCK_WIDTH

#endif


typedef struct 
{
  uint16_t start_x;   //按键的x起始坐标  
  uint16_t start_y;   //按键的y起始坐标
  uint16_t end_x;     //按键的x结束坐标 
  uint16_t end_y;     //按键的y结束坐标
  uint32_t para;      //颜色按钮中表示选择的颜色，笔迹形状按钮中表示选择的画刷
  uint8_t touch_flag; //按键按下的标志
    
  void (*draw_btn)(void * btn);     //按键描绘函数
  void (*btn_command)(void * btn);  //按键功能执行函数，例如切换颜色、画刷
 
}Touch_Button;


/*画刷形状列表*/
typedef enum 
{
  LINE_SINGLE_PIXCEL = 0,   //单像素线
  
  LINE_2_PIXCEL,  //2像素线
  
  LINE_4_PIXCEL,  //4像素线
  
  LINE_6_PIXCEL,  //6像素线
  
  LINE_8_PIXCEL,  //8像素线
  
  LINE_16_PIXCEL, //16像素线
  
  LINE_20_PIXCEL, //20像素线
  
  LINE_WITH_CIRCLE,  //珠子连线
    
  RUBBER,           //橡皮

}SHAPE;

/*画刷参数*/
typedef struct
{
  uint32_t color;
  
  SHAPE  shape;
  
}Brush_Style;

/*brush全局变量在其它文件有使用到*/
extern Brush_Style brush;

  
void Palette_Init(void);
void Touch_Button_Init(void);
void Touch_Button_Down(uint16_t x,uint16_t y);
void Touch_Button_Up(uint16_t x,uint16_t y);
void Draw_Trail(int16_t pre_x,int16_t pre_y,int16_t x,int16_t y,Brush_Style* brush);


#endif //_PALETTE_H

