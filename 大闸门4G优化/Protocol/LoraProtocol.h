#ifndef __LORAPROTOCOL_H
#define __LORAPROTOCOL_H

#include "main.h"
#include "lora_map.h"
#include "RS485.h"

#define LORA_PROTOCOL_VER    "01"
#define LORA_LOCAL_CODE    "101"
#define LORA_HEX_LEN       40        //定义的最大hex长度


u16 Str2Hex(unsigned char *outHex, unsigned char *inStr, int inStrLen);
s16 Slave_Check(u8 * buff,u16 len);
s32 GetHR(u8 *rxData,u8 *txData,u32 pDatalen);
s32 SetHR(u8 *rxData,u8 *txData,u32 pDatalen);
s32 SetMultiHR(u8 *rxData,u8 *txData,u32 pDatalen);
u16 Hex2Str(unsigned char *inHex, char *outStr,int inHexLen);
void Hex2StrTest(void);
u8 Lora_Rxanalysis(void);
bool fromCharHex(unsigned char c, unsigned char *outChar);
extern char lora_Master_Code[100];
//extern unsigned char outHex[LORA_HEX_LEN];
extern u8 txHex[RS485_TX_SIZE];
extern char txStr[RS485_TX_SIZE];
extern u8   LoraHex[LORA_HEX_LEN];  //lora协议中存储Hex数组
extern char LoraStr[LORA_HEX_LEN*2];  //lora协议中存储String
#endif

