#ifndef __LIU_H
#define __LIU_H

#include "main.h"
void liu_Lora_ReadTerminal(char *dev_code,char* Dev_type);
u8 Lora_Rxanalysis_liu(void);
u8 Slave_Lora_Map(u8 * rxData,u16 len);
#endif

