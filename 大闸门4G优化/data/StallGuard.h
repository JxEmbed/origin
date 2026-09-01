#ifndef STALLGUARD_H
#define STALLGUARD_H

#include "main.h"

typedef struct
{
	u8 StallGuard_occur;    //堵转检测发生一次
	u8 StallGuard_flag;     //堵转检测确认			堵转无法处理标志位
	u8 StallGuardCount;     //堵转发生次数
	u8 StallGuardStage;     //堵转所处阶段
	u32 StallGuard_Threshold; //堵转阈值
	u8 stallGuard_count;	//堵转发生秒数
}StallGuard_Struct;

extern StallGuard_Struct StallGuard_Par;
s8 Judge_Stall(void);
void StallGuardResetCheck(void);
#endif

