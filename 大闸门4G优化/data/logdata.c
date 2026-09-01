#include "logdata.h"
#include "hmi_driver.h"
//#include "stdio.h"
#include "string.h"
#include "math.h"
#include "printf-stdarg.h"
#include "rundata.h"
#include "ff_user.h"
#include "cmd_process.h"
#include "sddata.h"
DATALOG_STRUCT   Datalog_Par;  //'
DATALOG_STRUCT   Datalog_Par_new;//当前数据 
SpeedLOG_STRUCT  Speedlog_Par;   //历史数据
SpeedLOG_STRUCT  Speedlog_Par_new;   //历史数据
STATUSLOG_STRUCT Statlog_Par; //状态数据
STATUSLOG_STRUCT Statlog_Par_new; //当前状态
LOGLEN_STRUCT    Loglen_Par; //log数据长度
OPERALOG_STRUCT     Operalog_Par;
OPERALOG_STRUCT     Operalog_Par_old;  // //
OPERALOG_STRUCT OperaLog_Olds[10];
RUNLOG_STRUCT FlowLog_Olds[10];
void LCD_TEST(void)
{
	SD_Read((u8 *)&Datalog_Par,SD_DATALOG+sizeof(Datalog_Par)*0,sizeof(Datalog_Par));
}
static u32 lcd_head,lcd_tail;
static uint16_t lcd_i=0;
static u32 lcd_len;
static u32 lcd_show_len;
static u32 lcd_ptr=0;
static u32 lcd_temp;
char lcd_datu8[200];  //
u8 LCD_showflag=0;
void LCD_showNoFlowLog(void)
{
	uint16_t lcd_i=0;

	if(nosave_par.flow_page>Loglen_Par.flow_pageNum)
	{
		nosave_par.flow_page=Loglen_Par.flow_pageNum;
	}
	if(nosave_par.flow_page<1)
	{
		nosave_par.flow_page=1;
	}
	if(nosave_par.flow_page<Loglen_Par.flow_pageNum)
	{
		lcd_show_len=10;
	}
	else
	{
		lcd_show_len=Loglen_Par.flow_num-(nosave_par.flow_page-1)*10;
	}
	//数量10-()
	lcd_ptr=Loglen_Par.flow_num-(nosave_par.flow_page-1)*10; //指针
	for(lcd_i=0;lcd_i<lcd_show_len;lcd_i++)
	{
		SD_ResumeFlowLog(&FlowLog_Olds[lcd_i],lcd_ptr--);
	}
	Record_Clear(current_screen_id,7);
	Record_Add(current_screen_id,7,(u8 *)"时间;闸前水位m;闸后水位m;开度m;;;;");
	for(lcd_i=0;lcd_i<lcd_show_len;lcd_i++)
	{
		memset(ff_csv_Old,0,sizeof(ff_csv_Old));
		
		snprintf(ff_csv_Old,sizeof(ff_csv_Old),"%02d-%02d-%02d\r\n%02d:%02d:%02d;",
		FlowLog_Olds[lcd_i].time.years,FlowLog_Olds[lcd_i].time.months,FlowLog_Olds[lcd_i].time.days,
		FlowLog_Olds[lcd_i].time.hours,FlowLog_Olds[lcd_i].time.minutes,FlowLog_Olds[lcd_i].time.secs);
		snprintf(ff_csv_Old,sizeof(ff_csv_Old),"%s%.3f;%.3f;%.3f;;;;",ff_csv_Old,
		FlowLog_Olds[lcd_i].fro_level,
		FlowLog_Olds[lcd_i].aft_level,
		FlowLog_Olds[lcd_i].opening);
		Record_Add(current_screen_id,7,(u8 *)ff_csv_Old);	
	}
}
void LCD_showdLog(void)
{
	uint16_t lcd_i=0;
	
	if(nosave_par.flow_page>Loglen_Par.flow_pageNum)
	{
		nosave_par.flow_page=Loglen_Par.flow_pageNum;
	}
	if(nosave_par.flow_page<1)
	{
		nosave_par.flow_page=1;
	}
	if(nosave_par.flow_page<Loglen_Par.flow_pageNum)
	{
		lcd_show_len=10;
	}
	else
	{
		lcd_show_len=Loglen_Par.flow_num-(nosave_par.flow_page-1)*10;
	}
	//数量10-()
	lcd_ptr=Loglen_Par.flow_num-(nosave_par.flow_page-1)*10; //指针
	for(lcd_i=0;lcd_i<lcd_show_len;lcd_i++)
	{
		SD_ResumeFlowLog(&FlowLog_Olds[lcd_i],lcd_ptr--);
	}
	Record_Clear(current_screen_id,7);
	Record_Add(current_screen_id,7,(u8 *)"时间;闸前水位m;闸后水位m;开度m;实时流量m3/s;本次水量m3;累计水量m3;");
	for(lcd_i=0;lcd_i<lcd_show_len;lcd_i++)
	{
		memset(ff_csv_Old,0,sizeof(ff_csv_Old));
		
		snprintf(ff_csv_Old,sizeof(ff_csv_Old),"%02d-%02d-%02d\r\n%02d:%02d:%02d;",
		FlowLog_Olds[lcd_i].time.years,FlowLog_Olds[lcd_i].time.months,FlowLog_Olds[lcd_i].time.days,
		FlowLog_Olds[lcd_i].time.hours,FlowLog_Olds[lcd_i].time.minutes,FlowLog_Olds[lcd_i].time.secs);
		snprintf(ff_csv_Old,sizeof(ff_csv_Old),"%s%.3f;%.3f;%.3f;%.3f;%.3lf;%.3lf;",ff_csv_Old,
		FlowLog_Olds[lcd_i].fro_level,
		FlowLog_Olds[lcd_i].aft_level,
		FlowLog_Olds[lcd_i].opening,
		FlowLog_Olds[lcd_i].flow,
		FlowLog_Olds[lcd_i].this_water,
		FlowLog_Olds[lcd_i].accu_water);
		Record_Add(current_screen_id,7,(u8 *)ff_csv_Old);	
	}
}
/*
显示操作记录
*/
void LCD_showoperaLog(void)
{
	uint16_t lcd_i=0;
	delay_ms(10);
	
	if(nosave_par.opera_page>Loglen_Par.opera_pageNum)
	{
		nosave_par.opera_page=Loglen_Par.opera_pageNum;
	}
	if(nosave_par.opera_page<1)
	{
		nosave_par.opera_page=1;
	}
	if(nosave_par.opera_page<Loglen_Par.opera_pageNum)
	{
		lcd_show_len=10;
	}
	else
	{
		lcd_show_len=Loglen_Par.opera_num-(nosave_par.opera_page-1)*10;
	}
	lcd_ptr=Loglen_Par.opera_num-(nosave_par.opera_page-1)*10; //指针
  
	for(lcd_i=0;lcd_i<lcd_show_len;lcd_i++)
	{
		SD_ResumeOperaLog(&OperaLog_Olds[lcd_i],lcd_ptr--);
	}
	Record_Clear(current_screen_id,7);
	Record_Add(current_screen_id,7,(u8 *)"         时间;                                                                        操作内容;");

	for(lcd_i=0;lcd_i<lcd_show_len;lcd_i++)
	{
		if(OperaLog_Olds[lcd_i].time.years==0xffff)
		{
			break;
		}
		else
		{
			snprintf(lcd_datu8,sizeof(lcd_datu8),"      %02d-%02d-%02d\r\n      %02d:%02d:%02d;",      //时间
				OperaLog_Olds[lcd_i].time.years,	
				OperaLog_Olds[lcd_i].time.months,
				OperaLog_Olds[lcd_i].time.days,
				OperaLog_Olds[lcd_i].time.hours,
				OperaLog_Olds[lcd_i].time.minutes,
				OperaLog_Olds[lcd_i].time.secs);  
			snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,OperaLog_Olds[lcd_i].operastr); 
			Record_Add(current_screen_id,7,(u8 *)lcd_datu8);
		}
	}
}
/*
显示状态log
*/
void LCD_showstatusLog(void)
{
	uint16_t ptr;
	int16_t i=0;
	ptr=Loglen_Par.status_ptr; //灌溉记录指针
	Record_Clear(3,7);
	Record_Add(3,7,(u8 *)"时间;闸前水位;闸后水位;超声;GPS;4G;内存卡;电压;电机;限位");
	/***********************************************************************/
	snprintf(lcd_datu8,sizeof(lcd_datu8),"%02d-%02d\r\n%02d:%02d;",      //时间
		g_rtc.months,
		g_rtc.days,
		g_rtc.hours,
		g_rtc.minutes);  
	/***********************************************************************/
	if(Statlog_Par.fro_level==0)         //闸前水位
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"未知");
	}
	else if(Statlog_Par.fro_level==1)
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"正常"); 
	}
	else
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"故障"); 
	}
	/***********************************************************************/
	if(Statlog_Par.aft_level==0)          //闸后水位
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"未知"); 
	}
	else if(Statlog_Par.aft_level==1)
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"正常"); 
	}
	else
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"故障"); 
	}
	/***********************************************************************/
	
	/***********************************************************************/
	/***********************************************************************/
	for(i=0;i<(dev_SavePar.Ultra_Num-1);i++)
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%d-",lcd_datu8,Ultra_CALCU_Par.Error_Flag[i]);
	}
	snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%d",lcd_datu8,Ultra_CALCU_Par.Error_Flag[i]);		//此行应注销
	snprintf(lcd_datu8,sizeof(lcd_datu8),"%s;",lcd_datu8);
//	if(Statlog_Par.ultra==0)              //超声
//	{
//		
//		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"未知"); 
//	}
//	else if(Statlog_Par.ultra==1)
//	{
//		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"正常"); 
//	}
//	else
//	{
//		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"故障");
//	}
	/***********************************************************************/
	if(Statlog_Par.GPSState==0)              //GPS
	{
		
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"未知"); 
	}
	else if(Statlog_Par.GPSState==1)
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"正常"); 
	}
	else
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"故障");
	}
	/***********************************************************************/
	if(Statlog_Par._4G==0)              //4G
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"未知"); 
	}
	else if(Statlog_Par._4G==1)
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"正常"); 
	}
	else if(Statlog_Par._4G==2)
	{
		switch (Statlog_Par._sim) 
		{          
			case 2:                
				snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"未插卡"); 
				break;                 
			case 3:
				snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"无网络");
				break;
			case 4:
				snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"无卡号");
				break;
			default:             
				snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"故障");
				break;
		}
	}
	else 
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"故障");
	}
	/***********************************************************************/
	if(Statlog_Par.SDState==SD_UNKNOWN)              //SD
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"未知"); 
	}
	else if(Statlog_Par.SDState==SD_OK)
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"正常"); 
	}
	else if(Statlog_Par.SDState==SD_NO_EXIST)
	{
		
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"未插入");
	}
	else
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"未知故障");
	}
	
	/***********************************************************************/
	snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%.2f;",lcd_datu8,Statlog_Par_new.voltage); //电压
	/***********************************************************************/
	if(Statlog_Par.motor==0)              //电机
	{
		
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"未知"); 
	}
	else if(Statlog_Par.motor==1)
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"正常"); 
	}
	else
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"故障");
	}
	/***********************************************************************/
	
	Record_Add(3,7,(u8 *)lcd_datu8);
	/**/
	Record_Add(3,7,(u8 *)"电池电压;浮球;水位超高指示;浮球超高指示; ; ; ;;");
	snprintf(lcd_datu8,sizeof(lcd_datu8),"%.3f;",lcd_datu8,Statlog_Par_new.voltage); //显示电池电压
	/*浮球传感器*/
	if(Statlog_Par.FloatBall_State==0)
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"未知"); 
	}
	else if(Statlog_Par.FloatBall_State==1)
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"导通"); 
	}
	else if(Statlog_Par.FloatBall_State==2)
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"断开"); 
	}
	/*水位状态*/
	if(Statlog_Par.WatLev_Safe==0)
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"未知"); 
	}
	else if(Statlog_Par.WatLev_Safe==1)
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"安全"); 
	}
	else if(Statlog_Par.WatLev_Safe==2)
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"超高"); 
	}
	/*浮球状态*/
	if(Statlog_Par.FloatBall_Safe==0)
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"未知"); 
	}
	else if(Statlog_Par.FloatBall_Safe==1)
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"安全"); 
	}
	else if(Statlog_Par.FloatBall_Safe==2)
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;",lcd_datu8,"超高"); 
	}
	snprintf(lcd_datu8,sizeof(lcd_datu8),"%s;;;",lcd_datu8); 
	Record_Add(3,7,(u8 *)lcd_datu8);
	/***********************************************************************/
	Record_Add(3,7,(u8 *)"上限位;下限位;;;;; ; ; ");
	if(MOTOR_LIMIT_UP==0)              //限位
	{
		
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s;","0"); 
	}
	else 
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s;","1"); 
	}
	if(MOTOR_LIMIT_DOWN==0)              //限位
	{
		
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;;;;;;;;;;;",lcd_datu8,"0"); 
	}
	else
	{
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%s;;;;;;;;;;",lcd_datu8,"1"); 
	}
	Record_Add(3,7,(u8 *)lcd_datu8);
	Record_Add(3,7,(u8 *)"运行数据记录数;操作记录记录数;;;;; ; ; ");
	snprintf(lcd_datu8,sizeof(lcd_datu8),"%d;%d;;;;;;;;;;",Loglen_Par.flow_num,Loglen_Par.opera_num);
	Record_Add(3,7,(u8 *)lcd_datu8);
}
/*
显示速度信息
*/
void LCD_showSpeedLog(void)
{
	
	
	lcd_head=Loglen_Par.data_head; //灌溉记录指针
	lcd_len=Loglen_Par.data_len; //
	Record_Clear(9,7);
	if(nosave_par.data_page>SD_GetDataLogPage())
	{
		nosave_par.data_page=SD_GetDataLogPage();
	}
	if(nosave_par.data_page==0)
	{
		nosave_par.data_page=1;
	}
	Record_Add(9,7,(u8 *)"时间;流速1;流速2;流速3;流速4;流速5;流速6;流速7;流速8;流速9");
	/**
	0.124
	0.443
	0.333
	0.445
	0.454
	
	
	*/
	if(nosave_par.data_page<SD_GetDataLogPage())
	{
		lcd_show_len=10;
	}
	else
	{
		
		lcd_show_len=lcd_len-(nosave_par.data_page-1)*10;
	}
	/*
	10  -10
	0   -100
	100
	*/
	if(((s32)Loglen_Par.data_head-(s32)(nosave_par.data_page-1)*10)<0)
	{
		lcd_ptr=Loglen_Par.data_head+DataLogLen-(nosave_par.data_page-1)*10;
	}
	else
	{
		lcd_temp=(nosave_par.data_page-1)*10;
		lcd_ptr=Loglen_Par.data_head-(nosave_par.data_page-1)*10;
	}
	lcd_ptr=lcd_ptr;
	for(lcd_i=0;lcd_i<lcd_show_len;lcd_i++)
	{
		delay_ms(10);
		if(lcd_ptr==0)
		{
			lcd_ptr=DataLogLen-1;
		}
		else 
		{
			lcd_ptr--;
		}
//		SD_Read((u8 *)&Speedlog_Par,SD_DATALOG+sizeof(Speedlog_Par)*lcd_ptr,sizeof(Speedlog_Par));
		SD_Read((u8 *)&Speedlog_Par,SD_SPEEDLOG+sizeof(Speedlog_Par)*lcd_ptr,sizeof(Speedlog_Par));
//		SD_ResumeDatalog_Par(ptr);
	
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%02d-%02d\r\n%02d:%02d;",      //时间
			Speedlog_Par.time.months,
			Speedlog_Par.time.days,
			Speedlog_Par.time.hours,
			Speedlog_Par.time.minutes);  
		
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%.2f;",lcd_datu8,Speedlog_Par.V[0]); //闸前水位
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%.2f;",lcd_datu8,Speedlog_Par.V[1]); //闸前水位
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%.2f;",lcd_datu8,Speedlog_Par.V[2]); //闸前水位
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%.2f;",lcd_datu8,Speedlog_Par.V[3]); //闸前水位
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%.2f;",lcd_datu8,Speedlog_Par.V[4]); //闸前水位
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%.2f;",lcd_datu8,Speedlog_Par.V[5]); //闸前水位
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%.2f;",lcd_datu8,Speedlog_Par.V[6]); //闸前水位
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%.2f;",lcd_datu8,Speedlog_Par.V[7]); //闸前水位
		snprintf(lcd_datu8,sizeof(lcd_datu8),"%s%.2f;",lcd_datu8,Speedlog_Par.V[8]); //闸前水位

	
		Record_Add(9,7,(u8 *)lcd_datu8);
	
	}
}
/*
处理数据记录
*/
void rundata_process(void )
{
	
}
/*

*/
//void Datalog_Par_new_make(void)
//{
//	memcpy(&Datalog_Par_new.time,&g_rtc,sizeof(Datalog_Par_new.time));
//	Datalog_Par_new.fro_level=RS485_Sensor.Fro_h;
//	Datalog_Par_new.aft_level=RS485_Sensor.Aft_h;
//	Datalog_Par_new.opening=nosave_par.opening;
//	Datalog_Par_new.flow=Ultra_CALCU_Par.Q_Total;
//	Datalog_Par_new.this_water=nosave_par.this_water;
//	Datalog_Par_new.year_water=nosave_par.year_water;
//	Datalog_Par_new.accu_water=nosave_par.accu_water;
//	Datalog_Par_new.dev_openflag=(nosave_par.opening!=0);
//	Datalog_Par_new.controlmode=Control_Par.control_mode;
//	memcpy(&Speedlog_Par_new.time,&g_rtc,sizeof(Speedlog_Par_new.time));
//	Speedlog_Par_new.V[0]=Ultra_CALCU_Par.V[0];
//	Speedlog_Par_new.V[1]=Ultra_CALCU_Par.V[1];
//	Speedlog_Par_new.V[2]=Ultra_CALCU_Par.V[2];
//	Speedlog_Par_new.V[3]=Ultra_CALCU_Par.V[3];
//	Speedlog_Par_new.V[4]=Ultra_CALCU_Par.V[4];
//	Speedlog_Par_new.V[5]=Ultra_CALCU_Par.V[5];
//	Speedlog_Par_new.V[6]=Ultra_CALCU_Par.V[6];
//	Speedlog_Par_new.V[7]=Ultra_CALCU_Par.V[7];
//	Speedlog_Par_new.V[8]=Ultra_CALCU_Par.V[8];
//}

/*
获取数据页数
*/
u32 SD_GetLogPage(u32 log_rows)
{
	u32 result;
	
//返回大于或者等于指定表达式的最小整数
	result=ceil((double)log_rows/10);
	if(result==0)
	{
		result=1;
	}
	return result;
}

u32 SD_GetDataLogPage(void)
{
	u32 result;
	

	result=ceil((double)nosave_par.runlog_rows/10);
	if(result==0)
	{
		result=1;
	}
	return result;
}

u32 SD_GetOperaLogPage(void)
{
	u32 result;
	
	
	result=ceil((double)nosave_par.operalog_rows/10);
	if(result==0)
	{
		result=1;
	}
	return result;
}

