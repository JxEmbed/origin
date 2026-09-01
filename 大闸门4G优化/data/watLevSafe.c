#include "watLevSafe.h"

/*

*/
u8 watLev_Count=0;
u8 floatBall_count=0;
#define WATLEV_COUNT_MAX    5			//水位安全超高超过WATLEV_COUNT_MAX时长后  认定水位安全超高
#define FLOATBALL_COUNT_MAX 5

/*

*/
void WatLev_HighCheck(void)
{
	float tempf;
	/******
	后水位传感器检测
	Statlog_Par.aft_level 		0未知	1正常	2故障
	Statlog_Par.WatLev_Safe		0未知	1安全	2危险
	****/
	if(Statlog_Par.aft_level==2) 	//如果后水位传感器故障，则水位安全未知
	{
		Statlog_Par.WatLev_Safe=0;	//未知
	}
	else if(Statlog_Par.aft_level==1)
	{
		tempf=dev_SavePar.AftHeight_Air-RS485_Sensor.Aft_h;		//闸后净空-后水位液位==后水位传感器离水面距离
		//tempf不可能为负数    
		if(tempf < 0)
		{
			tempf=0;
		}
		//dev_SavePar.Safe_SpaceHigh  默认为0  表示关闭水位超高功能
		if(tempf<dev_SavePar.Safe_SpaceHigh)					//后水位离水面距离的高度   dev_SavePar.Safe_SpaceHigh不知道可填0.2米
		{
			if(watLev_Count<WATLEV_COUNT_MAX)					//连续超过安全水位5s 则认定水位危险
				watLev_Count++;
		}
		else
		{
			Statlog_Par.WatLev_Safe=1;	//安全
			watLev_Count=0;
		}
	}
	if(watLev_Count == WATLEV_COUNT_MAX)
	{
		Statlog_Par.WatLev_Safe=2;		//危险
	}
	
	/******
	浮球传感器检测		常闭   低电平
	****/
	if(FLOAT_BALL_IN==1)   //高电平
	{
		if(floatBall_count<FLOATBALL_COUNT_MAX)			//水位将浮球顶上去超过FLOATBALL_COUNT_MAX时长   
			floatBall_count++;
	}
	else                   //低电平
	{
		Statlog_Par.FloatBall_Safe=1;			//状态正常
		floatBall_count=0;
	}
	/*
		Statlog_Par.FloatBall_Safe  	0未知	1安全   2危险
		Statlog_Par.FloatBall_State		0未知	1导通	2断开
	*/
	//浮球故障(水位超高后，浮球不闭合)
	//floatBall_count==FLOATBALL_COUNT_MAX  水位超高
	if(floatBall_count==FLOATBALL_COUNT_MAX && Statlog_Par.aft_level==2) 
	{
		Statlog_Par.FloatBall_Safe=2;			
		Statlog_Par.FloatBall_State=2;		 
	}
	/*
		后水位传感器正常、液位未超安全超高	浮球断开		则		浮球状态未知  
		设置安全超高时--->浮球安装位置应在安全超高内
		若未装浮球则设置安全超高为0.2米
	*/
	else if(floatBall_count==FLOATBALL_COUNT_MAX && Statlog_Par.aft_level==1) 
	{
		Statlog_Par.FloatBall_State=2;   
		Statlog_Par.FloatBall_Safe=0;    
	}
	else if(floatBall_count < FLOATBALL_COUNT_MAX) 
	{
		Statlog_Par.FloatBall_State=1; 
	}
	
}
