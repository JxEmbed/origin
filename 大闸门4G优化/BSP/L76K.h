#ifndef __L76K_H
#define __L76K_H

#include "main.h"

#define L76K_EN               PGout(6)		//低电平导通
#define L76K_RESETN           PGout(7)		//高电平导通
#define L76K_WAKEUP           PGout(8)		//高电平导通
void L76K_Start(void);
void L76K_End(void);
void L76K_LowPower(void);
void L76K_init(uint32_t baudval);
void L76K_Send(u8 * buff,u32 len);
void L76K_Test(void);
#define L76K_REBUFF_LEN 500
extern u8 L76K_RxBuff[L76K_REBUFF_LEN];
extern u16 L76K_RxBuffPtr;
extern u8 L76K_RxFlag;
void L76K_Resend(void);
void L76K_SendStr(char * str);
void L76K_REG_Init(void);
typedef struct
{
	char UTC[16];   //定位的 UTC 时间。
//	char Status[5];
	char Lat[16];  //纬度
	char N_S[5];
	char Lon[16];  //经度
	char E_W[5];
	char Quality[5];
	char NumSatUsed[5];
	
	float F_Lat;  //浮点数表示的纬度
	float F_Lon;  //浮点数表示的经度
	
	char Union_Lat[17];  //浮点数表示的纬度
	char Union_Lon[17];  //浮点数表示的经度
}L76K_STRUCT;
extern L76K_STRUCT L76K_Par;
#endif
