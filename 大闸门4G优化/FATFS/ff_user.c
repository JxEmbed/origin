#include "ff_user.h"
#include "main.h"
#include "stdio.h"
#include "string.h"
#include "logdata.h"
#include "ff.h"
/*
存储运行数据	runlog.csv
存储操作记录    operalog.csv
*/

#define CSV_RUNLOG           "0:runlog.bin"
#define CSV_OPERALOG         "0:operalog.bin"
#define CSV_TESTLOG          "0:textlog.csv"

FATFS sd_fs;
static int s_total_row=0; //总行数，打开csv文件时更新
static int s_total_col=0; //总列数，打开csv文件时更新
#define MAX_LINE_SIZE 256   //fgets函数读取的最大字节数

char ff_csv_New[FF_CSV_LEN];
char ff_csv_Old[FF_CSV_LEN];

//static char s_stringBuf[MAX_LINE_SIZE];
FIL fp;
 u32 ff_total,ff_free;
void ff_init(void)
{

    u32 ret = 0;
    MKFS_PARM opt = {0};
    opt.fmt = FM_FAT;
    static u8 work_buf[512] = {0};
    ret = f_mount(&sd_fs, DISK_SD, 1);
    if(ret != FR_OK)
    {
        switch(ret)
        {
            case FR_NO_FILESYSTEM: //没有有效的FAT卷
            {
                ret = f_mkfs(DISK_SD, &opt, work_buf, sizeof(work_buf));
            }break;
            case FR_MKFS_ABORTED:  //f_mkfs（）因任何问题而中止
            {
                ret = f_mkfs(DISK_SD, &opt, work_buf, sizeof(work_buf));
            }break;
			case FR_NOT_READY:     //物理驱动器无法工作   
			{
				
			}break;
        }
    }
    if(ret != FR_OK)
    {
        SD_State.FAT_OK=0;
		Statlog_Par.SDState=SD_FAT_ERROR;//SD_FAT_ERROR
    }
    else
    {
        SD_State.FAT_OK=1;
		Statlog_Par.SDState=SD_OK;//SD_FAT_ERROR
    }
	
	Error_Par.SD_flag=ret;
	if(ret==FR_NOT_READY)
	{
		Error_Par.SD_flag=SD_NO_EXIST;
	}
	
    return;




}



void GetAllCSVrows(void)
{
//	nosave_par.runlog_rows=GetrunlogbinRows();
//	nosave_par.operalog_rows=GetoperalogbinRows();
	
	//存储最大条数限制
//	nosave_par.flow_numMax=SD_Calcflow_headMax();
//	nosave_par.opera_numMax=SD_Calcopera_headMax();
	//根据Loglen_Par.flow_num已存的条数计算页数
	Loglen_Par.flow_pageNum=SD_GetLogPage(Loglen_Par.flow_num);
	Loglen_Par.opera_pageNum=SD_GetLogPage(Loglen_Par.opera_num);
}
/*
获取csv文件的行数
*/
FRESULT ret;
u32 GetrunlogbinRows(void)
{
	u32 result = 0;
	char * tempptr;
//	char tempc[100];
	FILINFO fno;
	taskENTER_CRITICAL();
	ret=f_stat(CSV_TESTLOG, &fno);
	if(ret== FR_NO_FILE)
	{
		ret = f_open(&fp, CSV_TESTLOG, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
	}
	else
	{
		ret = f_open(&fp, CSV_TESTLOG, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
	}
	
    if(ret == FR_OK)
    {
		result=f_size(&fp)/FF_CSV_LEN;
    }
	else
	{
		
	}
	ret = f_close(&fp);
	taskEXIT_CRITICAL();
	return result;
}
u32 tempu321,tempu322;
u32 GetoperalogbinRows(void)
{
	u32 result = 0;
	char * tempptr;
//	char tempc[100];
	FILINFO fno;
	ret=f_stat(CSV_OPERALOG, &fno);
	if(ret== FR_NO_FILE)
	{
		ret = f_open(&fp, CSV_OPERALOG, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
	}
	else
	{
		ret = f_open(&fp, CSV_OPERALOG, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
	}
	
    if(ret == FR_OK)
    {
		tempu321=f_size(&fp);
		tempu322=sizeof(Operalog_Par);
		result=tempu321/tempu322;
    }
	else
	{
		
	}
	ret = f_close(&fp);
	
	return result;
}


u8 err_count_temp=0;
void ff_saverunlogtest()
{
	memcpy(&RunLog_Par.time,&g_rtc,sizeof(RunLog_Par.time));  //时间
	RunLog_Par.fro_level=1.234;    //渠道水位
	RunLog_Par.aft_level=1.234;    //闸后水位
	RunLog_Par.flow=1.234;   //实时流量
	RunLog_Par.this_water=1.234;    //本次水量
	RunLog_Par.day_water=1.234;     //当日水量
	RunLog_Par.year_water=1.234;   //本年水量
	RunLog_Par.accu_water=1.234;   //累计水量
	RunLog_Par.opening=0.234;
	RunLog_Par.sensor_V[0]=Ultra_CALCU_Par.V[0]; //
	RunLog_Par.sensor_V[1]=Ultra_CALCU_Par.V[1];
	RunLog_Par.sensor_V[2]=Ultra_CALCU_Par.V[2];
	RunLog_Par.sensor_V[3]=Ultra_CALCU_Par.V[3];
	RunLog_Par.sensor_V[4]=Ultra_CALCU_Par.V[4];
	RunLog_Par.sensor_V[5]=Ultra_CALCU_Par.V[5];
	RunLog_Par.sensor_V[6]=Ultra_CALCU_Par.V[6];
	RunLog_Par.sensor_V[7]=Ultra_CALCU_Par.V[7];
	RunLog_Par.sensor_V[8]=Ultra_CALCU_Par.V[8];
	RunLog_Par.sensor_V[9]=Ultra_CALCU_Par.V[0]; //
	RunLog_Par.sensor_V[10]=Ultra_CALCU_Par.V[10];
	RunLog_Par.sensor_V[11]=Ultra_CALCU_Par.V[11];
	RunLog_Par.sensor_V[12]=Ultra_CALCU_Par.V[12];
	RunLog_Par.sensor_V[13]=Ultra_CALCU_Par.V[13];
	RunLog_Par.sensor_V[14]=Ultra_CALCU_Par.V[14];
	RunLog_Par.sensor_V[15]=Ultra_CALCU_Par.V[15];
	ff_saverunlog(&RunLog_Par);//ff_readrunlog
	
}
void ff_test_make(void)
{
//	
	memcpy(&RunLog_Par.time,&g_rtc,sizeof(RunLog_Par.time));  //时间
	RunLog_Par.fro_level=RS485_Sensor.Fro_h;    //渠道水位
	RunLog_Par.aft_level=RS485_Sensor.Aft_h;    //闸后水位
	RunLog_Par.flow=Ultra_CALCU_Par.Q_Total;   //实时流量
	RunLog_Par.this_water=Log_Par.this_water;    //本次水量
	RunLog_Par.day_water=Log_Par.day_water;     //当日水量
	RunLog_Par.year_water=Log_Par.year_water;   //本年水量
	RunLog_Par.accu_water=Log_Par.accu_water;   //累计水量
	RunLog_Par.opening=nosave_par.opening;
	ff_csv_make();
}

void ff_csv_make(void)
{
	char tempc[50];
	memset(ff_csv_New,0,sizeof(ff_csv_New));
	snprintf(ff_csv_New,sizeof(ff_csv_New),"%02d-%02d-%02d  %02d:%02d:%02d",RunLog_Par.time.years,RunLog_Par.time.months,RunLog_Par.time.days,RunLog_Par.time.hours,RunLog_Par.time.minutes,RunLog_Par.time.secs);
	snprintf(ff_csv_New,sizeof(ff_csv_New),"%s,%5.3f",ff_csv_New,RunLog_Par.fro_level);
	snprintf(ff_csv_New,sizeof(ff_csv_New),"%s,%5.3f",ff_csv_New,RunLog_Par.aft_level);
	snprintf(ff_csv_New,sizeof(ff_csv_New),"%s,%8.3f",ff_csv_New,RunLog_Par.opening);
	snprintf(ff_csv_New,sizeof(ff_csv_New),"%s,%8.3f",ff_csv_New,RunLog_Par.flow);
	snprintf(ff_csv_New,sizeof(ff_csv_New),"%s,%8.4f",ff_csv_New,RunLog_Par.year_water);
	snprintf(ff_csv_New,sizeof(ff_csv_New),"%s,%8.4f\r\n",ff_csv_New,RunLog_Par.accu_water);
}


s32 ff_test_save(RUNLOG_STRUCT * runlog_par)
{
	FRESULT ret;
	UINT bw;
	FSIZE_t ofs;
	ff_test_make();
	while(1)
	{
		ret = f_open(&fp, CSV_TESTLOG, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
		
		if(ret != FR_OK)
		{
			break;
		}
		ret=f_sync(&fp);
		if(ret != FR_OK)
		{
			break;
		}
		ofs=nosave_par.runlog_rows*FF_CSV_LEN;
		ret=f_lseek(&fp, ofs);
//		ret=f_lseek(&fp, f_size(&fp));
		if(ret != FR_OK)
		{
			break;
		}
		ret=f_write(&fp,ff_csv_New,FF_CSV_LEN,&bw);
		if(ret != FR_OK)
		{
			break;
		}
		ret=f_sync(&fp);
		if(ret != FR_OK)
		{
			break;
		}
		nosave_par.runlog_rows++;
		break;
		
	}
	
	err_count_temp++;		//未调用
	
	ret = f_close(&fp);
	exf_getfree(DISK_SD,&ff_total,&ff_free);
	return -1;
}
s32 ff_test_read(char * runlog_par,u32 rows)
{
	//	char datstr[300];
	u8 i=0;
	char * tempptr;
//	char tempc[100];
	FRESULT ret;
	UINT br;
	FSIZE_t ofs;
	while(1)
	{
		ret = f_open(&fp, CSV_TESTLOG, FA_OPEN_ALWAYS  | FA_READ); //打开文件
		if(ret != FR_OK)
		{
			break;
		}
		ofs=(rows-1)*FF_CSV_LEN;
//		ret=f_lseek(&fp,(rows-1)*sizeof(*runlog_par));
		ret=f_lseek(&fp,ofs);
		if(ret != FR_OK)
		{
			break;
		}
		ret=f_read(&fp,runlog_par,FF_CSV_LEN,&br);
		if(ret != FR_OK)
		{
			break;
		}
//		ret=f_lseek(&fp,(rows-1)*sizeof(*runlog_par));
//		if(ret != FR_OK)
//		{
//			break;
//		}
//		runlog_par->day_water=0.0033;
//		runlog_par->accu_water=0.0044;
//		runlog_par->year_water=0.0044;
//		ret=f_write(&fp,runlog_par,sizeof(*runlog_par),&br);
//		if(ret != FR_OK)
//		{
//			break;
//		}
//		ret=f_lseek(&fp,(rows-1)*sizeof(*runlog_par));
//		if(ret != FR_OK)
//		{
//			break;
//		}
//		ret=f_read(&fp,runlog_par,sizeof(*runlog_par),&br);
//		if(ret != FR_OK)
//		{
//			break;
//		}
		break;
	}
    
	
	 ret = f_close(&fp);
    if(ret != FR_OK)
    {
        return -1;
    }
	return 0;
}
s32 ff_saverunlog(RUNLOG_STRUCT * runlog_par)
{
//	char datstr[300];
//	u8 i=0;
	FRESULT ret;
	UINT bw;
	FSIZE_t ofs;
//	u32 teu32=sizeof(*runlog_par);
	while(1)
	{
		ret = f_open(&fp, CSV_RUNLOG, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
		
		if(ret != FR_OK)
		{
			break;
		}
		ret=f_sync(&fp);
		if(ret != FR_OK)
		{
			break;
		}
//		ofs=nosave_par.runlog_rows*sizeof(*runlog_par);
//		ret=f_lseek(&fp, ofs);
		ret=f_lseek(&fp, f_size(&fp));
		if(ret != FR_OK)
		{
			break;
		}
		ret=f_write(&fp,runlog_par,sizeof(*runlog_par),&bw);
		if(ret != FR_OK)
		{
			break;
		}
		ret=f_sync(&fp);
		if(ret != FR_OK)
		{
			break;
		}
		nosave_par.runlog_rows++;
		ret = f_close(&fp);
		if(ret != FR_OK)
		{
			break;
		}
		return 0;
		
	}
	
	err_count_temp++;
	
	ret = f_close(&fp);
	return -1;
}
s32 ff_readrunlog(RUNLOG_STRUCT * runlog_par,u32 rows)
{
//	char datstr[300];
	u8 i=0;
	char * tempptr;
//	char tempc[100];
	FRESULT ret;
	UINT br;
	FSIZE_t ofs;
	while(1)
	{
		ret = f_open(&fp, CSV_RUNLOG, FA_OPEN_ALWAYS  | FA_READ); //打开文件
		if(ret != FR_OK)
		{
			break;
		}
		ofs=(rows-1)*sizeof(*runlog_par);
//		ret=f_lseek(&fp,(rows-1)*sizeof(*runlog_par));
		ret=f_lseek(&fp,ofs);
		if(ret != FR_OK)
		{
			break;
		}
		ret=f_read(&fp,runlog_par,sizeof(*runlog_par),&br);
		if(ret != FR_OK)
		{
			break;
		}
//		ret=f_lseek(&fp,(rows-1)*sizeof(*runlog_par));
//		if(ret != FR_OK)
//		{
//			break;
//		}
//		runlog_par->day_water=0.0033;
//		runlog_par->accu_water=0.0044;
//		runlog_par->year_water=0.0044;
//		ret=f_write(&fp,runlog_par,sizeof(*runlog_par),&br);
//		if(ret != FR_OK)
//		{
//			break;
//		}
//		ret=f_lseek(&fp,(rows-1)*sizeof(*runlog_par));
//		if(ret != FR_OK)
//		{
//			break;
//		}
//		ret=f_read(&fp,runlog_par,sizeof(*runlog_par),&br);
//		if(ret != FR_OK)
//		{
//			break;
//		}
		break;
	}
    
	
	 ret = f_close(&fp);
    if(ret != FR_OK)
    {
        return -1;
    }
	return 0;	
}
s32 ff_readrunlog2(RUNLOG_STRUCT * runlog_par,u32 rows)
{
//	char datstr[300];
	u8 i=0;
	char * tempptr;
//	char tempc[100];
	FRESULT ret;
	UINT br;
	
	while(1)
	{
		ret = f_open(&fp, CSV_RUNLOG, FA_OPEN_ALWAYS  | FA_READ); //打开文件
		if(ret != FR_OK)
		{
			break;
		}
		ret=f_lseek(&fp,(rows-1)*sizeof(*runlog_par));
		if(ret != FR_OK)
		{
			break;
		}
		ret=f_read(&fp,runlog_par,sizeof(*runlog_par),&br);
		if(ret != FR_OK)
		{
			break;
		}
		ret=f_lseek(&fp,(rows-1)*sizeof(*runlog_par));
		if(ret != FR_OK)
		{
			break;
		}
		runlog_par->day_water=0.0033;
		runlog_par->accu_water=0.0044;
		runlog_par->year_water=0.0044;
		ret=f_write(&fp,runlog_par,sizeof(*runlog_par),&br);
		if(ret != FR_OK)
		{
			break;
		}
		ret=f_lseek(&fp,(rows-1)*sizeof(*runlog_par));
		if(ret != FR_OK)
		{
			break;
		}
		ret=f_read(&fp,runlog_par,sizeof(*runlog_par),&br);
		if(ret != FR_OK)
		{
			break;
		}
		break;
	}
    
	
	 ret = f_close(&fp);
    if(ret != FR_OK)
    {
        return -1;
    }
	return 0;	
}
s32 ff_readrunlog3(RUNLOG_STRUCT * runlog_par,u32 rows)
{
//	char datstr[300];
	u8 i=0;
	char * tempptr;
//	char tempc[100];
	FRESULT ret;
	UINT br;
	
	while(1)
	{
		ret = f_open(&fp, CSV_RUNLOG, FA_OPEN_ALWAYS  |FA_WRITE| FA_READ); //打开文件
		if(ret != FR_OK)
		{
			break;
		}
		
		ret=f_lseek(&fp,(rows-1)*sizeof(*runlog_par));
		if(ret != FR_OK)
		{
			break;
		}
		runlog_par->day_water=0.0033;
		runlog_par->accu_water=0.0044;
		runlog_par->year_water=0.0044;
		ret=f_write(&fp,runlog_par,sizeof(*runlog_par),&br);
		if(ret != FR_OK)
		{
			break;
		}
		ret=f_lseek(&fp,(rows-1)*sizeof(*runlog_par));
		if(ret != FR_OK)
		{
			break;
		}
		ret=f_read(&fp,runlog_par,sizeof(*runlog_par),&br);
		if(ret != FR_OK)
		{
			break;
		}
		break;
	}
    
	
	 ret = f_close(&fp);
    if(ret != FR_OK)
    {
        return -1;
    }
	return 0;	
}
s32 ff_saveoperalog(OPERALOG_STRUCT * operalog_par)
{
//	char datstr[300];
//	u8 i=0;
	FRESULT ret;
	UINT bw;
	while(1)
	{
		ret = f_open(&fp, CSV_OPERALOG, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
		if(ret != FR_OK)
		{
			break;
		}
		ret=f_sync(&fp);
		if(ret != FR_OK)
		{
			break;
		}
		ret=f_lseek(&fp, f_size(&fp));
		if(ret != FR_OK)
		{
			break;
		}
		ret=f_write(&fp,operalog_par,sizeof(*operalog_par),&bw);
		if(ret != FR_OK)
		{
			break;
		}
		ret=f_sync(&fp);
		if(ret != FR_OK)
		{
			break;
		}
		nosave_par.operalog_rows++;
		break;
	}
	
	ret = f_close(&fp);
	return 0;
}
s32 ff_readoperalog(OPERALOG_STRUCT * operalog_par,u32 rows)
{
//	char datstr[300];
//	u8 i=0;
//	char * tempptr;
//	char tempc[100];
	FRESULT ret;
	UINT br;
	while(1)
	{
		ret = f_open(&fp, CSV_OPERALOG, FA_OPEN_ALWAYS  | FA_READ); //打开文件
		if(ret != FR_OK)
		{
			break;
		}
		ret=f_lseek(&fp,(rows-1)*sizeof(*operalog_par));
		if(ret != FR_OK)
		{
			break;
		}
		ret=f_read(&fp,operalog_par,sizeof(*operalog_par),&br);
		if(ret != FR_OK)
		{
			break;
		}
		break;
	}
	
	 ret = f_close(&fp);
    if(ret != FR_OK)
    {
        return -1;
    }
	return 0;	
}

void RunLog_Par_make(void)
{
	memcpy(&RunLog_Par.time,&g_rtc,sizeof(RunLog_Par.time));  //时间
	RunLog_Par.fro_level=RS485_Sensor.Fro_h;    //闸前水位
	RunLog_Par.aft_level=RS485_Sensor.Aft_h;    //闸后水位
	RunLog_Par.flow=Ultra_CALCU_Par.Q_Total;   	//实时流量
	RunLog_Par.this_water=Log_Par.this_water;    //本次水量
	RunLog_Par.day_water=Log_Par.day_water;     //当日水量
	RunLog_Par.year_water=Log_Par.year_water;   //本年水量
	RunLog_Par.accu_water=Log_Par.accu_water;   //累计水量
	RunLog_Par.opening=nosave_par.openingm;		//开度
	RunLog_Par.sensor_V[0]=Ultra_CALCU_Par.V[0]; //
	RunLog_Par.sensor_V[1]=Ultra_CALCU_Par.V[1];
	RunLog_Par.sensor_V[2]=Ultra_CALCU_Par.V[2];
	RunLog_Par.sensor_V[3]=Ultra_CALCU_Par.V[3];
	RunLog_Par.sensor_V[4]=Ultra_CALCU_Par.V[4];
	RunLog_Par.sensor_V[5]=Ultra_CALCU_Par.V[5];
	RunLog_Par.sensor_V[6]=Ultra_CALCU_Par.V[6];
	RunLog_Par.sensor_V[7]=Ultra_CALCU_Par.V[7];
	RunLog_Par.sensor_V[8]=Ultra_CALCU_Par.V[8];
	RunLog_Par.sensor_V[9]=Ultra_CALCU_Par.V[0]; //
	RunLog_Par.sensor_V[10]=Ultra_CALCU_Par.V[10];
	RunLog_Par.sensor_V[11]=Ultra_CALCU_Par.V[11];
	RunLog_Par.sensor_V[12]=Ultra_CALCU_Par.V[12];
	RunLog_Par.sensor_V[13]=Ultra_CALCU_Par.V[13];
	RunLog_Par.sensor_V[14]=Ultra_CALCU_Par.V[14];
	RunLog_Par.sensor_V[15]=Ultra_CALCU_Par.V[15];
}



void SaveOPERA(OPERALOGTYPE type,void * dat1,void * dat2)
{
	char tempu8[100];
	if(nosave_par.identity==IDENTITY_ADMIN)
	{
		snprintf(tempu8,sizeof(tempu8),"管理员(%s)",Author_Par.admin_phone);
	}
	if(nosave_par.identity==IDENTITY_OPERA1)
	{
		snprintf(tempu8,sizeof(tempu8),"操作员1(%s)",Author_Par.opera1_phone);
	}
	if(nosave_par.identity==IDENTITY_OPERA2)
	{
		snprintf(tempu8,sizeof(tempu8),"操作员2(%s)",Author_Par.opera2_phone);
	}
	if(nosave_par.identity==IDENTITY_MAINT)
	{
		snprintf(tempu8,sizeof(tempu8),"维护员(%s)",Author_Par.maint_phone);
	}
	if(nosave_par.identity==IDENTITY_TEMPO)
	{
		snprintf(tempu8,sizeof(tempu8),"临时身份(%s)",Author_Par.tempo_phone);
	}
	
	switch(type)
	{
		/******************		远程控制			***********************/
		case REMOTO_CONTROL_MODE:
		{
			if(strcmp("Error_Mode",dat2)==0)
				snprintf(tempu8,sizeof(tempu8),"远程:修改[控制模式] (%hhu)-->非法,已驳回",*(u8 *)dat1);
			else 
				snprintf(tempu8,sizeof(tempu8),"远程:修改[控制模式] (%hhu)-->(%d)",*(u8 *)dat1,*(int *)dat2);
			break;
		}
		case REMOTO_CONTROL_OPENING:
		{
			snprintf(tempu8,sizeof(tempu8),"远程:修改[开度] (%.2f)-->(%.2lf)",*(float *)dat1,*(double *)dat2);
			break;
		}
		case REMOTO_CONTROL_LEVEL:
		{
			snprintf(tempu8,sizeof(tempu8),"远程:修改[水位] (%.2f)-->(%.2lf)",*(float *)dat1,*(double *)dat2);
			break;
		}
		case REMOTO_CONTROL_FLOW:
		{
			snprintf(tempu8,sizeof(tempu8),"远程:修改[流量] (%.2f)-->(%.2lf)",*(float *)dat1,*(double *)dat2);
			break;
		}
		case REMOTO_CONTROL_RUN:
		{
			snprintf(tempu8,sizeof(tempu8),"远程:修改[控制状态] (%hhu)-->(%d)",*(u8 *)dat1,*(int *)dat2);
			break;
		}
		/*************			登录		***************/
		case OPERA_LOGIN:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:登录",tempu8);
			break;
		}
		/*********参数组一**************************************************/
		case OPERA_DEV_NAME:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[产品名称] (%s)-->(%s)",tempu8,dat1,dat2);
			break;
		}
		case OPERA_VERSION:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[软件版本] (%s)-->(%s)",tempu8,dat1,dat2);
			break;
		}
		case OPERA_FACTORY_TIME:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[安装时间] (%s)-->(%s)",tempu8,dat1,dat2);
			break;
		}
		case OPERA_DEV_CODE:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[设备编号] (%s)-->(%s)",tempu8,dat1,dat2);
			break;
		}
		case OPERA_FORM_FACTOR:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[外形尺寸] (%s)-->(%s)",tempu8,dat1,dat2);
			break;
		}
		case OPERA_SENDNEWADDR:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[上传地址] (%s)-->(%s)",tempu8,dat1,dat2);
			break;
		}
		/**********参数组二*************************************************/
		case OPERA_WATER_INLETWIDTH:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[进水宽度] (%f)-->(%f)",tempu8,*(float *)dat1,*(float *)dat2);
			break;
		}
		case OPERA_WATER_INLETHEIGHT:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[进水高度] (%f)-->(%f)",tempu8,*(float *)dat1,*(float *)dat2);
			break;
		}
		case OPERA_ULTRA_NUM:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[探头层数] (%d)-->(%d)",tempu8,*(u8 *)dat1,*(u8 *)dat2);
			break;
		}
		
		case OPERA_ULTRA_INITIALDIST:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[探头初距] (%f)-->(%f)",tempu8,*(float *)dat1,*(float *)dat2);
			break;
		}
		case OPERA_ULTRA_SPACING:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[探头间距] (%f)-->(%f)",tempu8,*(float *)dat1,*(float *)dat2);
			break;
		}
		case OPERA_GRADIENT:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[渠道比降] (%f)-->(%f)",tempu8,*(float *)dat1,*(float *)dat2);
			break;
		}
		
		case OPERA_FROHEIGHT_AIR:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[渠道净空] (%f)-->(%f)",tempu8,*(float *)dat1,*(float *)dat2);
			break;
		}
		case OPERA_STATISTICAL_TIME:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[统计时刻] (%d)-->(%d)",tempu8,*(u8 *)dat1,*(u8 *)dat2);
			break;
		}
		/**********参数组三*************************************************/
		case OPERA_FLOW_MIN:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[最小流量] (%f)-->(%f)",tempu8,*(float *)dat1,*(float *)dat2);
			break;
		}
		case OPERA_FLOW_MAX:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[最大流量] (%f)-->(%f)",tempu8,*(float *)dat1,*(float *)dat2);
			break;
		}
		case OPERA_FLOWDEVIATION:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[流量偏差] (%f)-->(%f)",tempu8,*(float *)dat1,*(float *)dat2);
			break;
		}
		
		case OPERA_LEVEL_MIN:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[最小水位] (%f)-->(%f)",tempu8,*(float *)dat1,*(float *)dat2);
			break;
		}
		case OPERA_LEVEL_MAX:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[最大水位] (%f)-->(%f)",tempu8,*(float *)dat1,*(float *)dat2);
			break;
		}
		case OPERA_LEVEL_DEVIATON:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[水位偏差] (%f)-->(%f)",tempu8,*(float *)dat1,*(float *)dat2);
			break;
		}
		
		case OPERA_DATA_INTERVAL:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[数据间隔] (%f)-->(%f)",tempu8,*(float *)dat1,*(float *)dat2);
			break;
		}
		case OPERA_SEND_INTERVAL:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[发送间隔] (%f)-->(%f)",tempu8,*(float *)dat1,*(float *)dat2);
			break;
		}
		case OPERA_COLLECT_INTERVAL:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[采集间隔] (%f)-->(%f)",tempu8,*(float *)dat1,*(float *)dat2);
			break;
		}
		case OPERA_CHANNEL_ANGLE:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[声道角度] (%f)-->(%f)",tempu8,*(float *)dat1,*(float *)dat2);
			break;
		}
		
		case OPERA_SIDE_K:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[边壁系数] (%f)-->(%f)",tempu8,*(float *)dat1,*(float *)dat2);
			break;
		}
		case OPERA_CORRECT_K:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[流量系数] (%f)-->(%f)",tempu8,*(float *)dat1,*(float *)dat2);
			break;
		}
		case OPERA_ROUGHNESS:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[明渠糙率] (%f)-->(%f)",tempu8,*(float *)dat1,*(float *)dat2);
			break;
		}
		/********************************************************************/
		case OPERA_ADMIN_PHONE:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[管理员手机号] (%s)-->(%s)",tempu8,dat1,dat2);
			break;
		}
		case OPERA_OPERA1_PHONE:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[操作员1手机号] (%s)-->(%s)",tempu8,dat1,dat2);
			break;
		}
		case OPERA_OPERA2_PHONE:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[操作员2手机号] (%s)-->(%s)",tempu8,dat1,dat2);
			break;
		}
		case OPERA_MAINT_PHONE:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[维护员手机号] (%s)-->(%s)",tempu8,dat1,dat2);
			break;
		}
		case OPERA_TEMPO_PHONE:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[临时手机号] (%s)-->(%s)",tempu8,dat1,dat2);
			break;
		}
		
		case OPERA_ADMIN_PASS:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[管理员密码]",tempu8);
			break;
		}
		case OPERA_OPERA1_PASS:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[操作员1密码]",tempu8);
			break;
		}
		case OPERA_OPERA2_PASS:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[操作员2密码]",tempu8);
			break;
		}
		case OPERA_MAINT_PASS:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[维护员密码]",tempu8);
			break;
		}
		case OPERA_TEMPO_PASS:
		{
			snprintf(tempu8,sizeof(tempu8),"%s:修改[临时密码]",tempu8);
			break;
		}
		/********************************************************************/
		case OPERA_CLEAN_RUNLOG:   //清空运行数据
		{
			snprintf(tempu8,sizeof(tempu8),"%s:清空运行数据",tempu8);
			break;
		}
		case OPERA_CLEAN_OPERALOG: //清空操作记录
		{
			snprintf(tempu8,sizeof(tempu8),"%s:清空操作记录",tempu8);
			break;
		}
		/************************************************************************/
		case OPERA_UPGRADE:
		{
			snprintf(tempu8,sizeof(tempu8),"远程升级 (%s)-->(%s)",dat1,dat2);
			break;
		}
		/********************************************************************/
		case OPERA_OPENING_FUN:
		{
			if(*(u8 *)dat2)
			{
				snprintf(tempu8,sizeof(tempu8),"开度功能 打开");
			}
			else
			{
				snprintf(tempu8,sizeof(tempu8),"开度功能 关闭");
			}
			break;
		}
		case OPERA_LEVEL_FUN:
		{
			if(*(u8 *)dat2)
			{
				snprintf(tempu8,sizeof(tempu8),"水位功能 打开");
			}
			else
			{
				snprintf(tempu8,sizeof(tempu8),"水位功能 关闭");
			}
			break;
		}
		case OPERA_FLOW_FUN:
		{
			if(*(u8 *)dat2)
			{
				snprintf(tempu8,sizeof(tempu8),"流量功能 打开");
			}
			else
			{
				snprintf(tempu8,sizeof(tempu8),"流量功能 关闭");
			}
			break;
		}
		case OPERA_4G_FUN:
		{
			if(*(u8 *)dat2)
			{
				snprintf(tempu8,sizeof(tempu8),"4G功能 打开");
			}
			else
			{
				snprintf(tempu8,sizeof(tempu8),"4G功能 关闭");
			}
			break;
		}
		case OPERA_LORA_FUN:
		{
			if(*(u8 *)dat2)
			{
				snprintf(tempu8,sizeof(tempu8),"Lora功能 打开");
			}
			else
			{
				snprintf(tempu8,sizeof(tempu8),"Lora功能 关闭");
			}
			break;
		}
		
		/********************************************************************/
		default:
			break;
	}
	if(nosave_par.time_OK!=1||g_rtc.years==0)
	{
		return;
	}
	
	Operalog_Par.rows=Loglen_Par.opera_num+1;      //存储行序号
	memcpy(&Operalog_Par.time,&g_rtc,sizeof(g_rtc)); //存储时间
	strcpy(Operalog_Par.operastr,tempu8);            //存储操作内容
	SD_SaveOperaLog(&Operalog_Par);
}

void ff_cleanrunlog(void)
{
//	char datstr[300];
//	u8 i=0;
	FRESULT ret;
	UINT bw;
	while(1)
	{
		ret = f_open(&fp, CSV_TESTLOG, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
		if(ret != FR_OK)
		{
			break;
		}
		break;
    
	}
	ret = f_close(&fp);
	
	Log_Par.accu_water=0;
	Log_Par.day_water=0;
	Log_Par.month_water=0;
	Log_Par.this_water=0;
	Log_Par.year_water=0;
	nosave_par.runlog_rows=0;
	Flash_SaveLog_Par();
}
void ff_cleanoperalog(void)
{
//	char datstr[300];
//	u8 i=0;
	FRESULT ret;
//	UINT bw;
	while(1)
	{
		ret = f_open(&fp, CSV_OPERALOG, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
		if(ret != FR_OK)
		{
			break;
		}
		break;
	}
	ret = f_close(&fp);
	nosave_par.operalog_rows=0;
	
}

//得到磁盘剩余容量
//drv:磁盘编号("0:"/"1:")
//total:总容量	 （单位KB）
//free:剩余容量	 （单位KB）
//返回值:0,正常.其他,错误代码
u8 exf_getfree(char *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //得到磁盘信息及空闲簇数量
    res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	//得到总扇区数
	    fre_sect=fre_clust*fs1->csize;			//得到空闲扇区数	   
#if FF_MAX_SS!=512				  				//扇区大小不是512字节,则转换为512字节
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total=tot_sect>>1;	//单位为KB
		*free=fre_sect>>1;	//单位为KB 
 	}
	return res;
}	

