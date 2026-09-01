#include "gprstask.h"

struct RANDER_FLOW_DATA Rander_Flow_Data = {7.5,0.5,12,8,1.5,0,0.5,1,0};

//数据流************************************************************************
//char Version[3];
//char	Air_Level[10];
//char	Ave_Speed[10];
//char	Water_Level[10];
//char	Water_Speed[30];
//char	Water_Flow[10];
//char	Sediment_Level[10];


//char GPRSModuleNO[17];
//char Time[17];
//char ST_data_8[100];
//char ST_data_12[100];
//char SM_data_8[100];
//char SM_data_12[100];
//DATA_STREAM data_stream[] = {
//									{"Water_Level", Water_Level, TYPE_STRING, 1},				//水深
//								//{"Version", Version, TYPE_STRING, 1},
//								//{"GPRSModuleNO", GPRSModuleNO, TYPE_STRING, 1},
//								//{"Time",Time, TYPE_STRING, 1},
//														};
//DATA_STREAM data_stream_temperture[] = {																//土壤温度数据 Soil Temperature
//								{"ST_data_8",ST_data_8, TYPE_STRING, 1},
//														};
/////4
//DATA_STREAM data_stream_moisture[] = {																	////土壤湿度数据 Soil moisture
//								{"Sediment_Level",Sediment_Level, TYPE_STRING, 1},
//														};
/////2
//DATA_STREAM data_stream_Water_Level[] = {																//土壤温度数据 Soil Temperature
//								{"Water_Level",Water_Level, TYPE_STRING, 1},
//								{"Air_Level",Air_Level, TYPE_STRING, 1},
//								{"Ave_Speed",Ave_Speed, TYPE_STRING, 1}
//														};
//////1
//DATA_STREAM data_stream_Water_Speed[] = {																	////土壤湿度数据 Soil moisture
//								{"Water_Speed",Water_Speed, TYPE_STRING, 1},
//														};
/////3
//DATA_STREAM data_stream_Water_Flow[] = {																	////土壤湿度数据 Soil moisture
//								{"Water_Flow",Water_Flow, TYPE_STRING, 1},
//														};
//unsigned char data_stream_cnt = sizeof(data_stream) / sizeof(data_stream[0]);
//unsigned char data_stream_cnt_st = sizeof(data_stream_temperture) / sizeof(data_stream_temperture[0]);
//unsigned char data_stream_cnt_sm = sizeof(data_stream_moisture) / sizeof(data_stream_moisture[0]);


//unsigned char data_stream_cnt_Water_Level = sizeof(data_stream_Water_Level) / sizeof(data_stream_Water_Level[0]);
//unsigned char data_stream_cnt_Water_Speed = sizeof(data_stream_Water_Speed) / sizeof(data_stream_Water_Speed[0]);
//unsigned char data_stream_cnt_Water_Flow = sizeof(data_stream_Water_Flow) / sizeof(data_stream_Water_Flow[0]);		

void Make_Rander_Water_Data()
{
//	int i;
//	char Temp[40];
//	float ftemp,sed_temp;
//	float *p = NULL;
//	
////	ftemp=GetMesQ(11,GetMesV(11));
////	ftemp = 10 - ftemp;
//	//ftemp = 0.45;
//	if(ftemp >= 10)
//	{
//		ftemp = 0;
//	}
//	ftemp = 	Rander_Flow_Data.h2;
//	sed_temp = Rander_Flow_Data.h2;
//	//ftemp = ftemp/100;
//	FloatToStr(ftemp,Temp,2);
//	sprintf(Sediment_Level,"%.3f",ftemp);
//	//探头到水面的距离
////	ftemp=GetMesQ(16,GetMesV(16));
//	ftemp = ftemp/10;
//	FloatToStr(ftemp,Temp,2);
//	sprintf(Air_Level,"%.2f",ftemp);	
//	//水深
////	ftemp=GetMesQ(16,GetMesV(16));
//	ftemp = ftemp/10;
//	ftemp = ftemp - Rander_Flow_Data.H1;
//	if( ftemp <= 0)
//	{
//		ftemp = 0;
//	}
//	else
//	{
//		ftemp = Rander_Flow_Data.H2 + sed_temp - ftemp;
//		if( ftemp < 0)
//		{
//			ftemp = 0;
//		}
//	}
//	FloatToStr(ftemp,Temp,2);
//	sprintf(Water_Level,"%.2f",ftemp);	
//	
////	strcat(ST_data_8,Temp_st_6);
////	strcat(ST_data_8,",");
//	//平均流速
//	ftemp = Rander_Flow_Data.average_speed;
//	FloatToStr(ftemp,Temp,2);
//	sprintf(Ave_Speed,"%.2f",ftemp);	

//	
////	ftemp=GetMesQ(12,GetMesV(12));
//	FloatToStr(ftemp,Temp,2);
//	sprintf(Water_Speed,"%.2f",ftemp);
//	strcat(Water_Speed,",");
////	ftemp=GetMesQ(13,GetMesV(13));
//	FloatToStr(ftemp,Temp,2);
//	//sprintf(Water_Speed,"%.2f",ftemp);
//	strcat(Water_Speed,Temp);
//	strcat(Water_Speed,",");
////	ftemp=GetMesQ(14,GetMesV(14));
//	FloatToStr(ftemp,Temp,2);
//	//sprintf(Water_Speed,"%.2f",ftemp);
//	strcat(Water_Speed,Temp);
//	
////	ftemp=GetMesQ(15,GetMesV(15));
//	//ftemp = ftemp/100;
//	FloatToStr(ftemp,Temp,2);
//	
//	sprintf(Water_Flow,"%.3f",ftemp);	
}
void SQ_DATA_Trans()
{
//	data_stream_cnt_sm = sizeof(data_stream_moisture) / sizeof(data_stream_moisture[0]);


//	data_stream_cnt_Water_Level = sizeof(data_stream_Water_Level) / sizeof(data_stream_Water_Level[0]);
//	data_stream_cnt_Water_Speed = sizeof(data_stream_Water_Speed) / sizeof(data_stream_Water_Speed[0]);
//	data_stream_cnt_Water_Flow = sizeof(data_stream_Water_Flow) / sizeof(data_stream_Water_Flow[0]);	
////		GetSensor();
//		memset(Water_Level, 0, 10);
//		memset(Water_Speed, 0, 30);
//		memset(Water_Flow, 0, 10);
//		memset(Sediment_Level, 0, 10);
//		
//		//memset(SM_data_12, 0, 30);
//		//MakeGPRS((char *)GPRS_Send_Buffer);		// 发送数据
//		//Make_verison_moduleNO();
//		//Make_Time();
//		Make_Rander_Water_Data();
////		onenet_info.send_data = SEND_TYPE_DATA;		//连接成功发送数据
////		Judge_SenDdata(SEND_TYPE_DATA); 
//		
//		delay_ms(500);
//		Judge_SenDdata(1); 
//		delay_ms(500);
//		Judge_SenDdata(6); 
//		delay_ms(500);
//		Judge_SenDdata(7);
//		delay_ms(500);
//		Judge_SenDdata(8);
//		delay_ms(500);

}