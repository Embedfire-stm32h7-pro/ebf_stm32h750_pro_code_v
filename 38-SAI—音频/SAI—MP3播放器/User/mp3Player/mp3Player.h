#ifndef __MP3PLAYER_H__
#define __MP3PLAYER_H__

#include <inttypes.h>
#include "./led/bsp_led.h" 

#define Delay_ms	HAL_Delay 	

#define SAI_AUDIOFREQ_DEFAULT   SAI_AUDIO_FREQUENCY_8K

/* 状态 */
enum
{
	STA_IDLE = 0,	/* 待机状态 */
	STA_PLAYING,	/* 放音状态 */
	STA_ERR,			/*  error  */
};

typedef struct
{
	uint8_t ucVolume;			/* 当前放音音量 */
	uint8_t ucStatus;			/* 状态，0表示待机，1表示播放中，2 出错 */	
	uint32_t ucFreq;			/* 采样频率 */
}MP3_TYPE;	

void mp3PlayerDemo(const char *mp3file);
void MusicPlayer_SAI_DMA_TX_Callback(void);
#endif  /* __MP3PLAYER_H__   */

