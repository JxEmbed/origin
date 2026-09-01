#ifndef __LORA_H
#define __LORA_H

#include "main.h"
#include "LoraProtocol.h"
#include "middleware.h"
/*
2021年9月12日20:46:42
*/
/*定义主机模块地址 信道*/
#define ALLCHANNEL   0x32
#define GATECHANNEL  0x33
#define ALLAddr    0x0a0a
/**********************************************/
#define Lora_PowerEN           PDout(4)
#define LoraM0   PDout(0)
#define LoraM1   PDout(1)
#define LoraAUX  PAin(0)

void Lora_init(void);
void Lora_ModePin_init(void);
s8 LoraConf(u16 addr,u8 channel);
void Lora_PWR_init(void);
void Lora_SendStr(char * str);
void Lora_Process(void);
u8 Lora_Rxanalysis(void);
void Lora_SendAllData(void);
void Lora_Packet(char *str);
void LoraSetMode(uint8_t mode); //设置工作模式
void Lora_Send(u8 * buff, u32 len);
#define Lora_RxbuffLen 200
extern u8 Lora_Rxbuff[Lora_RxbuffLen];
extern u8 Lora_RxFlag;
extern u16 Lora_RxBuffPtr;
extern u8 Lora_Txbuff[Lora_RxbuffLen];

//union
//{
//	
//	u8 Lora_Txbuff[Lora_RxbuffLen];
//}lora_TxStruct;
#endif



