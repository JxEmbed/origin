#ifndef __SPI_SD_H
#define __SPI_SD_H

#include "main.h"
#include "spi_sd_dma.h"

void spi0_init(void);
void SD_SPI_Init(void);
u8 SD_Select(void);
void SD_DisSelect(void);
#endif

