#include "lora_map.h"


s32 lora_map_GetHoldReg(u16 index,u32 *pOut)
{
	s32		ret=1;
	u16 data;
	data=*pOut;
	switch(index)
	{
		case 0:                      //运行标志
			*pOut=Control_Par.control_run;		
		break;
								
		case 1:                       //运行模式
			*pOut=Control_Par.control_mode;	
		break;
				
		case 2:                       //定开度
			*pOut=Control_Par.set_openingm*10;	
		break;
		
		case 3:                       //定水位
			*pOut=Control_Par.set_level*100;			
		break;
		//////////////////////////////////////////////////////////////		
		case 4:                        //定流量
			*pOut=Control_Par.set_flow*100;
		break;
				
		case 5:                       // SYSTEM VERSION
			*pOut=0;			
		break;
		
		case 6:                       // SYSTEM VERSION
			*pOut=0;			
		break;
				
		case 7:                       // SYSTEM VERSION
			*pOut=0;			
		break;
				
				
		default:
			*pOut=0;
		break;					
	}
	return ret;
}
/*
前水位
后水位
实时流速
实时流量
当日水量
本次水量
本年水量
累计水量
*/
s32 liu_lora_map_SetHoldReg(u32 index,float valuef)
{
	switch(index)
	{
		//////////////////////////////////////////////////////////////					
		case 0:                      // 前水位
			Log_Par.fro_lev=valuef;	
			RS485_Sensor.Aft_h=valuef;		//以巴歇尔槽前水位为后水位
			Statlog_Par.aft_level=1;
			RS485_Sensor.Aft_state=1;
			break;
		case 1:                       //后水位
			Log_Par.aft_lev=valuef;
			break;
		case 2:                     //实时流速
			Log_Par.V_current=valuef;
			break;
		case 3:                    //实时流量
			Log_Par.Q_current=valuef;
			Ultra_CALCU_Par.Q_Total=valuef;
			break;
		case 4:                    //当日水量
			Log_Par.day_water=valuef;
			break;
		case 5:                    //本次水量
			Log_Par.this_water=valuef;
			break;		
		//////////////////////////////////////////////////////////////////
		case 6:                    //本年水量
			Log_Par.year_water=valuef;
			break;
		case 7:                       //累计水量
			Log_Par.accu_water=valuef;
			break;
							
		default:
			break;
	}
}
s32 lora_map_SetHoldReg(u32 index,u16 *pOut)
{
	s32		ret=2;
//	u8		temp[16];
//	u8		pdata[4];
	u16		data;
//	s32     sdata;
//	float	ftdata;
//	u8		group;
	
	data=*pOut;

	switch(index)
	{
		//////////////////////////////////////////////////////////////					
		case 0:                      // 运行标志位
			Control_Par.control_run=data;	
			break;
		case 1:                       //控制模式
			Control_Par.control_mode=data;
			break;
		case 2:                     // 定开度
			Control_Par.set_openingm=(float)data/10;
			Flash_SaveDevData();		
			break;
		case 3:                    //定水位控制
			Control_Par.set_level=(float)data/100;
			Flash_SaveDevData();
			break;
		case 4:                    //定流量控制
			Control_Par.set_flow=(float)data/100;	
//			Control_Par.Remote_Flag=data;
			break;
		case 5:                        //开度设置
//			gate_mess_par[0].set_opening=(float)data/100;
			break;		
		//////////////////////////////////////////////////////////////////
		case 6:                      // RTC,分，秒
			break;
		case 7:                       // 星期,小时		
			break;
							
		default:
			break;

	}
	
	if(ret==1)
	{
	}
	
	return ret;
}

/*
*********************************************************************************************************
* 保持寄存器：03-06 指令
*********************************************************************************************************
*/
//s32 map_GetHoldReg(u32 index,u16 *pOut)
//{
//	s32		ret=2;
////	u8		temp[16];
////	u8		pdata[4];
//	u16		data;
////	s32     sdata;
////	float	ftdata;
////	u8		group;
//	
//	data=*pOut;

//	switch(index)
//	{	
//		//////////////////////////////////////////////////////////////					
//		case 0:                      // 闸前水位
//			flow_=(float)data/100;		
//			break;
//		case 1:                       //闸后水位
//			wat_gate_par.aft_lev=(float)data/100;	
//			break;
//		case 2:                     // 闸前净空
//			dev_SavePar.FroHeight_Air=(float)data/100;
//			Flash_SaveDevData();		
//			break;
//		case 3:                    //闸后净空
//			dev_SavePar.AftHeight_Air=(float)data/100;
//			Flash_SaveDevData();
//			break;
//		case 4:                        //运行控制
//			Control_Par.control_run=data;	
//			break;
//		case 5:                        //开度设置
//			break;		
//		//////////////////////////////////////////////////////////////////
//		case 6:                      // RTC,分，秒
//			break;
//		case 7:                       // 星期,小时		
//			break;
//							
//		default:
//			break;

//	}
//	
//	if(ret==1)
//	{
//	}
//	
//	return ret;
//}

