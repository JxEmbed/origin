#include "data.h"
#include "math.h"
#include "string.h"
#include "ff_user.h"
#include "myOnenet.h"
float TOF1;
float TIME1;
float TIME2;
float normLSB;
float CLOCKperiod;
float calCount;
u32 CALIBRATION1;  //寄存器
u32 CALIBRATION2;  //寄存器
float CALIBRATION2_PERIODS; //设置第二次校准
float CLOCK_COUNT1;
char imgData[2];
uint16_t imgData_Len=0;
u8 calcFlag=0;
#define Q_BUFF_LEN 10

u8 Q_Ptr=0;
float Q_Buff[Q_BUFF_LEN];  //历史十个流量数据
float Q_BuffPr[Q_BUFF_LEN]; //排序后的流量数据


u8 DEV_isZhang=0;

STRUCT_ACCWATER AccWater_Par;
float distance;
STRUCT_Log_Par  Log_Par;
void calc(void)
{
	//19 0001 1001
	CALIBRATION2=TDC7200_Read24(0x1c,0x00); //1c-
	CALIBRATION1=TDC7200_Read24(0x1b,0x00);
	CALIBRATION2_PERIODS=2;
	CLOCKperiod=1.0f/8000000;
	TIME1=TDC7200_Read24(0x10,0x00);
	CLOCK_COUNT1=TDC7200_Read24(0x11,0x00);
	TIME2=TDC7200_Read24(0x12,0x00);
	calCount=(CALIBRATION2-CALIBRATION1)/(CALIBRATION2_PERIODS-1);
	normLSB=CLOCKperiod/calCount;
	//这一步计算时间
	TOF1=TIME1*normLSB+CLOCK_COUNT1*CLOCKperiod-TIME2*normLSB;  
//	TOF1-=5.63793194*pow(10,-7);
	TOF1-=6.43722*pow(10,-7);
	distance=TOF1*1500;

	
	
}

float Filter_Smooth2(uint8_t num)
{
	float tempf=0;
	float result;
	uint8_t i,j;
	uint8_t datlen=0;
	for(i=0;i<num;i++)
	{
		datlen=0;
		for(j=0;j<FILTER_NUM;j++)
		{
			if(Ultra_CALCU_Par.Error_Flag2[j]!=1)
			{
				tempf+=Ultra_CALCU_Par.tofData_for[i][j];
				datlen++;
			}
		}
		Ultra_CALCU_Par.T_for[i]=tempf/datlen;
		
	}
	for(i=0;i<num;i++)
	{
		datlen=0;
		for(j=0;j<FILTER_NUM;j++)
		{
			if(Ultra_CALCU_Par.Error_Flag2[j]!=1)
			{
				tempf+=Ultra_CALCU_Par.tofData_rev[i][j];
				datlen++;
			}
		}
		Ultra_CALCU_Par.T_rev[i]=tempf/datlen;
		
	}
	
}
float Filter_Smooth(float * datbuff,uint8_t num,uint8_t remnum)
{
	float tempf;
	float result;
	uint8_t i,j;
	if((remnum*2)>num)
	{
		return -1;
	}
	//冒泡排序
	for(i=0;i<(num-1);i++)
	{
		for(j=i;j<num;j++)
		{
			if(datbuff[i]>datbuff[j])
			{
				tempf=datbuff[i];
				datbuff[i]=datbuff[j];
				datbuff[j]=tempf;
			}
		}
	}
	/*
	0,1,2去掉最小值
	3,4,5,6
	7,8,9去掉最大值
	
	*/
	tempf=0;
	for(i=remnum;i<(num-remnum);i++)
	{
		tempf+=datbuff[i];
	}
	result=tempf/(num-(remnum*2));
	return result;
}

/*
流量处理算法
*/
float Flow_Man_Fun1(float * datbuff,u8 channel,uint8_t remnum)
{
	float tempf;
	float result;
	uint8_t i,j;
	u8 negNum=0;
	float tempbuff[FILTER_NUM];

	for(i=0;i<FILTER_NUM;i++)
	{
		if(Ultra_CALCU_Par.Error_Flag2[i]==1)
		{
			continue;
		}
		if(datbuff[i]<0)
		{
			continue;
		}
		tempbuff[negNum++]=datbuff[i];
	}
	if(negNum<=6)           //如果非负数的数量小于等于6，则退出		
	{
		Ultra_CALCU_Par.Error_Flag[channel]=1;
		return 0.0f;
	}
	//冒泡排序
	for(i=0;i<(negNum-1);i++)
	{
		for(j=(i+1);j<negNum;j++)
		{
			if(tempbuff[i]<tempbuff[j])
			{
				tempf=tempbuff[i];
				tempbuff[i]=tempbuff[j];
				tempbuff[j]=tempf;
			}
		}
	}
	
	
	tempf=0;
	/*
	15
	0,1,2
	12,13,14
	15-6=9
	14
	0,1,2
	3----11
	*/
	for(i=3;i<(negNum-3);i++)
	{
		tempf+=tempbuff[i];
	}
	result=tempf/(negNum-6);
	Ultra_CALCU_Par.Error_Flag[channel]=0;
	return result;
}
void imgTrans(char * imgStr,uint8_t * buff,uint16_t len)
{
	uint16_t i=0;
	uint8_t tempu8;
	for(i=0;i<len;i++)
	{
		imgStr[i]=buff[i];
	}
//	for(i=0;i<len;i++)
//	{
//		tempu8=(buff[i]>>4);
//		if (tempu8 <= 9)
//            imgStr[i*2] = tempu8+ '0';
//        else
//            imgStr[i*2] = tempu8+ 'A' - 0x0a;
//		tempu8=(buff[i]&0xf);
//		if (tempu8 <= 9)
//            imgStr[i*2+1] = tempu8+ '0';
//        else
//            imgStr[i*2+1] = tempu8+ 'A' - 0x0a;
//	}
}

u8 DateCmp(DateTimeDef dateTime1,DateTimeDef dateTime2)
{
	if(dateTime1.years>dateTime2.years)
	{
		return 0;
	}
	if(dateTime1.months>dateTime2.months)
	{
		return 0;
	}
	if(dateTime1.days>dateTime2.days)
	{
		return 0;
	}
	
	return 1;
}

/*
流量处理程序
*/
void flow_Process(void )
{
	u8 i=0;
	u8 j=0;
	float tempf;
	//存储数据
	Q_Buff[Q_Ptr]=Ultra_CALCU_Par.Q_Total;
	Q_Ptr++;
	if(Q_Ptr>=Q_BUFF_LEN)
	{
		Q_Ptr=0;
	}
	//复制数据
	for(i=0;i<Q_BUFF_LEN;i++)
	{
		Q_BuffPr[i]=Q_Buff[i];
	}
	//处理数据
	for(i=0;i<(Q_BUFF_LEN-1);i++)
	{
		for(j=i;j<Q_BUFF_LEN;j++)
		{
			if(Q_BuffPr[i]>Q_BuffPr[j])
			{
				tempf=Q_BuffPr[i];
				Q_BuffPr[i]=Q_BuffPr[j];
				Q_BuffPr[j]=tempf;
			}
		}
	}
	//计算平均值
	/*
	01 234567 89
	10-7-3
	*/
	tempf=0;
	for(i=2;i<=(Q_BUFF_LEN-3);i++)
	{
		tempf+=Q_BuffPr[i];
	}
		
	Ultra_CALCU_Par.Q_Total=tempf/(Q_BUFF_LEN-4);
	if(isnan(Ultra_CALCU_Par.Q_Total))
	{
		Ultra_CALCU_Par.Q_Total=0;
	}
}
#define DATA_CHANGED_FLAG 3
void parameter_set(void)
{
	if(dev_SavePar.parSetFlag!=DATA_CHANGED_FLAG)
	{
		//产品名称
		strcpy(dev_SavePar.dev_name,"渠道智能测控分体板闸");
		//出厂时间
		dev_SavePar.factory_time.years=2024;
		dev_SavePar.factory_time.months=6;
		dev_SavePar.factory_time.days=1;
		dev_SavePar.factory_time.hours=0;
		dev_SavePar.factory_time.minutes=0;
		dev_SavePar.factory_time.secs=0;
		//设备编号
		//strcpy(dev_SavePar.dev_code,"3510584962370");
		//外形尺寸
		memcpy(dev_SavePar.form_factor,"0.6m*2.2m",sizeof(dev_SavePar.form_factor));
		//电机功率
		memcpy(dev_SavePar.motor_power,"150W",sizeof(dev_SavePar.motor_power));
/********************************************************************/		
		//进水宽度
		dev_SavePar.water_inletWidth=0.8;  
		//探头初距
		dev_SavePar.Ultra_initialDist=0.10;
		//闸前净空
		dev_SavePar.FroHeight_Air=5;		
		
		//进水高度
		dev_SavePar.water_inletHeight=1;
		//探头间距
		dev_SavePar.Ultra_Spacing=0.1;
		//闸后净空
		dev_SavePar.AftHeight_Air=5;
		
		////底板高差
		dev_SavePar.base_heightDiff=0;
		//探头层数
		dev_SavePar.Ultra_Num=8;
		//渠底比降
		dev_SavePar.Gradient=0.0007;
/*********************************************************/
		//最小流量
		dev_SavePar.flow_min=0;
		//最小水位
		dev_SavePar.level_min=0;
		//数据间隔
		dev_SavePar.data_interval=6*60;
		//边壁系数
		dev_SavePar.Side_K=0.85;
		
		//最大流量
		dev_SavePar.flow_max=0.7;
		//最大水位
		dev_SavePar.level_max=1;
		//发送间隔
		dev_SavePar.send_interval=(6*60*60);
		//采集间隔
		dev_SavePar.collect_interval=(10*60);
		//流量系数
		dev_SavePar.Correct_K=1.02;
		
		//流量偏差
		dev_SavePar.flow_deviation=5;
		//水位偏差
		dev_SavePar.level_deviation=5;
		//声道角度
		dev_SavePar.Channel_Angle=70;
		//明渠糙率
		dev_SavePar.Roughness=0.012;
		
		dev_SavePar.Safe_SpaceHigh=0;
		
//		//上传地址  8.131.252.104:8113
//		memset(dev_SavePar.send_netaddr,0,sizeof(dev_SavePar.send_netaddr));
//		memset(dev_SavePar.send_port,0,sizeof(dev_SavePar.send_port));
/*********************************************************/	
		strcpy(Author_Par.admin_phone,"1");
		strcpy(Author_Par.opera1_phone,"2");
		strcpy(Author_Par.opera2_phone,"3");
		strcpy(Author_Par.maint_phone,"4");
		strcpy(Author_Par.tempo_phone,"5");
		
		memcpy(Author_Par.admin_pass,"111111\0\0",sizeof(Author_Par.admin_pass));
		memcpy(Author_Par.opera1_pass,"222222\0\0",sizeof(Author_Par.opera1_pass));
		memcpy(Author_Par.opera2_pass,"333333\0\0",sizeof(Author_Par.opera2_pass));
		memcpy(Author_Par.maint_pass,"950821\0\0",sizeof(Author_Par.maint_pass));
		memcpy(Author_Par.tempo_pass,"000000\0\0",sizeof(Author_Par.tempo_pass));
		
		dev_SavePar.parSetFlag=DATA_CHANGED_FLAG;
		Flash_SaveDevData();
		Flash_SaveAuthor_Par();
	}
}


void GetSensor(void)
{
	u8 i=0;
	RS485_ReadLevel();	
	Ultra_ParPrePare();
	

	getAllChannelValue(dev_SavePar.Ultra_Num);
//	Get_AllChannel_Speed(dev_SavePar.Ultra_Num);
	Get_Flow_data(dev_SavePar.Ultra_Num);

	for(i=0;i<dev_SavePar.Ultra_Num;i++)
	{
		if(Ultra_CALCU_Par.V[i]<0)
		{
			return;
		}
		
	}
	
//	flow_Process();
	if(isnan(Ultra_CALCU_Par.Q_Total))
	{
		Ultra_CALCU_Par.Q_Total=0;
	}
	if(isinf(Ultra_CALCU_Par.Q_Total))
	{
		Ultra_CALCU_Par.Q_Total=0;
	}
}
RUNLOG_STRUCT temprunlog;
//STRUCT_ACCWATER
void readflowacc(void)
{
	
//	ff_readrunlog(&temprunlog,Log_Par.runlog_rows);
//	AccWater_Par._last_rtc.years=temprunlog.time.years;
//	AccWater_Par._last_rtc.months=temprunlog.time.months;
//	AccWater_Par._last_rtc.days=temprunlog.time.days;
//	AccWater_Par._last_rtc.hours=temprunlog.time.hours;
//	AccWater_Par._last_rtc.minutes=temprunlog.time.minutes;
//	AccWater_Par._last_rtc.secs=temprunlog.time.secs;
	
//	AccWater_Par.this_water =Log_Par.this_water;   //本次水量 
//	AccWater_Par.day_water  =Log_Par.day_water;   //当日水量
//	AccWater_Par.month_water=Log_Par.month_water;  //本月水量
//	AccWater_Par.year_water =Log_Par.year_water;   //本年水量
//	AccWater_Par.accu_water =Log_Par.accu_water;   //累计水量
	
	
	
}
u32 time_acc;
DateTimeDef clear_dt;
void CalcAccmuWater(void)
{
	//测流功能(0关闭)				测流设备为测流槽
	if((dev_SavePar.Flow_Fun==1) && (dev_SavePar.TypeUltra == 1))
	{
		return ;
	}
	
	u32 count1=0,count2=0;
	float accwater_temp=0;		//采集时间内的水量
	
	//水量=时间 * 瞬时流量
	if(nosave_par.time_OK==1&&AccWater_Par._last_rtc.years!=0)
	{
		count1=DateTime_ToCounts(AccWater_Par._last_rtc);
		count2=DateTime_ToCounts(g_rtc);
		time_acc=(count2-count1);
		accwater_temp=(count2-count1)*Ultra_CALCU_Par.Q_Total/10000;		//单位万方
		
		//数据异常过大
		if(accwater_temp>1)
		{
			accwater_temp=0;
		}
	}
	else if(nosave_par.time_OK==1)   //第一次计算时,存储采集开始时间点后退出
	{
		////memcpy(&AccWater_Par._last_rtc,&g_rtc,sizeof(g_rtc));
		AccWater_Par._last_rtc.years=g_rtc.years;
		AccWater_Par._last_rtc.months=g_rtc.months;
		AccWater_Par._last_rtc.days=g_rtc.days;
		AccWater_Par._last_rtc.hours=g_rtc.hours;
		AccWater_Par._last_rtc.minutes=g_rtc.minutes;
		AccWater_Par._last_rtc.secs=g_rtc.secs;
		return;
	}
	else 	//时间获取错误	_last_rtc.years==0
	{
		return;
	}
	
	//重置年、月累计水量
	if(AccWater_Par._last_rtc.years!=g_rtc.years)
	{
		Log_Par.year_water=0;		
	}
	if(AccWater_Par._last_rtc.months!=g_rtc.months)
	{
		Log_Par.month_water=0;		
	}
	if(AccWater_Par._last_rtc.days!=g_rtc.days)
	{
		Log_Par.day_water=0;
	}
	
//	//采集时间超过一天		不正常除非代码跑飞		当日流量从0点更改为早8重置
//	if(AccWater_Par._last_rtc.days!=g_rtc.days)
//	{
//		clear_dt.years=g_rtc.years;
//		clear_dt.months=g_rtc.months;
//		clear_dt.days=g_rtc.days;
//		clear_dt.hours=dev_SavePar.statistical_time;
//		clear_dt.minutes=0;
//		clear_dt.secs=0;
//		calcFlag=1;
//	}
//	if(calcFlag==4)
//	{
//		Log_Par.day_water=0;
//		calcFlag=0;
//	}
	
	//当Ultra_CALCU_Par.Q_Total大于当前阈值后，才进行本次水量的累加,否则本次水量置0
	if(Ultra_CALCU_Par.Q_Total<=0.003)
	{
		Log_Par.this_water=0;
	}
	
	//存储下一次水量计算起始时间点
	//memcpy(&AccWater_Par._last_rtc,&g_rtc,sizeof(g_rtc));
	AccWater_Par._last_rtc.years=g_rtc.years;
	AccWater_Par._last_rtc.months=g_rtc.months;
	AccWater_Par._last_rtc.days=g_rtc.days;
	AccWater_Par._last_rtc.hours=g_rtc.hours;
	AccWater_Par._last_rtc.minutes=g_rtc.minutes;
	AccWater_Par._last_rtc.secs=g_rtc.secs;
	
	//计算的水量累计
	Log_Par.this_water  +=accwater_temp;
	Log_Par.year_water  +=accwater_temp;
	Log_Par.month_water +=accwater_temp;
	Log_Par.day_water   +=accwater_temp;
	Log_Par.accu_water   +=accwater_temp;
	
//	if(calcFlag==2)
//	{
//		calcFlag=3;
//	}
	//数值无穷大  数据异常置0
	if(isinf(Log_Par.this_water))
	{
		Log_Par.this_water=0;
	}
	if(isinf(Log_Par.day_water))
	{
		Log_Par.day_water=0;
	}
	if(isinf(Log_Par.month_water))
	{
		Log_Par.month_water=0;
	}
	if(isinf(Log_Par.year_water))
	{
		Log_Par.year_water=0;
	}
	if(isinf(Log_Par.accu_water))
	{
		Log_Par.accu_water=0;
	}
	//数值无效    数据异常置0
	if(isnan(Log_Par.this_water))
	{
		Log_Par.this_water=0;
	}
	if(isnan(Log_Par.day_water))
	{
		Log_Par.day_water=0;
	}
	if(isnan(Log_Par.month_water))
	{
		Log_Par.month_water=0;
	}
	if(isnan(Log_Par.year_water))
	{
		Log_Par.year_water=0;
	}
	if(isnan(Log_Par.accu_water))
	{
		Log_Par.accu_water=0;
	}
//	Flash_SaveWaterAcc();
}

