#ifndef __FLASHDATA_H
#define __FLASHDATA_H

#include "main.h"

#define FLASH_FLASHMAG      1*4*1024    //存储号码
#define FLASH_AUTHOR        6*4*1024    //权限信息
#define FLASH_MESS         11*4*1024    //运行信息
#define FLASH_CONTROL      16*4*1024    //控制参数
#define FLASH_LOGLEN       21*4*1024    //log长度
#define FLASH_DATALOG      26*4*1024    //数据log
#define FLASH_STATUSLOG    36*4*1024    //Status_log
#define FLASH_MOTOR_ACTION 46*4*1024    //电机
#define FLASH_RUN_PAR      56*4*1024    //运行参数
#define FLAG_MYOTA_INFO    58*4*1024    //存储升级参数
#define FLAG_UPDATE_PAR    59*4*1024    //存储升级参数
#define FLAG_ONENET_INFO   60*4*1024    //存储ONENET参数
#define FLASH_LOG_PAR      61*4*1024    //
#define FLASH_ACCWATER_PAR 62*4*1024    //AccWater_Par
//Ultra_Num
#define ULTRA_NUM_MAX 16

void Flash_ResumeDevData(void);
void Flash_ResumeAll(void);
void Flash_ResumeAuthor_Par(void);
void Flash_ResumeMess_Par(void);
void Flash_ResumeControl_Par(void);
void Flash_ResumeDatalog_Par(u16 ptr);
void Flash_ResumeStatlog_Par(u16 ptr);
void Flash_ResumeLoglen_Par(void);
void Flash_ResumeMotor_Action(void);
void Flash_ResumeRun_Par(void);
void Flash_Resumemyota_info(void);
void Flash_ResumeUpdate_Par(void);
void Flash_ResumeONENET_Par(void);
void Flash_ResumeLog_Par(void);
void Flash_ResumeAccWater_Par(void);

void Flash_SaveDevData(void);
void Flash_SaveAuthor_Par(void);
void Flash_SaveDatalog_Par(u16 ptr);
void Flash_SaveStatlog_Par(u16 ptr);
void Flash_SaveControl_Par(void);
void Flash_SaveMess_Par(void);
void Flash_SaveMotor_Action(void);
void Flash_SaveLoglen_Par(void);
void Flash_SaveRun_Par(void);
void Flash_Savemyota_info(void);
void Flash_SaveUpdate_Par(void);
void Flash_SaveONENET_Par(void);
void Flash_SaveLog_Par(void);
void Flash_SaveAccWater_Par(void);
void ClearUpdate_Par(void);
#endif


