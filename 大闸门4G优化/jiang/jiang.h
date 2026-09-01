#ifndef __JIANG_H
#define __JIANG_H
#include "main.h"

#define EXTERNDEV_TYPE  "8"
#define BIG_GATE_TYPE   "7"

//巴歇尔槽配置地址和信道
#define ExternDevAddr    0x0A07
#define ExternDevChannel 0x07


typedef struct
{
//	u32 lora_devcode;
	char dev_name[17]; //最多存储10个字 15
//	DateTimeDef factory_time;  //出厂时间
//	DateTimeDef tempo_time;  //临时账号时间
	char dev_code[14]; //设备编号
	
//	char software_version[10];  //软件版本
//	char sim_number[25];   //sim卡号
//	char longitude[10];    //经度
//	char  latitude[10];    //纬度
//	float interval;					//流速间隔	
//	DateTimeDef base_time;  //基准时间
//	DateTimeDef current_time;  //当前年时间
//	/*---------------------*/
//	float TempData;
//	float RHDData;
//	float PData;
//	float WindData;
//	float RainData;
//	float RadData;
//	float WindDiretData;
//	float ETo_Data;
//	float ShangQing_Data;
//	float Mostrue_Data;
////	float ET0dataLcd;
//	float ET0day_data;
//	/***********************/
//	
//	float Collect_lasttime;//上一次采集步长
//	float Mail_Sendtime;//邮件发送周期
//	float Mail_Sendtime_lasttime;//上一次邮件发送周期

	float	cumulative_flow;								//累计水量
	float Irrigate_quota;									//灌水定额
	float Collect_ultrasonict;						//超声波采集步长
	float Collect_stresst;								//压力采集步长
	float Dev_voltage;											//设备电压
	u32 flow_collectT;			//小闸门超声波采集步长				单位s
	float Gate_b;						//闸门宽度
	float Fro_AirH;
	float Aft_AirH;
	
	u16 AMAX;								//加速度
	u16 VMAX;								//目标速度
	u16 repot_time;					//反馈时间				//0xff不需要反馈			中间值为多长时间反馈			0是结束反馈		
	u8 Dev_Type; 
	u8 Stop_time;						//进入低功耗时间			单位分钟
	u16 Lora_Addr;
}Dev_param;
typedef struct
{
	u16 check;
	u8 control_mode;
	float set_opening;   //闸门开度
	u8 set_direction;
	float Irrigate_quota;									//灌水定额
	float Collect_ultrasonict;						//采集步长
	float Collect_stresst;
	//累计流量
	double cumulative_flow;
	//压力传感器
	float stress;
	//气温
	float tempeture;
	//电池电压
	float voltage;
	float F_Lat;  //浮点数表示的纬度
	float F_Lon;  //浮点数表示的经度
	u8 GPSState;       //GPS定位状态
	u8 loraState;			//lora状态
//	DateTimeDef Irrigate_StartT;
//	DateTimeDef Irrigate_EndT;
	u16 VMAX;
	u16 AMAX;
	u32 flow_collectT;			//小闸门超声波采集步长				单位s
	char dev_code[21]; 		//设备编号
	u16 report_time;
	float Fro_AirH;
	float Aft_AirH;
}LORA_RECORD;
typedef enum {
	MANUAL=1,              //1
	SCHEDULED,             //2
	FIXEDFLOW,             //3
	STOP,                  //4
	FIXEDLEVEL,            //5
	CANCEL,                //6
	CONTINUE               //7
}CONTROL_MODE;
void Lora_SetDevCode(u8 check);
void Lora_Report(void);
void Lora_Send_StartupMess(void);
void Lora_Answer(u8 Dev_type,u8 function,u16 crcRx,float set_direction);
u8 Lora_Rxanalysis_jiang(void);
void Lora_ReadTerminal(char *dev_code,char * Dev_type);
void Lora_SendAddr(char * Dev_Type);
#endif

