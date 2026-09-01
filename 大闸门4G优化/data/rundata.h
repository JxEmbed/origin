#ifndef __RUNDATA_H
#define __RUNDATA_H

#include "type.h"
#include "RTC.h"

#define CHANNEL_MAX  16					//定义最大通道数
typedef struct
{
	u32         rows;  //4
	DateTimeDef time;  //时间  2
	float fro_level;   //渠道水位 4
	float aft_level;   //闸后水位;
	float opening;     //开度
	float flow;        //实时流量 4
	double day_water;  //本日水量  8
	double this_water;   //本次水量
	double month_water;  //本月水量
	double year_water;  //本年水量 8
	double accu_water;  //累计水量 8
	float sensor_V[CHANNEL_MAX]; //传感器流速
}RUNLOG_STRUCT;

typedef enum {
	IDENTITY_NULL=0U,      //空
	IDENTITY_TEMPO = 1U,   //临时身份
	IDENTITY_OPERA1=2U,    //操作员1身份
	IDENTITY_OPERA2,       //操作员2身份
    IDENTITY_ADMIN,        //管理员身份 
	
	IDENTITY_MAINT,        //维护员身份

} identity_enum;
typedef enum{
	ID_NULL=0U,    //空
	ID_TEMPO,      //临时身份
	ID_OPERA,      //操作员
	ID_ADMIN,      //管理员
	ID_MAINT,      //维护员
}id_enum;

typedef struct
{	
	u32 RunLog_ParSave_t;
	u8 RunLog_ParSave_flag;         //数据显示
//	u32 data_t;
//	u8 data_flag;         //数据显示
	u8 opera_page;
	u32 data_page;
	u8 opera_flag;        //存储操作记录
	u32 dataUpload_t;
	u8  dataUpload_flag;  //发送运行数据
	u32 dev_Save_t;
	u8  dev_Save_flag;    //发送存储的设置参数标志位
	u32 checkup_t;        //升级标志位计时
	u8 checkupflag;       //升级标志位
	u32 SendVer_t;    
	u8 SendVerflag;     //发送版本
	u32 checkNet_t;     //
	u8  checkNetflag;   //检查联网状态
	u32 Author_Par_t;   
	u8 Author_Par_flag;   //发送权限信息
	u8 SendControlflag;  //发送控制参数
	
	u32 LCD_timecount;   //LCD液晶屏亮的时间
	u8 mess_flag;        //存储步数
	u8 control_flag;     //存储控制参数
	
	u32 runlog_rows;
	u32 operalog_rows;
	
	identity_enum identity;  //身份
	u8 pass[10];  //密码
	u8 confirmpass[10]; //确认密码
	identity_enum current_identity;  //身份
	u32 up_step;  //向上移动的步进
	uint16_t this_year;   //当前年
	uint16_t last_year;   //上一年
	float opening;  //闸门开度
	float openingm; //以m为单位的闸门开度
//	float this_water;  //本次水量
//	float year_water;  //本年水量
//	float accu_water;  //累计水量
	
	u8 time_OK;      //系统时间正常标志位
	u8 flow_ok;      //流量读取正常
	u32 sensor_get_t;  //传感器读取计时
//	u8 sensor_get_flag; //传感器读取标志位
	u8 flow_flag;
	u8 cleanlog_ptr;
	u8 test_step_flag;  //标志位
	u8 test_step_state; //状态
	u32 test_step_count; //计步
	u8 save_Mess_Par_flag;	//存储电机真实步数的标志位
	u8 data_OK;          //如果数据已经整理完成
/*******************************
蒋祥新增变量
*/	
	u8 Status_Irrigate;			//灌溉状态   0是默认  1是开始灌溉			2是灌溉结束			3是过程反馈
	DateTimeDef Irrigate_StartT;
	DateTimeDef Irrigate_EndT;
	u8 Send_Count;					//重发次数
	u8 report_flag;									//lora发送信息标志位
	u32 reportflag_t;   
	u32 GROUP_ID;
	u32 DEV_ID;
	u32 ZONE_ID;						//该设备属于哪个区    在每次接受控制命令时进行赋值
	u8 stopmotor_flag;							//停止		为1时停止  0为正常工作
	u8 cancelmotor_flag;						//取消
	u8 SetPara_flag;
	//判断重复数据			只在执行功能码判断
	u16 Recive_Crc;
	u16 Enter_StopT;       //进入低功耗模式计时
	u8 Enter_StopF;        //进入低功耗模式标志位
	u8 SetDev_Code;
	u8 fixed_flowStartTF;
	
	u8 Read_AftLF;		   //读取后水位标志位	
	u32 Read_AftLT;
	
	//测试数据
	u32 MqttStatus_SendC;	//确认状态发送次数
	u32 MqttStatus_YC;		//在线次数
	u32 MqttStatus_NC;		//不在线次数
	
	char Externdev_code[21];	//外部设备的设备编码
	u8 Read_ExterndevF;			//读取外部设备标志位
	u32 Read_ExterndevT;	
	u8 Auto_airh;				//净空自测标志位
	u8 L76K_SendF;				//定位标志位
	
	u8 Devcode_SetF;			//0设备编码未设置   1是设备编码已设置
	u32 flow_numMax;
	u32 flow_page;     //流量数据行数
	u32 opera_numMax; //操作记录数最大值
	u8 URC_ComF;		//4G模块收到平台下发指令
}NOSAVE_STRUCT;
/*
u8 admin_pass[10];  //管理员密码
	u8 opera1_pass[10];  //操作员1密码
	u8 opera2_pass[10];  //操作员2密码
	u8 maint_pass[10];  //维护员密码
	u8 tempo_pass[10];  //临时身份密码
*/



extern NOSAVE_STRUCT nosave_par;
void Run_Par_Process(void);
extern RUNLOG_STRUCT RunLog_Par;  //运行数据
extern RUNLOG_STRUCT RunLog_Par_Old;  //运行数据
#endif


