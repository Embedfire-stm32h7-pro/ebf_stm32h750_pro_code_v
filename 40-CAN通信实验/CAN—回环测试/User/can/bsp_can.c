/**
  ******************************************************************************
  * @file    bsp_can.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   can驱动（回环模式）
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火STM32 H750 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./can/bsp_can.h"

FDCAN_HandleTypeDef hfdcan;
uint8_t TxData0[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
/*
 * 函数名：CAN_GPIO_Config
 * 描述  ：CAN的GPIO 配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;

	/* 使能引脚时钟 */
	CAN_TX_GPIO_CLK_ENABLE();
	CAN_RX_GPIO_CLK_ENABLE();	

	/* Select PLL1Q as source of FDCANx clock */
	RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
	RCC_PeriphClkInit.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL;
	HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);

	/* 配置CAN发送引脚 */
	GPIO_InitStructure.Pin = CAN_TX_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pull  = GPIO_PULLUP;
	GPIO_InitStructure.Alternate =  GPIO_AF9_FDCAN1;
	HAL_GPIO_Init(CAN_TX_GPIO_PORT, &GPIO_InitStructure);

	/* 配置CAN接收引脚 */
	GPIO_InitStructure.Pin = CAN_RX_PIN ;
	HAL_GPIO_Init(CAN_RX_GPIO_PORT, &GPIO_InitStructure);
}

/*
 * 函数名：CAN_NVIC_Config
 * 描述  ：CAN的NVIC 配置,第1优先级组，0，0优先级
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_NVIC_Config(void)
{
  /* NVIC for FDCANx */
  HAL_NVIC_SetPriority(FDCANx_IT0_IRQn, 0, 1);
  HAL_NVIC_SetPriority(FDCANx_IT1_IRQn, 0, 1);
  HAL_NVIC_SetPriority(FDCAN_CAL_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(FDCANx_IT0_IRQn);
  HAL_NVIC_EnableIRQ(FDCANx_IT1_IRQn);
  HAL_NVIC_EnableIRQ(FDCAN_CAL_IRQn);
}

/*
 * 函数名：CAN_Mode_Config
 * 描述  ：CAN的模式 配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_Mode_Config(void)
{

	/************************CAN通信参数设置**********************************/
	/* 使能CAN时钟 */
	CAN_CLK_ENABLE();

  /* 初始化FDCAN外设工作在环回模式 */
  hfdcan.Instance = CANx;
  hfdcan.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan.Init.Mode = FDCAN_MODE_EXTERNAL_LOOPBACK;
  hfdcan.Init.AutoRetransmission = ENABLE;
  hfdcan.Init.TransmitPause = DISABLE;
  hfdcan.Init.ProtocolException = ENABLE;
  /* 位时序配置:
    ************************
    位时序参数         		| Nominal      |  Data
    ----------------------|--------------|----------------
    CAN子系统内核时钟输入	| 40 MHz       | 40 MHz
    时间常量 (tq)      		| 25 ns        | 25 ns
    同步段    						| 1 tq         | 1 tq
    传播段        				| 23 tq        | 23 tq
    相位段1            		| 8 tq         | 8 tq
    相位段2  	          	| 8 tq         | 8 tq
    同步跳转宽度					| 8 tq         | 8 tq
    位长度         				| 40 tq = 1 us | 40 tq = 1 us
    位速率             		| 1 MBit/s  	 | 1 MBit/s
  */
  hfdcan.Init.NominalPrescaler = 0x1; /* tq = NominalPrescaler x (1/40MHz) */
  hfdcan.Init.NominalSyncJumpWidth = 0x8;
  hfdcan.Init.NominalTimeSeg1 = 0x1F; /* NominalTimeSeg1 = 传播段 + 相位段1 */
  hfdcan.Init.NominalTimeSeg2 = 0x8;
  hfdcan.Init.MessageRAMOffset = 0;
  hfdcan.Init.StdFiltersNbr = 1;
  hfdcan.Init.ExtFiltersNbr = 2;
  hfdcan.Init.RxFifo0ElmtsNbr = 1;
  hfdcan.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan.Init.RxFifo1ElmtsNbr = 2;
  hfdcan.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan.Init.RxBuffersNbr = 4;
  hfdcan.Init.RxBufferSize = FDCAN_DATA_BYTES_8;
  hfdcan.Init.TxEventsNbr = 2;
  hfdcan.Init.TxBuffersNbr = 1;
  hfdcan.Init.TxFifoQueueElmtsNbr = 2;
  hfdcan.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  hfdcan.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
  HAL_FDCAN_Init(&hfdcan);
}

/*
 * 函数名：CAN_Filter_Config
 * 描述  ：CAN的过滤器 配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_Filter_Config(void)
{
	FDCAN_FilterTypeDef  sFilterConfig;

  /* 配置标准ID接收过滤器到Rx缓冲区0 */
  sFilterConfig.IdType = FDCAN_EXTENDED_ID;
  sFilterConfig.FilterIndex = 0;
  sFilterConfig.FilterType = FDCAN_FILTER_DUAL;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXBUFFER;
  sFilterConfig.FilterID1 = 0x2568;
  sFilterConfig.FilterID2 = 0x2568;
  sFilterConfig.RxBufferIndex = 0;
  HAL_FDCAN_ConfigFilter(&hfdcan, &sFilterConfig);
  

}


/*
 * 函数名：CAN_Config
 * 描述  ：完整配置CAN的功能
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void CAN_Config(void)
{
  CAN_GPIO_Config();
  CAN_NVIC_Config();
  CAN_Mode_Config();
  CAN_Filter_Config();

}

/*
 * 函数名：CAN_SetMsg
 * 描述  ：CAN通信报文内容设置,设置一个数据内容为0-7的数据包
 * 输入  ：发送报文结构体
 * 输出  : 无
 * 调用  ：外部调用
 */	 
void CAN_SetMsg(void)
{
	FDCAN_TxHeaderTypeDef TxHeader;	
  /* 配置Tx缓冲区消息 */
  TxHeader.Identifier = 0x2568;
  TxHeader.IdType = FDCAN_EXTENDED_ID;
  TxHeader.TxFrameType = FDCAN_DATA_FRAME;
  TxHeader.DataLength = FDCAN_DLC_BYTES_8;
  TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
  TxHeader.FDFormat = FDCAN_FD_CAN;
  TxHeader.TxEventFifoControl = FDCAN_STORE_TX_EVENTS;
  TxHeader.MessageMarker = 0x01;
  HAL_FDCAN_AddMessageToTxBuffer(&hfdcan, &TxHeader, TxData0, FDCAN_TX_BUFFER0);

  /* 启动FDCAN模块 */
  HAL_FDCAN_Start(&hfdcan);	
	/* 发送缓冲区消息 */
  HAL_FDCAN_EnableTxBufferRequest(&hfdcan, FDCAN_TX_BUFFER0);	 
}



/**************************END OF FILE************************************/
