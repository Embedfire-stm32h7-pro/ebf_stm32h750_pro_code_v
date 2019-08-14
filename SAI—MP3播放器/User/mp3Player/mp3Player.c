/*
******************************************************************************
* @file    recorder.c
* @author  fire
* @version V1.0
* @date    2015-xx-xx
* @brief   WM8978放音功能测试+mp3解码
******************************************************************************
* @attention
*
* 实验平台:野火  STM32 H750 开发板  
* 论坛    :http://www.chuxue123.com
* 淘宝    :http://firestm32.taobao.com
*
******************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include "./usart/bsp_debug_usart.h"
#include "./wm8978/bsp_wm8978.h"
#include "ff.h" 
#include "./mp3Player/mp3Player.h"
#include "mp3dec.h"
#include "./sai/bsp_sai.h" 
/* 推荐使用以下格式mp3文件：
 * 采样率：44100Hz
 * 声  道：2
 * 比特率：320kbps
 */

/* 处理立体声音频数据时，输出缓冲区需要的最大大小为2304*16/8字节(16为PCM数据为16位)，
 * 这里我们定义MP3BUFFER_SIZE为2304，实际输出缓冲区为MP3BUFFER_SIZE*2个字节
 */
#define MP3BUFFER_SIZE  2304		
#define INPUTBUF_SIZE   3000	

static HMP3Decoder		Mp3Decoder;			/* mp3解码器指针	*/
static MP3FrameInfo		Mp3FrameInfo;		/* mP3帧信息  */
static MP3_TYPE mp3player;         /* mp3播放设备 */
volatile uint8_t Isread=0;           /* DMA传输完成标志 */
static uint8_t bufflag=0;          /* 数据缓存区选择标志 */

uint32_t led_delay=0;

uint8_t inputbuf[INPUTBUF_SIZE]={0};        /* 解码输入缓冲区，1940字节为最大MP3帧大小  */
__attribute__((at(0x30000000))) short outbuffer[2][MP3BUFFER_SIZE];  /* 解码输出缓冲区，也是I2S输入数据，实际占用字节数：RECBUFFER_SIZE*2 */

FIL file;											/* file objects */
FRESULT result; 
UINT bw;            					/* File R/W count */


/**
  * @brief   MP3格式音频播放主程序
  * @param  无
  * @retval 无
  */
int times = 0;
void mp3PlayerDemo(const char *mp3file)
{
	uint8_t *read_ptr=inputbuf;
	uint32_t frames=0;
	int err=0, i=0, outputSamps=0;	
	int	read_offset = 0;				/* 读偏移指针 */
	int	bytes_left = 0;					/* 剩余字节数 */	
	
	mp3player.ucFreq = SAI_AUDIOFREQ_DEFAULT;
	mp3player.ucStatus = STA_IDLE;
	mp3player.ucVolume = 40;
	
	result=f_open(&file,mp3file,FA_READ);
	if(result!=FR_OK)
	{
		printf("Open mp3file :%s fail!!!->%d\r\n",mp3file,result);
		result = f_close (&file);
		return;	/* 停止播放 */
	}
	printf("当前播放文件 -> %s\n",mp3file);
	
	//初始化MP3解码器
	Mp3Decoder = MP3InitDecoder();	
	if(Mp3Decoder==0)
	{
		printf("初始化helix解码库设备\n");
		return;	/* 停止播放 */
	}
	printf("初始化中...\n");
	
	Delay_ms(10);	/* 延迟一段时间，等待I2S中断结束 */
	wm8978_Reset();		/* 复位WM8978到复位状态 */
	/* 配置WM8978芯片，输入为DAC，输出为耳机 */
	wm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);

	/* 调节音量，左右相同音量 */
	wm8978_SetOUT1Volume(mp3player.ucVolume);

	/* 配置WM8978音频接口为飞利浦标准I2S接口，16bit */
	wm8978_CfgAudioIF(SAI_I2S_STANDARD, 16);
	
	/*  初始化并配置I2S  */
  SAI_Play_Stop();
	SAI_GPIO_Config();
//	SAIxA_Tx_Config(SAI_I2S_STANDARD,SAI_PROTOCOL_DATASIZE_16BIT,mp3player.ucFreq);	
//	SAIA_TX_DMA_Init();	
	
	bufflag=0;
	Isread=0;
	
	mp3player.ucStatus = STA_PLAYING;		/* 放音状态 */
  result=f_read(&file,inputbuf,INPUTBUF_SIZE,&bw);
	if(result!=FR_OK)
	{
		printf("读取%s失败 -> %d\r\n",mp3file,result);
		MP3FreeDecoder(Mp3Decoder);
		return;
	}
	read_ptr=inputbuf;
	bytes_left=bw;
	/* 进入主程序循环体 */
	while(mp3player.ucStatus == STA_PLAYING)
	{
		read_offset = MP3FindSyncWord(read_ptr, bytes_left);	//寻找帧同步，返回第一个同步字的位置
		if(read_offset < 0)										//没有找到同步字
		{
			result=f_read(&file,inputbuf,INPUTBUF_SIZE,&bw);
			if(result!=FR_OK)
			{
				printf("读取%s失败 -> %d\r\n",mp3file,result);
				break;
			}
			read_ptr=inputbuf;
			bytes_left=bw;
			continue;		//跳出循环2，回到循环1	
		}
		read_ptr += read_offset;				//偏移至同步字的位置
		bytes_left -= read_offset;				//同步字之后的数据大小	
		if(bytes_left < 1024)							//补充数据
		{
			/* 注意这个地方因为采用的是DMA读取，所以一定要4字节对齐  */
			i=(uint32_t)(bytes_left)&3;			//判断多余的字节
			if(i) i=4-i;						//需要补充的字节
			memcpy(inputbuf+i, read_ptr, bytes_left);	//从对齐位置开始复制
			read_ptr = inputbuf+i;										//指向数据对齐位置
			result = f_read(&file, inputbuf+bytes_left+i, INPUTBUF_SIZE-bytes_left-i, &bw);//补充数据
			if(result!=FR_OK)
			{
				printf("读取%s失败 -> %d\r\n",mp3file,result);
				break;
			}
			bytes_left += bw;		//有效数据流大小
		}
		err = MP3Decode(Mp3Decoder, &read_ptr, &bytes_left, outbuffer[bufflag], 0);	//bufflag开始解码 参数：mp3解码结构体、输入流指针、输入流大小、输出流指针、数据格式
		frames++;	
		if (err != ERR_MP3_NONE)	//错误处理
		{
			switch (err)
			{
				case ERR_MP3_INDATA_UNDERFLOW:
					printf("ERR_MP3_INDATA_UNDERFLOW\r\n");
					result = f_read(&file, inputbuf, INPUTBUF_SIZE, &bw);
					read_ptr = inputbuf;
					bytes_left = bw;
					break;		
				case ERR_MP3_MAINDATA_UNDERFLOW:
					/* do nothing - next call to decode will provide more mainData */
					printf("ERR_MP3_MAINDATA_UNDERFLOW\r\n");
					break;		
				default:
					printf("UNKNOWN ERROR:%d\r\n", err);		
					// 跳过此帧
					if (bytes_left > 0)
					{
						bytes_left --;
						read_ptr ++;
					}	
					break;
			}
			Isread=1;
		}
		else		//解码无错误，准备把数据输出到PCM
		{
			MP3GetLastFrameInfo(Mp3Decoder, &Mp3FrameInfo);		//获取解码信息				
			/* 输出到DAC */
			outputSamps = Mp3FrameInfo.outputSamps;							//PCM数据个数
			if (outputSamps > 0)
			{
				if (Mp3FrameInfo.nChans == 1)	//单声道
				{
					//单声道数据需要复制一份到另一个声道
					for (i = outputSamps - 1; i >= 0; i--)
					{
						outbuffer[bufflag][i * 2] = outbuffer[bufflag][i];
						outbuffer[bufflag][i * 2 + 1] = outbuffer[bufflag][i];
					}
					outputSamps *= 2;
				}//if (Mp3FrameInfo.nChans == 1)	//单声道
			}//if (outputSamps > 0)
			
			/* 根据解码信息设置采样率 */
			if (Mp3FrameInfo.samprate != mp3player.ucFreq)	//采样率 
			{
				mp3player.ucFreq = Mp3FrameInfo.samprate;
				
				printf(" \r\n Bitrate       %dKbps", Mp3FrameInfo.bitrate/1000);
				printf(" \r\n Samprate      %dHz", mp3player.ucFreq);
				printf(" \r\n BitsPerSample %db", Mp3FrameInfo.bitsPerSample);
				printf(" \r\n nChans        %d", Mp3FrameInfo.nChans);
				printf(" \r\n Layer         %d", Mp3FrameInfo.layer);
				printf(" \r\n Version       %d", Mp3FrameInfo.version);
				printf(" \r\n OutputSamps   %d", Mp3FrameInfo.outputSamps);
				printf("\r\n");
				if(mp3player.ucFreq >= SAI_AUDIOFREQ_DEFAULT)	//I2S_AudioFreq_Default = 2，正常的帧，每次都要改速率
				{
          
					SAIxA_Tx_Config(SAI_I2S_STANDARD,SAI_PROTOCOL_DATASIZE_16BIT,mp3player.ucFreq);						//根据采样率修改iis速率
          SAIA_TX_DMA_Init((uint32_t)(&outbuffer[0]),(uint32_t)&outbuffer[1],outputSamps);
				}

				SAI_Play_Start();
			}
		}//else 解码正常
		
		if(file.fptr==file.fsize) 		//mp3文件读取完成，退出
		{
			printf("END\r\n");
			break;
		}	

		while(Isread==0)
		{
			led_delay++;
			if(led_delay==0xffffff)
			{
				led_delay=0;
//				LED4_TOGGLE;
			}
			//Input_scan();		//等待DMA传输完成，此间可以运行按键扫描及处理事件
		}
		Isread=0;
	}
	SAI_Play_Stop();
	mp3player.ucStatus=STA_IDLE;
	MP3FreeDecoder(Mp3Decoder);
	f_close(&file);	
}

/* DMA发送完成中断回调函数 */
/* 缓冲区内容已经播放完成，需要切换缓冲区，进行新缓冲区内容播放 
   同时读取WAV文件数据填充到已播缓冲区  */
void MusicPlayer_SAI_DMA_TX_Callback(void)
{

  if(DMA_Instance->CR&(1<<19)) //当前使用Memory1数据
  {
    bufflag=0;                       //可以将数据读取到缓冲区0
  }
  else                               //当前使用Memory0数据
  {
    bufflag=1;                       //可以将数据读取到缓冲区1
  }
  Isread=1;                          // DMA传输完成标志
	
}

/***************************** (END OF FILE) *********************************/
