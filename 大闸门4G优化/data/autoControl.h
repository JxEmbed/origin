#ifndef __AUTOCONTROL_H
#define __AUTOCONTROL_H

#include "main.h"

typedef struct
{
	float aftLev;
	float aftLevSet;
	float watFlo;
	float watFloSet;
	float gateHigh;  //
	
	
	
}autoControl_Struct;
typedef struct
{
	float Kp;
	float T; //PWM计算周期--采样周期
	float Ti;
	float Td;
	
	float Ek;    //本次误差
	float Ek_1;  //上次误差
	float SEk;   //历史误差之和
	
	float Iout;  //积分输出
	float Pout;  //比例输出
	float Dout;  //微分输出
	
	
	float OUT0;  //0输出
	float OUT;   //总输出 
	
	uint16_t C10ms;
	
	uint16_t pwmcycle;  //PWM周期
	
	uint8_t ON;     //开关  =1 打开温度控制 =0 关闭温度控制
	
	float ti;
	float	ki;
	float td;
	float kd;
	///
	float Stage1_Flow;
	float speed;
}autC_PID_Struct;
extern autoControl_Struct autC_Par;
extern autC_PID_Struct autC_PID;
void autC_LevControl(void);
void autC_FloControl(void);
void autC_LevControl2(void);
void autC_reset(void);
#endif

