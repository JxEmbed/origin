#ifndef __RS485_H
#define __RS485_H

#include "main.h"

#define FRO_H_ADDR 0x01
#define AFT_H_ADDR 0x02

//A01-防水一体化模组空高寄存器值
#define ULTRA_VALUEREG  0x0100
//雷达水位计空高寄存器值
#define RADAR_VALUEREG  0x0A0F

#define Code_ReadKeepReg  0x03
#define Code_ReadInputReg 0x04
#define Code_WriteKeepReg 0x06

#define RS485_TIMEOUT 55
#define RS485_TX_SIZE   20
#define RS485_MAX_SIZE  100
#define RS485_xBlockTime 1000
#define SD_xBlockTime 1000
#define SDFF_xBlockTime 3000

#define DE_EN485 PAout(1)
void USART6_Config(uint32_t baudval);
void RS485_SendBuff(u8 * buff,u32 len);
s8 RS485_RxProcess(u8 addr,u8 fun,u16 offset,u16 dat);
s8 RS485_ReadKeepReg(u8 dev_addr,u16 reg_addr,u16 num);

extern u32 RS485_RxPtr;                 //接收指针
extern volatile u8 RS485_RxState;                //接收状态
extern u8 RS485_RxBuff[RS485_MAX_SIZE]; //接收数组
void RS485_Resend(void);
void RS485_init(uint32_t baudval);
void RS485_WriteREG(void);
s8 RS485_QueryVersion(void);
s8 RS485_StartPhoto(void);
void RS485_TakePhoto(void);
void RS485_ReadLevel(void);
void RS485_power_init(void);
void Calc_Wat_head(void);
typedef struct
{
	//水位设备485  设备地址	寄存器地址
//	u8  Addr_DevLevel;
	u16 Addr_ValueReg;
	
	float Fro_hs;
	float Aft_hs; 
	float Fro_h;    //闸前水深
	float Aft_h;    //闸后水深
	float Wat_head; //过水水头
	u8 Fro_state;
	u8 Aft_state;
	/*
蒋祥新增
	*/	
	//压力传感器
	float stress;
	//气温
	float tempeture;
}RS485_Struct;

extern RS485_Struct RS485_Sensor;
float Fro_h_Process(float f_re_in);
float Aft_h_Process(float f_re_in);
#endif

