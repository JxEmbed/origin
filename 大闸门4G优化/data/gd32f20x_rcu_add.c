#include "gd32f20x_rcu_add.h"

RCC_ClocksTypeDef rccClock;
/*
	
*/
void RCC_GetClocksFreq(RCC_ClocksTypeDef* RCC_Clocks)
{
	RCC_Clocks->SYSCLK_Frequency=rcu_clock_freq_get(CK_SYS);	//120MHZ
	RCC_Clocks->HCLK_Frequency=rcu_clock_freq_get(CK_AHB);		//120MHZ
	RCC_Clocks->PCLK1_Frequency=rcu_clock_freq_get(CK_APB1);	//60MHZ
	RCC_Clocks->PCLK2_Frequency=rcu_clock_freq_get(CK_APB2);	//120MHZ
}
