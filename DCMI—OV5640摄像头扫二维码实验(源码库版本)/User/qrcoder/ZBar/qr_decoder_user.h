#ifndef __QR_DECODER_USER_H
#define	__QR_DECODER_USER_H

#include "stm32h7xx.h"
#include <string.h>
#include <stdio.h>

// 开辟SDRAM的3M字节作为数据缓存，这里使用显存以外的空间，
// 0xD0800000-0x300000 = 0xD0500000
#define  QR_FRAME_BUFFER  ((uint32_t)0xD0500000) 

/*扫描窗口参数*/
#define  Frame_width           ((uint16_t)320)//扫描窗口边长（正方形）

/*扫描框线条参数*/
#define  Frame_line_length     ((uint16_t)30) //扫描框线条长度
#define  Frame_line_size       ((uint16_t)3)  //扫描框线条宽度

#define  QR_SYMBOL_NUM  5    //识别二维码的最大个数
#define  QR_SYMBOL_SIZE 512  //每组二维码的的最大容量

//解码数据封装为二维数组decoded_buf，格式为：
// （第一组：解码类型长度(8bit)+解码类型名称+解码数据长度(16bit,高位在前低位在后)+解码数据）
// （第二组：解码类型长度(8bit)+解码类型名称+解码数据长度(16bit,高位在前低位在后)+解码数据）
//  。。。
//以此类推
extern char decoded_buf[QR_SYMBOL_NUM][QR_SYMBOL_SIZE];

//解码函数，返回值为识别条码的个数
char QR_decoder(void);

//获取一帧图像
void get_image(uint32_t src_addr,uint16_t img_width,uint16_t img_height);
 void LCD_Open_QR_Window(void);
  void LCD_Line_Scan(void);
#endif /* __QR_DECODER_USER_H */
