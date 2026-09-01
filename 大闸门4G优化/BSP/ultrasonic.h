#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H

#include "main.h"


/*********************************************************************************************************
// 名称: Parameter_From_CALCU
// 功能:计算获得的参数
// 备注:  CHANNEL_TOTAL  5					//定义通道数

#define FILTER_NUM
*********************************************************************************************************/
#define CHANNEL_TOTAL  7					//定义通道数
#define FILTER_NUM		15					//通道采集飞行时间滤波次数
#define CHANNEL_MAX  16					//定义最大通道数
typedef struct
{
	float tofData_Temp;
	float tofData_for[CHANNEL_MAX][FILTER_NUM];				//正向时间
	float tofData_rev[CHANNEL_MAX][FILTER_NUM];				//反向时间s
	float T_for[CHANNEL_MAX];             //正向结果
	float T_rev[CHANNEL_MAX];             //反向结果
	u8  Error_Flag[16];
	u8  Error_Flag2[FILTER_NUM];
	float V[CHANNEL_MAX];         //对V_Filter处理后的流速
	float V_Filter[FILTER_NUM];	  //采集计算出来的速度
	float Delta_T;    //
	float Average_T;
	float Q_Calc;
	float Q_Total;			//瞬时流量
//	float Q_Total2;
//	float Q_Total3;
	float Q[CHANNEL_MAX];
	float V_Top;
	float V_Low;
	float Side; //边壁系数
	float Q_Top;  //最顶层流量
	float Q_Low;  //最底层流量
	float k;      //修正系数
	
	float h;      //闸后水位
	float hc;     //探头间距
	float distence_for2[CHANNEL_MAX][FILTER_NUM]; //正向距离
	float distence_rev2[CHANNEL_MAX][FILTER_NUM]; //反向距离
	float distence_for[CHANNEL_MAX]; //距离
	float distence_rev[CHANNEL_MAX]; //距离
	
}PARAMETER_FLOW_CALCU;
typedef struct
{
	float L;    //声波传播的通道的长度 
	float L2;
	float angle;
	float b;    //底宽
	float n;    //糙率
	float p;
}DEVICE_SETTING_STRUCT;

typedef struct
{
	float Parame_ti_1[16];											//箱底流道底板标高
	float Parame_ti_2[16];											//箱体流道宽度
	float tofTemp_1;				//用于临时的存放定时器读取到的数据
	float tofTemp_2;				//用于临时的存放定时器读取到的数据
	float tofDataTemp_1[CHANNEL_TOTAL][FILTER_NUM];				//用于临时的存放定时器读取到的数据
	float tofDataTemp_2[CHANNEL_TOTAL][FILTER_NUM];				//用于临时的存放定时器读取到的数据
	float sumTOF_Data[CHANNEL_TOTAL][FILTER_NUM];					//用于存放飞行时间相加后的数据
	float subTOF_Data[CHANNEL_TOTAL][FILTER_NUM];	
	float aveSubTemp[CHANNEL_TOTAL];
	float tofDataProcess_1[CHANNEL_TOTAL][FILTER_NUM];				//用于临时的存放定时器读取到的数据
	float tofDataProcess_2[CHANNEL_TOTAL][FILTER_NUM];				//用于临时的存放定时器读取到的数据
	float Parame_Hi[16];															//测得的闸前水位
	float Parame_Gi[16];															//测得的闸后水位
	float Parame_Vi[16];															//某一声道线均流速
	//float Parame_Ki[16];														//系数
	float Parame_Vi_AVE[16];													//i 断面均流速
	float Parame_Qi[16];															//计算i断面的流量
	float Parame_Q;																		//计算流量值
	unsigned int Parame_Bi;														//闸门开度
	float Parame_hi;																	//箱体内部水深
	unsigned int Parame_n;														//淹没的流道数
	float Parame_WaterQ_DAY;													//本次灌水量
	float Parame_WaterQ_YEAR;													//年度灌水量
	float Parame_WaterQ_OneDay;												//当日灌水量
/*******************************/
//	u16 originalTimerValue[32];
//	u16 timerValueTOF[32];
	u8  ERRO[16];
	u8	erroCount[16];
	u8  speedValueERRO[16];
	u16 timeA;
	u16	timeB;
	u8	saveFlashTimeFlag;
	u8	dayTimeEndFlag;
	u8	dayTimeStartFlag;
	u8	yearTimeEndFlag;
	u8	yearTimeStartFlag;
} PARAMETER_FROM_CALCU;
void SPI_Software_init(void);
void Ultra_MUX_init(void);
s8 getOneChannelValue(u8 channel,u8 filter,u8 dir);
void Ultra_All_init(void);
void TDC1000_REG_Init(void);
void Ultra_Power_init(void);
void Ultra_Other_init(void);
void Ultra_channelSwitch(u16 Channel);
void TDC7200_Trigger(void);
void TDC1000_REG_InitRead(void);
void SPI_Delay(__IO uint32_t count);
void TDC7200_REG_Init(void);
void Power_5V_init(void);
void Ultra_total(void);
float Get_Channel_Speed(u8 channel);
uint32_t TDC7200_ReadBuff(unsigned char addr,unsigned char data,uint8_t num);
uint32_t TDC7200_Read24(unsigned char addr,unsigned char data);
uint8_t TDC7200_Read(unsigned char addr,unsigned char data);

extern PARAMETER_FLOW_CALCU Ultra_CALCU_Par; //超声波计算参数
extern float Aver_FlowV;
void RS485_Level_Read(void);
void sensor_Read_zhang(void);
void ultra_test(void);
void Ultra_ParPrePare(void);
u8 Get_AllChannel_Speed(u8 channelNUM);
u8 getAllChannelValue(u8 channelNUM);
u8 Get_Flow_data(u8 channelNUM);
s8 Get_Flow_data2(u8 channelNUM);
#define Power_5V_EN PCout(1)
/*MUX********************************************/
#define Ultra_D0    PBout(0)
#define Ultra_D1    PFout(11)
#define Ultra_D2    PFout(13)
#define Ultra_D3    PBout(5)
/***MUX36S16IPWR通道选择芯片******************************************/
#define Ultra_TX1_A_EN_Gpio PFout(10)  
#define Ultra_TX2_A_EN_Gpio PFout(12)
#define Ultra_RX1_A_EN_Gpio PBout(9)
#define Ultra_RX2_A_EN_Gpio PBout(1)
/*SPI通信片选信号*/
#define Ultra_CSB_A PFout(3)
//#define Ultra_CSB_B PGout(9)
#define TDC7200_CSB_A PCout(0)
//#define TDC7200_CSB_B PAout(11)
/*TDC7200其他引脚*/
//#define TDC7200_Trigger_A PFout(8)
//#define TDC7200_Trigger_B PGout(10)
#define TDC7200_EN_A        PCout(1)
//#define TDC7200_EN_B        PDout(6)
#define TDC7200_INT         PCin(2)
/*SPI 引脚*/
#define Ultra_SPI_MISO PFin(1)
#define Ultra_SPI_MOSI PFout(7)
#define Ultra_SPI_SCLK PFout(9)
/*TDC1000其他引脚*/
#define Ultra_RESET_A  PFout(2)
#define Ultra_CHSEL_A  PFout(4)



void Ultra_test(void);

/**UCC27537引脚************/
#define UCC_TX1_AT        PGout(14)
#define UCC_TX2_AB        PGout(15)
//#define UCC_TX1_BT        PGout(3)
//#define UCC_TX2_BB        PGout(4)
/****30V电源使能引脚****************************/
#define Power_30V_EN PCout(3)=1 
#define Power_30V_DIS PCout(3)=0 
/********************************/
#define CHSEL_A_H Ultra_CHSEL_A=1
#define CHSEL_A_L Ultra_CHSEL_A=0
#define Ultra_TX1_A_DIS Ultra_TX1_A_EN_Gpio=0
#define Ultra_TX1_A_EN  Ultra_TX1_A_EN_Gpio=1
#define Ultra_RX1_A_DIS Ultra_RX1_A_EN_Gpio=0
#define Ultra_RX1_A_EN  Ultra_RX1_A_EN_Gpio=1

#define Ultra_TX2_A_DIS Ultra_TX2_A_EN_Gpio=0
#define Ultra_TX2_A_EN  Ultra_TX2_A_EN_Gpio=1
#define Ultra_RX2_A_DIS Ultra_RX2_A_EN_Gpio=0
#define Ultra_RX2_A_EN  Ultra_RX2_A_EN_Gpio=1

#define UCC_DIS_TX1_AT  UCC_TX1_AT=0
#define UCC_EN_TX1_AT   UCC_TX1_AT=1
#define UCC_DIS_TX2_AB  UCC_TX2_AB=0
#define UCC_EN_TX2_AB   UCC_TX2_AB=1

#define TRIGGER_A_H Ultra_TRIGGER_A=1
#define TRIGGER_A_L Ultra_TRIGGER_A=0




#endif
