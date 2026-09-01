#ifndef __LOGDATA_H
#define __LOGDATA_H


#include "type.h"
#include "RTC.h"

typedef struct
{
	u32         rows;  //4
	DateTimeDef time;  //时间  2
	char operastr[100];
}OPERALOG_STRUCT;

typedef struct
{
	DateTimeDef time;
	float fro_level;   //闸前水位
	float aft_level;   //闸后水位
	float opening;     //开度
	float flow;        //实时流量
	double this_water;  //本次水量
	double year_water;  //本年水量
	double accu_water;  //累计水量
	u8 dev_openflag;   //设备开闭
	u8 controlmode;    //控制模式
}DATALOG_STRUCT;

typedef struct
{
	DateTimeDef time;
	float V[9];
}SpeedLOG_STRUCT;

typedef struct
{
	DateTimeDef time;
	u8 fro_level;      //闸前水位
	u8 aft_level;      //闸后水位
	u8 ultra;          //超声
	u8 bluetooth;      //蓝牙
	u8 _4G;            //4G  =0:未知 1正常 2故障
	u8 _sim;           //sim卡状态	0未知  1正常   2未插卡   3无网络   4查询不到SIM卡号
	u8 charge;         //充电
	u8 voltage_flag;   //电压状态
	float voltage;     //电压
	u8 motor;          //电机
	u8 limit;          //限位
	u8 SDState;        //SD卡状态
	u8 GPSState;       //GPS定位状态
	u8 WatLev_Safe;  //超过安全水位  =0:未知，=1:安全，=2:超高
	u8 FloatBall_State; //浮球传感器工作状态 =0:未知，=1:正常，=2:故障
	u8 FloatBall_Safe;      //浮球传感器检测水位状态 0:未知，1:安全，2:超高
	/*
	蒋祥新增
	*/	
	u8 loraState;			//lora状态
}STATUSLOG_STRUCT;

typedef struct 
{
	uint32_t data_head;
//	uint32_t data_tail;
	uint32_t data_len;
	uint32_t status_ptr;
	
	u32 flow_head; //头部指针
	u32 flow_num;  //存储条数
	u32 flow_pageNum; //存储页数
	
	u32 opera_head; //头部指针
	u32 opera_num;  //存储条数
	u32 opera_pageNum; //存储页数
}LOGLEN_STRUCT;

extern DATALOG_STRUCT   Datalog_Par;  //运行数据
extern DATALOG_STRUCT   Datalog_Par_new;//当前数据 
extern SpeedLOG_STRUCT  Speedlog_Par;   //历史数据
extern SpeedLOG_STRUCT  Speedlog_Par_new;   //历史数据
extern STATUSLOG_STRUCT Statlog_Par; //状态数据
extern STATUSLOG_STRUCT Statlog_Par_new; //当前状态
extern LOGLEN_STRUCT    Loglen_Par; //log数据长度
extern OPERALOG_STRUCT     Operalog_Par;
extern OPERALOG_STRUCT     Operalog_Par_new;  //
extern OPERALOG_STRUCT     Operalog_Par_old;  
void LCD_showdLog(void);
void LCD_showstatusLog(void);
#define DataLogLen   200000  //914,285
#define OpearLogLen  200000
#define StatusLogLen 200000
void Datalog_Par_new_make(void);
u32 SD_GetDataLogLen(void);
u32 SD_GetDataLogPage(void);
u32 SD_GetDataLogLen(void);
void LCD_showoperaLog(void);
void LCD_TEST(void);
void LCD_showSpeedLog(void);
void LCD_showNoFlowLog(void);
u32 SD_GetLogPage(u32 log_rows);


extern u32 lcd_head,lcd_tail;
extern uint16_t lcd_i;
extern u32 lcd_len;
extern u32 lcd_show_len;
extern u32 lcd_ptr;
extern u32 lcd_temp;
extern char lcd_datu8[200]; 
extern u8 LCD_showflag;
#endif



