#ifndef __ERROR_DATA_H
#define __ERROR_DATA_H

#include "main.h"
typedef enum {
	SD_UNKNOWN,             //未知
	SD_OK = 1,				/* (0) Succeeded */
	SD_NO_EXIST,			/* (1) A hard error occurred in the low level disk I/O layer */
	SD_FAT_ERROR,           /*fat挂载失败*/
} SD_FRESULT;
/*
堵转三次后才会确认
*/

typedef struct
{
//	u8 StallGuard_occur;    //堵转检测发生一次
//	u8 StallGuard_flag;     //堵转检测确认
//	u8 StallGuardCount;     //堵转发生次数
//	u8 StallGuardStage;     //堵转所处阶段
//	u32 StallGuard_Threshold; //堵转阈值
	u8 SD_flag;             //SD卡故障标志位
}ERROR_DATA_STRUCT;

typedef struct
{
	u8 SD_exist;
	u8 FAT_OK;
}SD_State_STRUCT;

extern ERROR_DATA_STRUCT Error_Par;

extern SD_State_STRUCT   SD_State;
#endif

