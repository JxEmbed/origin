#include "autoControl.h"
#include "TMC5160A.h"
#include "math.h"
autoControl_Struct autC_Par;
autC_PID_Struct autC_PID;
/*
自动水位控制
*/
void autC_reset(void)
{
	autC_PID.SEk=0;    //历史偏差
}
void autC_LevControl(void)
{
	float tempf;
	autC_PID.kd=1500;
	//如果水位
	//如果>0 则需上行
	//如果<0 则需下行
	autC_Par.aftLev=RS485_Sensor.Aft_h;
	autC_Par.aftLevSet=Control_Par.set_level;
	autC_PID.Ek=autC_Par.aftLevSet-autC_Par.aftLev; //计算误差
	autC_PID.speed=autC_PID.Ek*autC_PID.kd; //计算误差
	//速度限幅
	tempf=Mess_Par.run_speed;
	if(autC_PID.speed<(-tempf))
	{
		autC_PID.speed=(-tempf);
	}
	if(autC_PID.speed>(tempf))
	{
		autC_PID.speed=tempf;
	}
	if(fabs(autC_PID.speed)<10)
	{
		Control_Par.setState=GATE_STOP;
		TMC5160A_STOP();
	}
//	if(Control_Par.setState==GATE_UP&&(autC_PID.speed<0))
//	{
//		Control_Par.control_run=0;
//		motor_AutoControl();
//		nosave_par.SendControlflag=1;
//		Control_Par.setState=GATE_STOP;
//	}
//	else if(Control_Par.setState==GATE_DOWN&&(autC_PID.speed>0))
//	{
//		Control_Par.control_run=0;
//		motor_AutoControl();
//		nosave_par.SendControlflag=1;
//		Control_Par.setState=GATE_STOP;
//	}
	else if((autC_PID.speed>0)&&(MOTOR_LIMIT_UP!=1))
	{
		TMC5160A_RUN2(autC_PID.speed);
	}
	else if((autC_PID.speed<0)&&(MOTOR_LIMIT_DOWN!=1))
	{
		TMC5160A_RUN2(autC_PID.speed);
	}
}
/*
根据水位控制闸门高度
*/
void autC_LevControl2(void)
{
	float tempf;
	autC_PID.kd=1500;
	autC_PID.ki=10;
	//如果水位
	//如果>0 则需上行
	//如果<0 则需下行
	autC_Par.aftLev=RS485_Sensor.Aft_h;             //反馈值
	autC_Par.aftLevSet=Control_Par.set_level;       //设定值
	autC_PID.Ek=autC_Par.aftLevSet-autC_Par.aftLev; //计算误差
	autC_PID.SEk+=autC_PID.Ek;                      //历史偏差总和
	
	autC_PID.Pout=autC_PID.Ek*autC_PID.kd; //计算比例输出
	autC_PID.Iout=autC_PID.SEk*autC_PID.ki;//计算积分输出
	
	
	autC_PID.OUT=autC_PID.Pout+autC_PID.Iout;  //计算输出
	//速度限幅
	if(autC_PID.OUT<0)
	{
		autC_PID.OUT=0;
	}
	if(autC_PID.OUT>1)
	{
		autC_PID.OUT=1;
	}
	if(fabs(autC_PID.OUT-nosave_par.opening)<dev_SavePar.SetLevel_Dev)
	{
		Control_Par.setState=GATE_STOP;
		TMC5160A_STOP();
	}
//	if(Control_Par.setState==GATE_UP&&(autC_PID.speed<0))
//	{
//		Control_Par.control_run=0;
//		motor_AutoControl();
//		nosave_par.SendControlflag=1;
//		Control_Par.setState=GATE_STOP;
//	}
//	else if(Control_Par.setState==GATE_DOWN&&(autC_PID.speed>0))
//	{
//		Control_Par.control_run=0;
//		motor_AutoControl();
//		nosave_par.SendControlflag=1;
//		Control_Par.setState=GATE_STOP;
//	}
	else if((autC_PID.OUT-nosave_par.opening>0)&&(MOTOR_LIMIT_UP!=1))
	{
		TMC5160A_RUNUP();
	}
	else if((autC_PID.OUT-nosave_par.opening<0)&&(MOTOR_LIMIT_DOWN!=1))
	{
		TMC5160A_RUNDOWN();
	}
	
}
/*
自动流量控制
*/
void autC_FloControl(void)
{
	float tempf;
	autC_PID.kd=100;
	//如果水位
	//如果>0 则需上行
	//如果<0 则需下行
	autC_Par.watFlo=Ultra_CALCU_Par.Q_Total;
	autC_Par.watFloSet=Control_Par.set_flow;
	autC_PID.Ek=autC_Par.watFloSet-autC_Par.watFlo; //计算误差
	autC_PID.OUT=autC_PID.Ek*autC_PID.kd; //计算误差
	//速度限幅
	if(autC_PID.OUT<0)
	{
		autC_PID.OUT=0;
	}
	if(autC_PID.OUT>1)
	{
		autC_PID.OUT=1;
	}
	if(fabs(autC_PID.OUT-nosave_par.opening)<dev_SavePar.SetFlow_Dev)
	{
		Control_Par.setState=GATE_STOP;
		TMC5160A_STOP();
	}
//	if(Control_Par.setState==GATE_UP&&(autC_PID.speed<0))
//	{
////		Control_Par.control_run=0;
////		motor_AutoControl();
//		Control_Par.setState=GATE_STOP;
//		TMC5160A_STOP();
//		nosave_par.SendControlflag=1;
//	}
//	else if(Control_Par.setState==GATE_DOWN&&(autC_PID.speed>0))
//	{
////		Control_Par.control_run=0;
////		motor_AutoControl();
//		Control_Par.setState=GATE_STOP;
//		TMC5160A_STOP();
//		nosave_par.SendControlflag=1;
//	}
	else if((autC_PID.OUT-nosave_par.opening>0)&&(MOTOR_LIMIT_UP!=1))
	{
		TMC5160A_RUNUP();
	}
	else if((autC_PID.OUT-nosave_par.opening<0)&&(MOTOR_LIMIT_DOWN!=1))
	{
		TMC5160A_RUNDOWN();
	}
}
/*
根据流量控制闸门高度
*/
void autC_FloControl2(void)
{
	float tempf;
	autC_PID.kd=100;
	//如果水位
	//如果>0 则需上行
	//如果<0 则需下行
	autC_Par.watFlo=Ultra_CALCU_Par.Q_Total;
	autC_Par.watFloSet=Control_Par.set_flow;
	autC_PID.Ek=autC_Par.watFloSet-autC_Par.watFlo; //计算误差
	autC_PID.speed=autC_PID.Ek*autC_PID.kd; //计算误差
	//速度限幅
	if(autC_PID.OUT<0)
	{
		autC_PID.OUT=0;
	}
	if(autC_PID.OUT>1)
	{
		autC_PID.OUT=1;
	}
	if(fabs(autC_PID.OUT-nosave_par.opening)<0.01)
	{
		Control_Par.setState=GATE_STOP;
		TMC5160A_STOP();
	}
//	if(Control_Par.setState==GATE_UP&&(autC_PID.speed<0))
//	{
////		Control_Par.control_run=0;
////		motor_AutoControl();
//		Control_Par.setState=GATE_STOP;
//		TMC5160A_STOP();
//		nosave_par.SendControlflag=1;
//	}
//	else if(Control_Par.setState==GATE_DOWN&&(autC_PID.speed>0))
//	{
////		Control_Par.control_run=0;
////		motor_AutoControl();
//		Control_Par.setState=GATE_STOP;
//		TMC5160A_STOP();
//		nosave_par.SendControlflag=1;
//	}
	else if((autC_PID.OUT>0)&&(MOTOR_LIMIT_UP!=1))
	{
		TMC5160A_RUNUP();
	}
	else if((autC_PID.OUT<0)&&(MOTOR_LIMIT_DOWN!=1))
	{
		TMC5160A_RUNDOWN();
	}
}