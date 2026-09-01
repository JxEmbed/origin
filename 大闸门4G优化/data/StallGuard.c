#include "StallGuard.h"
#include "AnglePulse.h"
#include "TMC5160A.h"


StallGuard_Struct StallGuard_Par;
#define STALLGUARD_COUND_MAX 1
s8 Judge_Stall(void)
{
	/*
	3400 ----200
	StallGuard_Par.StallGuard_Threshold=Mess_Par.run_speed*17/5;
	StallGuard_Par.StallGuard_Threshold  堵转阈值转速
	Mess_Par.run_speed	电机的目标转速
	*/ 
	StallGuard_Par.StallGuard_Threshold=Mess_Par.run_speed*dev_SavePar.StallGuard_SpeedThreshold;
	if((Control_Par.setState==GATE_UP)||(Control_Par.setState==GATE_DOWN))
	{
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_WHITE"Control_Par.setState=%hhu\r\n",Control_Par.setState);
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_WHITE"AngleP_Speed=%u\r\n",AngleP_Speed);
		if(AngleP_Speed<StallGuard_Par.StallGuard_Threshold)					//真实转速<堵转阈值(5转/秒)
		{
			if(StallGuard_Par.stallGuard_count<dev_SavePar.StallGuard_Secs)		//<堵转阈值秒数  <   堵转检测时长
			{
				StallGuard_Par.stallGuard_count++;								
			}
		}
		else
		{
			StallGuard_Par.stallGuard_count=0;
		}
	}
	
	if(StallGuard_Par.stallGuard_count>=dev_SavePar.StallGuard_Secs)
	{
		StallGuard_Par.StallGuard_occur=1;  		//发生堵转标志位
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_WHITE"StallGuard_Par.StallGuard_occur=%hhu\r\n",StallGuard_Par.StallGuard_occur);
	}
	
}

/*
复位堵转检测
	u8 StallGuard_occur;    //堵转检测发生一次
	u8 StallGuard_flag;     //堵转检测确认
	u8 StallGuardCount;     //堵转发生次数
	u8 StallGuardStage;     //堵转所处阶段
	u32 StallGuard_Threshold; //堵转阈值
*/
void StallGuardResetCheck(void)
{
	StallGuard_Par.StallGuard_occur=0;//
	StallGuard_Par.StallGuard_flag=0;
	StallGuard_Par.StallGuardCount=0;//清零堵转次数
	StallGuard_Par.StallGuardStage=0;//堵转阶段清零
	StallGuard_Par.stallGuard_count=0; //
}
