#include "task1.h"


#include "RTC.h"
#include "hmi_driver.h"
#include "cmd_queue.h"
#include "cmd_process.h"
#include "string.h"
#include "myOnenet.h"
#include "LCD.h"
#include "ff_user.h"
#include "TMC5160A.h"
#include "motor.h"
#include "AnglePulse.h"
#include "other_bsp.h"
#include "spi_sd.h"
#include "stdio.h"
#include "diskio.h"
#include "Lora.h"
#include "jiang.h"
#include "middleware.h"
#include "liu.h"
#include "sddata.h"
/***************************************************************************
变量声明
***************************************************************************/
 TaskHandle_t xHandleTask0 = NULL;
 TaskHandle_t xHandleTask1 = NULL;
 TaskHandle_t xHandleTask2 = NULL;
 TaskHandle_t xHandleTask3 = NULL;
 TaskHandle_t xHandleTask4 = NULL;
 TaskHandle_t xHandleTask5 = NULL;
/*
串口屏解析
*/
u32 a,b,c,d;
void BSP_init(void)
{
//	RTC_AlarmConfig();
	
	LCD_All_init();
	RS485_init(9600);
	MOS_init();			//通过风扇接头控制升压模块开关
	
	Ultra_All_init();   
	
	DS3231_init();
	
	//开启独立看门狗
	gd32_wdgt_init();
	
	//120MHZ时钟源
	timer1_config();		//定时1S
	timer2_config(1199,9999);		//配置成0.1s  100ms
	timer3_config();		//定时1S
	
	dev_SavePar._4G_Fun=1;
	_4GCat1_init(115200);
	L76K_init(9600);    
//	motor_dirver();
	TMC5160_SPIMode_init();
	motor_limit();
	AnglePulse_init();	   //双向霍尔
	
	SD_init();
	
	Lora_init(); //lora初始化
	
	Float_Ball_init();			

	GetAllCSVrows();
	
	power_adc_init2();

//	Bluetooth_Init();

}
void StartTask(void *pvParameters)
{
//	W25QXX_Erase_Chip();
	
	Flash_ResumeAll();			//存储在flash中数据进行读取赋值
	BSP_init();
	
	//参数设置  只在第一次烧录代码存在、再烧录时注释
//	myOnenet_infoSet();		
//	parameter_set();		//第一次上电时赋初始值,后再上电由于标志位被置位不再赋值			
	
	xSemaphoreGive(LCD_semTx);       
	xSemaphoreGive(SD_SemSR);       
	xSemaphoreGive(Flash_SemSR);
	Calc_Opening();
	
	//请求设备编码
//	nosave_par.Devcode_SetF=1;
//	Set_DevCode();

	LCD_POWER_EN=1;
	nosave_par.LCD_timecount=LCD_SHOWTIME;

	while(1)
	{
		AppTaskCreate();
		vTaskDelete(xHandleTask0);
		vTaskDelay(5000);
	}
}
void vTask1(void *pvParameters)
{
	static u16 readRTC_Count=1000;
	SEGGER_RTT_printf(0,RTT_CTRL_TEXT_MAGENTA"vTask1 run\r\n");
	while(1) 
	{
		DS3231_GetRTC();     				 //获取时间
		CMD_Process();       				 //指令解析
		TMC5160A_TestStep2();				 //调试中计算总步数
		if(nosave_par.save_Mess_Par_flag==1) //存储步数
		{
			Flash_SaveMess_Par();
			nosave_par.save_Mess_Par_flag=0;
		}
		readRTC_Count++;
		if(readRTC_Count>1000)
		{
			readRTC_Count=0;
//			ReadRTC();
		}
		a=uxTaskGetStackHighWaterMark(NULL);
		vTaskDelay(50);
	}
}
/*
串口屏更新
*/
//    特别注意
//    MCU不要频繁向串口屏发送数据，否则串口屏的内部缓存区会满，从而导致数据丢失(缓冲区大小：标准型8K，基本型4.7K)
//    1) 一般情况下，控制MCU向串口屏发送数据的周期大于100ms，就可以避免数据丢失的问题；
//    2) 如果仍然有数据丢失的问题，请判断串口屏的BUSY引脚，为高时不能发送数据给串口屏。

//    TODO: 添加用户代码
//    数据有更新时标志位发生改变，定时100毫秒刷新屏幕
float testasfsf;
u8 readRTC_Count=100;
void vTask2(void *pvParameters)
{
	SEGGER_RTT_printf(0,RTT_CTRL_TEXT_MAGENTA"vTask2 run\r\n");

	current_screen_id=1;
	delay_ms(2000);
	while(1)
	{
		Power_ADC_Read(); //电压
		UpdateUI();                               //更新界面
		
		readRTC_Count++;                       
		if(readRTC_Count>100)
		{
			readRTC_Count=0;
			ReadRTC();
		}
		b=uxTaskGetStackHighWaterMark(NULL);
		vTaskDelay(500);
	}
}
/*
4G通信
*/
void vTask3(void *pvParameters)
{
	s8 res;
	SEGGER_RTT_printf(0,RTT_CTRL_TEXT_MAGENTA"vTask3 run\r\n");
	
	res=PowerUpGSM();			//开机  是否插卡 读取sim卡号
	if(res == 0)
	{
		InitSAPBR();			//注册网络
		InitNTP();				//同步网络时间  查询时间
		GPRS_connect2(onenet_info.ip,onenet_info.port);		//连接onenet平台  订阅主题
	}
	
	GPRS_LowPower();			//5s进入睡眠
	_4GCat_CloseMSUB();	
	
	nosave_par.checkNetflag=1;	  //开机先检测一次连接状态
	nosave_par.checkupflag=1;     //检查版本升级
	nosave_par.SendVerflag=1;     //发送版本信息
	
	nosave_par.Author_Par_flag=1; //发送权限信息
	nosave_par.dev_Save_flag=1;//发送存储信息
	nosave_par.dataUpload_flag=1; //发送运行数据
	
	while(1)
	{
		c=uxTaskGetStackHighWaterMark(NULL);
		
//		OneNET_CmdHandle2();     //接受指令解析   在接收到指令后才解析
		
		/*
			在Air780EG心跳包之前  且   在上传任何数据之前
		*/
		if(nosave_par.checkNetflag==1) 		//4min查一次  开机查一次
		{
			GPRS_LowPower_wake();
			
			myOnenet_CheckLink();
			nosave_par.checkNetflag=0;
		}
		while(Statlog_Par._4G!=1)  			//如果连接失败，则重新连接
		{
			GsmShutDown();
			res=PowerUpGSM();
			if(res==0)
			{
				InitSAPBR();
				InitNTP();
				GPRS_connect2(onenet_info.ip,onenet_info.port);
				
				myOnenet_CheckLink();
			}
		}
		
		//是否需要发送运行数据  闸门关 6h  闸门开15min	开机发一次
		if(nosave_par.dataUpload_flag==1 && nosave_par.data_OK==1) 	
		{
			GPRS_LowPower_wake();		//防止模块睡眠
			
			OneNET_SendRun_Par();
			nosave_par.dataUpload_flag=0;
		}
		
		//是否需要发送版本号 1小时 开机发一次
		if(nosave_par.SendVerflag==1)  
		{
			GPRS_LowPower_wake();
			
			ReportVersion();
			nosave_par.SendVerflag=0;
		}
		//如果检查升级标志位成立	5分钟 开机发一次
		if(nosave_par.checkupflag==1)   
		{
			GPRS_LowPower_wake();
			
//			_4GCat_CloseMSUB();
			CheckUpdate(); 		//HTTPREAD
//			_4GCat_PrintMSUB();
			if(strlen(Update_Par.tid)!=0)
			{
				SEGGER_RTT_SetTerminal(0); 
				SEGGER_RTT_printf(0,RTT_CTRL_TEXT_BLUE"need Update!\r\n");
				myota_info.Update_flag=1;		//未使用
				Flash_Savemyota_info();
				Flash_SaveUpdate_Par();
				__set_FAULTMASK(1); // 关闭所有中端
				NVIC_SystemReset(); // 复位
			}
			nosave_par.checkupflag=0;
		}
		vTaskDelay(500);
		continue;
		
		if(nosave_par.dev_Save_flag==1)   //是否发送存储信息
		{
			OneNET_Sendev_SavePar();
			nosave_par.dev_Save_flag=0;
		}
		
		if(nosave_par.SendControlflag==1) //发送控制
		{
			nosave_par.SendControlflag=0;
			OneNET_SenControl_Par();
		}
		if(nosave_par.Author_Par_flag==1) //发送权限信息
		{
			nosave_par.Author_Par_flag=0;
			OneNET_SenAuthor_Par();
		}
		vTaskDelay(500);
	}
}
/*
传感器读取
*/
s8 test1;
u8 debug_t1=0;
u32 lora_send;
u32 lora_rece;
void vTask4(void *pvParameters)
{
	SEGGER_RTT_printf(0,RTT_CTRL_TEXT_MAGENTA"vTask4 run\r\n");
	
	nosave_par.Read_ExterndevF=1;		//读取外部设备标志位
	nosave_par.RunLog_ParSave_flag=1;	//存储数据标志位
	nosave_par.Read_AftLF=1;			//读取后水位标志位
	
//	memcpy(dev_SavePar.Externdev_code,"test4",sizeof("test4"));
//	dev_SavePar.TypeUltra = 1;
	while(1)
	{
		//读取巴歇尔槽的水量数据
		liu_Lora_ReadTerminal(dev_SavePar.Externdev_code,EXTERNDEV_TYPE);
		
		RS485_ReadLevel();  //读取水位传感器数据
		Ultra_total();		//超声波读取
		
		vTaskDelay(200);
		Lora_Process();
		
		nosave_par.data_OK=1;
		if(nosave_par.time_OK!=1||g_rtc.years==0)
		{
			
		}
		else
		{
			Calc_Wat_head();	//根据前后水位计算水头
			CalcAccmuWater();   //测流槽计算流量		巴歇尔槽跳过

			nosave_par.flow_ok=1;		//未使用
			RunLog_Par_make();			//运行数据赋值		lora通讯时间<数据间隔
			if(nosave_par.RunLog_ParSave_flag)      //CSV_TESTLOG存储历史运行数据	上电存一次,后按照时间间隔存-300秒
			{
				nosave_par.RunLog_ParSave_flag=0;
				SD_SaveFlowLog();
			}
		}
		vTaskDelay(500);
	}
}

DSTATUS sd_status;
void vTask5(void *pvParameters)
{
	SEGGER_RTT_printf(0,RTT_CTRL_TEXT_MAGENTA"vTask5 run\r\n");
	//L76K_Start();
	nosave_par.L76K_SendF=1;
	float tempf;
	
	while(1)
	{
	//	Lora_Send_StartupMess();
		
		//堵转处理  
		TMC5160_ReadTSTEP_Value();		//读取电机芯片寄存器值  是否到达指定位置
		TMC5160_StallGuardProcess(); 	//堵转处理	
		
		motor_control1();        		//电机运行控制
		
		L76K_LowPower();
		
		memcpy(&lora_rtc,&g_rtc,sizeof(lora_rtc));  //时间
		
//		Bt_SendWatLev();		//蓝牙发送后水位
		vTaskDelay(500);
	}
}

 void AppTaskCreate (void)
{
  xTaskCreate( vTask1, "vTask1",1200, NULL,1, &xHandleTask1);
  xTaskCreate( vTask2, "vTask2",1000, NULL,1, &xHandleTask2);
  xTaskCreate( vTask3, "vTask3",2000,  NULL,1, &xHandleTask3);
  xTaskCreate( vTask4, "vTask4",1000,  NULL,4, &xHandleTask4);
  xTaskCreate( vTask5, "vTask5",1200,  NULL,1, &xHandleTask5);
}


//configCHECK_FOR_STACK_OVERFLOW=1
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName )
{
	u8 a;
	a=0;
}






