#ifndef __SPI_SD_DMA_H
#define __SPI_SD_DMA_H

#include "main.h"

u8 SD_SPI_DMA_ReadWriteByte(u8 ByteSend);
void spi_dma_test(void);
void spi_dma_A(void);
u8 spi_dma_reset(u8 txbuf);
#endif

