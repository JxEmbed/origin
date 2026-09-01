#include "sddata.h"
#include "string.h"
SD_STRUCT SD_Par;

/*************************************
存储流量历史数据


0,1,2,3,4,5           */
u32 SD_Calcflow_headMax(void)
{
	u32 max;
	max=SD_FLOWLOG_SIZE/sizeof(RUNLOG_STRUCT)-1;
	return max;
}
u32 SD_Calcopera_headMax(void)
{
	u32 max;
	max=SD_OPERALOG_SIZE/sizeof(Operalog_Par)-1;
	return max;
}
void SD_Test(void)
{
//	SD_Write((u8 *)&FlowLog_New,SD_FLOWLOG_POS,sizeof(FlowLog_New));
//	SD_Read ((u8 *)&FlowLog_Old,SD_FLOWLOG_POS,sizeof(FlowLog_Old));
}
/*
存储操作记录
*/
void SD_SaveOperaLog(OPERALOG_STRUCT * operalog_par)
{
	SD_Write((u8 *)operalog_par,SD_OPERALOG_POS+sizeof(OPERALOG_STRUCT)*Loglen_Par.opera_head,sizeof(OPERALOG_STRUCT));
	if(Loglen_Par.opera_head>SD_Calcopera_headMax())
	{
		Loglen_Par.opera_head=0;
	}
	else
	{
		Loglen_Par.opera_head++;   //头部指针
		Loglen_Par.opera_num++;    //
		Loglen_Par.opera_pageNum=SD_GetLogPage(Loglen_Par.opera_num);
	}
	Flash_SaveLoglen_Par();
	SD_ResumeOperaLog(&Operalog_Par_old,Loglen_Par.flow_head-1);
}
void SD_ResumeOperaLog(OPERALOG_STRUCT * operaLog,u32 log_ptr)
{
	u32 ptr=SD_Calc_Log_Ptr(log_ptr,Loglen_Par.opera_head,Loglen_Par.opera_num,nosave_par.opera_numMax);
	SD_Read((u8 *)operaLog,SD_OPERALOG_POS+sizeof(OPERALOG_STRUCT)*ptr,sizeof(OPERALOG_STRUCT));
}
void SD_SaveFlowLog(void)
{
	//Loglen_Par.flow_head 写入一次后需要更新
	SD_Write((u8 *)&RunLog_Par,SD_FLOWLOG_POS+sizeof(RunLog_Par)*Loglen_Par.flow_head,sizeof(RunLog_Par));
	if(Loglen_Par.flow_head>SD_Calcflow_headMax())
	{
		Loglen_Par.flow_head=0;			//大于设置的最大行数就覆盖写
	}
	else
	{
		Loglen_Par.flow_head++;   //头部指针
		Loglen_Par.flow_num++;    //
		Loglen_Par.flow_pageNum=SD_GetLogPage(Loglen_Par.flow_num);		//计算页数
	}
	Flash_SaveLoglen_Par();		//防止掉电丢失Loglen_Par
	SD_ResumeFlowLog(&RunLog_Par_Old,Loglen_Par.flow_head-1);		//读取写入数据是否写入
}


void SD_ClearFlowLog(void)
{
	Loglen_Par.flow_head=0;
	Loglen_Par.flow_num=0;
	Loglen_Par.flow_pageNum=0;		
	Flash_SaveLoglen_Par();
}
void SD_ClearOperaLog(void)
{
	Loglen_Par.opera_head=0;
	Loglen_Par.opera_num=0;
	Loglen_Par.opera_pageNum=0;		
	Flash_SaveLoglen_Par();
}
/*
result=ptr
*/
u32 SD_Calc_Log_Ptr(u32 ptr,u32 head,u32 num,u32 max)
{
	u32 result;
	if(num<max)				//已存条数小于最大存储条数
	{
		if(ptr>0)
		{
			result=ptr-1;	
		}
		
	}
	else if(ptr+head>=(num+1))
	{
		result=ptr+head-num-1;
	}
	else
	{
		result=ptr+head-1;
	}
	return result;
}
/*************************************
恢复流量历史数据
*/
void SD_ResumeFlowLog(RUNLOG_STRUCT * flowLog,u32 log_ptr)
{
	u32 ptr=SD_Calc_Log_Ptr(log_ptr,Loglen_Par.flow_head,Loglen_Par.flow_num,nosave_par.flow_numMax);
	SD_Read((u8 *)flowLog,SD_FLOWLOG_POS+sizeof(RUNLOG_STRUCT)*ptr,sizeof(RUNLOG_STRUCT));
}


void SD_SaveDatalog_Par(u32 ptr)
{
	if(Statlog_Par.SDState!=0)
	{
		return;
	}
	SD_Write((u8 *)&Datalog_Par_new,SD_DATALOG+sizeof(Datalog_Par_new)*Loglen_Par.data_head,sizeof(Datalog_Par_new));
	SD_Write((u8 *)&Speedlog_Par_new,SD_SPEEDLOG+sizeof(Speedlog_Par_new)*Loglen_Par.data_head,sizeof(Speedlog_Par_new));
	Loglen_Par.data_head++;
	if(Loglen_Par.data_head>=DataLogLen)
	{
		Loglen_Par.data_head=0;
	}
	if(Loglen_Par.data_len<DataLogLen)
	{
		Loglen_Par.data_len++;
	}
//	Flash_SaveLoglen_Par();
}

void SD_SaveDatalog_Par2(u32 ptr)
{
	if(Statlog_Par.SDState!=0)
	{
		return;
	}
	
	SD_Write((u8 *)&Datalog_Par_new,SD_DATALOG+sizeof(Datalog_Par_new)*ptr,sizeof(Datalog_Par_new));
	
}

void SD_ResumeDatalog_Par(u32 ptr)
{
	SD_Read((u8 *)&Datalog_Par,SD_DATALOG+sizeof(Datalog_Par)*ptr,sizeof(Datalog_Par));
}

extern u8 SD_BUF[512];	 

/*
存储速度历史数据
*/
void SD_SaveSpeedlog_Par(u32 ptr)
{
	SD_Write((u8 *)&Speedlog_Par_new,SD_DATALOG+sizeof(Speedlog_Par_new)*Loglen_Par.data_head,sizeof(Speedlog_Par_new));
}
/*
读取速度历史数据
*/
void SD_ResumeSpeedlog_Par(u32 ptr)
{
	SD_Read((u8 *)&Speedlog_Par,SD_DATALOG+sizeof(Speedlog_Par)*ptr,sizeof(Speedlog_Par));
}


