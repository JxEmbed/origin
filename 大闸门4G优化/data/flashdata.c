#include "flashdata.h"
#include "string.h"
#include "math.h"
void Flash_ResumeAll(void)
{
	Flash_ResumeDevData();
	Flash_ResumeAuthor_Par();
	Flash_ResumeMess_Par();		//运行步数、总步数、运行速度
//	Flash_ResumeControl_Par();
//	Flash_SaveLoglen_Par();
	Flash_ResumeLoglen_Par();
//	Flash_ResumeMotor_Action();
//	Flash_ResumeRun_Par();
	Flash_Resumemyota_info();
	Flash_ResumeUpdate_Par();
	Flash_ResumeONENET_Par();
	strcpy((char *)Author_Par.tempo_pass,"000000");
	strcpy((char *)Author_Par.maint_pass,"950821");
	Flash_ResumeLog_Par();
	Flash_ResumeAccWater_Par();
}
void Flash_ResumeDevData(void)
{
	W25QXX_Read((u8 *)&dev_SavePar,FLASH_FLASHMAG,sizeof(dev_SavePar));
	
	//未被设置参数时设定默认值
	if(dev_SavePar.Ultra_Num>CHANNEL_MAX)
	{
		dev_SavePar.Ultra_Num=CHANNEL_MAX;
	}
	if(isnan(dev_SavePar.StallGuard_SpeedThreshold))
	{
		dev_SavePar.StallGuard_SpeedThreshold=1;
	}
	if(dev_SavePar.StallGuard_Secs==0xff)
	{
		dev_SavePar.StallGuard_Secs=3;
	}
	
	//使用设备种类默认值
	if(dev_SavePar.TypeLevel==0xff)
	{
		dev_SavePar.TypeLevel=0;
	}
	if(dev_SavePar.TypeUltra==0xff)
	{
		dev_SavePar.TypeUltra=0;
	}
	if(dev_SavePar.dev_code[0]==0xff)
	{
		//nosave_par.Devcode_SetF=1;
	}
	
}

void Flash_SaveDevData(void)
{
	W25QXX_Write((u8 *)&dev_SavePar,FLASH_FLASHMAG,sizeof(dev_SavePar));
}

void Flash_ResumeAuthor_Par(void)
{
	W25QXX_Read((u8 *)&Author_Par,FLASH_AUTHOR,sizeof(Author_Par));
//	if(Author_Par.admin_pass[0]==0xff)
//	{
//		memset(Author_Par.admin_pass,0,sizeof(Author_Par.admin_pass));
//		memcpy(Author_Par.admin_pass,"123456\0\0",sizeof(Author_Par.admin_pass));
//	}
//	if(Author_Par.opera_pass[0]==0xff)
//	{
//		memset(Author_Par.opera_pass,0,sizeof(Author_Par.opera_pass));
//		memcpy(Author_Par.opera_pass,"111111\0\0",sizeof(Author_Par.opera_pass));
//	}
//	if(Author_Par.maint_pass[0]==0xff)
//	{
//		memset(Author_Par.maint_pass,0,sizeof(Author_Par.maint_pass));
//		memcpy(Author_Par.maint_pass,"950821\0\0",sizeof(Author_Par.maint_pass));
//	}
//	if(Author_Par.tempo_pass[0]==0xff)
//	{
//		memset(Author_Par.tempo_pass,0,sizeof(Author_Par.tempo_pass));
//		memcpy(Author_Par.tempo_pass,"000000\0\0",sizeof(Author_Par.tempo_pass));
//	}
}

void Flash_SaveAuthor_Par(void)
{
	W25QXX_Write((u8 *)&Author_Par,FLASH_AUTHOR,sizeof(Author_Par));
}

void Flash_SaveMess_Par(void)
{
	W25QXX_Write((u8 *)&Mess_Par,FLASH_MESS,sizeof(Mess_Par));
}
void Flash_ResumeMess_Par(void)
{
	W25QXX_Read((u8 *)&Mess_Par,FLASH_MESS,sizeof(Mess_Par));

}

/*
存储控制参数
*/
void Flash_SaveControl_Par(void)
{
	W25QXX_Write((u8 *)&Control_Par,FLASH_CONTROL,sizeof(Control_Par));
}
/*
回复控制参数
*/
void Flash_ResumeControl_Par(void)
{
	W25QXX_Read((u8 *)&Control_Par,FLASH_CONTROL,sizeof(Control_Par));
//	if(isnan(Control_Par.set_opening))
//	{
//		Control_Par.set_opening=0;
//	}
//	if(isnan(Control_Par.set_level))
//	{
//		Control_Par.set_level=0;
//	}
//	if(isnan(Control_Par.set_flow))
//	{
//		Control_Par.set_flow=0;
//	}
//	if(Control_Par.control_mode==0xff)
//	{
//		Control_Par.control_mode=0;
//	}
//	if(Control_Par.control_run==0xff)
//	{
//		Control_Par.control_run=0;
//	}
}
void Flash_SaveLoglen_Par(void)
{
	W25QXX_Write((u8 *)&Loglen_Par,FLASH_LOGLEN,sizeof(Loglen_Par));
}

void Flash_ResumeLoglen_Par(void)
{
	W25QXX_Read((u8 *)&Loglen_Par,FLASH_LOGLEN,sizeof(Loglen_Par));
	nosave_par.flow_numMax=SD_Calcflow_headMax();
	if(Loglen_Par.flow_num>nosave_par.flow_numMax)
	{
		Loglen_Par.flow_head=0;
		Loglen_Par.flow_num=0;
	}
	nosave_par.opera_numMax=SD_Calcopera_headMax();
	if(Loglen_Par.opera_num>nosave_par.opera_numMax)
	{
		Loglen_Par.opera_head=0;
		Loglen_Par.opera_num=0;
	}
}
/*
存储数据log
*/
void Flash_SaveDatalog_Par(u16 ptr)
{
	W25QXX_Write((u8 *)&Datalog_Par,FLASH_DATALOG+sizeof(Datalog_Par)*ptr,sizeof(Datalog_Par));
}
/*
恢复数据log
*/
void Flash_ResumeDatalog_Par(u16 ptr)
{
	W25QXX_Read((u8 *)&Datalog_Par,FLASH_DATALOG+sizeof(Datalog_Par)*ptr,sizeof(Datalog_Par));
}
/*
存储状态log
*/
void Flash_SaveStatlog_Par(u16 ptr)
{
	W25QXX_Write((u8 *)&Statlog_Par,FLASH_STATUSLOG+sizeof(Statlog_Par)*ptr,sizeof(Statlog_Par));
}
/*
恢复状态log
*/
void Flash_ResumeStatlog_Par(u16 ptr)
{
	W25QXX_Read((u8 *)&Statlog_Par,FLASH_STATUSLOG+sizeof(Statlog_Par)*ptr,sizeof(Statlog_Par));
}



/*
存储升级参数
*/
void Flash_Savemyota_info(void)
{
	W25QXX_Write((u8 *)&myota_info,FLAG_MYOTA_INFO,sizeof(myota_info));
	
}

void Flash_Resumemyota_info(void)
{
	W25QXX_Read((u8 *)&myota_info,FLAG_MYOTA_INFO,sizeof(myota_info));
//	if(myota_info.Update_flag==0xff)
//	{
//		strcpy(myota_info.lastVersion,"V1.1.3");
//		strcpy(myota_info.nowVersion,"V1.1.3");
//		myota_info.Update_flag=0;
//		Flash_Savemyota_info();
//	}
}

void Flash_SaveUpdate_Par(void)
{
	W25QXX_Write((u8 *)&Update_Par,FLAG_UPDATE_PAR,sizeof(Update_Par));
	
}

void Flash_ResumeUpdate_Par(void)
{
	W25QXX_Read((u8 *)&Update_Par,FLAG_UPDATE_PAR,sizeof(Update_Par));
//	if((u8)Update_Par.md5[0]==0xff)
//	{
//		memset(Update_Par.md5,0,sizeof(Update_Par.md5));
//		memset(Update_Par.size,0,sizeof(Update_Par.size));
//		memset(Update_Par.status,0,sizeof(Update_Par.status));
//		memset(Update_Par.target,0,sizeof(Update_Par.target));
//		memset(Update_Par.tid,0,sizeof(Update_Par.tid));
//		memset(Update_Par.type,0,sizeof(Update_Par.type));
//		Flash_SaveUpdate_Par();
//	}
}

void ClearUpdate_Par(void)
{
	memset(Update_Par.md5,0,sizeof(Update_Par.md5));
	memset(Update_Par.size,0,sizeof(Update_Par.size));
	memset(Update_Par.status,0,sizeof(Update_Par.status));
	memset(Update_Par.target,0,sizeof(Update_Par.target));
	memset(Update_Par.tid,0,sizeof(Update_Par.tid));
	memset(Update_Par.type,0,sizeof(Update_Par.type));
	Flash_SaveUpdate_Par();
}
void Flash_SaveONENET_Par(void)
{
	W25QXX_Write((u8 *)&onenet_info,FLAG_ONENET_INFO,sizeof(onenet_info));
	
}
/*
	ONETNET_INFO onenet_info_flowm_test2 = {"2170570427", "Z1l1ZXAyaWFLTVNrQWpvYWcwZGNKbkswcWc0cFBMam8=", "test2",
							"5j4TiFSe9M","FlowMeas", "Sy2luWmHC0u3mxW7G7//Cf/tafFje29tUS1bI+PxJS0=",
							"183.230.40.96", "1883",1, 0, 0, 1, 0, 0, 0, 0, 0, 0};
*/
void Flash_ResumeONENET_Par(void)
{
	W25QXX_Read((u8 *)&onenet_info,FLAG_ONENET_INFO,sizeof(onenet_info));
	
}
void Flash_SaveLog_Par(void)
{
//	Log_Par.this_water =AccWater_Par.this_water;   //本次水量 
//	Log_Par.day_water  =AccWater_Par.day_water;   //当日水量
//	Log_Par.month_water=AccWater_Par.month_water;  //本月水量
//	Log_Par.year_water =AccWater_Par.year_water;   //本年水量
//	Log_Par.accu_water =AccWater_Par.accu_water;   //累计水量
	W25QXX_Write((u8 *)&Log_Par,FLASH_LOG_PAR,sizeof(Log_Par));
}
void Flash_ResumeLog_Par(void)
{
	W25QXX_Read((u8 *)&Log_Par,FLASH_LOG_PAR,sizeof(Log_Par));
}
void Flash_SaveAccWater_Par(void)
{
	W25QXX_Write((u8 *)&AccWater_Par,FLASH_ACCWATER_PAR,sizeof(AccWater_Par));
}
void Flash_ResumeAccWater_Par(void)
{
	W25QXX_Read((u8 *)&AccWater_Par,FLASH_ACCWATER_PAR,sizeof(AccWater_Par));
	if(isnan(AccWater_Par.accu_water))
	{
		AccWater_Par.accu_water=0;
	}
	if(isnan(AccWater_Par.day_water))
	{
		AccWater_Par.day_water=0;
	}
	if(isnan(AccWater_Par.month_water))
	{
		AccWater_Par.month_water=0;
	}
	if(isnan(AccWater_Par.this_water))
	{
		AccWater_Par.this_water=0;
	}
	if(isnan(AccWater_Par.year_water))
	{
		AccWater_Par.year_water=0;
	}
	if(AccWater_Par._last_rtc.days==0xff)
	{
		AccWater_Par._last_rtc.days=0;
	}
}

