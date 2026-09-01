#include "dma.h"

//DMAx的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMA通道选择,@ref DMA_channel DMA_Channel_0~DMA_Channel_7
//par:外设地址
//mar:存储器地址
//ndtr:数据传输量  

MYDMA_typedef MYDMA_struct;
void MYDMA_Config(DMA_Channel_TypeDef *DMA_Streamx,u32 par,u32 mar,u16 ndtr)
{ 
 
	DMA_InitTypeDef  DMA_InitStructure;  //DMA1 初始化结构体
	NVIC_InitTypeDef NVIC_InitStructure; //中断 初始化结构体
	
	//1）使能 DMA 时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);//DMA1时钟使能
	
	// 2）初始化 DMA 通道 4 参数
  DMA_DeInit(DMA_Streamx);
	DMA_InitStructure.DMA_PeripheralBaseAddr=par;//地址
	DMA_InitStructure.DMA_MemoryBaseAddr=mar;    //存储器基地址
	DMA_InitStructure.DMA_DIR= DMA_DIR_PeripheralDST; //从内存读取发送到外设
	
	DMA_InitStructure.DMA_BufferSize = ndtr; //DMA 通道的 DMA 缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //内存地址递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //8 位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // 8 位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; //工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMA 通道 x 拥有中优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //非内存到内存传输
	DMA_Init(DMA_Streamx, &DMA_InitStructure); //根据指定的参数初始化
	
	//3）使能串口 DMA 发送
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);

	
	//DMA发送中断设置
//    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//		DMA_ITConfig(DMA_Streamx,DMA_IT_TC,ENABLE);
}
//开启一次DMA传输
//DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7 
//ndtr:数据传输量  
void MYDMA_Enable(void)
{
	
//	USART1_TxBuff[0]=ndtr;
	DMA_Cmd(DMA1_Channel4, DISABLE ); //关闭 USART1 TX DMA1 所指示的通道 
	DMA_SetCurrDataCounter(DMA1_Channel4,MYDMA_struct.ptr);//设置 DMA 缓存的大小
	DMA_Cmd(DMA1_Channel4, ENABLE); //使能 USART1 TX DMA1 所指示的通道
	MYDMA_struct.ptr=0;
	delay_ms(20);
//	while(DMA_GetFlagStatus(DMA1_FLAG_TC4)==RESET);
//	DMA_ClearFlag(DMA1_FLAG_TC4);//清除通道 4 传输完成标志
}
void DMA1_Channel4_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC4)!=RESET)
	{
		DMA_Cmd (DMA1_Channel4,DISABLE);//关闭DMA通道
		DMA_ClearFlag(DMA1_IT_TC4);//清中断标志，否则会一直中断
	//        DMA_SetCurrDataCounter(DMA1_Channel4, 2);//重置传输数目，当再次达到这个数目就会进中断
	//        DMA_Cmd(DMA1_Channel4,ENABLE);//开启DMA通道
	}
}

void DMA_SetCurrDataCounter(DMA_Channel_TypeDef* DMAy_Streamx, uint16_t Counter)
{
  /* Check the parameters */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Streamx));

  /* Write the number of data units to be transferred */
  DMAy_Streamx->CNDTR = (uint16_t)Counter;
}
void DMA_AddData(uint8_t dat)
{
	 MYDMA_struct.buff[MYDMA_struct.ptr++]=dat;
	if(MYDMA_struct.ptr>=SEND_BUF_SIZE)
	{
		MYDMA_struct.ptr=0;
	}
}
//void TX_32(void)
//{
//	TX_8(0xff);
//	TX_8(0xfc);
//	TX_8(0xff);
//	TX_8(0xff);
//}

