/*
******************************************************************************
* @file    recorder.c
* @author  fire
* @version V1.0
* @date    2015-xx-xx
* @brief   WM8978录音功能测试
******************************************************************************
* @attention
*
* 实验平台:野火  STM32 H750 开发板  
* 论坛    :http://www.firebbs.cn
* 淘宝    :https://fire-stm32.taobao.com
*
******************************************************************************
*/
#include "./usart/bsp_debug_usart.h"
#include "./key/bsp_key.h" 
#include "./wm8978/bsp_wm8978.h"
//#include "./touch_pad/bsp_touchpad.h"
//#include "./systick/bsp_SysTick.h"
#include "ff.h" 
#include "./Recorder/Recorder.h"
#include "./delay/core_delay.h" 
#include "./sai/bsp_sai.h" 
/* 音频格式切换列表(可以自定义) */
#define RECORDERDIR	"0:/recorder"
#define FMT_COUNT	6		/* 音频格式数组元素个数 */
static REC_TYPE Recorder;          /* 录音设备 */
static uint8_t Isread=0;           /* DMA传输完成标志 */
static uint8_t bufflag=0;          /* 数据缓存区选择标志 */
static WavHead rec_wav;            /* WAV设备  */
static uint32_t wavsize=0;         /* wav音频数据大小 */
FIL file;											/* file objects */
FRESULT result; 
UINT bw;            					/* File R/W count */
uint16_t buffer0[RECBUFFER_SIZE];  /* 数据缓存区1 ，实际占用字节数：RECBUFFER_SIZE*2 */
uint16_t buffer1[RECBUFFER_SIZE];  /* 数据缓存区2 ，实际占用字节数：RECBUFFER_SIZE*2 */

uint32_t g_FmtList[FMT_COUNT][3] =
{
	{SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_16BIT, SAI_AUDIO_FREQUENCY_44K},
	{SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_16BIT, SAI_AUDIO_FREQUENCY_44K},
	{SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_16BIT, SAI_AUDIO_FREQUENCY_44K},
	{SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_16BIT, SAI_AUDIO_FREQUENCY_44K},
	{SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_16BIT, SAI_AUDIO_FREQUENCY_44K},
	{SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_16BIT, SAI_AUDIO_FREQUENCY_44K},
};

/**
  * @brief   WAV格式音频播放主程序
	* @note   
  * @param  无
  * @retval 无
  */
int i = 0;
uint16_t data[100];
int Buffer[100];
void fill(int *p)
{
  int i = 0;
  for(;i < 100; i++)
    Buffer[i] = i;
}
/**
  * @brief  显示当前状态
  * @param  无
  * @retval 无
  */
static void DispStatus(void)
{
	char buf[100];
	printf("\n    按下KEY2键开始录音    \n");
	printf(  "   按下电容按键开始回放    \n");	
	printf(  "      按下KEY1键结束    \n\r");
	/* 显示当前音频格式 */
	sprintf(buf, "音频格式: Philips,16Bit,%d.%dkHz     ",
		g_FmtList[Recorder.ucFmtIdx][2]/1000,
		(g_FmtList[Recorder.ucFmtIdx][2]%1000)/100
		);
	printf("%s\n",buf);

	if (Recorder.ucStatus == STA_IDLE)
	{
		printf("状态 = 空闲    ");
	}
	else if (Recorder.ucStatus == STA_RECORDING)
	{
		printf("状态 = 正在录音");
	}
	else if (Recorder.ucStatus == STA_PLAYING)
	{
		printf("状态 = 正在回放");
	}

	sprintf(buf, "  增益 = %d ", Recorder.ucGain);
	printf("%s\n",buf);

	sprintf(buf, "耳机音量 = %d \r", Recorder.ucVolume);
	printf("%s\n",buf);
}
void StartPlay(const char *filename)
{
	printf("当前播放文件 -> %s\n",filename);
	
	result=f_open(&file, "0:FatFs读写测试文件.wav", FA_OPEN_EXISTING | FA_READ);
	if(result!=FR_OK)
	{
		printf("打开音频文件失败!!!->%d\r\n",result);
		result = f_close (&file);
		Recorder.ucStatus = STA_ERR;
		return;
	}
	//读取WAV文件头
	result = f_read(&file,&rec_wav,sizeof(rec_wav),&bw);
	//先读取音频数据到缓冲区
	result = f_read(&file,(uint16_t *)buffer0,RECBUFFER_SIZE*2,&bw);
	result = f_read(&file,(uint16_t *)buffer1,RECBUFFER_SIZE*2,&bw);
	
	HAL_Delay(10);	/* 延迟一段时间，等待I2S中断结束 */
	SAI_Play_Stop();			/* 停止I2S录音和放音 */
	wm8978_Reset();		/* 复位WM8978到复位状态 */	
	wm8978_CtrlGPIO1(1);
	Recorder.ucStatus = STA_PLAYING;		/* 放音状态 */

	/* 配置WM8978芯片，输入为DAC，输出为耳机 */
	wm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);
	/* 调节音量，左右相同音量 */
	wm8978_SetOUT1Volume(Recorder.ucVolume);
	/* 配置WM8978音频接口为飞利浦标准I2S接口，16bit */
	wm8978_CfgAudioIF(SAI_I2S_STANDARD, 16);
	
	SAIxA_Tx_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);
//	I2Sxext_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);
	
//	I2Sxext_RX_DMA_Init((uint32_t)&recplaybuf[0],(uint32_t)&recplaybuf[1],1);
//	
//  I2Sxext_Recorde_Stop();
	
	SAIA_TX_DMA_Init((uint32_t)buffer0,(uint32_t)buffer1,RECBUFFER_SIZE);		
	SAI_Play_Start();
}
void RecorderDemo(void)
{
	uint8_t i;
	uint8_t ucRefresh;	/* 通过串口打印相关信息标志 */
	DIR dir;
	
	Recorder.ucStatus=STA_IDLE;    /* 开始设置为空闲状态  */
	Recorder.ucInput=0;            /* 缺省MIC输入  */
	Recorder.ucFmtIdx=3;           /* 缺省飞利浦I2S标准，16bit数据长度，44K采样率  */
	Recorder.ucVolume=35;          /* 缺省耳机音量  */
	if(Recorder.ucInput==0) //MIC 
	{
		Recorder.ucGain=50;          /* 缺省MIC增益  */
		rec_wav.wChannels=2;         /* 缺省MIC单通道 */
	}
	else                    //LINE
	{
		Recorder.ucGain=6;           /* 缺省线路输入增益 */
		rec_wav.wChannels=2;         /* 缺省线路输入双声道 */
	}
	
	rec_wav.riff=0x46464952;       /* “RIFF”; RIFF 标志 */
	rec_wav.size_8=0;              /* 文件长度，未确定 */
	rec_wav.wave=0x45564157;       /* “WAVE”; WAVE 标志 */ 
	
	rec_wav.fmt=0x20746d66;        /* “fmt ”; fmt 标志，最后一位为空 */
	rec_wav.fmtSize=16;            /* sizeof(PCMWAVEFORMAT) */ 
	rec_wav.wFormatTag=1;          /* 1 表示为PCM 形式的声音数据 */ 
	/* 每样本的数据位数，表示每个声道中各个样本的数据位数。 */
	rec_wav.wBitsPerSample=16;
	/* 采样频率（每秒样本数） */
	rec_wav.dwSamplesPerSec=g_FmtList[Recorder.ucFmtIdx][2];
	/* 每秒数据量；其值为通道数×每秒数据位数×每样本的数据位数／ 8。 */
	rec_wav.dwAvgBytesPerSec=rec_wav.wChannels*rec_wav.dwSamplesPerSec*rec_wav.wBitsPerSample/8;  
	/* 数据块的调整数（按字节算的），其值为通道数×每样本的数据位值／8。 */
	rec_wav.wBlockAlign=rec_wav.wChannels*rec_wav.wBitsPerSample/8; 
	
	rec_wav.data=0x61746164;       /* “data”; 数据标记符 */
	rec_wav.datasize=0;            /* 语音数据大小 目前未确定*/
  
	/*  如果路径不存在，创建文件夹  */
	//result = f_opendir(&dir,RECORDERDIR);
//	while(result != FR_OK)
//	{
//		f_mkdir(RECORDERDIR);
//		result = f_opendir(&dir,RECORDERDIR);
//	}	
  SAI_GPIO_Config();
  //SAI_Play_Stop();
  SAIxA_Tx_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);
	
  SAI_DMA_TX_Callback=MusicPlayer_SAI_DMA_TX_Callback;
  ucRefresh = 1;
	bufflag=0;
	Isread=0;
	/* 进入主程序循环体 */
	while (1)
	{
		/* 如果使能串口打印标志则打印相关信息 */
		if (ucRefresh == 1)
		{						
			DispStatus();		/* 显示当前状态，频率，音量等 */
			ucRefresh = 0;
		}
		if(Recorder.ucStatus == STA_IDLE)
		{				
//			/*  KEY2开始录音  */
//			if(Key_Scan(KEY2_GPIO_PORT,KEY2_PIN)==KEY_ON)
//			{
//				/* 寻找合适文件名 */
//				for(i=1;i<0xff;++i)
//				{
//					sprintf(recfilename,"0:/recorder/rec%03d.wav",i);
//					result=f_open(&file,(const TCHAR *)recfilename,FA_READ);
//					if(result==FR_NO_FILE)break;					
//				}
//				f_close(&file);
//				
//				if(i==0xff)
//				{
//					Recorder.ucStatus =STA_ERR;
//					continue;
//				}
//				/* 开始录音 */
//				StartRecord(recfilename);
//				ucRefresh = 1;
//			}
//			/*  TouchPAD开始回放录音  */
//			if(TPAD_Scan(0))
//			{			
//				/* 开始回放 */
        StartPlay("0:rec002.wav");
				ucRefresh = 1;				
//			}
		}
//		else
//		{			
//			/*  KEY1停止录音或回放  */
//			if(Key_Scan(KEY1_GPIO_PORT,KEY1_PIN)==KEY_ON)
//			{
//				/* 对于录音，需要把WAV文件内容填充完整 */
//				if(Recorder.ucStatus == STA_RECORDING)
//				{
//					I2Sxext_Recorde_Stop();
//					I2S_Play_Stop();
//					rec_wav.size_8=wavsize+36;
//					rec_wav.datasize=wavsize;
//					result=f_lseek(&file,0);
//					result=f_write(&file,(const void *)&rec_wav,sizeof(rec_wav),&bw);
//					result=f_close(&file);
//					printf("录音结束\r\n");
//				}
//				ucRefresh = 1;
//				Recorder.ucStatus = STA_IDLE;		/* 待机状态 */
//				I2S_Stop();		/* 停止I2S录音和放音 */
//				wm8978_Reset();	/* 复位WM8978到复位状态 */
//			}
//		}
		/* DMA传输完成 */
		if(Isread==1)
		{
			Isread=0;
			switch (Recorder.ucStatus)
			{
				case STA_RECORDING:  // 录音功能，写入数据到文件           
						if(bufflag==0){
              result=f_write(&file,buffer0,RECBUFFER_SIZE*2,(UINT*)&bw);//写入文件
            }              
						else{  
              result=f_write(&file,buffer1,RECBUFFER_SIZE*2,(UINT*)&bw);//写入文件.         
            }
						wavsize+=RECBUFFER_SIZE*2;	
					break;
				case STA_PLAYING:   // 回放功能，读取数据到播放缓冲区
						if(bufflag==0){
							result = f_read(&file,buffer0,RECBUFFER_SIZE*2,&bw);
            }
						else
            {
							result = f_read(&file,buffer1,RECBUFFER_SIZE*2,&bw);

            }
						/* 播放完成或读取出错停止工作 */
						if((result!=FR_OK)||(file.fptr==file.fsize))
						{
							printf("播放完或者读取出错退出...->%d\r\n", result);
							SAI_Play_Stop();
							file.fptr=0;
							f_close(&file);
							Recorder.ucStatus = STA_IDLE;		/* 待机状态 */
							SAI_Play_Start();		/* 停止I2S录音和放音 */
							wm8978_Reset();	/* 复位WM8978到复位状态 */							
						}		
					break;
			}			
		}
		
	}

  
}


/* DMA发送完成中断回调函数 */
/* 缓冲区内容已经播放完成，需要切换缓冲区，进行新缓冲区内容播放 
   同时读取WAV文件数据填充到已播缓冲区  */
void MusicPlayer_SAI_DMA_TX_Callback(void)
{
	if(Recorder.ucStatus == STA_PLAYING)
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
}
///***************************** (END OF FILE) *********************************/
