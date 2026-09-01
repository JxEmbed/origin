#ifndef __DEV_DATA_H
#define __DEV_DATA_H
#include "main.h"

typedef struct
{
	char dev_name[31]; // 设备名称  最多存储10个字 15
	DateTimeDef factory_time;  //出厂时间
	char dev_code[21];    		//设备编号
	
	char software_version[10];  //软件版本
	char sim_number[25];   //sim卡号
	char form_factor[10];  //外形尺寸
	
	char motor_power[10];  //电机功率
	char longitude[10];    //经度
	char  latitude[10];    //纬度
	
	/*---------------------*/
	float water_inletWidth;  //进水宽度
	float water_inletHeight; //进水高度
	float Ultra_Spacing;     //探头间距
	float base_heightDiff;   //底板高差
	float Ultra_initialDist; //探头初距
	float Ultra_Num;         //探头数量
	
	float FroHeight_Air;        //闸前净空
	float AftHeight_Air;        //闸后净空
	float Gradient;            //渠底比降
	u8 statistical_time;
	/*---------------------*/
	float flow_min;         //最小流量
	float flow_max;         //最大流量
	float flow_deviation;     //流量偏差
	float Side_K;           //边壁系数
	
	float level_min;        //最小水位
	float level_max;        //最大水位
	float level_deviation;  //水位偏差
	float Correct_K;        //修正系数
	
	float data_interval;    //数据间隔			存数据的间隔
	float send_interval;  //发送间隔				给云平台发送数据间隔	
	float collect_interval;	//采集间隔			后水位和超声波的采集间隔
	float Channel_Angle;    //声道角度
	float Roughness;        //明渠糙率 //Roughness of open channel
	
	char send_netaddr[25]; //上传地址
	char send_port[8];     //发送端口
	u8 parSetFlag;
	
	uint8_t runCurrent:5;  //运行电流  占用5个位，范围0到31
	uint32_t TCOOLTHRS;  //TCOOLTHRS
	int8_t   SGT:7;        //SGT
	uint16_t AMAX;          //加速度
	
	float Safe_SpaceHigh;  //安全超高
	
	u8 Opening_Fun;		//开度功能标志位
	u8 Level_Fun;		//水位功能标志位
	u8 Flow_Fun;		//测流功能标志位
	u8 _4G_Fun;			//4G功能标志位
	u8 Lora_Fun;		//Lora功能标志位
	
	/*
		某一功能使用那种设备
		水位设备种类		 0默认A01-防水一体化模组	1是雷达水位计
		超声波设备种类	 0默认测流槽		1是巴歇尔槽(外部设备时需要输入此设备的设备编码)	
	*/
	u8 TypeLevel;		
	u8 TypeUltra;		
	char Externdev_code[21];	//外部设备的设备编码
	char Upload_URL[15];		//上传地址IP
	
	float SetLevel_Dev;  		//定水位误差
	float SetFlow_Dev;   		//定流量误差
	u8 StallGuard_Switch; 		//堵转开关
	float StallGuard_SpeedThreshold; //堵转速度阈值
	u8 StallGuard_Secs;  		//堵转监测秒数
}DEV_STRUCT;


typedef struct
{
	u8 admin_pass[10];  //管理员密码
	u8 opera1_pass[10];  //操作员1密码
	u8 opera2_pass[10];  //操作员2密码
	u8 maint_pass[10];  //维护员密码
	u8 tempo_pass[10];  //临时身份密码
	
	char admin_phone[15]; //管理员手机
	char opera1_phone[15]; //操作员1手机
	char opera2_phone[15]; //操作员2手机
	char maint_phone[15];  //维护员手机号
	char tempo_phone[15];  //临时身份密码
}Author_STRUCT;

typedef struct
{
	s32 run_step;       //运行的步数
	s32 sum_step;  //总步进
	u32 run_speed; //期望闸门运行速度
	
	
}RUNMESS_STRUCT;

typedef struct
{
	u8 control_mode;    //控制模式			0是开度；1是水位；2是流量
	u8 control_run;     //控制开始标志位		0是未控制；1是控制状态
	float set_opening;  //闸门开度
	float set_openingm; //闸门设定开度m
	float set_level;    //定水位
	float set_flow;     //定流量
	u8 setState;        //设定状态
	u8 nowState;        //当前状态
	u8 upFlag;          //闸门在最上面标志位
	u8 downFlag;        //闸门在最下面标志位
	/****************
蒋祥新增
*/
	u8 set_direction;	
	u32 ZONE_ID;						//该设备属于哪个区    在每次接受控制命令时进行赋值	
	u32 GROUP_ID;
	u32 DEV_ID;
	u8 manual_run;
	u8 fixed_level_run;
	u8 fixed_flow_run;
	u8 scheduled_run;		//定时运行标志
	float scheduled;			//定时控制的时间		单位秒				可存储49715 天 22 小时
	float	fixed_flow;			//定流量控制流量
	float fixed_level;
}CONTROL_STRUCT;  //控制参数
//typedef struct
//{
//	u8 runflag;        //运行标志位
//	u8 moto_pul_h;
//	u8 upflag;         //闸门在最上面标志位
//	u8 downflag;       //闸门在最下面标志位
//	
//	u8 modir;          //电机旋转方向  =0:向上； =1:向下
//	
//}motor_struct;

//extern MOTOR_STRUCT Motor_Action;
extern RUNMESS_STRUCT Mess_Par;
extern Author_STRUCT Author_Par;
extern CONTROL_STRUCT Control_Par;  //控制参数




extern DEV_STRUCT dev_SavePar;  //设备存储的信息
void Zhangbaoqu_DevPar(void);
#endif
