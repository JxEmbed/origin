#ifndef __GD32F20X_RCU_ADD_H
#define __GD32F20X_RCU_ADD_H

#include "main.h"

typedef struct
{
  uint32_t SYSCLK_Frequency;  /*!< returns SYSCLK clock frequency expressed in Hz */
  uint32_t HCLK_Frequency;    /*!< returns HCLK clock frequency expressed in Hz */
  uint32_t PCLK1_Frequency;   /*!< returns PCLK1 clock frequency expressed in Hz */
  uint32_t PCLK2_Frequency;   /*!< returns PCLK2 clock frequency expressed in Hz */
}RCC_ClocksTypeDef;

extern RCC_ClocksTypeDef rccClock;

void RCC_GetClocksFreq(RCC_ClocksTypeDef* RCC_Clocks);
#endif




