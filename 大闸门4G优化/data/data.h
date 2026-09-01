#ifndef __DATA_H
#define __DATA_H

#include "Dev_data.h"
#include "flashdata.h"
#include "rundata.h"
#include "logdata.h"
//#include "sddata.h"
#include "ultrasonic.h"
#include "onenetdata.h"
#include "autoControl.h"
#include "error_data.h"
#include "watLevSafe.h"

extern float TOF1;
extern char imgData[2];
extern uint16_t imgData_Len;
void calc(void);
float Filter_Smooth(float * datbuff,uint8_t num,uint8_t remnum);
void imgTrans(char * imgStr,uint8_t * buff,uint16_t len);
float Filter_Smooth2(uint8_t num);
u8 DateCmp(DateTimeDef dateTime1,DateTimeDef dateTime2);
void flow_Process(void );
void parameter_set(void);
void GetSensor(void);
void CalcAccmuWater(void);
void readflowacc(void);
float Flow_Man_Fun1(float * datbuff,u8 channel,uint8_t remnum);
//#define	Error      0
//#define	ZhangBaoQu 1
//#define	Other      2

//#define DEV_LOCATION ZhangBaoQu

//#define DEV_LOCATION OTHER

extern u8 DEV_isZhang;
typedef struct
{
	double day_water;  //本日水量  8
	double this_water;   //本次水量
	double month_water;  //本月水量
	double year_water;  //本年水量 8
	double accu_water;  //累计水量 8
	DateTimeDef _last_rtc;
}STRUCT_ACCWATER;
typedef struct
{
	float fro_lev;      //前水位
	float aft_lev;      //后水位
	float V_current;    //实时流速
	float Q_current;    //实时流量
	double day_water;  //本日水量  8
	double this_water;   //本次水量
	double month_water;  //本月水量
	double year_water;  //本年水量 8
	double accu_water;  //累计水量 8
}STRUCT_Log_Par;
extern STRUCT_ACCWATER AccWater_Par;
extern STRUCT_Log_Par  Log_Par;
extern u8 calcFlag;
extern float distance;
float qiupingjun(float* buff,u8 num);
#endif

