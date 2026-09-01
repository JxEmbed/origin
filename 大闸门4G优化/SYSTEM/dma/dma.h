#ifndef __DMA_H
#define __DMA_H
#include "sys.h"
#define SEND_BUF_SIZE 20
typedef struct
{
	uint16_t ptr;
	char buff[SEND_BUF_SIZE];
}MYDMA_typedef;

extern MYDMA_typedef MYDMA_struct;


extern char USART1_TxBuff[10];
void MYDMA_Config(DMA_Channel_TypeDef *DMA_Streamx,u32 par,u32 mar,u16 ndtr);
void MYDMA_Enable(void);

void DMA_SetCurrDataCounter(DMA_Channel_TypeDef* DMAy_Streamx, uint16_t Counter);
void DMA_AddData(uint8_t dat);
void TX_32(void);
#endif

