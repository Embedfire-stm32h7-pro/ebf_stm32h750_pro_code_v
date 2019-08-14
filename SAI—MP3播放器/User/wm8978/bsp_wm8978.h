#ifndef __WM8978_H__
#define	__WM8978_H__

#include "stm32h7xx.h"
/*---------------------------------------------------------------------------------------------*/
/*------------------------   I2C控制WM8978配置部分  -------------------------------------------*/
#define WM8978_SLAVE_ADDRESS    0x34	/* WM8978 I2C从机地址 */
/* 定义最大音量 */
#define VOLUME_MAX		63		/* 最大音量 */
#define VOLUME_STEP		1		/* 音量调节步长 */

/* 定义最大MIC增益 */
#define GAIN_MAX		63		/* 最大增益 */
#define GAIN_STEP		1		/* 增益步长 */

/* WM8978 音频输入通道控制选项, 可以选择多路，比如 MIC_LEFT_ON | LINE_ON */
typedef enum
{
	IN_PATH_OFF		= 0x00,	/* 无输入 */
	MIC_LEFT_ON 	= 0x01,	/* LIN,LIP脚，MIC左声道（开发板没用到）*/
	MIC_RIGHT_ON	= 0x02,	/* RIN,RIP脚，MIC右声道（接板载咪头）  */
	LINE_ON			= 0x04, /* L2,R2 立体声输入 */
	AUX_ON			= 0x08,	/* AUXL,AUXR 立体声输入（开发板没用到） */
	DAC_ON			= 0x10,	/* I2S数据DAC (CPU产生音频信号) */
	ADC_ON			= 0x20	/* 输入的音频馈入WM8978内部ADC （I2S录音) */
}IN_PATH_E;

/* WM8978 音频输出通道控制选项, 可以选择多路 */
typedef enum
{
	OUT_PATH_OFF	= 0x00,	/* 无输出 */
	EAR_LEFT_ON 	= 0x01,	/* LOUT1 耳机左声道 */
	EAR_RIGHT_ON	= 0x02,	/* ROUT1 耳机右声道 */
	SPK_ON			= 0x04,	/* LOUT2和ROUT2反相输出单声道,接扬声器 */
	OUT3_4_ON		= 0x08,	/* OUT3 和 OUT4 输出单声道音频*/
}OUT_PATH_E;


/*等待超时时间*/
#define WM8978_I2C_FLAG_TIMEOUT         ((uint32_t)0x4000)
#define WM8978_I2C_LONG_TIMEOUT         ((uint32_t)(10 * WM8978_I2C_FLAG_TIMEOUT))

/*信息输出*/
#define WM8978_DEBUG_ON         1

#define WM8978_INFO(fmt,arg...)           printf("<<-EEPROM-INFO->> "fmt"\n",##arg)
#define WM8978_ERROR(fmt,arg...)          printf("<<-EEPROM-ERROR->> "fmt"\n",##arg)
#define WM8978_DEBUG(fmt,arg...)          do{\
                                          if(EEPROM_DEBUG_ON)\
                                          printf("<<-EEPROM-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

/* 供外部引用的函数声明 */
uint8_t wm8978_Init(void);
uint8_t wm8978_Reset(void);
void wm8978_CfgAudioIF(uint16_t _usStandard, uint8_t _ucWordLen);
void wm8978_OutMute(uint8_t _ucMute);
void wm8978_PowerDown(void);
void wm8978_CfgAudioPath(uint16_t _InPath, uint16_t _OutPath);
void wm8978_SetMicGain(uint8_t _ucGain);
void wm8978_SetLineGain(uint8_t _ucGain);
void wm8978_SetOUT2Volume(uint8_t _ucVolume);
void wm8978_SetOUT1Volume(uint8_t _ucVolume);
uint8_t wm8978_ReadOUT1Volume(void);
uint8_t wm8978_ReadOUT2Volume(void);
void wm8978_NotchFilter(uint16_t _NFA0, uint16_t _NFA1);
void I2S_READ(uint32_t buffer0,uint32_t buffer1,const uint32_t num);
																					
/*---------------------------------------------------------------------------------------------*/
/*------------------------   I2S控制数据传输部分  ---------------------------------------------*/																					
/**
	* I2S总线传输音频数据口线
	* WM8978_LRC    -> PB12/I2S2_WS
	* WM8978_BCLK   -> PD3/I2S2_CK
	* WM8978_ADCDAT -> PC2/I2S2_SD
	* WM8978_DACDAT -> PI3/I2S2_SD
	* WM8978_MCLK   -> PC6/I2S2_MCK
	*/
#define WM8978_CLK_ENABLE()            __SPI2_CLK_ENABLE()
#define WM8978_CLK_DISABLE()           __SPI2_CLK_DISABLE()
#define WM8978_I2Sx_SPI                SPI2

#define WM8978_LRC_GPIO_CLK()          __GPIOB_CLK_ENABLE()
#define WM8978_LRC_PORT            	   GPIOB
#define WM8978_LRC_PIN             	   GPIO_PIN_12
#define WM8978_LRC_AF                  GPIO_AF5_SPI2

#define WM8978_BCLK_GPIO_CLK()         __GPIOD_CLK_ENABLE()
#define WM8978_BCLK_PORT            	 GPIOD
#define WM8978_BCLK_PIN             	 GPIO_PIN_3
#define WM8978_BCLK_AF                 GPIO_AF5_SPI2

#define WM8978_ADCDAT_GPIO_CLK()       __GPIOC_CLK_ENABLE()
#define WM8978_ADCDAT_PORT             GPIOC
#define WM8978_ADCDAT_PIN              GPIO_PIN_2
#define WM8978_ADCDAT_AF               GPIO_AF5_SPI2

#define WM8978_DACDAT_GPIO_CLK()       __GPIOI_CLK_ENABLE()
#define WM8978_DACDAT_PORT             GPIOI
#define WM8978_DACDAT_PIN              GPIO_PIN_3
#define WM8978_DACDAT_AF               GPIO_AF5_SPI2

#define WM8978_MCLK_GPIO_CLK()         __GPIOC_CLK_ENABLE()
#define WM8978_MCLK_PORT            	 GPIOC
#define WM8978_MCLK_PIN             	 GPIO_PIN_6
#define WM8978_MCLK_AF                 GPIO_AF5_SPI2

#define I2Sx_DMA                       DMA1
#define I2Sx_DMA_CLK_ENABLE()          __HAL_RCC_DMA1_CLK_ENABLE()
#define I2Sx_TX_DMA_STREAM             DMA1_Stream4
#define I2Sx_TX_DMA_STREAM_IRQn        DMA1_Stream4_IRQn 

#define I2Sx_RX_DMA_STREAM             DMA1_Stream0
#define I2Sx_RX_DMA_STREAM_IRQn        DMA1_Stream0_IRQn 

static HAL_StatusTypeDef I2Cx_ReadMultiple(I2C_HandleTypeDef *i2c_handler, uint8_t Addr, uint16_t Reg, uint16_t MemAddSize, uint8_t *Buffer, uint16_t Length);
static HAL_StatusTypeDef I2Cx_WriteMultiple(I2C_HandleTypeDef *i2c_handler, uint8_t Addr, uint16_t Reg, uint16_t MemAddSize, uint8_t *Buffer, uint16_t Length);
static HAL_StatusTypeDef I2Cx_IsDeviceReady(I2C_HandleTypeDef *i2c_handler, uint16_t DevAddress, uint32_t Trials);
static void              I2Cx_Error(I2C_HandleTypeDef *i2c_handler, uint8_t Addr);

void wm8978_CtrlGPIO1(uint8_t _ucValue);
extern void (*I2S_DMA_TX_Callback)(void);		//I2S DMA TX回调函数指针  
extern void (*I2S_DMA_RX_Callback)(void);	  //I2S DMA RX回调函数
extern void I2S_DMAConvCplt(DMA_HandleTypeDef *hdma);
extern void I2Sxext_DMAConvCplt(DMA_HandleTypeDef *hdma);

void I2S_GPIO_Config(void);
void I2S_Stop(void);
void I2Sx_Mode_Config(const uint16_t _usStandard, const uint16_t _usWordLen,const uint32_t _usAudioFreq);
void I2Sx_TX_DMA_Init(uint32_t buffer0,uint32_t buffer1,const uint32_t num);
void I2S_Play_Start(void);
void I2S_Play_Stop(void);
extern void I2Sx_TX_DMA_STREAM_IRQFUN(void);
extern void I2Sx_RX_DMA_STREAM_IRQFUN(void);
void I2Sxext_Mode_Config(const uint16_t _usStandard, const uint16_t _usWordLen,const uint32_t _usAudioFreq);
void I2Sxext_RX_DMA_Init(uint32_t buffer0,uint32_t buffer1,const uint32_t num);
void I2Sxext_Recorde_Start(void);
void I2Sxext_Recorde_Stop(void);

#define DMA_MAX_SZE          0xFFFF
#define DMA_MAX(x)           (((x) <= 0xFFFF)? (x):0xFFFF)
#define AUDIODATA_SIZE        2   /* 16-bits audio data size */


uint32_t AUDIO_Play(uint16_t* pBuffer, uint32_t Size);
uint32_t AudioFlashPlay(uint16_t* pBuffer, uint32_t FullSize, uint32_t StartAdd);

#endif /* __WM8978_H__ */
