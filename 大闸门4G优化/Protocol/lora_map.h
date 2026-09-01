#ifndef __LORA_MAP_H
#define __LORA_MAP_H

#include "main.h"


s32 lora_map_GetHoldReg(u16 index,u32 *pOut);
s32 liu_lora_map_SetHoldReg(u32 index,float valuef);
s32 lora_map_SetHoldReg(u32 index,u16 *pOut);
#endif

