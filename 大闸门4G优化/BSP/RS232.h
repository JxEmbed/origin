#ifndef __RS232_H
#define __RS232_H
#include "main.h"


void RS232_init(uint32_t baudval);
void USART1_Config(uint32_t baudval);
void RS232_ENinit(void);
#endif

