#include "jiang.h"
#include "stdio.h"
#include "string.h"
#include "middleware.h"

u8 lora_Send_Flag=1;
Dev_param  Weather_dev_SavePar;
char Mac_String[32];
LORA_RECORD Lora_Record;
void Lora_Send_StartupMess(void)
{
	if(lora_Send_Flag==1&&Statlog_Par.voltage_flag==1)
	{
		lora_Send_Flag=0;
		Weather_dev_SavePar.Dev_voltage=Statlog_Par_new.voltage;
		Weather_dev_SavePar.Dev_Type=7;       
		strcpy(Weather_dev_SavePar.dev_code,dev_SavePar.dev_code);
		Lora_Report();
	}
}
/*
Lora启动后发送
Dev_Type 设备类型
dev_code 设备编号
set_opening 设定开度
set_direction 设定方向
Irrigate_quota
cumulative_flow
Weather_dev_SavePar.Dev_voltage 设备电压
*/
void Lora_Report(void)
{
	uint16_t crcCalc; //存储计算出来的crc结果
	char tempc[300];
	u32 tx_len=0;
	u32 i=0;
	memset(E22_TxBuff,0,sizeof(E22_TxBuff));
	
	E22_TxBuff[0]=(HostAddr>>8);
	E22_TxBuff[1]=(HostAddr&0xff);
	E22_TxBuff[2]=HostChannel;							//给不同信道的相应地址的lora通信
	
	//分包限定240字节
	snprintf((char *)tempc,sizeof(tempc),"%u|%s|7;%f;%u;%f;%lf;%f;%f;%f;%f;%f;%hhu;%hhu;%04d-%02d-%02d %02d:%02d;%04d-%02d-%02d %02d:%02d;%u;%f;%hu;%hu;%hu;%.3f;%hu;%u;%hhu;%u;%.3f;%u;|",
		Weather_dev_SavePar.Dev_Type,Weather_dev_SavePar.dev_code,
		Control_Par.set_opening,Control_Par.set_direction,Weather_dev_SavePar.Irrigate_quota,Weather_dev_SavePar.cumulative_flow,
		RS485_Sensor.stress,RS485_Sensor.tempeture,L76K_Par.F_Lon,L76K_Par.F_Lat,Weather_dev_SavePar.Dev_voltage,Statlog_Par.loraState,Statlog_Par.GPSState,
		nosave_par.Irrigate_StartT.years,nosave_par.Irrigate_StartT.months,nosave_par.Irrigate_StartT.days,nosave_par.Irrigate_StartT.hours,nosave_par.Irrigate_StartT.secs,
		nosave_par.Irrigate_EndT.years,nosave_par.Irrigate_EndT.months,nosave_par.Irrigate_EndT.days,nosave_par.Irrigate_EndT.hours,nosave_par.Irrigate_EndT.minutes,
		Weather_dev_SavePar.flow_collectT,Weather_dev_SavePar.Collect_stresst,Weather_dev_SavePar.AMAX,Weather_dev_SavePar.VMAX,Weather_dev_SavePar.repot_time,
		(Weather_dev_SavePar.Fro_AirH*1000),Weather_dev_SavePar.Lora_Addr,Control_Par.ZONE_ID,nosave_par.Status_Irrigate,Control_Par.DEV_ID,(Weather_dev_SavePar.Aft_AirH*1000),Control_Par.GROUP_ID
	);

	strcat((char *)E22_TxBuff,tempc);
	tx_len=strlen((char *)E22_TxBuff);
	crcCalc=crc(E22_TxBuff+3,tx_len-3);                       //计算的crc
	E22_TxBuff[tx_len++]=(crcCalc>>8);
	E22_TxBuff[tx_len++]=(crcCalc&0xff);
	
	E22_400T22S_SetMode(0);
	
	
	vTaskSuspendAll();													//挂起调度器		
	E22_400T22S_Send((u8 *)E22_TxBuff,tx_len);
	xTaskResumeAll();		

	E22_400T22S_SetMode(1);
}
u8 Lora_Rxanalysis_jiang(void)
{
	char buff[Lora_RxbuffLen]={0};
	uint16_t crcCalc; //存储计算出来的crc结果
	uint16_t crcRx;   //接收到的crc
	char * tempptr;

	if(E22_RxBuffPtr<4)     //如果数据过短则退出
	{
		return 0; //如果接收到的数据小于4，则退出
	}

	crcCalc=crc(E22_RxBuff,E22_RxBuffPtr-2);                       					//计算的crc
	crcRx=(E22_RxBuff[E22_RxBuffPtr-2]<<8)+E22_RxBuff[E22_RxBuffPtr-1]; 		//接收到的crc
	if(crcCalc!=crcRx)
	{
		return 0;
	}

	memcpy(buff,E22_RxBuff,E22_RxBuffPtr);
	
	tempptr = strtok((char *)buff, "|");		//第一次切割为设备类型

	if(strcmp(tempptr,BIG_GATE_TYPE)==0)	
	{
		tempptr = strtok(NULL, "|");

		if(13==strlen(tempptr))				//设备编码
		{
			if(strcmp(tempptr,Weather_dev_SavePar.dev_code)	!= 0)					//不是自己设备编码				
			{
				//自己没有任何执行情况
//				if((Control_Par.control_run==0) && (Control_Par.manual_run ==0) && (Control_Par.fixed_flow_run==0) && (Control_Par.scheduled_run==0) && (Control_Par.fixed_level_run==0) && (nosave_par.report_flag == 1) && (nosave_par.SetPara_flag == 0))
//				{
//					nosave_par.Enter_StopF=1;
//				}
				//接收监控设备的水位数据
			}
			else			//确认中控下发命令的设备
			{
//				nosave_par.Enter_StopT=0;
				tempptr = strtok(NULL, "|");				//功能码
				/*
				snprintf((char *)tempc,sizeof(tempc),"%hhu|%s|%hhu;%hu;|",dev_type,dev_code,function,crcRx);				
				snprintf((char *)tempc,sizeof(tempc),"3|%s|6;%u;%hu-%hhu-%hhu %hhu:%hhu;%.2f;%u|",dev_code,control_mode,lora_rtc.years,lora_rtc.months,lora_rtc.days,lora_rtc.hours,lora_rtc.minutes,set_open,set_direction);
				snprintf((char *)tempc,sizeof(tempc),"3|%s|3;|",dev_code);
				*/
				sscanf(tempptr,"%hu;%hhu;%hu-%hhu-%hhu %hhu:%hhu;%hhu;%f;%hhu;%u;%u;%u;",
				&Lora_Record.check,&Lora_Record.control_mode,&lora_rtc.years,&lora_rtc.months,
				&lora_rtc.days,&lora_rtc.hours,&lora_rtc.minutes,&lora_rtc.secs,&Control_Par.set_opening,
				&Control_Par.set_direction,&nosave_par.ZONE_ID,&nosave_par.DEV_ID,&nosave_par.GROUP_ID);
				//记录接收到的crcRx排除重复报文
				if(Lora_Record.check==7)									//终端上报  中控回复
				{
					nosave_par.report_flag=1;
				}
//				else if(Lora_Record.check==3)
//				{
//					Lora_Answer(Weather_dev_SavePar.Dev_Type,3,crcRx,Control_Par.set_direction);					//中控发送读取或控制 回复
//					
//					nosave_par.Status_Irrigate = 0;
//					memcpy(&nosave_par.Irrigate_EndT,&g_rtc,sizeof(g_rtc));		
//					nosave_par.Send_Count = 0;
//					nosave_par.reportflag_t=0;
//					nosave_par.report_flag=2;
//				}
				else if(Lora_Record.check==6)				//开三种模式			1、手动开闭		2、定时开闭		3、定流量开闭		4、三种模式都停止
				{
					nosave_par.Status_Irrigate = 0;
					Lora_Answer(Weather_dev_SavePar.Dev_Type,6,crcRx,Control_Par.set_direction);					//中控发送读取或控制 回复
					
					//相同执行指令  只回复不执行			指令执行完毕后将此清空
					if(nosave_par.Recive_Crc ==crcRx)
					{
						return 0;									
					}
					nosave_par.Recive_Crc=crcRx;
//					Control_Par.control_run=Control_Par.set_direction;
//					if(Control_Par.set_direction==STOP)
//					{
//						nosave_par.stopmotor_flag=1;
//						return 0;
//					}
					if(Control_Par.set_direction == CANCEL) //取消
					{
						Control_Par.control_run = 0;
						return 0;
					}
					else if(Control_Par.set_direction == CONTINUE) //继续
					{
						Control_Par.control_run=1;
						return 0;
					}
					
//					switch(Lora_Record.control_mode)
//					{
//						case SCHEDULED:
//							Control_Par.control_mode=Lora_Record.control_mode;
//							Control_Par.scheduled=Control_Par.set_opening*60;					//发过来为min 	s
//						break;
//						
//						case MANUAL:
//							Control_Par.control_mode=Lora_Record.control_mode;
//						break;
//						
//						case FIXEDFLOW:
//							Control_Par.control_mode=Lora_Record.control_mode;
//							Control_Par.fixed_flow=Control_Par.set_opening;					//发过来为水量	方			
//						break;
//						
//						case FIXEDLEVEL:
//							Control_Par.control_mode=Lora_Record.control_mode;							//发过来为水位		mm		
//							if(Control_Par.set_opening >= (Weather_dev_SavePar.Aft_AirH*1000))
//							{
//								Control_Par.fixed_level = (Weather_dev_SavePar.Aft_AirH*1000);
//							}
//							else
//							{
//								Control_Par.fixed_level=Control_Par.set_opening;						
//							}
//						break;
//						
//						default:
//							return 0;
//						break;
//					}
//					
//					Control_Par.DEV_ID = nosave_par.DEV_ID;
//					Control_Par.ZONE_ID = nosave_par.ZONE_ID;
//					Control_Par.GROUP_ID = nosave_par.GROUP_ID;
//					
//					nosave_par.stopmotor_flag=0;
//					nosave_par.cancelmotor_flag = 0;
//					
//					Control_Par.control_run=1;	
				}
				sscanf(tempptr,"%hu;%f;%hhu;%f;%lf;%f;%f;%f;%f;%f;%hhu;%hhu;%hu-%hhu-%hhu %hhu:%hhu;%hu-%hhu-%hhu %hhu;%hhu;%u;%f;%hu;%hu;%hu;%f;%f;|",
				&Lora_Record.check,&Lora_Record.set_opening,&Lora_Record.set_direction,&Lora_Record.Irrigate_quota,&Lora_Record.cumulative_flow,
				&Lora_Record.stress,&Lora_Record.tempeture,&Lora_Record.F_Lon,&Lora_Record.F_Lat,&Lora_Record.voltage,&Lora_Record.loraState,&Lora_Record.GPSState,
				&nosave_par.Irrigate_StartT.years,&nosave_par.Irrigate_StartT.months,&nosave_par.Irrigate_StartT.days,&nosave_par.Irrigate_StartT.hours,&nosave_par.Irrigate_StartT.minutes,
				&nosave_par.Irrigate_EndT.years,&nosave_par.Irrigate_EndT.months,&nosave_par.Irrigate_EndT.days,&nosave_par.Irrigate_EndT.hours,&nosave_par.Irrigate_EndT.minutes,
				&Lora_Record.flow_collectT,&Lora_Record.Collect_stresst,&Lora_Record.AMAX,&Lora_Record.VMAX,&Lora_Record.report_time,&Lora_Record.Fro_AirH,&Lora_Record.Aft_AirH
				);
				
				if(Lora_Record.check==5)			//中控设置终端
				{
					Lora_Answer(Weather_dev_SavePar.Dev_Type,5,crcRx,Control_Par.set_direction);
					if(Lora_Record.set_direction==0) //定开度
					{
						Control_Par.control_mode=0;
						Control_Par.set_openingm=Lora_Record.set_opening;
						Flash_SaveControl_Par();
					}
					else if(Lora_Record.set_direction==1)
					{
						Control_Par.control_mode=1;
						Control_Par.set_level=Lora_Record.set_opening;
						Flash_SaveControl_Par();
					}
					else if(Lora_Record.set_direction==2)
					{
						Control_Par.control_mode=2;
						Control_Par.set_flow=Lora_Record.set_opening;
						Flash_SaveControl_Par();
					}
					
				}
			}
			
			/*
				snprintf((char *)tempc,sizeof(tempc),"3|%s|5;%f;%u;%f;%lf;%f;%f;%f;%f;%f;%hhu;%hhu;%04u-%02u-%02u %02u:%02u;%04u-%02u-%02u %02u;%02u;%u;%f;%hu;%hu;%hu;|",lora_record.dev_code,
		lora_record.set_opening,lora_record.set_direction,lora_record.Irrigate_quota,lora_record.cumulative_flow,
		lora_record.stress,lora_record.tempeture,lora_record.F_Lon,lora_record.F_Lat,lora_record.voltage,lora_record.loraState,lora_record.GPSState,
		lora_record.Irrigate_StartT.years,lora_record.Irrigate_StartT.months,lora_record.Irrigate_StartT.days,lora_record.Irrigate_StartT.hours,lora_record.Irrigate_StartT.secs,
		lora_record.Irrigate_EndT.years,lora_record.Irrigate_EndT.months,lora_record.Irrigate_EndT.days,lora_record.Irrigate_EndT.hours,lora_record.Irrigate_EndT.minutes,
		lora_record.flow_collectT,lora_record.Collect_stresst,lora_record.AMAX,lora_record.VMAX,lora_record.report_time
	);
			*/
			
//			sscanf(tempptr,"%hu;%f;%hhu;%f;%lf;%f;%f;%f;%f;%f;%hhu;%hhu;%hu-%hhu-%hhu %hhu:%hhu;%hu-%hhu-%hhu %hhu;%hhu;%u;%f;%hu;%hu;%hu;%f;%f;|",&Lora_Record.check,&Lora_Record.set_opening,&Lora_Record.set_direction,&Lora_Record.Irrigate_quota,&Lora_Record.cumulative_flow,
//			&Lora_Record.stress,&Lora_Record.tempeture,&Lora_Record.F_Lon,&Lora_Record.F_Lat,&Lora_Record.voltage,&Lora_Record.loraState,&Lora_Record.GPSState,
//			&nosave_par.Irrigate_StartT.years,&nosave_par.Irrigate_StartT.months,&nosave_par.Irrigate_StartT.days,&nosave_par.Irrigate_StartT.hours,&nosave_par.Irrigate_StartT.minutes,
//			&nosave_par.Irrigate_EndT.years,&nosave_par.Irrigate_EndT.months,&nosave_par.Irrigate_EndT.days,&nosave_par.Irrigate_EndT.hours,&nosave_par.Irrigate_EndT.minutes,
//			&Lora_Record.flow_collectT,&Lora_Record.Collect_stresst,&Lora_Record.AMAX,&Lora_Record.VMAX,&Lora_Record.report_time,&Lora_Record.Fro_AirH,&Lora_Record.Aft_AirH
//			);
//			
//			if(Lora_Record.check==5)			//中控设置终端
//			{
//				Lora_Answer(Weather_dev_SavePar.Dev_Type,5,crcRx,Control_Par.set_direction);					
//				nosave_par.SetPara_flag=1;		
//			}
		}
	}
	
	if(strcmp(tempptr,EXTERNDEV_TYPE)==0)
	{
		tempptr = strtok(NULL, "|");
		if(strcmp(tempptr,dev_SavePar.Externdev_code) == 0)
		{
			tempptr = strtok(NULL, "|");
			
			if(strcmp(tempptr,"3")==0)
			{
				tempptr = strtok(NULL, "|");
				sscanf(tempptr,"%lf;%lf;%lf;",&Log_Par.this_water,&Log_Par.year_water,&Log_Par.accu_water);
			}
		}
	}
}
void Lora_SetDevCode(u8 check)
{
	uint16_t crcCalc; //存储计算出来的crc结果
	char tempc[300];
	u32 tx_len=0;
	u32 i=0;
	memset(E22_TxBuff,0,sizeof(E22_TxBuff));
	
	E22_TxBuff[0]=(HostAddr>>8);
	E22_TxBuff[1]=(HostAddr&0xff);
	E22_TxBuff[2]=HostChannel;							//给不同信道的相应地址的lora通信

	//分包限定240字节
//	snprintf((char *)tempc,sizeof(tempc),"5|%s|7;%f;%u;%f;%lf;%f;%f;%f;%f;%f;%u;%u;%04d-%02d-%02d %02d:%02d;%04d-%02d-%02d %02d;%02d;%f;%f;0;0;|",Weather_dev_SavePar.dev_code,
//		Control_Par.set_opening,Control_Par.set_direction,Weather_dev_SavePar.Irrigate_quota,RS485_Sensor.cumulative_flow,
//		RS485_Sensor.stress,RS485_Sensor.tempeture,L76K_Par.F_Lon,L76K_Par.F_Lat,Weather_dev_SavePar.Dev_voltage,Statlog_Par.loraState,Statlog_Par.GPSState,
//		nosave_par.Irrigate_StartT.years,nosave_par.Irrigate_StartT.months,nosave_par.Irrigate_StartT.days,nosave_par.Irrigate_StartT.hours,nosave_par.Irrigate_StartT.secs,
//		nosave_par.Irrigate_EndT.years,nosave_par.Irrigate_EndT.months,nosave_par.Irrigate_EndT.days,nosave_par.Irrigate_EndT.hours,nosave_par.Irrigate_EndT.minutes,
//		Weather_dev_SavePar.Collect_ultrasonict,Weather_dev_SavePar.Collect_stresst
//	);
	snprintf((char *)tempc,sizeof(tempc),"3|%s|%hhu|",Mac_String,check);	
		
	strcat((char *)E22_TxBuff,tempc);
	tx_len=strlen((char *)E22_TxBuff);
	crcCalc=crc(E22_TxBuff+3,tx_len-3);                       //计算的crc
	E22_TxBuff[tx_len++]=(crcCalc>>8);
	E22_TxBuff[tx_len++]=(crcCalc&0xff);
	
	E22_400T22S_SetMode(0);
	
	vTaskSuspendAll();													//挂起调度器		
	E22_400T22S_Send((u8 *)E22_TxBuff,tx_len);
	xTaskResumeAll();		

	E22_400T22S_SetMode(1);
}
void Lora_Answer(u8 Dev_type,u8 function,u16 crcRx,float set_direction)
{
	uint16_t crcCalc; //存储计算出来的crc结果
	char tempc[100];
	u32 tx_len=0;
	memset(E22_TxBuff,0,sizeof(E22_TxBuff));
	
	E22_TxBuff[0]=(HostAddr>>8);
	E22_TxBuff[1]=(HostAddr&0xff);
	E22_TxBuff[2]=HostChannel;			//给不同信道的相应地址的lora通信
	
	snprintf((char *)tempc,sizeof(tempc),"%hhu|%s|%hhu;%f;%hu;|",Dev_type,Weather_dev_SavePar.dev_code,function,set_direction,crcRx);							
	strcat((char *)E22_TxBuff,tempc);
	
	tx_len=strlen((char *)E22_TxBuff);
	crcCalc=crc(E22_TxBuff+3,tx_len-3);                       //计算的crc
	E22_TxBuff[tx_len++]=(crcCalc>>8);
	E22_TxBuff[tx_len++]=(crcCalc&0xff);
	
	E22_400T22S_SetMode(0);	
	
	vTaskSuspendAll();													//挂起调度器		
	E22_400T22S_Send((u8 *)E22_TxBuff,tx_len);
	xTaskResumeAll();							

	E22_400T22S_SetMode(1);
}

void Lora_ReadTerminal(char *dev_code,char* Dev_type)
{
	if((dev_SavePar.Flow_Fun==0) || (dev_SavePar.TypeUltra != 1) || (nosave_par.Read_ExterndevF != 1))
	{
		return ;
	}
	
	nosave_par.Read_ExterndevF=0;
	
	uint16_t crcCalc; //存储计算出来的crc结果
	char tempc[300]="";
	u32 tx_len=0;
	memset(Lora_Txbuff,0,sizeof(Lora_Txbuff));
	
	Lora_SendAddr(Dev_type);
	snprintf((char *)Lora_Txbuff,sizeof(Lora_Txbuff),"%s%s|",Lora_Txbuff,LORA_PROTOCOL_VER); //协议版本
	snprintf((char *)Lora_Txbuff,sizeof(Lora_Txbuff),"%s%s|",Lora_Txbuff,dev_SavePar.dev_code); //设备编号
//	snprintf((char *)Lora_Txbuff,sizeof(Lora_Txbuff),"%s%s|",Lora_Txbuff,dev_code); //关联设备
	
	
	snprintf((char *)tempc,sizeof(tempc),"%s|%s|3|",Dev_type,dev_code);
	strcat((char *)Lora_Txbuff,tempc);
	
	tx_len=strlen((char *)Lora_Txbuff);
	crcCalc=crc(Lora_Txbuff+3,tx_len-3);                       //计算的crc
	Lora_Txbuff[tx_len++]=(crcCalc>>8);
	Lora_Txbuff[tx_len++]=(crcCalc&0xff);
	
	vTaskSuspendAll();													//挂起调度器		
	Lora_Send((u8 *)Lora_Txbuff,tx_len);
	xTaskResumeAll();	
}

void Lora_SendAddr(char * Dev_Type)
{
	if(strcmp(Dev_Type,EXTERNDEV_TYPE) == 0)
	{
		Lora_Txbuff[0]=(ExternDevAddr>>8);
		Lora_Txbuff[1]=(ExternDevAddr&0xff);
		Lora_Txbuff[2]=ExternDevChannel;	
	}
}

