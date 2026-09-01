#ifndef __FF_USER_H
#define __FF_USER_H
#include "rundata.h"


void ff_init(void);
s32 ff_saverunlog(RUNLOG_STRUCT * runlog_par);

//s32 ff_readrunlog(RUNLOG_STRUCT * runlog_par);
u32 GetCSVrows(char * str);
u32 GetrunlogbinRows(void);
u32 GetoperalogbinRows(void);
void GetAllCSVrows(void);
s32 ff_readrunlog(RUNLOG_STRUCT * runlog_par,u32 rows);
s32 ff_readrunlog2(RUNLOG_STRUCT * runlog_par,u32 rows);
s32 ff_readrunlog3(RUNLOG_STRUCT * runlog_par,u32 rows);
void RunLog_Par_make(void);
#define DISK_SD       "0:"
typedef enum {
	OPERA_LOGIN,   //登录
	/***********************************************************/
	OPERA_DEV_NAME, //修改产品名称
	OPERA_VERSION, //修改软件版本
	OPERA_MOTOR_POWER,//修改电机功率//motor_power
	OPERA_FACTORY_TIME, //修改安装时间
	OPERA_DEV_CODE,   //设备编号
	OPERA_FORM_FACTOR, //修改外形尺寸
	OPERA_SENDNEWADDR,  //修改上传地址
	/*********************************/
	OPERA_WATER_INLETWIDTH, //修改进水宽度
	OPERA_WATER_INLETHEIGHT, //修改进水高度
	OPERA_ULTRA_NUM,     //修改探头层数
	OPERA_ULTRA_INITIALDIST, //修改探头初距
	OPERA_ULTRA_SPACING,  //修改探头间距
	OPERA_GRADIENT,       //修改渠道比降
	OPERA_FROHEIGHT_AIR,  //修改渠道净空
	OPERA_STATISTICAL_TIME,//修改统计时刻   ///Statistical_Time
	OPERA_BASE_HEIGHTDIFF, //修改底板高差//base_heightDiff
	/******************************************************************/
	OPERA_FLOW_MIN,       //修改最小流量
	OPERA_FLOW_MAX,       //修改最大流量
	OPERA_FLOWDEVIATION, //修改流量偏差
	
	OPERA_LEVEL_MIN,     //修改最小水位
	OPERA_LEVEL_MAX,     //修改最大水位
	OPERA_LEVEL_DEVIATON, //修改水位偏差
	
	OPERA_DATA_INTERVAL,  //修改数据间隔
	OPERA_COLLECT_INTERVAL,  //修改采集间隔
	OPERA_SEND_INTERVAL, //修改发送间隔
	OPERA_CHANNEL_ANGLE,   //修改声道角度
	
	OPERA_SIDE_K,          //修改边壁系数
	OPERA_CORRECT_K,       //修改流量系数
	OPERA_ROUGHNESS,       //修改明渠糙率
	/******************************************************************/
	OPERA_ADMIN_PASS,      //修改管理员密码
	OPERA_OPERA1_PASS,     //修改操作员1密码
	OPERA_OPERA2_PASS,     //修改操作员2密码
	OPERA_MAINT_PASS,      //修改维护员密码
	OPERA_TEMPO_PASS,      //修改临时身份密码
	
	OPERA_ADMIN_PHONE,     //修改管理员手机号
	OPERA_OPERA1_PHONE,    //修改操作员1手机号
	OPERA_OPERA2_PHONE,    //修改操作员2手机号
	OPERA_MAINT_PHONE,     //修改维护员手机号
	OPERA_TEMPO_PHONE,     //修改临时手机号
	/******************************************************************/
	OPERA_CLEAN_RUNLOG,    //清空运行数据
	OPERA_CLEAN_OPERALOG,  //清空操作记录
	/******远程升级记录************************************************************/
	OPERA_UPGRADE,         //远程升级
	REMOTO_CONTROL_RUN,   //远程控制运行
	REMOTO_CONTROL_MODE,   //远程控制修改控制模式
	REMOTO_CONTROL_OPENING,   //远程控制修改开度
	REMOTO_CONTROL_LEVEL,  	 //远程控制修改水位
	REMOTO_CONTROL_FLOW,   	//远程控制修改流量
	/******远程升级记录************************************************************/
	OPERA_OPENING_FUN,         //修改开度功能
	OPERA_LEVEL_FUN,           //修改水位功能
	OPERA_FLOW_FUN,            //修改流量功能
	OPERA_4G_FUN,              //修改4G功能
	OPERA_LORA_FUN            //修改Lora功能
	
} OPERALOGTYPE;
void SaveOPERA(OPERALOGTYPE type,void * dat1,void * dat2);
s32 ff_readoperalog(OPERALOG_STRUCT * operalog_par,u32 rows);
void ff_cleanrunlog(void);
void ff_cleanoperalog(void);
u8 exf_getfree(char *drv,u32 *total,u32 *free);
void ff_saverunlogtest();
s32 ff_test_save(RUNLOG_STRUCT * runlog_par);
s32 ff_test_read(char * runlog_par,u32 rows);
#define SDFF_xBlockTime 3000
extern u32 ff_total,ff_free;

#define FF_CSV_LEN  200
extern char ff_csv_New[FF_CSV_LEN];
extern char ff_csv_Old[FF_CSV_LEN];
void ff_csv_make(void);
#endif
