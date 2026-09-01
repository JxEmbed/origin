#ifndef MIDDLEWARE_H
#define MIDDLEWARE_H
#include "main.h"
#include "Lora.h"

#define E22_TxBuff  Lora_Txbuff
#define E22_400T22S_SetMode LoraSetMode
#define E22_400T22S_Send    Lora_Send
#define HostAddr    0x0a0a
#define HostChannel    0x02
#define E22_RxBuffPtr Lora_RxBuffPtr
#define E22_RxBuff   Lora_Rxbuff
//大闸门配置地址和信道
#define BIGGateAddr    0x0a07
#define BIGGateChannel 0x07
#endif


