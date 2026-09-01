#ifndef __LCD_H
#define __LCD_H

#include "main.h"

#define LCD_POWER_EN PGout(1)

void LCD_Power_init(void);
void LCD_RS232_ENinit(void);
void LCD_RS232_init(uint32_t baudval);
void LCD_All_init(void);
void LCD_DMA_Rx_Config(void);
void LCD_DMA_init(u32 len);
void UART6_SendDMAConfig(unsigned char *buf,unsigned short int len);
#endif

