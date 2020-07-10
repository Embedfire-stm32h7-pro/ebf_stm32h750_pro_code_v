/**
  ******************************************************************
  * @file    bsp_sai.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   按键应用bsp（扫描模式）
  ******************************************************************
  * @attention
  *
  * 实验平台:野火 STM32H750开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************
  */ 
#include "./sai/bsp_sai.h" 
#include "./Recorder/Recorder.h"
SAI_HandleTypeDef h_sai;
SAI_HandleTypeDef h_saib;
DMA_HandleTypeDef h_txdma;   //DMA发送句柄
DMA_HandleTypeDef h_rxdma;   //DMA发送句柄
void (*SAI_DMA_TX_Callback)(void);
/**
  * @brief  SAI_MSP_Init
  * @param  无
  * @retval 无
  */
void SAI_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  //Msp Clock Init
  SAI_LRC_GPIO_CLK_ENABLE();
  SAI_BCLK_GPIO_CLK_ENABLE();
  SAI_SDA_GPIO_CLK_ENABLE();
  SAI_MCLK_GPIO_CLK_ENABLE();  
  
  //SAI1_blockA
  GPIO_InitStruct.Pin = SAI_LRC_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = SAI_LRC_GPIO_AF;
  HAL_GPIO_Init(SAI_LRC_PORT, &GPIO_InitStruct);  
  
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Alternate = GPIO_AF6_SAI1;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = SAI_BCLK_PIN;
  GPIO_InitStruct.Alternate = SAI_BCLK_GPIO_AF;
  HAL_GPIO_Init(SAI_BCLK_PORT, &GPIO_InitStruct);    

  GPIO_InitStruct.Pin = SAI_SDA_PIN;
  GPIO_InitStruct.Alternate = SAI_SDA_GPIO_AF;
  HAL_GPIO_Init(SAI_SDA_PORT, &GPIO_InitStruct);  

  GPIO_InitStruct.Pin = SAI_SAD_PIN;
  GPIO_InitStruct.Alternate = SAI_SDA_GPIO_AF;
  HAL_GPIO_Init(SAI_SAD_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = SAI_MCLK_PIN;
  GPIO_InitStruct.Alternate = SAI_MCLK_GPIO_AF;
  HAL_GPIO_Init(SAI_MCLK_PORT, &GPIO_InitStruct);     
}

void BSP_AUDIO_OUT_ClockConfig(uint32_t AudioFreq)
{
  RCC_PeriphCLKInitTypeDef RCC_ExCLKInitStruct; 
	RCC_ExCLKInitStruct.PeriphClockSelection=RCC_PERIPHCLK_SAI1;
	RCC_ExCLKInitStruct.Sai1ClockSelection=RCC_SAI1CLKSOURCE_PLL2;  
  RCC_ExCLKInitStruct.PLL2.PLL2M=25;
  switch(AudioFreq)
  {
    case SAI_AUDIO_FREQUENCY_44K :
    {
      RCC_ExCLKInitStruct.PLL2.PLL2N=344;
      RCC_ExCLKInitStruct.PLL2.PLL2P=2;      
      break;
    }
    default:
      while(1);
  }
  HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStruct);
  
}


/**
  * @brief  SAI_BlockA_Init
  * @param  无
  * @retval 无
  */
void SAIxA_Tx_Config(const uint16_t _usStandard, const uint16_t _usWordLen, const uint32_t _usAudioFreq)
{
  BSP_AUDIO_OUT_ClockConfig(_usAudioFreq);
  SAI_CLK_ENABLE();
  h_sai.Instance = SAI1_Block_A;
  
  h_sai.Init.AudioMode = SAI_MODEMASTER_TX;//配置为发送模式
  h_sai.Init.Synchro = SAI_ASYNCHRONOUS; //模块内部为异步
  h_sai.Init.OutputDrive = SAI_OUTPUTDRIVE_ENABLE;//立刻输出
  h_sai.Init.NoDivider=SAI_MASTERDIVIDER_ENABLE;//使能MCK输出NOACK
  h_sai.Init.FIFOThreshold=SAI_FIFOTHRESHOLD_1QF;//1/4FIFO
  h_sai.Init.MonoStereoMode=SAI_STEREOMODE;
  h_sai.Init.AudioFrequency = _usAudioFreq;
  h_sai.Init.Protocol=SAI_FREE_PROTOCOL;           //设置SAI1协议为:自由协议(支持I2S/LSB/MSB/TDM/PCM/DSP等协议)
  h_sai.Init.DataSize=SAI_DATASIZE_16;                     //设置数据大小
  h_sai.Init.FirstBit=SAI_FIRSTBIT_MSB;            //数据MSB位优先
  h_sai.Init.ClockStrobing=SAI_CLOCKSTROBING_RISINGEDGE;                   //数据在时钟的上升/下降沿选通
  
  //帧设置
  h_sai.FrameInit.FrameLength=32;                  
  h_sai.FrameInit.ActiveFrameLength=16;            
  h_sai.FrameInit.FSDefinition=SAI_FS_CHANNEL_IDENTIFICATION;//FS信号为SOF信号+通道识别信号
  h_sai.FrameInit.FSPolarity=SAI_FS_ACTIVE_LOW;    //FS低电平有效(下降沿)
  h_sai.FrameInit.FSOffset=SAI_FS_BEFOREFIRSTBIT;  //在slot0的第一位的前一位使能FS,以匹配飞利浦标准	

  //SLOT设置
  h_sai.SlotInit.FirstBitOffset=0;                 //slot偏移(FBOFF)为0
  h_sai.SlotInit.SlotSize=SAI_SLOTSIZE_16B;        //slot大小为32位
  h_sai.SlotInit.SlotNumber=2;                     //slot数为2个    
  h_sai.SlotInit.SlotActive=SAI_SLOTACTIVE_0|SAI_SLOTACTIVE_1;//使能slot0和slot1
    
  HAL_SAI_Init(&h_sai);                            //初始化SAI
  
//  HAL_SAI_InitProtocol(&h_sai, SAI_I2S_STANDARD, _usWordLen, 2);//2--left_channel and right_channel
	SAI1_Block_A->CR1|=1<<17;			    //写入CR1寄存器中
  __HAL_SAI_ENABLE(&h_sai);
}

void SAIxB_Rx_Config(const uint16_t _usStandard, const uint16_t _usWordLen, const uint32_t _usAudioFreq)
{
  h_saib.Instance = SAI1_Block_B;
  
  h_saib.Init.AudioMode = SAI_MODESLAVE_RX;//配置为发送模式
  h_saib.Init.Synchro = SAI_SYNCHRONOUS; //模块内部为异步
  h_saib.Init.OutputDrive = SAI_OUTPUTDRIVE_ENABLE;//立刻输出
  h_saib.Init.NoDivider=SAI_MASTERDIVIDER_ENABLE;//使能MCK输出NOACK
  h_saib.Init.FIFOThreshold=SAI_FIFOTHRESHOLD_1QF;//1/4FIFO
  h_saib.Init.MonoStereoMode=SAI_STEREOMODE;
  h_saib.Init.AudioFrequency = _usAudioFreq;

  h_saib.Init.Protocol=SAI_FREE_PROTOCOL;          //设置SAI1协议为:自由协议(支持I2S/LSB/MSB/TDM/PCM/DSP等协议)
  h_saib.Init.DataSize=SAI_DATASIZE_16;                    //设置数据大小
  h_saib.Init.FirstBit=SAI_FIRSTBIT_MSB;           //数据MSB位优先
  h_saib.Init.ClockStrobing=SAI_CLOCKSTROBING_RISINGEDGE;        //数据在时钟的上升/下降沿选通

  //帧设置
  h_saib.FrameInit.FrameLength=32;                 //设置帧长度为64,左通道32个SCK,右通道32个SCK.
  h_saib.FrameInit.ActiveFrameLength=16;           //设置帧同步有效电平长度,在I2S模式下=1/2帧长.
  h_saib.FrameInit.FSDefinition=SAI_FS_CHANNEL_IDENTIFICATION;//FS信号为SOF信号+通道识别信号
  h_saib.FrameInit.FSPolarity=SAI_FS_ACTIVE_LOW;   //FS低电平有效(下降沿)
  h_saib.FrameInit.FSOffset=SAI_FS_BEFOREFIRSTBIT; //在slot0的第一位的前一位使能FS,以匹配飞利浦标准	

  //SLOT设置
  h_saib.SlotInit.FirstBitOffset=0;                //slot偏移(FBOFF)为0
  h_saib.SlotInit.SlotSize=SAI_SLOTSIZE_16B;       //slot大小为32位
  h_saib.SlotInit.SlotNumber=2;                    //slot数为2个    
  h_saib.SlotInit.SlotActive=SAI_SLOTACTIVE_0|SAI_SLOTACTIVE_1;//使能slot0和slot1  
  
  HAL_SAI_Init(&h_saib);                            //初始化SAIB
	SAI1_Block_B->CR1|=1<<17;		 
  __HAL_SAI_ENABLE(&h_saib);
}

void SAIA_TX_DMA_Init(uint32_t buffer0,uint32_t buffer1,const uint32_t num)
{
  DMA_CLK_ENABLE();
  
  h_txdma.Instance = DMA_Instance;
  h_txdma.Init.Request = DMA_REQUEST_SAI1_A;
  h_txdma.Init.Direction = DMA_MEMORY_TO_PERIPH;
  h_txdma.Init.PeriphInc = DMA_PINC_DISABLE;
  h_txdma.Init.MemInc = DMA_MINC_ENABLE;
  h_txdma.Init.Mode = DMA_CIRCULAR;
  h_txdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  h_txdma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD ;
  h_txdma.Init.FIFOMode = DMA_FIFOMODE_DISABLE ;
  h_txdma.Init.Priority = DMA_PRIORITY_HIGH;
  h_txdma.Init.MemBurst=DMA_MBURST_SINGLE;             //存储器单次突发传输
  h_txdma.Init.PeriphBurst=DMA_PBURST_SINGLE;          //外设突发单次传输 
  
  __HAL_LINKDMA(&h_sai,hdmatx,h_txdma);        //将DMA与SAI联系起来
  HAL_DMA_Init(&h_txdma);	                            //初始化DMA
  
  __HAL_DMA_ENABLE_IT(&h_txdma,DMA_IT_TC);
  HAL_DMAEx_MultiBufferStart(&h_txdma,(uint32_t)buffer0,(uint32_t)&SAI1_Block_A->DR,(uint32_t)buffer1,num);//开启双缓冲
  
  HAL_NVIC_SetPriority(DMA_IRQn,0,0);                    //DMA中断优先级
  HAL_NVIC_EnableIRQ(DMA_IRQn);

}

void SAIB_RX_DMA_Init(uint32_t buffer0,uint32_t buffer1,const uint32_t num)
{
  DMA_CLK_ENABLE();
  
  h_rxdma.Instance = DMA1_Stream3;
  h_rxdma.Init.Request = DMA_REQUEST_SAI1_B;
  h_rxdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
  h_rxdma.Init.PeriphInc = DMA_PINC_DISABLE;
  h_rxdma.Init.MemInc = DMA_MINC_ENABLE;
  h_rxdma.Init.Mode = DMA_CIRCULAR;
  h_rxdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  h_rxdma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD ;
  h_rxdma.Init.FIFOMode = DMA_FIFOMODE_DISABLE ;
  h_rxdma.Init.Priority = DMA_PRIORITY_HIGH;
  h_rxdma.Init.MemBurst=DMA_MBURST_SINGLE;             //存储器单次突发传输
  h_rxdma.Init.PeriphBurst=DMA_PBURST_SINGLE;          //外设突发单次传输 
  
  __HAL_LINKDMA(&h_sai,hdmarx,h_rxdma);        //将DMA与SAI联系起来
  HAL_DMA_Init(&h_rxdma);	                            //初始化DMA
  
  __HAL_DMA_ENABLE_IT(&h_rxdma,DMA_IT_TC);
  HAL_DMAEx_MultiBufferStart(&h_rxdma,(uint32_t)&SAI1_Block_B->DR,(uint32_t)buffer0,(uint32_t)buffer1,num);//开启双缓冲
  
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn,0,0);                    //DMA中断优先级
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
}

void SAI_Play_Stop(void)
{
  //__HAL_SAI_DISABLE(&h_sai);
  __HAL_DMA_DISABLE(&h_txdma);
}


void SAI_Play_Start(void)
{

  __HAL_DMA_ENABLE(&h_txdma);//开启DMA TX传输 
}


//SAI开始录音
void SAI_Rec_Start(void)
{ 
  __HAL_DMA_ENABLE(&h_rxdma);	//开启DMA RX传输      		
}

//关闭SAI录音
void SAI_Rec_Stop(void)
{   
  __HAL_DMA_DISABLE(&h_rxdma);	//结束录音    
}
/**
* @brief  DMA conversion complete callback. 
* @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
*                the configuration information for the specified DMA module.
* @retval None
*/
void SAI_DMAConvCplt(DMA_HandleTypeDef *hdma)
{
    MusicPlayer_SAI_DMA_TX_Callback();
}
void SAIB_DMAConvCplt(DMA_HandleTypeDef *hdma)
{
    MusicPlayer_SAI_DMA_RX_Callback();
}
/**
	* @brief  SPIx_TX_DMA_STREAM中断服务函数
	* @param  无
	* @retval 无
	*/
void SAI_TX_DMA_STREAM_IRQFUN(void)
{  
	//执行回调函数,读取数据等操作在这里面处理	
	h_txdma.XferCpltCallback = SAI_DMAConvCplt;
	h_txdma.XferM1CpltCallback = SAI_DMAConvCplt;
	HAL_DMA_IRQHandler(&h_txdma);   	
	
} 
void SAI_RX_DMA_STREAM_IRQFUN(void)
{  
	//执行回调函数,读取数据等操作在这里面处理	
	h_rxdma.XferCpltCallback = SAIB_DMAConvCplt;
	h_rxdma.XferM1CpltCallback = SAIB_DMAConvCplt;
	HAL_DMA_IRQHandler(&h_rxdma);   	
	
} 

