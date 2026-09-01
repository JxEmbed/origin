#include "cmd_process.h"
#include "hmi_driver.h"
#include "cmd_queue.h"

//#include "sys.h"
#include "rtc.h"
#include "stdio.h"
#include "data.h"
#include "stdlib.h"
#include "main.h"

#include "math.h"
#include "string.h"
#include "ff_user.h"
#include "TMC5160A.h"
#include "LCD.h"
#include "TMC5160A.h"
#include "hmi_chart.h"
#include "test.h"
#include "StallGuard.h"
#include "AnglePulse.h"
#include "lora.h"
volatile uint8  cmd_buffer[CMD_MAX_SIZE];                                                     //指令缓存
volatile uint16 Last_screen_id=0;                                                         //上一个画面ID
volatile uint16 current_screen_id = 0;                                                 //当前画面ID

u8 show_title[SHOW_TITLE_LEN];
u8 show_content[SHOW_CONTENT_LEN];
//static int32 progress_value = 0;                                                     //进度条测试值
//static int32 test_value = 0;                                                         //测试值
// uint8 update_en = 0;                                                          //更新标记
//static int32 meter_flag = 0;                                                         //仪表指针往返标志位
//static int32 num = 0;                                                                //曲线采样点计数
//static int sec = 1;                                                                  //时间秒
//static int32 curves_type = 0;                                                        //曲线标志位  0为正弦波，1为锯齿波                  
//static int32 second_flag=0;                                                          //时间标志位
//static int32 icon_flag = 0;                                                          //图标标志位
//static uint8 Select_H ;                                                              //滑动选择小时
//static uint8 Select_M ;                                                              //滑动选择分钟 
//static uint8 Last_H ;                                                                //上一个选择小时
//static uint8 Last_M;                                                                 //上一个选择分钟 
//static int32 Progress_Value = 0;                                                     //进度条的值 
void CMD_Process(void)
{
	qsize  size = 0; 
//	xSemaphoreTake(LCDRX_Mutex,LCDRX_xBlockTime);  //申请互斥信号量//帧头

	size = queue_find_cmd(cmd_buffer,CMD_MAX_SIZE);                              //从缓冲区中获取一条指令      
//	xSemaphoreGive(LCDRX_Mutex);       //帧尾	

	if(size>0&&cmd_buffer[1]!=0x07)                                              //接收到指令 ，及判断是否为开机提示
	{
			ProcessMessage((PCTRL_MSG)cmd_buffer, size);                             //指令处理	
			//vTaskDelay(100);
	}
	else if(size>0&&cmd_buffer[1]==0x07)                                         //如果为指令0x07就软重置STM32  
	{

	                                                                                                                                                        
	}
	
}

/*! 
*  \brief  更新数据
*/ 

void UpdateUI()
{
	char tempu8[100];
	//登录界面
	if(current_screen_id==0)
	{
		SetTextValue(0,5,(u8 *)dev_SavePar.dev_name);
		SetTextValue(0,3,(u8 *)nosave_par.pass);
		if(nosave_par.identity==IDENTITY_ADMIN)
		{
			snprintf(tempu8,sizeof(tempu8),"%s",Author_Par.admin_phone);
			SetTextValue(0,2,(u8 *)tempu8);
		}
		else if(nosave_par.identity==IDENTITY_OPERA1)
		{
			snprintf(tempu8,sizeof(tempu8),"%s",Author_Par.opera1_phone);
			SetTextValue(0,2,(u8 *)tempu8);
		}
		else if(nosave_par.identity==IDENTITY_OPERA2)
		{
			snprintf(tempu8,sizeof(tempu8),"%s",Author_Par.opera2_phone);
			SetTextValue(0,2,(u8 *)tempu8);
		}
		else if(nosave_par.identity==IDENTITY_MAINT)
		{
			snprintf(tempu8,sizeof(tempu8),"%s",Author_Par.maint_phone);
			SetTextValue(0,2,(u8 *)tempu8);
		}
		else if(nosave_par.identity==IDENTITY_TEMPO)
		{
			snprintf(tempu8,sizeof(tempu8),"%s",Author_Par.tempo_phone);
			SetTextValue(0,2,(u8 *)tempu8);
		}
		else
		{
			SetTextValue(0,2,(u8 *)"请选择登录身份");
		
		}
		
	}
	//系统工况
	if(current_screen_id==1)
	{
		SetProgressValue(1,21,RS485_Sensor.Fro_h*100/2.3f); //显示闸前水位
		SetProgressValue(1,22,RS485_Sensor.Aft_h*100/2.3f); //显示闸后水位
		SetSliderValue(1,23,nosave_par.opening*100);     //设置闸板显示高度
		SetTextFloat(1,7,RS485_Sensor.Fro_h,3,1); //闸前水位
		SetTextFloat(1,8,RS485_Sensor.Aft_h,3,1); //闸后水位
		SetTextFloat(1,36,RS485_Sensor.Wat_head,3,1); //预警水位
		SetTextFloat(1,9,nosave_par.openingm,2,1);   //开度
		SetTextFloat(1,11,Log_Par.this_water,4,1); //本次水量
		SetTextFloat(1,12,Log_Par.year_water,4,1); //本年水量
		SetTextFloat(1,13,Log_Par.accu_water,4,1); //累计水量
		SetTextFloat(1,10,Ultra_CALCU_Par.Q_Total,3,1); //实时流量
		
		SetTextFloat(1,14,Control_Par.set_openingm,2,1);  //显示设定闸门开度
		
		
		if(Control_Par.control_run==0)   //如果没开启控制
		{
			SetButtonValue(1,19,0);
			SetButtonValue(1,20,1);
		}
		else
		{
			SetButtonValue(1,19,1);
			SetButtonValue(1,20,0);
		}
		
		SetButtonValue(current_screen_id,17,Control_Par.control_mode==0);
		SetButtonValue(current_screen_id,27,Control_Par.control_mode==1);
		SetButtonValue(current_screen_id,28,Control_Par.control_mode==2);
		
		SetButtonValue(1,15,Control_Par.setState==GATE_UP);
		SetButtonValue(1,16,Control_Par.setState==GATE_DOWN);
		SetButtonValue(1,18,Control_Par.setState==GATE_STOP);
		
		SetTextFloat(current_screen_id,29,Control_Par.set_level,2,1);
		SetTextFloat(current_screen_id,31,Control_Par.set_flow,2,1);
		if(StallGuard_Par.StallGuard_flag==1)
		{
			AnimationPlayFrame(current_screen_id,37,1);
		}
		else
		{
			AnimationPlayFrame(current_screen_id,37,0);
		}
		if(nosave_par.current_identity>=IDENTITY_MAINT)
		{
			SetControlVisiable(1,15,1);
			SetControlVisiable(1,16,1);
			SetControlVisiable(1,18,1);
		}
		else
		{
			SetControlVisiable(1,15,0);
			SetControlVisiable(1,16,0);
			SetControlVisiable(1,18,0);
		}
		if(dev_SavePar.Opening_Fun)
		{
			SetControlEnable(current_screen_id,17,1);
			SetControlEnable(current_screen_id,14,1);
			SetControlEnable(current_screen_id,24,1);
			SetControlEnable(current_screen_id,26,1);
		}
		else
		{
			SetControlEnable(current_screen_id,17,0);
			SetControlEnable(current_screen_id,14,0);
			SetControlEnable(current_screen_id,24,0);
			SetControlEnable(current_screen_id,26,0);
		}
		if(dev_SavePar.Level_Fun)
		{
			SetControlEnable(current_screen_id,27,1);
			SetControlEnable(current_screen_id,29,1);
			SetControlEnable(current_screen_id,32,1);
			SetControlEnable(current_screen_id,33,1);
		}
		else
		{
			SetControlEnable(current_screen_id,27,0);
			SetControlEnable(current_screen_id,29,0);
			SetControlEnable(current_screen_id,32,0);
			SetControlEnable(current_screen_id,33,0);
		}
		if(dev_SavePar.Flow_Fun)
		{
			SetControlEnable(current_screen_id,28,1);
			SetControlEnable(current_screen_id,31,1);
			SetControlEnable(current_screen_id,34,1);
			SetControlEnable(current_screen_id,35,1);
		}
		else
		{
			SetControlEnable(current_screen_id,28,0);
			SetControlEnable(current_screen_id,31,0);
			SetControlEnable(current_screen_id,34,0);
			SetControlEnable(current_screen_id,35,0);
		}
		SetControlVisiable(current_screen_id,38,!dev_SavePar.Opening_Fun);
		SetControlVisiable(current_screen_id,39,!dev_SavePar.Level_Fun);
		SetControlVisiable(current_screen_id,40,!dev_SavePar.Flow_Fun);
		SetControlEnable(current_screen_id,38,0);
		SetControlEnable(current_screen_id,39,0);
		SetControlEnable(current_screen_id,40,0);
	}
	if(current_screen_id==2)
	{
		if(dev_SavePar.Flow_Fun)
		{
			LCD_showdLog(); //显示运行数据  CSV_TESTLOG
		}
		else
		{
			LCD_showNoFlowLog(); //显示没有流量的运行数据  CSV_RUNLOG
		}
		
		SetTextInt32(2,11,Loglen_Par.flow_pageNum,0,0);		//总记录
	
		SetTextInt32(2,8,nosave_par.flow_page,0,0);		//当前页数
		SetControlEnable(2,16,0);
		vTaskDelay(500);
	}
	if(current_screen_id==3) //设备状态
	{
		LCD_showstatusLog(); //显示设备状态
		vTaskDelay(500);
	}
	if(current_screen_id==4)
	{
		SetTextInt32(4,11,Loglen_Par.opera_pageNum,0,0);
		
		SetTextInt32(4,8,nosave_par.opera_page,0,0);
	}
	if(current_screen_id==5) //系统设置-设备信息
	{
		SetTextValue(5,9,(uint8_t *)dev_SavePar.dev_name);         //产品名称
		SetTextValue(5,10,(uint8_t *)myota_info.nowVersion); 	   //软件版本myota_info.nowVersion
		SetTextValue(5,11,(uint8_t *)dev_SavePar.motor_power); 	   //电机功率
		
		snprintf(tempu8,sizeof(tempu8),"%d-%02d-%02d",dev_SavePar.factory_time.years,dev_SavePar.factory_time.months,dev_SavePar.factory_time.days);
		SetTextValue(5,12,(uint8_t *)tempu8); //出厂时间
//		SetTextValue(5,12,(uint8_t *)dev_SavePar.factory_time); //出厂时间
		if((Statlog_Par._sim == 1) || (Statlog_Par._sim == 3))
		{
			SetTextValue(5,13,(uint8_t *)dev_SavePar.sim_number);   //SIM卡信息
		}
		else		//0   2    4
		{
			SetTextValue(5,13,(uint8_t *)"故障");   //SIM卡信息
		}
		SetTextValue(5,33,(u8 *)L76K_Par.Union_Lon);    //经度  L76K_Par.F_Lat
		
		
		SetTextValue(5,14,(uint8_t *)dev_SavePar.dev_code);     //设备编号
		SetTextValue(5,15,(uint8_t *)dev_SavePar.Upload_URL);  	//上传地址 
		SetTextValue(5,36,(u8 *)L76K_Par.Union_Lat);     		//纬度
	/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/	
		SetTextFloat(5,16,dev_SavePar.water_inletWidth,2,1); //进水宽度
		SetTextFloat(5,17,dev_SavePar.Ultra_initialDist,2,1); //探头初距
		SetTextFloat(5,32,dev_SavePar.FroHeight_Air,3,1);     //闸前净空    
		
		SetTextFloat(5,18,dev_SavePar.water_inletHeight,2,1); //进水高度
		SetTextFloat(5,19,dev_SavePar.Ultra_Spacing,2,1);   //探头间距
		SetTextFloat(5,35,dev_SavePar.AftHeight_Air,3,1);     //闸后净空
		
		SetTextFloat(5,20,dev_SavePar.base_heightDiff,3,1);   //底板高差
		SetTextFloat(5,21,dev_SavePar.Ultra_Num,0,1);         //探头数量
		SetTextFloat(5,44,dev_SavePar.Gradient,4,1);          //渠底比降
		
		if(dev_SavePar.TypeUltra==0)
		{
			SetTextValue(5,26,(u8 *)"测流槽");
		}
		else if(dev_SavePar.TypeUltra==1)
		{
			SetTextValue(5,26,(u8 *)"巴歇尔槽");
		}
	}
	if(current_screen_id==6)
	{
		SetTextValue(6,9,(uint8_t *)Author_Par.admin_phone);  //管理员手机号
		SetTextValue(6,10,(uint8_t *)Author_Par.opera1_phone); //操作员1手机号
		SetTextValue(6,11,(uint8_t *)Author_Par.opera2_phone); //操作员2手机号
		SetTextValue(6,24,(uint8_t *)Author_Par.maint_phone); //维护员手机号
		SetTextValue(6,25,(uint8_t *)Author_Par.tempo_phone); //临时手机号
		if(nosave_par.current_identity==IDENTITY_MAINT)  //维护员
		{
			SetTextValue(6,12,(uint8_t *)Author_Par.admin_pass);   //管理员密码
			SetTextValue(6,13,(uint8_t *)Author_Par.opera1_pass); //操作员1密码
			SetTextValue(6,14,(uint8_t *)Author_Par.opera2_pass); //操作员2密码
			SetTextValue(6,23,(uint8_t *)Author_Par.maint_pass); //维护员密码
			SetTextValue(6,26,(uint8_t *)Author_Par.tempo_pass); //临时身份密码
		}
		else if(nosave_par.current_identity==IDENTITY_OPERA1) //操作员1
		{
			SetTextValue(6,12,(uint8_t *)"******");   //管理员密码
			SetTextValue(6,13,(uint8_t *)Author_Par.opera1_pass); //操作员1密码
			SetTextValue(6,14,(uint8_t *)"******"); //操作员2密码
			SetTextValue(6,23,(uint8_t *)"******"); //维护员密码
			SetTextValue(6,26,(uint8_t *)"******"); //临时身份密码
		}
		else if(nosave_par.current_identity==IDENTITY_OPERA2) //操作员2
		{
			SetTextValue(6,12,(uint8_t *)"******");   //管理员密码
			SetTextValue(6,13,(uint8_t *)"******"); //操作员1密码
			SetTextValue(6,14,(uint8_t *)Author_Par.opera2_pass); //操作员2密码
			SetTextValue(6,23,(uint8_t *)"******"); //维护员密码
			SetTextValue(6,26,(uint8_t *)"******"); //临时身份密码
		}
		else if(nosave_par.current_identity==IDENTITY_ADMIN) //管理员
		{
			SetTextValue(6,12,(uint8_t *)Author_Par.admin_pass);   //管理员密码
			SetTextValue(6,13,(uint8_t *)Author_Par.opera1_pass); //操作员1密码
			SetTextValue(6,14,(uint8_t *)Author_Par.opera2_pass); //操作员2密码
			SetTextValue(6,23,(uint8_t *)"******"); //维护员密码
			SetTextValue(6,26,(uint8_t *)"******"); //临时身份密码
		}
		else
		{
			SetTextValue(6,12,(uint8_t *)"******");   //管理员密码
			SetTextValue(6,13,(uint8_t *)"******"); //操作员1密码
			SetTextValue(6,14,(uint8_t *)"******"); //操作员2密码
			SetTextValue(6,23,(uint8_t *)"******"); //维护员密码
			SetTextValue(6,26,(uint8_t *)"******"); //临时身份密码
		}
	}
	if(current_screen_id==10)
	{
		snprintf(tempu8,sizeof(tempu8),"%s",Author_Par.admin_phone);
		SetTextValue(10,2,(u8 *)tempu8);
		snprintf(tempu8,sizeof(tempu8),"%s",Author_Par.opera1_phone);
		SetTextValue(10,3,(u8 *)tempu8);
		snprintf(tempu8,sizeof(tempu8),"%s",Author_Par.opera2_phone);
		SetTextValue(10,4,(u8 *)tempu8);
		snprintf(tempu8,sizeof(tempu8),"%s",Author_Par.maint_phone);
		SetTextValue(10,10,(u8 *)tempu8);
		snprintf(tempu8,sizeof(tempu8),"%s",Author_Par.tempo_phone);
		SetTextValue(10,11,(u8 *)tempu8);
	}
	if(current_screen_id==11)
	{
		SetTextValue(11,12,(u8 *)nosave_par.confirmpass);
		//清空运行数据
		if(nosave_par.cleanlog_ptr==1)
		{
			SetTextValue(11,4,(u8 *)"确认清空运行数据？");
			SetTextValue(11,8,(u8 *)"请输入密码并确认");
		}
		//清空操作数据
		if(nosave_par.cleanlog_ptr==2)
		{
			SetTextValue(11,4,(u8 *)"确认清空操作数据？");
			SetTextValue(11,8,(u8 *)"请输入密码并确认");
		}
	}
	if(current_screen_id==12) //调试界面
	{
		SetTextFloat(12,7,nosave_par.opening,2,1);   //开度
		SetTextInt32(12,8,Mess_Par.run_step,1,0);     //当前步数
		SetTextInt32(12,9,Mess_Par.sum_step,1,0);     //总步数
		SetTextInt32(12,10,Mess_Par.run_speed,1,0);     //电机速度 AMAX
		
		//条件表达式，返回 true 或 false  nosave_par.test_step_flag==1
		SetButtonValue(12,13,nosave_par.test_step_flag==1);
		SetButtonValue(12,15,nosave_par.test_step_flag==0);
		SetTextInt32(12,19,TMC5160_Registers.TSTEP,1,0);		//测试步数
		//堵转灵敏度设置  -64-64
		SetTextInt32(12,20,TMC5160_Registers.DRV_STATUS.SG_RESULT,1,0);		
		//配置的运行电流比例
		SetTextInt32(12,16,dev_SavePar.runCurrent,1,0);
		//TCOOLTHRS≥TSTEP   发生堵转
		SetTextInt32(12,17,dev_SavePar.TCOOLTHRS,1,0);
		//堵转发生时 电机输入电平灵敏度设置  值越大，灵敏度越低--->电机堵转的电平值越高--->让电机堵转的力矩越大
		SetTextInt32(12,18,dev_SavePar.SGT,1,0);
		SetButtonValue(12,21,Control_Par.setState==GATE_UP);
		SetButtonValue(12,22,Control_Par.setState==GATE_DOWN);
		SetButtonValue(12,23,Control_Par.setState==GATE_STOP);
		SetTextInt32(12,24,Log_Par.this_water,1,0);
		SetTextInt32(12,28,Log_Par.year_water,1,0);
		SetTextInt32(12,29,Log_Par.accu_water,1,0);
	}
	if(current_screen_id==13)
	{
		SetTextFloat(current_screen_id,7,nosave_par.opening,2,1);   //开度
		SetTextInt32(current_screen_id,8,Mess_Par.run_step,1,0);     //当前步数
		SetTextInt32(current_screen_id,9,Mess_Par.sum_step,1,0);     //总步数
		SetTextInt32(current_screen_id,10,Mess_Par.run_speed,1,0);     //总步数
		SetTextInt32(current_screen_id,14,nosave_par.test_step_count,1,0);//测试步数
		SetTextInt32(current_screen_id,15,AngleP_Speed,1,0);           //实时速度
		SetTextInt32(current_screen_id,19,TMC5160_Registers.TSTEP,1,0);//测试步数
		SetTextInt32(current_screen_id,20,TMC5160_Registers.DRV_STATUS.SG_RESULT,1,0);//测试步数
		SetTextInt32(current_screen_id,16,dev_SavePar.runCurrent,1,0);
		SetTextInt32(current_screen_id,17,dev_SavePar.TCOOLTHRS,1,0);
		SetTextInt32(current_screen_id,18,dev_SavePar.SGT,1,0);
		SetTextInt32(current_screen_id,14,dev_SavePar.AMAX,1,0);
		SetButtonValue(current_screen_id,21,Control_Par.setState==GATE_UP);
		SetButtonValue(current_screen_id,22,Control_Par.setState==GATE_DOWN);
		SetButtonValue(current_screen_id,23,Control_Par.setState==GATE_STOP);
		SetButtonValue(current_screen_id,26,dev_SavePar.StallGuard_Switch==1);
		SetTextFloat(current_screen_id,27,dev_SavePar.StallGuard_SpeedThreshold,1,1);
		SetTextInt32(current_screen_id,28,dev_SavePar.StallGuard_Secs,0,0);
		hmi_StallGuard2_update();
	}
	if(current_screen_id==14)
	{
		SetTextFloat(current_screen_id,7,nosave_par.opening,2,1);   //开度
		SetTextInt32(current_screen_id,8,Mess_Par.run_step,1,0);     //当前步数
		SetTextInt32(current_screen_id,9,Mess_Par.sum_step,1,0);     //总步数
		SetTextInt32(current_screen_id,10,Mess_Par.run_speed,1,0);     //总步数
		SetTextInt32(current_screen_id,14,nosave_par.test_step_count,1,0);//测试步数
		SetButtonValue(current_screen_id,13,nosave_par.test_step_flag==1);
		SetButtonValue(current_screen_id,15,nosave_par.test_step_flag==0);
		SetTextInt32(current_screen_id,19,TMC5160_Registers.TSTEP,1,0);//测试步数
		SetTextInt32(current_screen_id,20,TMC5160_Registers.DRV_STATUS.SG_RESULT,1,0);//测试步数
		SetTextInt32(current_screen_id,16,dev_SavePar.runCurrent,1,0);
		SetTextInt32(current_screen_id,17,dev_SavePar.TCOOLTHRS,1,0);
		SetTextInt32(current_screen_id,18,dev_SavePar.SGT,1,0);
		SetTextInt32(current_screen_id,14,dev_SavePar.AMAX,1,0);
		SetButtonValue(current_screen_id,21,Control_Par.setState==GATE_UP);
		SetButtonValue(current_screen_id,22,Control_Par.setState==GATE_DOWN);
		SetButtonValue(current_screen_id,23,Control_Par.setState==GATE_STOP);
		hmi_StallGuard2_update();
	}
	if(current_screen_id==17) //系统设置-运行参数
	{
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/		
		SetTextFloat(current_screen_id,22,dev_SavePar.flow_min,2,1);          //最小流量
		SetTextFloat(current_screen_id,23,dev_SavePar.level_min,2,1);         //最小水位
		SetTextFloat(current_screen_id,24,dev_SavePar.data_interval,2,1);     //数据间隔
		SetTextFloat(current_screen_id,37,dev_SavePar.Side_K,2,1);            //边壁系数
		
		SetTextFloat(current_screen_id,25,dev_SavePar.flow_max,2,1);       //最大流量
		SetTextFloat(current_screen_id,26,dev_SavePar.level_max,2,1);      //最大水位
		SetTextFloat(current_screen_id,27,dev_SavePar.send_interval,2,1);  //状态间隔
		SetTextFloat(current_screen_id,38,dev_SavePar.Correct_K,2,1);        //修正系数
		
		SetTextFloat(current_screen_id,28,dev_SavePar.flow_deviation,2,1); //流量偏差
		SetTextFloat(current_screen_id,29,dev_SavePar.level_deviation,2,1); //水位偏差
		SetTextFloat(current_screen_id,39,dev_SavePar.Channel_Angle,2,1);      //声道角度
		SetTextFloat(current_screen_id,40,dev_SavePar.Roughness,3,1);         //明渠糙率
		SetTextFloat(current_screen_id,48,dev_SavePar.Safe_SpaceHigh,3,1);         //安全超高
		
		SetButtonValue(current_screen_id,17,dev_SavePar.Opening_Fun);
		SetButtonValue(current_screen_id,10,dev_SavePar.Level_Fun);
		SetButtonValue(current_screen_id,11,dev_SavePar.Flow_Fun);
		SetButtonValue(current_screen_id,16,dev_SavePar._4G_Fun);
		SetButtonValue(current_screen_id,18,dev_SavePar.Lora_Fun);

		SetTextFloat(current_screen_id,14,dev_SavePar.collect_interval,2,1);

		
		
		SetTextFloat(current_screen_id,12,dev_SavePar.SetLevel_Dev,3,1);
		SetTextFloat(current_screen_id,13,dev_SavePar.SetFlow_Dev,3,1);
	}
	if(current_screen_id==18) //提示界面
	{
		SetTextValue(18,4,(u8*)show_title);
		SetTextValue(18,3,(u8*)show_content);
	}
	if(current_screen_id==19) //外部设备
	{
		
	}
}
/*! 
*  \brief  消息处理流程
*  \param msg 待处理消息
*  \param size 消息长度
*/
void ProcessMessage( PCTRL_MSG msg, uint16 size )
{
    uint8 cmd_type = msg->cmd_type;                                                  //指令类型
    uint8 ctrl_msg = msg->ctrl_msg;                                                  //消息的类型
    uint8 control_type = msg->control_type;                                          //控件类型
    uint16 screen_id = PTR2U16(&msg->screen_id);                                     //画面ID
    uint16 control_id = PTR2U16(&msg->control_id);                                   //控件ID
    uint32 value = PTR2U32(msg->param);                                              //数值


    switch(cmd_type)
    {  
    case NOTIFY_TOUCH_PRESS:                                                        //触摸屏按下
    case NOTIFY_TOUCH_RELEASE:                                                      //触摸屏松开
        NotifyTouchXY(cmd_buffer[1],PTR2U16(cmd_buffer+2),PTR2U16(cmd_buffer+4)); 
        break;                                                                    
    case NOTIFY_WRITE_FLASH_OK:                                                     //写FLASH成功
        NotifyWriteFlash(1);                                                      
        break;                                                                    
    case NOTIFY_WRITE_FLASH_FAILD:                                                  //写FLASH失败
        NotifyWriteFlash(0);                                                      
        break;                                                                    
    case NOTIFY_READ_FLASH_OK:                                                      //读取FLASH成功
        NotifyReadFlash(1,cmd_buffer+2,size-6);                                     //去除帧头帧尾
        break;                                                                    
    case NOTIFY_READ_FLASH_FAILD:                                                   //读取FLASH失败
        NotifyReadFlash(0,0,0);                                                   
        break;                                                                    
    case NOTIFY_READ_RTC:                                                           //读取RTC时间
        NotifyReadRTC(cmd_buffer[2],cmd_buffer[3],cmd_buffer[4],cmd_buffer[5],cmd_buffer[6],cmd_buffer[7],cmd_buffer[8]);
        break;
    case NOTIFY_CONTROL:
        {
            if(ctrl_msg==MSG_GET_CURRENT_SCREEN)                                    //画面ID变化通知
            {
                NotifyScreen(screen_id);                                            //画面切换调动的函数
            }
            else
            {
                switch(control_type)
                {
                case kCtrlButton:                                                   //按钮控件
                    NotifyButton(screen_id,control_id,msg->param[1]);                  
                    break;                                                             
                case kCtrlText:                                                     //文本控件
                    NotifyText(screen_id,control_id,msg->param);                       
                    break;                                                             
                case kCtrlProgress:                                                 //进度条控件
                    NotifyProgress(screen_id,control_id,value);                        
                    break;                                                             
                case kCtrlSlider:                                                   //滑动条控件
                    NotifySlider(screen_id,control_id,value);                          
                    break;                                                             
                case kCtrlMeter:                                                    //仪表控件
                    NotifyMeter(screen_id,control_id,value);                           
                    break;                                                             
                case kCtrlMenu:                                                     //菜单控件
                    NotifyMenu(screen_id,control_id,msg->param[0],msg->param[1]);      
                    break;                                                              
                case kCtrlSelector:                                                 //选择控件
                    NotifySelector(screen_id,control_id,msg->param[0]);                
                    break;                                                              
                case kCtrlRTC:                                                      //倒计时控件
                    NotifyTimer(screen_id,control_id);
                    break;
                default:
                    break;
                }
            } 
            break;  
        } 
    case NOTIFY_HandShake:                                                          //握手通知                                                     
        NOTIFYHandShake();
        break;
    default:
        break;
    }
}
//LCD恢复显示
void LCD_ResumeData()
{
//	SetTextFloat(0,7,(float)fertilizer.Area,1,1); 
//	SetTextFloat(0,8,(float)fertilizer.Fer,1,1); 
//	SetTextFloat(0,9,(float)fertilizer.Ratio,1,1); 
//	SetTextFloat(0,11,(float)fertilizer.Press,1,1); 
}


/*! 
*  \brief  握手通知
*/
void NOTIFYHandShake()
{
   SetButtonValue(3,2,1);
}

/*! 
*  \brief  画面切换通知
*  \details  当前画面改变时(或调用GetScreen)，执行此函数
*  \param screen_id 当前画面ID
*/
void NotifyScreen(uint16 screen_id)
{
	char ctemp[100];
    //TODO: 添加用户代码
	nosave_par.LCD_timecount=LCD_SHOWTIME;
	if((current_screen_id!=10&&current_screen_id!=0))  //如果切换后的界面是0
	{
		Last_screen_id=current_screen_id;
	}
    current_screen_id = screen_id;                                                   //在工程配置中开启画面切换通知，记录当前画面ID
	if(current_screen_id==0)  //如果切换后的界面是登录界面
	{
		nosave_par.current_identity=IDENTITY_NULL;
		memset(nosave_par.pass,0,sizeof(nosave_par.pass));
		SetTextValue(0,3,(u8*)"");
	}
	if(current_screen_id==1)
	{
//			Record_Add(1,2,"");
//	Record_Add(1,2,"t1;2;3;4;5;6;7;8;9;10");
//			ShowHistory();
	}
	if(current_screen_id==2)
	{
		if(dev_SavePar.Flow_Fun)
		{
			LCD_showdLog(); //显示运行数据  CSV_TESTLOG
		}
		else
		{
			LCD_showNoFlowLog(); //显示没有流量的运行数据  CSV_RUNLOG
		}
		//LCD_showdLog(); //显示运行数据
	}
	if(current_screen_id==3)
	{
		
	}
	if(current_screen_id==4) 
	{
		LCD_showoperaLog(); //显示操作记录
	}
	if(current_screen_id==5) 
	{
		if(dev_SavePar.TypeLevel==0)
		{
			SetTextValue(5,23,(u8 *)"超声波");
		}
		else if(dev_SavePar.TypeLevel==1)
		{
			SetTextValue(5,23,(u8 *)"雷达");
		}
	}
	if(current_screen_id==7)
	{
		snprintf(ctemp,sizeof(ctemp),"http://d.591ll.com/iot/iotInfo.aspx?num=%s&openid=",dev_SavePar.sim_number);
		QRCode_Set(7,3,ctemp);
	}
	if(current_screen_id==9)
	{
		LCD_showSpeedLog();
	}
	if(current_screen_id==19)	//外部设备
	{
		if(dev_SavePar.TypeUltra==0)
		{
			SetTextValue(19,12,(u8*)nosave_par.Externdev_code);
			SetTextValue(19,8,(u8*)"请输入外部设备编码");
		}
		else if(dev_SavePar.TypeUltra==1)
		{
			SetTextValue(19,12,(u8*)dev_SavePar.Externdev_code);
			SetTextValue(19,8,(u8*)"已绑定外部设备");
		}
	}
}

/*! 
*  \brief  触摸坐标事件响应
*  \param press 1按下触摸屏，3松开触摸屏
*  \param x x坐标
*  \param y y坐标
*/
void NotifyTouchXY(uint8 press,uint16 x,uint16 y)
{ 
    //TODO: 添加用户代码
}





/*! 
*  \brief  按钮控件通知
*  \details  当按钮状态改变(或调用GetControlValue)时，执行此函数
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param state 按钮状态：0弹起，1按下
*/
void NotifyButton(uint16 screen_id, uint16 control_id, uint8  state)
{ 
	u32 tempu32;
	nosave_par.LCD_timecount=LCD_SHOWTIME;
	if(screen_id==0)
	{
		if(control_id==4&&state==0) ///登录按键
		{
			if(nosave_par.identity==IDENTITY_ADMIN) //管理员
			{
				if(strlen(Author_Par.admin_phone)==0)
				{
					SetTextValue(0,8,(u8*)"未设置手机号");
					nosave_par.current_identity=IDENTITY_NULL;
				}
				else if(strcmp((char *)nosave_par.pass,(char *)Author_Par.admin_pass)==0)
				{
					SetScreen(Last_screen_id);
					SetTextValue(0,8,(u8*)"");
					SaveOPERA(OPERA_LOGIN,NULL,NULL);
					SetControlEnable(5,7,0);
					SetControlEnable(6,7,0);
					nosave_par.current_identity=nosave_par.identity;
				}
				else
				{
					SetTextValue(0,8,(u8*)"密码错误");
					nosave_par.current_identity=IDENTITY_NULL;
				}
			}
			else if(nosave_par.identity==IDENTITY_OPERA1) //操作员1
			{
				
				if(strlen(Author_Par.opera1_phone)==0)
				{
					SetTextValue(0,8,"未设置手机号");
				}
				else if(strcmp((char *)nosave_par.pass,(char *)Author_Par.opera1_pass)==0)
				{
					SetScreen(Last_screen_id);
					SetTextValue(0,8,"");
					SaveOPERA(OPERA_LOGIN,NULL,NULL);
					SetControlEnable(5,7,0);
					SetControlEnable(6,7,0);
					nosave_par.current_identity=nosave_par.identity;
				}
				else
				{
					SetTextValue(0,8,"密码错误");
				}
			}
			else if(nosave_par.identity==IDENTITY_OPERA2) //操作员2
			{
				if(strlen(Author_Par.opera2_phone)==0)
				{
					SetTextValue(0,8,"未设置手机号");
				}
				else if(strcmp((char *)nosave_par.pass,(char *)Author_Par.opera2_pass)==0)
				{
					SetScreen(Last_screen_id);
					SetTextValue(0,8,"");
					SaveOPERA(OPERA_LOGIN,NULL,NULL);
					SetControlEnable(5,7,0);
					SetControlEnable(6,7,0);
					nosave_par.current_identity=nosave_par.identity;
				}
				else
				{
					SetTextValue(0,8,"密码错误");
				}
			}
			else if(nosave_par.identity==IDENTITY_MAINT) //维护员
			{
//				if(strlen(Author_Par.maint_phone)==0)
//				{
//					SetTextValue(0,8,"未设置手机号");
//				}
//				else 
				if(strcmp((char *)nosave_par.pass,(char *)Author_Par.maint_pass)==0)
				{
					SetScreen(Last_screen_id);
					SetTextValue(0,8,"");
					SaveOPERA(OPERA_LOGIN,NULL,NULL);
					SetControlEnable(5,7,1);
					SetControlEnable(6,7,1);
					nosave_par.current_identity=nosave_par.identity;
				}
				else
				{
					SetTextValue(0,8,"密码错误");
				}
			}
			else if(nosave_par.identity==IDENTITY_TEMPO) //临时身份
			{
				if(strlen(Author_Par.tempo_phone)==0)
				{
					SetTextValue(0,8,"未设置手机号");
				}
				else if(DateCmp(g_rtc,dev_SavePar.factory_time)==0)
				{
					SetTextValue(0,8,"临时密码过期");
				}
				if(strcmp((char *)nosave_par.pass,(char *)Author_Par.tempo_pass)==0)
				{
					SetScreen(Last_screen_id);
					SetTextValue(0,8,"");
					SaveOPERA(OPERA_LOGIN,NULL,NULL);
					SetControlEnable(5,7,0);
					SetControlEnable(6,7,0);
					nosave_par.current_identity=nosave_par.identity;
				}
				else
				{
					SetTextValue(0,8,"密码错误");
				}
			}
		}
		if(control_id==6&&state==0)
		{
			nosave_par.current_identity=IDENTITY_NULL;
			nosave_par.LCD_timecount=0;
		}
		if(control_id==9&&state==0)
		{
			nosave_par.current_identity=IDENTITY_NULL;
			SetScreen(Last_screen_id);
		}
	}
	
	if(screen_id==1)  //系统状态
	{
		if(control_id==15&&state==1&&identityJudge(ID_OPERA))              //上行
		{
			if(MOTOR_LIMIT_UP==1) //向上
			{
				return;
			}
			StallGuardResetCheck();
			Control_Par.control_run=0;
			Control_Par.setState=GATE_UP;
			TMC5160A_RUNUP();
		}
		if(control_id==16&&state==1&&identityJudge(ID_OPERA))              //下行
		{
			if(MOTOR_LIMIT_DOWN==1) //向下
			{
				return;
			}
			StallGuardResetCheck();
			Control_Par.control_run=0;
			Control_Par.setState=GATE_DOWN;
			TMC5160A_RUNDOWN();
		}
		if(control_id==17&&state==1&&identityJudge(ID_OPERA)&&Control_Par.control_run==0)              //定开度控制
		{
			Control_Par.control_mode=0;
		}
		if(control_id==18&&state==1&&identityJudge(ID_OPERA))              //停止
		{
			Control_Par.control_run=0;
			Control_Par.setState=GATE_STOP;
			TMC5160A_STOP();
		}
		
		if(control_id==19&&state==1&&identityJudge(ID_OPERA))  //打开控制
		{
			if(Control_Par.control_mode==0&&!dev_SavePar.Opening_Fun)
			{
				return;
			}
			if(Control_Par.control_mode==1&&!dev_SavePar.Level_Fun) 	//默认dev_SavePar.Level_Fun==0关闭   1是开启
			{
				return;
			}
			if(Control_Par.control_mode==2&&!dev_SavePar.Flow_Fun)
			{
				return;
			}
			StallGuardResetCheck();
			Control_Par.control_run=1;				//三种控制模式自动控制打开
//			if(Control_Par.control_mode==0)
//			{
//				motor_AutoControl();
//			}
			nosave_par.SendControlflag=1;
		}
		if(control_id==20&&state==1&&identityJudge(ID_OPERA)) //关闭控制
		{
			Control_Par.control_run=0;
			Control_Par.setState=GATE_STOP;
			TMC5160A_STOP();
			nosave_par.SendControlflag=1;
		}
		
		if(control_id==24&&state==0&&identityJudge(ID_OPERA)&&Control_Par.control_mode==0) //定开度增加
		{
			Control_Par.set_openingm+=0.1;
			
			if(Control_Par.set_openingm>dev_SavePar.water_inletHeight)
			{
				Control_Par.set_openingm=dev_SavePar.water_inletHeight;
				snprintf((char *)show_title,sizeof(show_title),"提示");
				snprintf((char *)show_content,sizeof(show_content),"开度不得大于闸门最大高度(%.2fm)",dev_SavePar.water_inletHeight);
				SetScreen(18);
			}
			nosave_par.SendControlflag=1;
		}
		if(control_id==26&&state==0&&identityJudge(ID_OPERA)&&Control_Par.control_mode==0) //定开度减少
		{
			
			Control_Par.set_openingm-=0.1;
			
			if(Control_Par.set_openingm<0)
			{
				Control_Par.set_openingm=0;
			}
			nosave_par.SendControlflag=1;
		}
		if(control_id==25&&state==0&&identityJudge(ID_OPERA))
		{
			nosave_par.LCD_timecount=0;
		}
		if(control_id==27&&state==1&&identityJudge(ID_OPERA)&&Control_Par.control_run==0)  //定水位控制
		{
			Control_Par.control_mode=1;
		}
		if(control_id==28&&state==1&&identityJudge(ID_OPERA)&&Control_Par.control_run==0)  //定流量控制
		{
			Control_Par.control_mode=2;
		}
		if(control_id==32&&state==0&&identityJudge(ID_OPERA)&&Control_Par.control_mode==1) //定水位增加
		{
			if(round(Control_Par.set_level*10)<10)
			{
				Control_Par.set_level+=0.1;
			}
			if(Control_Par.set_level>dev_SavePar.level_max)
			{
				Control_Par.set_level=dev_SavePar.level_max;
			}
			nosave_par.SendControlflag=1;
			
		}
		if(control_id==33&&state==0&&identityJudge(ID_OPERA)&&Control_Par.control_mode==1) //定水位减少
		{
			if(round(Control_Par.set_level*10)>0)
			{
				Control_Par.set_level-=0.1;
			}
			if(Control_Par.set_level<0)
			{
				Control_Par.set_level=0;
			}
			nosave_par.SendControlflag=1;
		}
		if(control_id==34&&state==0&&identityJudge(ID_OPERA)&&Control_Par.control_mode==2) //定流量增加
		{
			if(round(Control_Par.set_flow*10)<10)  //round 浮点数四舍五入为最接近的整数
			{
				Control_Par.set_flow+=0.1;
			}
			if(Control_Par.set_flow>dev_SavePar.flow_max)
			{
				Control_Par.set_flow=dev_SavePar.flow_max;
			}
			nosave_par.SendControlflag=1;
		}
		if(control_id==35&&state==0&&identityJudge(ID_OPERA)&&Control_Par.control_mode==2) //定流量减少
		{
			if(round(Control_Par.set_flow*10)>0)
			{
				Control_Par.set_flow-=0.1;
			}
			if(Control_Par.set_flow<0)
			{
				Control_Par.set_flow=0;
			}
			nosave_par.SendControlflag=1;
		}
		if(control_id==38&&state==0)
		{
			ReadRTC();
		}
	}
	if(screen_id==2) //运行数据
	{
		tempu32=SD_GetLogPage(Loglen_Par.flow_num);
		if(control_id==25&&state==0)
		{
			nosave_par.LCD_timecount=0;
		}
		if(control_id==9&&state==0)
		{
			if(nosave_par.data_page<tempu32)
			{
				nosave_par.flow_page++;
				if(dev_SavePar.Flow_Fun)
				{
					LCD_showdLog(); 
				}
				else
				{
					LCD_showNoFlowLog(); 
				}
			}
			
		}
		if(control_id==10&&state==0)
		{
			if(nosave_par.flow_page>1)
			{
				nosave_par.flow_page--;
				if(dev_SavePar.Flow_Fun)
				{
					LCD_showdLog(); 
				}
				else
				{
					LCD_showNoFlowLog(); 
				}
			}
		}
		if(control_id==12&&state==0)
		{
			LCD_showflag=!LCD_showflag;
		}
	}
	if(screen_id==3) //设备状态
	{
		if(control_id==25&&state==0)
		{
			nosave_par.LCD_timecount=0;
		}
	}
	if(screen_id==4) //操作记录
	{
		if(control_id==25&&state==0)
		{
			nosave_par.LCD_timecount=0;
		}
		if(control_id==9&&state==0)
		{
			nosave_par.opera_page++;
			LCD_showoperaLog();
		}
		if(control_id==10&&state==0)
		{
			nosave_par.opera_page--;
			LCD_showoperaLog();
		}
	}
	if(screen_id==5) //系统信息
	{
		if(control_id==31&&state==0)
		{
			nosave_par.LCD_timecount=0;
		}
		if(control_id==45&&state==0&&identityJudge(ID_MAINT))  
		{
			SetScreen(12);  //调试界面
		}
		if(control_id==46&&state==0&&identityJudge(ID_MAINT))
		{
			SetScreen(13);  //调试界面
		}
		if(control_id==47&&state==0&&identityJudge(ID_MAINT))
		{
			SetScreen(16);  //SD卡测试
		}
		if(control_id==42&&state==0&&identityJudge(ID_OPERA)) //清空运行数据
		{
			nosave_par.cleanlog_ptr=1;
			strcpy((char *)nosave_par.confirmpass,"");
			SetTextValue(11,3,(u8 *)"");
			SetScreen(11);
		}
		if(control_id==43&&state==0&&identityJudge(ID_OPERA)) //清空操作数据
		{
			nosave_par.cleanlog_ptr=2;
			strcpy((char *)nosave_par.confirmpass,"");
			SetTextValue(11,3,(u8 *)"");
			SetScreen(11);
		}
	}
	if(screen_id==6) //权限管理
	{
		if(control_id==31&&state==0)
		{
			nosave_par.LCD_timecount=0;
		}
	}
	if(screen_id==10)
	{
		if(control_id==5&&state==0)
		{
			nosave_par.identity=IDENTITY_ADMIN;
			SetScreen(0);
		}
		if(control_id==6&&state==0)
		{
			nosave_par.identity=IDENTITY_OPERA1;
			SetScreen(0);
		}
		if(control_id==7&&state==0)
		{
			nosave_par.identity=IDENTITY_OPERA2;
			SetScreen(0);
		}
		if(control_id==8&&state==0)
		{
			nosave_par.identity=IDENTITY_MAINT;
			SetScreen(0);
		}
		if(control_id==9&&state==0)
		{
			nosave_par.identity=IDENTITY_TEMPO;
			SetScreen(0);
		}
		if(control_id==12&&state==0)
		{
			nosave_par.LCD_timecount=0;
		}
	}
	if(screen_id==11) //确认界面
	{
		if(control_id==15&&state==0)
		{
			if(strcmp((char *)nosave_par.confirmpass,(char *)Author_Par.maint_pass)==0)
			{
				if(nosave_par.cleanlog_ptr==1)
				{
					SD_ClearFlowLog();
					SaveOPERA(OPERA_CLEAN_RUNLOG,NULL,NULL);
				}
				else if(nosave_par.cleanlog_ptr==2)
				{
					SD_ClearOperaLog();
					SaveOPERA(OPERA_CLEAN_OPERALOG,NULL,NULL);
				}
				SetScreen(5);
				
				
			}
			else
			{
				SetTextValue(11,3,(u8 *)"密码错误");
			}
		}
		if(control_id==16&&state==0)
		{
			SetScreen(5);
		}
		if(control_id==25&&state==0)
		{
			nosave_par.LCD_timecount=0;
		}
	}
	if(screen_id==12) //调试界面
	{
		if(control_id==13&&state==1&&identityJudge(ID_MAINT))
		{
			nosave_par.test_step_flag=1;
			TMC5160A_TestStep();
		}
		if(control_id==15&&state==1&&identityJudge(ID_MAINT))
		{
			nosave_par.test_step_flag=0;
			nosave_par.test_step_state=0;
			TMC5160A_EN=1;
		}
		if(control_id==21&&state==1&&identityJudge(ID_MAINT))              //上行
		{
			if(MOTOR_LIMIT_UP==1) //向上
			{
				return;
			}
			StallGuardResetCheck();
			Control_Par.control_run=0;
			Control_Par.setState=GATE_UP;
			TMC5160A_RUNUP();
		}
		if(control_id==22&&state==1&&identityJudge(ID_MAINT))              //下行
		{
			if(MOTOR_LIMIT_DOWN==1) //向下
			{
				return;
			}
			StallGuardResetCheck();
			Control_Par.control_run=0;
			Control_Par.setState=GATE_DOWN;
			TMC5160A_RUNDOWN();
		}
		if(control_id==23&&state==1&&identityJudge(ID_MAINT))              //停止
		{
//			TMC5160A_EN=!TMC5160A_EN;
//			MOS_PIN=!MOS_PIN;
			Control_Par.control_run=0;
			Control_Par.setState=GATE_STOP;
			TMC5160A_STOP();
		}
	}
	if(screen_id==13) //调试界面
	{
		if(control_id==13&&state==1&&identityJudge(ID_MAINT))
		{
			nosave_par.test_step_flag=1;
			TMC5160A_TestStep();
		}
		if(control_id==15&&state==1&&identityJudge(ID_MAINT))
		{
			nosave_par.test_step_flag=0;
			nosave_par.test_step_state=0;
			TMC5160A_EN=1;
		}
		if(control_id==21&&state==1&&identityJudge(ID_MAINT))              //上行
		{
			if(MOTOR_LIMIT_UP==1) //向上
			{
				return;
			}
			StallGuardResetCheck();
			Control_Par.control_run=0;
			Control_Par.setState=GATE_UP;
			TMC5160A_RUNUP();
		}
		if(control_id==22&&state==1&&identityJudge(ID_MAINT))              //下行
		{
			if(MOTOR_LIMIT_DOWN==1) //向下
			{
				return;
			}
			StallGuardResetCheck();
			Control_Par.control_run=0;
			Control_Par.setState=GATE_DOWN;
			TMC5160A_RUNDOWN();
		}
		if(control_id==23&&state==1&&identityJudge(ID_MAINT))              //停止
		{
			Control_Par.control_run=0;
			Control_Par.setState=GATE_STOP;
			TMC5160A_STOP();
		}
		if(control_id==26)
		{
			dev_SavePar.StallGuard_Switch=state;
			Flash_SaveDevData();
		}
	}
	if(screen_id==16) //SD测试
	{
		if(control_id==8&&state==0)
		{
			Record_Clear(16,9);
			testSD((u8*)"0:");
		}
	}
	if(screen_id==17) //系统设置-运行参数
	{
		if(control_id==31&&state==0)
		{
			nosave_par.LCD_timecount=0;
		}
		
		if(control_id==17&&identityJudge(ID_MAINT))		//开度功能
		{
			dev_SavePar.Opening_Fun=state;
			Flash_SaveDevData();
			SaveOPERA(OPERA_OPENING_FUN,&dev_SavePar.Opening_Fun,&state);
		}
		if(control_id==10&&identityJudge(ID_MAINT))		//水位功能
		{
			dev_SavePar.Level_Fun=state;
			Flash_SaveDevData();
			SaveOPERA(OPERA_LEVEL_FUN,&dev_SavePar.Level_Fun,&state);
		}
		if(control_id==11&&identityJudge(ID_MAINT))		//流量功能
		{
			dev_SavePar.Flow_Fun=state;
			Flash_SaveDevData();
			SaveOPERA(OPERA_FLOW_FUN,&dev_SavePar.Flow_Fun,&state);
		}
		if(control_id==15&&identityJudge(ID_MAINT))		//净空自测
		{
			nosave_par.Auto_airh=1;
		}
		if(control_id==16&&identityJudge(ID_MAINT))		//4G功能
		{
			return ;
			dev_SavePar._4G_Fun=state;
			Flash_SaveDevData();
			SaveOPERA(OPERA_4G_FUN,&dev_SavePar._4G_Fun,&state);
		}
		if(control_id==18&&identityJudge(ID_MAINT))		//Lora功能
		{
			dev_SavePar.Lora_Fun=state;
			Flash_SaveDevData();
			SaveOPERA(OPERA_LORA_FUN,&dev_SavePar.Lora_Fun,&state);
		}
	}
	if(screen_id==18) //提示界面
	{
		if(control_id==31&&state==0)
		{
			nosave_par.LCD_timecount=0;
		}
		if(control_id==2&&state==0)
		{
			SetScreen(Last_screen_id);
		}
	}
	
	if(screen_id==19) //外部设备
	{
		if(control_id==15&&state==0)
		{
			//与输入设备编码的外部设备进行通信  是否通信正常
			memcpy(dev_SavePar.Externdev_code,nosave_par.Externdev_code,sizeof(nosave_par.Externdev_code));
			dev_SavePar.TypeUltra=1;
			Flash_SaveDevData();
			SetTextValue(19,12,(u8 *)"巴歇尔槽");
			nosave_par.Read_ExterndevF=1;
			SetScreen(5);
		}
		if(control_id==16&&state==0)
		{
			//取消返回
			SetScreen(5);
		}
		if(control_id==25&&state==0)
		{
			nosave_par.LCD_timecount=0;
		}
	}
}
/*
判断是否满足权限
*/

s8 identityJudge(id_enum identity)
{
	//操作员1
	if(identity==ID_OPERA)
	{
		//如果当前的身份和密码匹配则ok
		if(nosave_par.current_identity==IDENTITY_OPERA1&&strcmp((char *)nosave_par.pass,(char *)Author_Par.opera1_pass)==0)
		{
			return 1;
		}
		if(nosave_par.current_identity==IDENTITY_OPERA2&&strcmp((char *)nosave_par.pass,(char *)Author_Par.opera2_pass)==0)
		{
			return 1;
		}
		if(nosave_par.current_identity==IDENTITY_ADMIN&&strcmp((char *)nosave_par.pass,(char *)Author_Par.admin_pass)==0)
		{
			return 1;
		}
		if(nosave_par.current_identity==IDENTITY_MAINT&&strcmp((char *)nosave_par.pass,(char *)Author_Par.maint_pass)==0)
		{
			return 1;
		}
	
	}
	if(identity==ID_MAINT) //维护员
	{
		if(nosave_par.current_identity==IDENTITY_MAINT&&strcmp((char *)nosave_par.pass,(char *)Author_Par.maint_pass)==0)
		{
			return 1;
		}
	}
	SetScreen(0);
	return 0;
}
/**/
s8 identityJudge2(identity_enum need_id)
{
	
	if(need_id==IDENTITY_OPERA1) //操作员1
	{
		//如果当前的身份和密码匹配则ok
		if(nosave_par.current_identity==IDENTITY_OPERA1&&strcmp((char *)nosave_par.pass,(char *)Author_Par.opera1_pass)==0)
		{
			return 1;
		}
		if(nosave_par.current_identity==IDENTITY_ADMIN&&strcmp((char *)nosave_par.pass,(char *)Author_Par.admin_pass)==0)
		{
			return 1;
		}
		if(nosave_par.current_identity==IDENTITY_MAINT&&strcmp((char *)nosave_par.pass,(char *)Author_Par.maint_pass)==0)
		{
			return 1;
		}
	
	}
	if(need_id==IDENTITY_OPERA2) //操作员2
	{
		//如果当前的身份和密码匹配则ok
		if(nosave_par.current_identity==IDENTITY_OPERA2&&strcmp((char *)nosave_par.pass,(char *)Author_Par.opera2_pass)==0)
		{
			return 1;
		}
		if(nosave_par.current_identity==IDENTITY_ADMIN&&strcmp((char *)nosave_par.pass,(char *)Author_Par.admin_pass)==0)
		{
			return 1;
		}
		if(nosave_par.current_identity==IDENTITY_MAINT&&strcmp((char *)nosave_par.pass,(char *)Author_Par.maint_pass)==0)
		{
			return 1;
		}
	}
	if(need_id==IDENTITY_ADMIN)  //管理员
	{
		//如果当前的身份和密码匹配则ok
		
		if(nosave_par.current_identity==IDENTITY_ADMIN&&strcmp((char *)nosave_par.pass,(char *)Author_Par.admin_pass)==0)
		{
			return 1;
		}
		if(nosave_par.current_identity==IDENTITY_MAINT&&strcmp((char *)nosave_par.pass,(char *)Author_Par.maint_pass)==0)
		{
			return 1;
		}
	}
	if(need_id==IDENTITY_MAINT)
	{
		if(nosave_par.current_identity==IDENTITY_MAINT&&strcmp((char *)nosave_par.pass,(char *)Author_Par.maint_pass)==0)
		{
			return 1;
		}
	}
	snprintf((char *)show_title,sizeof(show_title),"提示");
	snprintf((char *)show_content,sizeof(show_content),"身份错误，无法修改！");
	SetScreen(18);
	return 0;
}
/*! 
*  \brief  文本控件通知
*  \details  当文本通过键盘更新(或调用GetControlValue)时，执行此函数
*  \details  文本控件的内容以字符串形式下发到MCU，如果文本控件内容是浮点值，
*  \details  则需要在此函数中将下发字符串重新转回浮点值。
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param str 文本控件内容
*/

void NotifyText(uint16 screen_id, uint16 control_id, uint8 *str)
{
	char tempu8[50];
	u8 tempu81;
	float tempf,tempf2;
	u32 tempu32,tempu322;
	u8 test1=20;
	s32 temps32;
//	char tempc[50];
	nosave_par.LCD_timecount=LCD_SHOWTIME;
	
	if(screen_id==0)
	{
		//必须保证存储空间大于要存储的数据
		if(strlen((char *)str)<sizeof(nosave_par.pass))
		{
			memcpy(nosave_par.pass,str,sizeof(nosave_par.pass));
		}
		
	}
	if(screen_id==1)
	{
		//
		if(control_id==14&&identityJudge(ID_OPERA)&&Control_Par.control_mode==0)
		{
			sscanf((const char *)str,"%f",&tempf);
			Control_Par.set_openingm=tempf;
			if(Control_Par.set_openingm>dev_SavePar.water_inletHeight)
			{
				Control_Par.set_openingm=dev_SavePar.water_inletHeight;
				snprintf((char *)show_title,sizeof(show_title),"提示");
				snprintf((char *)show_content,sizeof(show_content),"开度不得大于闸门最大高度(%.2fm)",dev_SavePar.water_inletHeight);
				SetScreen(18);
			}
			if(Control_Par.set_openingm<0)
			{
				Control_Par.set_openingm=0;
			}
			Flash_SaveControl_Par();
			nosave_par.SendControlflag=1;
		}
		if(control_id==29&&identityJudge(ID_OPERA)&&Control_Par.control_mode==1)
		{
			sscanf((const char *)str,"%f",&tempf);
			Control_Par.set_level=tempf;
			Flash_SaveControl_Par();
		}
		if(control_id==31&&identityJudge(ID_OPERA)&&Control_Par.control_mode==2)
		{
			sscanf((const char *)str,"%f",&tempf);
			Control_Par.set_flow=tempf;
			Flash_SaveControl_Par();
		}
	}
	if(screen_id==2)
	{
		tempu322=SD_GetDataLogPage();
		if(control_id==8)
		{
			sscanf((const char *)str,"%d",&tempu32);
			if(tempu32>0&&tempu32<=tempu322)
			{
				nosave_par.data_page=tempu32;
				//显示运行数据
				LCD_showdLog();
			}
		}
	}
	if(screen_id==4)
	{
		tempu322=SD_GetOperaLogPage();
		
		
		if(control_id==8)
		{
			sscanf((const char *)str,"%d",&tempu32);
			if(tempu32>0&&tempu32<=tempu322)
			{
				nosave_par.opera_page=tempu32;
				//显示运行数据
				LCD_showoperaLog();
			}
		}
	}
	if(screen_id==5)  //系统设置-系统信息
	{
		if(!identityJudge(ID_MAINT))
		{
			return;
		}
		if(control_id==9) //产品名称
		{
			SaveOPERA(OPERA_DEV_NAME,dev_SavePar.dev_name,str);
			memcpy(dev_SavePar.dev_name,str,sizeof(dev_SavePar.dev_name));
			Flash_SaveDevData();
		}
		if(control_id==10) //软件版本
		{
			SaveOPERA(OPERA_DEV_NAME,myota_info.nowVersion,str);
			memcpy(myota_info.nowVersion,str,sizeof(myota_info.nowVersion));
			Flash_Savemyota_info();
		}
		if(control_id==11) //电机功率
		{
			SaveOPERA(OPERA_MOTOR_POWER,myota_info.nowVersion,str);		//操作记录   变量使用错误  应使用dev_SavePar.motor_power
			memcpy(dev_SavePar.motor_power,str,sizeof(dev_SavePar.motor_power));
			
			Flash_SaveDevData();
		}
		if(control_id==12) //出厂时间
		{
			snprintf(tempu8,sizeof(tempu8),"%d%02d%02d",dev_SavePar.factory_time.years,dev_SavePar.factory_time.months,dev_SavePar.factory_time.days);
			
			//memcpy(dev_SavePar.factory_time,str,sizeof(dev_SavePar.factory_time));
			test1=sscanf((const char *)str,"%hd-%hhd-%hhd",&dev_SavePar.factory_time.years,&dev_SavePar.factory_time.months,&dev_SavePar.factory_time.days);
			if(test1==3)
			{
				SaveOPERA(OPERA_FACTORY_TIME,tempu8,str);
				Flash_SaveDevData();
//				Loglen_Par.data_len=0;
//				Loglen_Par.data_head=0;
//				Loglen_Par.opera_len=0;
//				Loglen_Par.opera_head=0;
				
				Flash_SaveLoglen_Par();
				
			}
		}
		
		if(control_id==14) //设备编号
		{
			SaveOPERA(OPERA_DEV_CODE,dev_SavePar.dev_code,str);
			memcpy(dev_SavePar.dev_code,str,sizeof(dev_SavePar.dev_code));
			Flash_SaveDevData();
		}
		if(control_id==15) //上传地址
		{
			SaveOPERA(OPERA_SENDNEWADDR,dev_SavePar.Upload_URL,str);
			memcpy(dev_SavePar.Upload_URL,str,sizeof(dev_SavePar.Upload_URL));
			Flash_SaveDevData();
		}
	/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
		if(control_id==16) //进水宽度
		{
			tempf2=dev_SavePar.water_inletWidth;
			sscanf((const char *)str,"%f",&tempf);
			dev_SavePar.water_inletWidth=tempf;
			SaveOPERA(OPERA_WATER_INLETWIDTH,&tempf2,&dev_SavePar.water_inletWidth);
			
			
			Flash_SaveDevData();
		}
		if(control_id==17) //探头初距
		{
			sscanf((const char *)str,"%f",&tempf);
			SaveOPERA(OPERA_ULTRA_INITIALDIST,&dev_SavePar.Ultra_initialDist,&tempf);
			dev_SavePar.Ultra_initialDist=tempf;
			
			Flash_SaveDevData();
		}
		if(control_id==32) //闸前净空
		{
			sscanf((const char *)str,"%f",&tempf);
			SaveOPERA(OPERA_FROHEIGHT_AIR,&dev_SavePar.FroHeight_Air,&tempf);
			dev_SavePar.FroHeight_Air=tempf;
			
			Flash_SaveDevData();
		}
		
		if(control_id==18) 		//进水高度		对应		开度
		{
			sscanf((const char *)str,"%f",&tempf);
			SaveOPERA(OPERA_WATER_INLETHEIGHT,&dev_SavePar.water_inletHeight,&tempf);
			dev_SavePar.water_inletHeight=tempf;
			
			Flash_SaveDevData();
		}
		if(control_id==19) //探头间距
		{
			sscanf((const char *)str,"%f",&tempf);
			SaveOPERA(OPERA_ULTRA_SPACING,&dev_SavePar.Ultra_Spacing,&tempf);
			dev_SavePar.Ultra_Spacing=tempf;
			
			Flash_SaveDevData();
		}
		if(control_id==35) //闸后净空
		{
			sscanf((const char *)str,"%f",&tempf);
			SaveOPERA(OPERA_ULTRA_SPACING,&dev_SavePar.Ultra_Spacing,&tempf);			//操作记录错误
			dev_SavePar.AftHeight_Air=tempf;
			Flash_SaveDevData();
		}
		
		if(control_id==20) //底板高差
		{
			sscanf((const char *)str,"%f",&tempf);
			SaveOPERA(OPERA_BASE_HEIGHTDIFF,&dev_SavePar.base_heightDiff,&tempf);
			dev_SavePar.base_heightDiff=tempf;
			Flash_SaveDevData();
		}
		if(control_id==21) //探头层数
		{
			sscanf((const char *)str,"%f",&tempf);
			SaveOPERA(OPERA_ULTRA_NUM,&dev_SavePar.Ultra_Num,&tempf);
			dev_SavePar.Ultra_Num=tempf;
			Flash_SaveDevData();
		}
		
		
		if(control_id==44) //渠道比降
		{
			sscanf((const char *)str,"%f",&tempf);
			SaveOPERA(OPERA_GRADIENT,&dev_SavePar.Gradient,&tempf);
			dev_SavePar.Gradient=tempf;
			Flash_SaveDevData();
		}
	}
	if(screen_id==6)  //系统设置-权限设置
	{
		if(control_id==9&&identityJudge2(IDENTITY_ADMIN))  //管理员手机号
		{
			memcpy(Author_Par.admin_phone,str,sizeof(Author_Par.admin_phone));
			Flash_SaveAuthor_Par();
		}
		if(control_id==12&&identityJudge2(IDENTITY_ADMIN)) //管理员密码
		{
			memcpy(Author_Par.admin_pass,str,sizeof(Author_Par.admin_pass));
			Flash_SaveAuthor_Par();
		}
		if(control_id==10&&identityJudge2(IDENTITY_OPERA1)) //操作员1手机号
		{
			memcpy(Author_Par.opera1_phone,str,sizeof(Author_Par.opera1_phone));
			Flash_SaveAuthor_Par();
		}
		if(control_id==13&&identityJudge2(IDENTITY_OPERA1)) //操作员1密码
		{
			memcpy(Author_Par.opera1_pass,str,sizeof(Author_Par.opera1_pass));
			Flash_SaveAuthor_Par();
		}
		if(control_id==11&&identityJudge2(IDENTITY_OPERA2)) //操作员2手机号
		{
			memcpy(Author_Par.opera2_phone,str,sizeof(Author_Par.opera2_phone));
			Flash_SaveAuthor_Par();
		}
		if(control_id==14&&identityJudge2(IDENTITY_OPERA2)) //操作员2密码
		{
			memcpy(Author_Par.opera2_pass,str,sizeof(Author_Par.opera2_pass));
			Flash_SaveAuthor_Par();
		}
		if(control_id==24&&identityJudge2(IDENTITY_MAINT)) //维护员手机号
		{
			memcpy(Author_Par.maint_phone,str,sizeof(Author_Par.maint_phone));
			Flash_SaveAuthor_Par();
		}
		if(control_id==23&&identityJudge2(IDENTITY_MAINT)) //维护员密码
		{
			memcpy(Author_Par.maint_pass,str,sizeof(Author_Par.maint_pass));
			Flash_SaveAuthor_Par();
		}
		if(control_id==25&&identityJudge2(IDENTITY_TEMPO)) //临时手机号
		{
			memcpy(Author_Par.tempo_phone,str,sizeof(Author_Par.tempo_phone));
			Flash_SaveAuthor_Par();
		}
		if(control_id==26&&identityJudge2(IDENTITY_TEMPO)) //临时密码
		{
			memcpy(Author_Par.tempo_pass,str,sizeof(Author_Par.tempo_pass));
			Flash_SaveAuthor_Par();
		}
		
	}
	if(screen_id==9)
	{
		tempu322=SD_GetDataLogPage();
		
		
		if(control_id==8)
		{
			sscanf((const char *)str,"%d",&tempu32);
			if(tempu32>0&&tempu32<=tempu322)
			{
				nosave_par.data_page=tempu32;
				//显示运行数据
				LCD_showSpeedLog();
			}
		}
	}
	if(screen_id==11) //确认界面
	{
		if(control_id==12) //输入确认密码
		{
			memcpy(nosave_par.confirmpass,str,sizeof(nosave_par.confirmpass));
		}
		
	}
	if(screen_id==12)
	{
		if(control_id==8) //当前步数
		{
			sscanf((const char *)str,"%d",&tempu32);
			Mess_Par.run_step=tempu32;
			Flash_SaveMess_Par();
		}
		if(control_id==9) //总步数
		{
			sscanf((const char *)str,"%d",&tempu32);
			Mess_Par.sum_step=tempu32;
			Flash_SaveMess_Par();
		}
		if(control_id==10) //速度
		{
			sscanf((const char *)str,"%d",&tempu32);
			if(tempu32>3000||tempu32<10)
			{
				return;
			}
			Mess_Par.run_speed=tempu32;
			Flash_SaveMess_Par();
			TMC5160A_ChangeSpeed();
		}
		if(control_id==16) //运行电流
		{
			sscanf((const char *)str,"%d",&tempu32);
//			SaveOPERA(OPERA_ULTRA_INITIALDIST,&dev_SavePar.Ultra_initialDist,&tempf);
			dev_SavePar.runCurrent=tempu32;
			
			Flash_SaveDevData();
			TMC5160_SetCurrent();
			
		}
		if(control_id==17)  //监测阈值
		{
			sscanf((const char *)str,"%d",&tempu32);
//			SaveOPERA(OPERA_ULTRA_INITIALDIST,&dev_SavePar.Ultra_initialDist,&tempf);
			dev_SavePar.TCOOLTHRS=tempu32;
			
			Flash_SaveDevData();
		}
		if(control_id==18) //监测灵敏度
		{
			sscanf((const char *)str,"%d",&temps32);
//			SaveOPERA(OPERA_ULTRA_INITIALDIST,&dev_SavePar.Ultra_initialDist,&tempf);
			dev_SavePar.SGT=temps32;
			
			Flash_SaveDevData();
			TMC5160_SetSGT();
		}
		if(control_id==24)
		{
			sscanf((const char *)str,"%d",&temps32);
//			SaveOPERA(OPERA_ULTRA_INITIALDIST,&dev_SavePar.Ultra_initialDist,&tempf);
			Log_Par.this_water=temps32;
			
			Flash_SaveLog_Par();
		}
		if(control_id==28)
		{
			sscanf((const char *)str,"%d",&temps32);
//			SaveOPERA(OPERA_ULTRA_INITIALDIST,&dev_SavePar.Ultra_initialDist,&tempf);
			Log_Par.year_water=temps32;
			
			Flash_SaveLog_Par();
		}
		if(control_id==29)
		{
			sscanf((const char *)str,"%d",&temps32);
//			SaveOPERA(OPERA_ULTRA_INITIALDIST,&dev_SavePar.Ultra_initialDist,&tempf);
			Log_Par.accu_water=temps32;
			
			Flash_SaveLog_Par();
		}
			
	}
	if(screen_id==13)
	{
		if(control_id==8) //当前步数
		{
			sscanf((const char *)str,"%d",&tempu32);
			Mess_Par.run_step=tempu32;
			Flash_SaveMess_Par();
		}
		if(control_id==9) //总步数
		{
			sscanf((const char *)str,"%d",&tempu32);
			Mess_Par.sum_step=tempu32;
			Flash_SaveMess_Par();
		}
		if(control_id==10) //速度
		{
			sscanf((const char *)str,"%d",&tempu32);
			if(tempu32>3000)
			{
				return;
			}
			Mess_Par.run_speed=tempu32;
			Flash_SaveMess_Par();
			TMC5160A_ChangeSpeed();
		}
		if(control_id==14)
		{
			//dev_SavePar.AMAX
			sscanf((const char *)str,"%d",&tempu32);
//			SaveOPERA(OPERA_ULTRA_INITIALDIST,&dev_SavePar.Ultra_initialDist,&tempf);
			dev_SavePar.AMAX=tempu32;
			
			Flash_SaveDevData();
			TMC5160_SetAMAX();
		}
		if(control_id==16) //运行电流
		{
			sscanf((const char *)str,"%d",&tempu32);
//			SaveOPERA(OPERA_ULTRA_INITIALDIST,&dev_SavePar.Ultra_initialDist,&tempf);
			dev_SavePar.runCurrent=tempu32;
			
			Flash_SaveDevData();
			TMC5160_SetCurrent();
			
		}
		if(control_id==17)  //监测阈值
		{
			sscanf((const char *)str,"%d",&tempu32);
//			SaveOPERA(OPERA_ULTRA_INITIALDIST,&dev_SavePar.Ultra_initialDist,&tempf);
			dev_SavePar.TCOOLTHRS=tempu32;
			
			Flash_SaveDevData();
		}
		if(control_id==18) //监测灵敏度
		{
			sscanf((const char *)str,"%d",&temps32);
//			SaveOPERA(OPERA_ULTRA_INITIALDIST,&dev_SavePar.Ultra_initialDist,&tempf);
			dev_SavePar.SGT=temps32;
			
			Flash_SaveDevData();
			TMC5160_SetSGT();
		}
		if(control_id==27)
		{
			sscanf((char *)str,"%f",&tempf);
			dev_SavePar.StallGuard_SpeedThreshold=tempf;
			Flash_SaveDevData();
		}
		if(control_id==28)
		{
			sscanf((char *)str,"%hhd",&tempu81);
			dev_SavePar.StallGuard_Secs=tempu81;
			Flash_SaveDevData();
		}
	} 
	if(screen_id==14)
	{
		if(control_id==8) //当前步数
		{
			sscanf((const char *)str,"%d",&tempu32);
			Mess_Par.run_step=tempu32;
			Flash_SaveMess_Par();
		}
		if(control_id==9) //总步数
		{
			sscanf((const char *)str,"%d",&tempu32);
			Mess_Par.sum_step=tempu32;
			Flash_SaveMess_Par();
		}
		if(control_id==10) //速度
		{
			sscanf((const char *)str,"%d",&tempu32);
			if(tempu32>3000)
			{
				return;
			}
			Mess_Par.run_speed=tempu32;
			Flash_SaveMess_Par();
			TMC5160A_ChangeSpeed();
		}
		if(control_id==14)
		{
			//dev_SavePar.AMAX
			sscanf((const char *)str,"%d",&tempu32);
//			SaveOPERA(OPERA_ULTRA_INITIALDIST,&dev_SavePar.Ultra_initialDist,&tempf);
			dev_SavePar.AMAX=tempu32;
			
			Flash_SaveDevData();
			TMC5160_SetAMAX();
		}
		if(control_id==15) //位置模式
		{
			sscanf((const char *)str,"%d",&tempu32);
			TMC5160A_1_Write(0x20,0x00000000);
			TMC5160_ClearStallGuard();
	//		TMC5160_moveBy(300000);  //提示:最大可能位移为+ / - ( ( 2^31 ) - 1 )
			TMC5160_moveBy(tempu32);//51200
		}
		if(control_id==16) //运行电流
		{
			sscanf((const char *)str,"%d",&tempu32);
//			SaveOPERA(OPERA_ULTRA_INITIALDIST,&dev_SavePar.Ultra_initialDist,&tempf);
			dev_SavePar.runCurrent=tempu32;
			
			Flash_SaveDevData();
			TMC5160_SetCurrent();
			
		}
		if(control_id==17)  //监测阈值
		{
			sscanf((const char *)str,"%d",&tempu32);
//			SaveOPERA(OPERA_ULTRA_INITIALDIST,&dev_SavePar.Ultra_initialDist,&tempf);
			dev_SavePar.TCOOLTHRS=tempu32;
			
			Flash_SaveDevData();
		}
		if(control_id==18) //监测灵敏度
		{
			sscanf((const char *)str,"%d",&temps32);
//			SaveOPERA(OPERA_ULTRA_INITIALDIST,&dev_SavePar.Ultra_initialDist,&tempf);
			dev_SavePar.SGT=temps32;
			
			Flash_SaveDevData();
			TMC5160_SetSGT();
		}
	}
	if(screen_id==17)
	{
		/*。。。。。。。*/
		if(!identityJudge(ID_MAINT))
		{
			return;
		}
		if(control_id==22) //最小流量
		{
			sscanf((const char *)str,"%f",&tempf);
			SaveOPERA(OPERA_FLOW_MIN,&dev_SavePar.flow_min,&tempf);
			dev_SavePar.flow_min=tempf;
			Flash_SaveDevData();
		}
		if(control_id==23) //最小水位
		{
			sscanf((const char *)str,"%f",&tempf);
			SaveOPERA(OPERA_LEVEL_MIN,&dev_SavePar.level_min,&tempf);
			dev_SavePar.level_min=tempf;
			Flash_SaveDevData();
		}
		if(control_id==24) //数据间隔		//存储运行数据间隔
		{
			sscanf((const char *)str,"%f",&tempf);
			SaveOPERA(OPERA_DATA_INTERVAL,&dev_SavePar.data_interval,&tempf);
			dev_SavePar.data_interval=tempf;
			Flash_SaveDevData();
		}
		if(control_id==14) //采集间隔		//关闸时采集后水位和超声波的间隔
		{
			sscanf((const char *)str,"%f",&tempf);
			SaveOPERA(OPERA_COLLECT_INTERVAL,&dev_SavePar.collect_interval,&tempf);
			dev_SavePar.collect_interval=tempf;
			Flash_SaveDevData();
		}
		
		if(control_id==25) //最大流量
		{
			sscanf((const char *)str,"%f",&tempf);
			SaveOPERA(OPERA_FLOW_MAX,&dev_SavePar.flow_max,&tempf);
			dev_SavePar.flow_max=tempf;
			Flash_SaveDevData();
		}
		if(control_id==26) //最大水位
		{
			sscanf((const char *)str,"%f",&tempf);
			SaveOPERA(OPERA_LEVEL_MAX,&dev_SavePar.level_max,&tempf);
			dev_SavePar.level_max=tempf;
			Flash_SaveDevData();
		}
		if(control_id==27) //状态间隔			//发送运行数据给云平台
		{
			sscanf((const char *)str,"%f",&tempf);
			SaveOPERA(OPERA_SEND_INTERVAL,&dev_SavePar.send_interval,&tempf);
			dev_SavePar.send_interval=tempf;
			Flash_SaveDevData();
		}
		
		if(control_id==28) //流量偏差
		{
			sscanf((const char *)str,"%f",&tempf);
			SaveOPERA(OPERA_FLOWDEVIATION,&dev_SavePar.flow_deviation,&tempf);
			dev_SavePar.flow_deviation=tempf;
			Flash_SaveDevData();
		}
		if(control_id==29) //水位偏差
		{
			sscanf((const char *)str,"%f",&tempf);
			SaveOPERA(OPERA_LEVEL_DEVIATON,&dev_SavePar.level_deviation,&tempf);
			dev_SavePar.level_deviation=tempf;
			Flash_SaveDevData();
		}
		
		if(control_id==37) //边壁系数
		{
			sscanf((const char *)str,"%f",&tempf);
			SaveOPERA(OPERA_SIDE_K,&dev_SavePar.Side_K,&tempf);
			dev_SavePar.Side_K=tempf;
			Flash_SaveDevData();
		}
		if(control_id==38) //修正系数
		{
			sscanf((const char *)str,"%f",&tempf);
			SaveOPERA(OPERA_CORRECT_K,&dev_SavePar.Correct_K,&tempf);
			dev_SavePar.Correct_K=tempf;
			Flash_SaveDevData();
		}
		if(control_id==39) //声道角度
		{
			sscanf((const char *)str,"%f",&tempf);
			SaveOPERA(OPERA_CHANNEL_ANGLE,&dev_SavePar.Channel_Angle,&tempf);
			dev_SavePar.Channel_Angle=tempf;
			Flash_SaveDevData();
		}
		if(control_id==40) //明渠糙率
		{
			sscanf((const char *)str,"%f",&tempf);
			SaveOPERA(OPERA_ROUGHNESS,&dev_SavePar.Roughness,&tempf);
			dev_SavePar.Roughness=tempf;
			Flash_SaveDevData();
		}
		if(control_id==48) //安全超高
		{
			sscanf((const char *)str,"%f",&tempf);
			SaveOPERA(OPERA_ROUGHNESS,&dev_SavePar.Roughness,&tempf);
			dev_SavePar.Safe_SpaceHigh=tempf;
			Flash_SaveDevData();
		}
		if(control_id==12) //定水位误差
		{
			sscanf((char*)str,"%f",&tempf);
			dev_SavePar.SetLevel_Dev=tempf;
			Flash_SaveDevData();
		}
		if(control_id==13) //定流量误差
		{
			sscanf((char *)str,"%f",&tempf);
			dev_SavePar.SetFlow_Dev=tempf;
			Flash_SaveDevData();
		}
	}
	
	if(screen_id==19) //外部设备
	{
		if(control_id==12) //输入设备编码
		{
			if(strlen(str) != 13)
			{
				SetTextValue(19,8,(u8 *)"输入设备编码不合法");
				SetTextValue(19,12,(u8 *)"");
				return ;
			}
			
			memcpy(nosave_par.Externdev_code,str,sizeof(nosave_par.Externdev_code));
		}
	}
}                                                                                

/*!                                                                              
*  \brief  进度条控件通知                                                       
*  \details  调用GetControlValue时，执行此函数                                  
*  \param screen_id 画面ID                                                      
*  \param control_id 控件ID                                                     
*  \param value 值                                                              
*/                                                                              
void NotifyProgress(uint16 screen_id, uint16 control_id, uint32 value)           
{  
      
}                                                                                

/*!                                                                              
*  \brief  滑动条控件通知                                                       
*  \details  当滑动条改变(或调用GetControlValue)时，执行此函数                  
*  \param screen_id 画面ID                                                      
*  \param control_id 控件ID                                                     
*  \param value 值                                                              
*/                                                                              
void NotifySlider(uint16 screen_id, uint16 control_id, uint32 value)             
{                                                             
    uchar back[1] = {0};
    if(screen_id==7&&control_id==2)                                                  //滑块控制
    {            
        if(value<100||value>0)                                                       
        {                                                                            
            SetProgressValue(7,1,value);                                             //更新进度条数值
            SetTextInt32(7,3,value,0,1); 
            sprintf(back,"%c",(255-value*2));                                        //设置背光亮度 背光值范围 0~255，0最亮，255最暗
            SetBackLight(back[0]);
        }
    }
    if(screen_id==7&&control_id==5)                                                  //滑块控制
    {                                                                              
        if(value<100||value>0)                                                       
        {                                                                            
            SetProgressValue(7,4,value);                                             //更新进度条数值
            SetTextInt32(7,6,value,0,1);                                             
        }
    }
}

/*! 
*  \brief  仪表控件通知
*  \details  调用GetControlValue时，执行此函数
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param value 值
*/
void NotifyMeter(uint16 screen_id, uint16 control_id, uint32 value)
{
    //TODO: 添加用户代码
}

/*! 
*  \brief  菜单控件通知
*  \details  当菜单项按下或松开时，执行此函数
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param item 菜单项索引
*  \param state 按钮状态：0松开，1按下
*/
void NotifyMenu(uint16 screen_id, uint16 control_id, uint8 item, uint8 state)
{
	if(identityJudge(ID_MAINT) != 1)
	{
		return	;
	}
	
	if(screen_id==0)
	{
		if(control_id==5)
		{
			if(state==0)
			{
				nosave_par.identity=item;
			}
		}
	}
	
	if(screen_id==5)
	{
		if(control_id==25 && state==1)
		{
			dev_SavePar.TypeLevel=item;
			Flash_SaveDevData();
		}
		
		if(control_id==28 && state==1)
		{
			if(item==0)			//测流槽
			{
				dev_SavePar.TypeUltra=0;
			}
			else if(item==1)	//巴歇尔槽
			{
				SetScreen(19);
			}
		}
	}
	
	
	
}

/*! 
*  \brief  选择控件通知
*  \details  当选择控件变化时，执行此函数
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param item 当前选项
*/
void NotifySelector(uint16 screen_id, uint16 control_id, uint8  item)
{
    
}

/*! 
*  \brief  定时器超时通知处理
*  \param screen_id 画面ID
*  \param control_id 控件ID
*/
void NotifyTimer(uint16 screen_id, uint16 control_id)
{
    if(screen_id==8&&control_id == 7)
    {
        SetBuzzer(100);
    } 
}

/*! 
*  \brief  读取用户FLASH状态返回
*  \param status 0失败，1成功
*  \param _data 返回数据
*  \param length 数据长度
*/
void NotifyReadFlash(uint8 status,uint8 *_data,uint16 length)
{
    //TODO: 添加用户代码
}

/*! 
*  \brief  写用户FLASH状态返回
*  \param status 0失败，1成功
*/
void NotifyWriteFlash(uint8 status)
{
    //TODO: 添加用户代码
}

/*! 
*  \brief  读取RTC时间，注意返回的是BCD码
*  \param year 年（BCD）
*  \param month 月（BCD）
*  \param week 星期（BCD）
*  \param day 日（BCD）
*  \param hour 时（BCD）
*  \param minute 分（BCD）
*  \param second 秒（BCD）
*/
void NotifyReadRTC(uint8 year,uint8 month,uint8 week,uint8 day,uint8 hour,uint8 minute,uint8 second)
{
    int years,months,weeks,days,hours,minutes,sec;
       
    sec    =(0xff & (second>>4))*10 +(0xf & second);                                    //BCD码转十进制
    years   =(0xff & (year>>4))*10 +(0xf & year);                                      
    months  =(0xff & (month>>4))*10 +(0xf & month);                                     
    weeks   =(0xff & (week>>4))*10 +(0xf & week);                                      
    days    =(0xff & (day>>4))*10 +(0xf & day);                                      
    hours   =(0xff & (hour>>4))*10 +(0xf & hour);                                       
    minutes =(0xff & (minute>>4))*10 +(0xf & minute);                                      
	lcd_rtc.secs=sec;
	lcd_rtc.years=years;
	lcd_rtc.months=months;
	lcd_rtc.days=days;
	lcd_rtc.hours=hours;
	lcd_rtc.minutes=minutes;
	memcpy(&g_rtc,&lcd_rtc,sizeof(g_rtc)); //存储时间
	DS3231_SetRTC(lcd_rtc);
//    SetTextInt32(8,1,years,1,1);
//    SetTextInt32(8,2,months,1,1);
//    SetTextInt32(8,3,days,1,1);
//    SetTextInt32(8,4,hours,1,1);
//    SetTextInt32(8,5,minutes,1,1);
//    SetTextInt32(8,6,sec,1,1);
//    if(weeks == 0)
//    {
//         SetTextValue(8,7,"星期日");
//    }
//    else if(weeks == 1)
//    {
//         SetTextValue(8,7,"星期一");
//    }
//    else if(weeks == 2)
//    {
//         SetTextValue(8,7,"星期二");
//    }
//    else if(weeks == 3)
//    {
//         SetTextValue(8,7,"星期三");
//    }
//    else if(weeks == 4)
//    {
//         SetTextValue(8,7,"星期四"); 
//    }
//    else if(weeks == 5)
//    { 
//         SetTextValue(8,7,"星期五");
//    }
//    else if(weeks == 6)
//    {
//         SetTextValue(8,7,"星期六");
//    }
}

