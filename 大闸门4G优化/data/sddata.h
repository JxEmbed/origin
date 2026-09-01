#ifndef __SDDATA_H
#define __SDDATA_H

#include "type.h"
#include "RTC.h"

typedef struct
{
	u32 total;
	u32 free1;
	u32 user;
}SD_STRUCT;

//985088
//985088*512
#define SD_DATALOG         1*512
#define SD_SPEEDLOG        400000*512

/******************************************
存储记录行数
990887936字节=1935328*512
100000000字节
*/
//100000000字节
#define SD_FLOWLOG_POS      0
#define SD_OPERALOG_POS     100000000
#define SD_FLOWLOG_SIZE     100000000
#define SD_OPERALOG_SIZE    100000000
extern SD_STRUCT SD_Par;

void SD_SaveDatalog_Par(u32 ptr);
void SD_ResumeDatalog_Par(u32 ptr);
void SD_SaveOperalog_Par(u32 ptr);
void SD_ResumeOperalog_Par(u32 ptr);
void SD_SaveSpeedlog_Par(u32 ptr);
void SD_ResumeSpeedlog_Par(u32 ptr);
u32 SD_Calcflow_headMax(void);
void SD_Test(void);
u32 SD_Calcopera_headMax(void);
u32 SD_Calc_Log_Ptr(u32 ptr,u32 head,u32 num,u32 max);
void SD_SaveOperaLog(OPERALOG_STRUCT * operalog_par);
void SD_ResumeOperaLog(OPERALOG_STRUCT * operaLog,u32 log_ptr);
void SD_SaveFlowLog(void);
void SD_ResumeFlowLog(RUNLOG_STRUCT * flowLog,u32 log_ptr);
void SD_ClearFlowLog(void);
void SD_ClearOperaLog(void);
#endif
