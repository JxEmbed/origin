#include "4GCat1.h"
#include "string.h"
#include "onenet.h"
#include "stdio.h"
#include "MqttKit.h"
#include "cJSON.h"
#include "ota.h"
#include "myOTA.h"
#include "md5.h"
#include "Authorization.h"
#include "myOnenet.h"
/*
Air780E
*/

char gprxBuffer[300];	
u8 sms_SendBuffer[SMS_BUFFER_SIZE];		// 短信接收缓存
u8 sms_ResaveBuffer[SMS_BUFFER_SIZE];	// 短信发送缓存
S_UART _4GCat1_Usart;
char prxBuffer[UART_BUFFER_LEN];	
/****************************************************************
发送数据时的临时变量
*/
char chbuff[100];
char chbuff2[100];
char tempbuff[300];
char authorization_buf[160];
char rangeStr[100];   //0-99;
char strtemp[200];
//char test[300];
char cmdtopic[150];
/****************************************************************/
u32 _4G_Usart_Rxlen=0;
/*
4G模块初始化
	//PWR_4G
	//RXD_4G
	//TXD_4G
	//EN_4G
	//RI_4G
	//DTR_4G
*/
void TIM1_Config()
{
	/* ----------------------------------------------------------------------------
    TIMER1 Configuration:
    TIMER1CLK = SystemCoreClock/12000 = 10KHz, the period is 1s(10000/10000 = 1s).
	//120000000/12000=10000  10000/10=1000hz
    ---------------------------------------------------------------------------- */
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER1);

    timer_deinit(TIMER1);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER1 configuration */
    timer_initpara.prescaler         = 11999;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 599;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER1, &timer_initpara);

    /* enable the TIMER interrupt */
    timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
    timer_interrupt_enable(TIMER1, TIMER_INT_UP);
	nvic_irq_enable(TIMER1_IRQn,0U, 0U);
    timer_enable(TIMER1);
}
void UART7_Config(uint32_t baudval)
{
	/* 初始化GPIO外设 */
    rcu_periph_clock_enable(RCU_GPIOE);
    /* TX管脚，PA9，复用推挽输出，速度50MHz */
    gpio_init(GPIOE, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
    /* RX管脚，PA10，下拉输入，速度50MHz */
    gpio_init(GPIOE, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, GPIO_PIN_0);

    /* 初始化USART外设 */
    rcu_periph_clock_enable(RCU_UART7);  // 使能串口0时钟
		usart_deinit(UART7);
    usart_baudrate_set(UART7, baudval);  // 波特率115200
    usart_parity_config(UART7, USART_PM_NONE);  // 无校检
    usart_word_length_set(UART7, USART_WL_8BIT);  // 8位数据位
    usart_stop_bit_set(UART7, USART_STB_1BIT);  // 1位停止位
	usart_hardware_flow_rts_config(UART7, USART_RTS_DISABLE); /* 禁用rts */
    
    usart_hardware_flow_cts_config(UART7, USART_CTS_DISABLE); /* 无硬件数据流控制 */

    usart_transmit_config(UART7, USART_TRANSMIT_ENABLE);  // 使能串口发送
    usart_receive_config(UART7, USART_RECEIVE_ENABLE);  // 使能串口接收
    usart_enable(UART7);  // 使能串口
		
	nvic_irq_enable(UART7_IRQn,0U, 0U);
	usart_interrupt_enable(UART7, USART_INT_RBNE);  /* 参数：读数据缓冲区非空中断和过载错误中断 */
//	usart_interrupt_enable(UART7, USART_INT_IDLE);  /* 参数：读数据缓冲区非空中断和过载错误中断 */
//	usart_interrupt_disable(UART7, USART_INT_TBE);  /*发送器缓冲区空中断 */
}	
void _4GCat_Gpio_Config(void)
{
	//4G模块开机/关机控制脚初始化  --PWR_4G
	rcu_periph_clock_enable(RCU_GPIOF);
	gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
	//4G模块供电控制脚初始化  --EN_4G
	rcu_periph_clock_enable(RCU_GPIOC);
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);	
	//Reset引脚	PE4
	rcu_periph_clock_enable(RCU_GPIOE);
	gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
		//DTR_4G PE3		模块唤醒管脚  拉低触发中断(默认高)
//	gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
	/*
		过 AT+CFGRI 指令设置 URC 上报提醒，当 AT+CFGRI=1 时，网络 URC 来到时，也会产生 120ms 低脉冲
		待机状态下：为高电平
	*/		
	//RI_4G  PE2
	gpio_init(GPIOE, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOE, GPIO_PIN_SOURCE_2);
	exti_init(EXTI_2, EXTI_INTERRUPT, EXTI_TRIG_RISING);
	exti_interrupt_flag_clear(EXTI_2);
	
	nvic_irq_enable(EXTI2_IRQn, 2U, 0U);
	
	_4GCat1_POWEREN=1;
	_4GCat1_PWRKEY=1;
	_4GCat1_RESET=0;
}

void _4GCat1_init(uint32_t BaudRate)
{
	
	
	/*
	AT 固件功能管脚，拉高允许模块进入休眠模式；在休眠模
	式下，拉低可唤醒模块
	*/
	//RXD_4G
	//TXD_4G
	//EN_4G
	//RI_4G
//4G串口初始化
//	TIM1_Config();
	UART7_Config(115200);
	_4GCat_Gpio_Config();
}
u8 testa=0;
//打开GSM
u8 StartGSM(void)
{ 
	_4GCat1_POWEREN=0; //通电
//	delay_ms (300);  
//	_4GCat1_PWRKEY=1; //按下开机键
//	delay_ms (500);
//	_4GCat1_PWRKEY=0; 
	testa++;
	
}
u8 GsmShutDown(void)
{  
	u8 i=0;
	_4GCat1_POWEREN=1; //断电
	delay_ms (1500);
	return 1;
}

s8 PowerUpGSM(void)
{
	s8 result=0;
	char *pRet=0;
	u8 i=0;
	char tempc[30];
	//开机
	do
	{
		StartGSM();
	}while(testGSM()!=0);
	
	//关闭命令回显  只回复最终响应
	for(i=0;i<5;i++)
	{
		pRet=SendATCommand("ATE0\r","OK",50); 
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	
	//未插卡DET引脚为低电平   插卡DET引脚为高电平
	//检测SIM卡是否插入
//	for(i=0;i<5;i++)
//	{
//		pRet=SendATCommand("AT*SIMDETEC=1\r",",SIM",50); 	
//		if(pRet!=NULL)
//		{
//			Statlog_Par._sim=1;
//			break;
//		}
//		else if(i==4)
//		{
//			Statlog_Par._sim=2;		//未插卡
//			Statlog_Par._4G=2;		//故障--->未插卡
//			return -1;
//		}
//		delay_ms(500);
//	}
	
	//查询SIM卡ICCID号
	for(i=0;i<5;i++)
	{
		pRet=SendATCommand("AT+ICCID\r","+ICCID",50); 
		if(pRet!=NULL)
		{
			Statlog_Par._sim=1;			//查询到卡号
			sscanf((const char *)pRet,"+ICCID: %s\r",dev_SavePar.sim_number);
			break;
		}
		else if(i==4)
		{
			Statlog_Par._sim=4;		//查询不到SIM卡号
			Statlog_Par._4G=2; 		//故障--->查询不到SIM卡号
			return -1;
		}
		delay_ms(500);
	}
	
	//查询网络附着状态   防止SIM已被绑定
	for(i=0;i<5;i++)
	{
		pRet=SendATCommand("AT+CGATT?\r","+CGATT: 1",50); 
		if(pRet!=NULL)
		{
			break;
		}
		else if(i==4)
		{
			Statlog_Par._sim=3;		//无网络附着
			Statlog_Par._4G=2; 		//故障--->无网络
			return -1;
		}
		delay_ms(500);
	}
	
	return result;
}
u8 testde=0;
u8 testGSM(void)
{
	char *pRet=0;
	u8 i=0;
	for(i=0;i<5;i++)
	{
		_4GCat1_read((u8 *)prxBuffer,300,1);  //?????????????????  
		testde++;
	}
	delay_ms(100);
	pRet=SendATCommand("AT\r","OK",50);
	delay_ms(100);
	pRet=SendATCommand("AT\r","OK",50);
	delay_ms(100);
	pRet=SendATCommand("AT\r","OK",50);
	if(pRet==0)
	{
		return 1;
	}
	else 
	{
		return 0;
	}
	
}
char *pRet=0;
u8 setCat1Sleep(void)
{
	delay_ms (500);
	pRet=SendATCommand("AT+CSCLK=2\r","OK",20);
	
	
	delay_ms (500);	
	pRet=SendATCommand("AT+CSCLK?\r","OK",8);   //查询睡眠
}
u8 testGSM1(void)
{
	
	pRet=SendATCommand("AT\r","OK",20);

	delay_ms (500);	

	pRet=SendATCommand("AT\r","OK",20);

	delay_ms (500);	

	pRet=SendATCommand("AT\r","OK",20);
	delay_ms (500);	
	pRet=SendATCommand("AT\r","OK",5);
	delay_ms(500);
	pRet=SendATCommand("ATE0\r","OK",8);        //关闭回显模式
	delay_ms (500);	
	pRet=SendATCommand("AT+CMEE=1\r","OK",8);   //启用设备错误结果码+ CME ERROR: <err>，并使用数字型的<err>取值
	delay_ms(500);
//	pRet=SendATCommand("ATD15703286965\r","OK",8); //只适用于CAT4
	pRet=SendATCommand("AT+CSMS=1\r","OK",8); //选择短消息服务
	delay_ms(500);
	pRet=SendATCommand("AT+CNMI=2,1,0,0,1\r","OK",8); 
	delay_ms(500);
	SendATCommand("AT+CIPSHUT\r","OK",5);        //关闭移动场景
	return 0;
	pRet=SendATCommand("AT+CSCLK=0\r","OK",8);  //关闭睡眠
	delay_ms (500);	
	pRet=SendATCommand("AT+CSCLK?\r","OK",8);   //查询睡眠
	delay_ms(500);
	pRet=SendATCommand("AT+CGMI\r","AT+CGMI",8); //查询制造商名称
	delay_ms(500);
	pRet=SendATCommand("AT+CGMM\r","AT+CGMM",8); //查询模块型号
	delay_ms(500);
	pRet=SendATCommand("AT+CGMR\r","AT+CGMR",8); //查询模块版本信息
	delay_ms(500);
	pRet=SendATCommand("AT+CGSN\r","AT+CGSN",8); //查询IMEI号
	delay_ms(500);
	pRet=SendATCommand("AT+CCID\r","AT+CCID",8); //查询 SIM 卡 ICCID 号码：AT+CCID(/ICCID)
	delay_ms(500);
	pRet=SendATCommand("AT+CIMI\r","AT+CIMI",8); //查询 IMSI：AT+CIMI
	delay_ms(500);
	pRet=SendATCommand("ATI\r","ATI",8);         //查询产品信息：ATI
	delay_ms(500);
	pRet=SendATCommand("AT+VER\r","AT+VER",8);  //查询模块 FIRMWARE 版本：AT+VER
	delay_ms(500);
	pRet=SendATCommand("AT^HVER\r","AT^HVER",8); //查询平台硬件版本：AT^HVER
	delay_ms(500);
	pRet=SendATCommand("AT*I\r","AT*I",8);       //查询各种信息：AT*I
	delay_ms(500);
	pRet=SendATCommand("AT+CFUN=0\r","AT+CFUN",8); //设置功能模式
	delay_ms(500);
	pRet=SendATCommand("AT+IPR?\r","AT+IPR?",8);   //查询波特率
	delay_ms(500);
//	pRet=SendATCommand("AT+CSCLK=2\r","AT+CSCLK=2",8);  //设置睡眠
//	delay_ms(500);
//	pRet=SendATCommand("AT+WAKETIM?\r","AT+WAKETIM?",8);  //查询进入睡眠的时间
//	delay_ms(500);
//	pRet=SendATCommand("AT+WAKETIM=8\r","AT+WAKETIM=8",8); //设置进入睡眠的时间
//	delay_ms(500);
	pRet=SendATCommand("AT+CADC?\r","AT+CADC",8);    //读取ADC
	delay_ms(500);
	pRet=SendATCommand("AT+CBC\r","AT+CBC",8);       //读取VBAT电压
	delay_ms(500);
	
}
u8 _4GCat1_GPS(void)
{
	char *pRet=0;
	pRet=SendATCommand("AT+CGNSPWR?\r","OK",8);       //读取VBAT电压
	pRet=SendATCommand("AT+CGNSPWR=1\r","OK",8);       //读取VBAT电压
	//AT+CGNSAID=31,1,1,1
	pRet=SendATCommand("AT+CGNSAID=31,1,1,1\r","OK",8);       //读取VBAT电压
	while(1)
	{
		delay_ms(2000);
		pRet=SendATCommand("AT+CGNSINF\r","OK",8);       //读取VBAT电压
		//AT+CGNSINF
	}
}
/*
启动多链接模式

*/
u8 MuxLinkMode(void)
{
	u8 i=0;
	//设置为多链接模式
	while(1)   
	{
		pRet=SendATCommand("AT+CIPMUX=1\r","OK",50);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	//AT+CIPQSEND=1
	while(1)   
	{
		pRet=SendATCommand("AT+CIPQSEND=1\r","OK",50);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	//AT+CSTT
	while(1)   
	{
		pRet=SendATCommand("AT+CSTT\r","OK",50);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	//AT+CIICR
	while(1)   
	{
		pRet=SendATCommand("AT+CIICR\r","OK",50);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
		//AT+CIFSR
	for(i=0;i<3;i++) 
	{
		pRet=SendATCommand("AT+CIFSR\r","OK",50);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
}

u8 GPRS_connect2(char* IP,char* port)
{
	char *pRet=0;
	u8 i=0;

	for(i=0;i<3;i++)
	{
		pRet=SendATCommand("AT+CGATT?\r","+CGATT: 1",50); //查询网络附着状态
		if(pRet!=NULL)
		{
			break;
		}
		SEGGER_RTT_printf(0,"%s\r\n",pRet);
		delay_ms(500);
	}

	/*
		启动任务并设置接入点APN、用户名、密码：AT+CSTT
		直接输入AT+CSTT即可，模块会按照自动获取的<apn>来设置CSTT的APN
	*/
	for(i=0;i<3;i++)
	{
		pRet=SendATCommand("AT+CSTT\r","OK",50); 
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	//激活移动场景(或发起GPRS或CSD无线连接)：AT+CIICR
	for(i=0;i<3;i++)
	{
		pRet=SendATCommand("AT+CIICR\r","OK",50); 
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	//查询本地IP地址：AT+CIFSR
	for(i=0;i<3;i++)
	{
		pRet=SendATCommand("AT+CIFSR\r",".",50); //查询本地IP地址
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	
	//查询时间
	for(i=0;i<3;i++)
	{
		pRet=SendATCommand("AT+CCLK?\r","+CCLK:",50); //设置使用的PDP的<cid>=1
		if(pRet!=NULL)
		{
			_4G_rtc.years=(*(pRet+8)-0x30)*10+(*(pRet+9)-0x30);
			_4G_rtc.months=(*(pRet+11)-0x30)*10+(*(pRet+12)-0x30);
			_4G_rtc.days=(*(pRet+14)-0x30)*10+(*(pRet+15)-0x30);
			_4G_rtc.hours=(*(pRet+17)-0x30)*10+(*(pRet+18)-0x30);
			_4G_rtc.minutes=(*(pRet+20)-0x30)*10+(*(pRet+21)-0x30);
			_4G_rtc.secs=(*(pRet+23)-0x30)*10+(*(pRet+24)-0x30);
			DS3231_SetRTC(_4G_rtc);
//			sscanf(pRet,"+CCLK: \"%2d/%2d/%2d,%2d:%2d:%2d+",(int*)&_4G_rtc.years,(int*)&_4G_rtc.months,(int*)&_4G_rtc.days,(int*)&_4G_rtc.hours,(int*)&_4G_rtc.minutes,(int*)&_4G_rtc.secs);
			break;
		}
		delay_ms(500);
	}
	//计算鉴权  onenet_info.dev_name, onenet_info.pro_id, onenet_info.key
	OneNET_Authorization_V1((char *)onenet_info.pro_id,(char *)onenet_info.key, (char *)onenet_info.dev_name,
								authorization_buf, sizeof(authorization_buf), 0);
	strcpy(tempbuff,"AT+MCONFIG=\"");
	strcat(tempbuff,onenet_info.dev_name);
	strcat(tempbuff,"\",\"");
	strcat(tempbuff,onenet_info.pro_id);
	strcat(tempbuff,"\",\"");
	strcat(tempbuff,authorization_buf);
	strcat(tempbuff,"\"\r");
	
	for(i=0;i<3;i++)
	{
//		_4GCat1_write(test,strlen(test));
		pRet=SendATCommand(tempbuff,"OK",200);
//		SendString("AT+MCONFIG=\"test1\",\"290724\",\"version=2018-10-31&res=products%2F731727251%2Fdevices%2Ftest1&et=1956499200&method=md5&sign=%2BCTFCZVXCWZv%2BrxaByQfXA%3D%3D\"\r");
//	
		//pRet=SendATCommand("AT+MCONFIG=\"86873905\",\"q23GT\",\"version=2018-10\"\r","OK",50);
		if(pRet!=NULL)
		{
			break;
		}
//		pRet=SendATCommand("AT+MCONFIG=?\r","OK",5);
		delay_ms(500);
//		pRet=SendATCommand("AT+MCONFIG=?\r","OK",50);
		delay_ms(500);
		
	}
	
	//建立TCP连接：AT+MIPSTART
	for(i=0;i<3;i++)
	{
		pRet=SendATCommand("AT+MIPSTART=\"183.230.40.96\",1883\r","OK",200);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	//客户端向服务器请求会话连接：AT+MCONNECT
	for(i=0;i<3;i++)   
	{
		pRet=SendATCommand("AT+MCONNECT=1,360,1\r","CONNACK OK",50);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	
	for(i=0;i<3;i++)
	{
		sprintf(tempbuff,"AT+MSUB=\"$sys/%s/%s/dp/post/json/accepted\",%d\r",onenet_info.pro_id, onenet_info.dev_name,MQTT_QOS_LEVEL1);
		
		pRet=SendATCommand(tempbuff,"SUBACK",50);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	for(i=0;i<3;i++)
	{
		sprintf(tempbuff,"AT+MSUB=\"$sys/%s/%s/dp/post/json/rejected\",%d\r",onenet_info.pro_id,onenet_info.dev_name,MQTT_QOS_LEVEL1);
		pRet=SendATCommand(tempbuff,"SUBACK",50);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
/****************此主题只有物模型才有********************/
	//$sys/{pid}/{device-name}/ota/inform
	//$sys/{pid}/{device-name}/ota/inform
//	for(i=0;i<3;i++)
//	{
//		sprintf(tempbuff,"AT+MSUB=\"$sys/%s/%s/ota/inform\",%d\r",onenet_info.pro_id, onenet_info.dev_name,MQTT_QOS_LEVEL1);
////		sprintf(tempbuff,"AT+MSUB=\"$sys/%s/%s/ota/inform\",%d\r",onenet_info.pro_id, onenet_info.dev_name,MQTT_QOS_LEVEL1);
//		pRet=SendATCommand(tempbuff,"SUBACK",50);
//		if(pRet!=NULL)
//		{
//			break;
//		}
//		delay_ms(500);
//	}
/*******************************************************/
	for(i=0;i<3;i++)
	{
		sprintf(tempbuff,"AT+MSUB=\"$sys/%s/%s/cmd/#\",%d\r",onenet_info.pro_id,onenet_info.dev_name,MQTT_QOS_LEVEL0);
		pRet=SendATCommand(tempbuff,"SUBACK",50);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	//$sys/{pid}/{device-name}/ota/inform
	//$sys/%s/%s/dp/post/json/accepted\"
	
	//发布消息：AT+MPUB
	for(i=0;i<3;i++)
	{
		pRet=SendATCommand("AT+MPUB=?\r","OK",50);		//测试发布信息
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	return 0;
}
/**********************************************____________________________________
 string deftcpIP="47.117.126.245";
        string deftcpPort="8000";
TCP/UDP测试网页TCP/UDP测试网页: https://netlab.luatos.com/
112.125.89.8:46619
*/
u8 GPRS_connectTCP(char* IP,char* port)
{
	char *pRet=0;
	u8 i=0;
//	char test[300];
//	char authorization_buf[160];
//	char cmdtopic[150];

	for(i=0;i<3;i++)   
	{
		pRet=SendATCommand("AT+CREG?\r","+CREG: 0,1",50);//网络注册信息
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	for(i=0;i<3;i++)  
	{
		pRet=SendATCommand("AT+CGATT?\r","+CGATT: 1",50);//查看当前GPRS附着状态
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}

	for(i=0;i<3;i++)  
	{
		pRet=SendATCommand("AT+CIPQSEND=1\r","OK",50);//设置为快发模式（推荐使用这种模式）
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	
	//AT+CIPSTATUS
	for(i=0;i<3;i++)  
	{
		pRet=SendATCommand("AT+CIPSTATUS\r","OK",50);//查询下链接状态
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	//AT+CIPSTART="TCP","60.166.18.9",7500
	for(i=0;i<3;i++)  
	{
		strcpy(tempbuff,"AT+CIPSTART=");   //建立 TCP 连接或注册 UDP 端口号：AT+CIPSTART
		strcat(tempbuff,"\"TCP\",\"");     //"TCP","IP","port"\r
		strcat(tempbuff,IP);
		strcat(tempbuff,"\",\"");
		strcat(tempbuff,port);
		strcat(tempbuff,"\"\r");  
		pRet=SendATCommand(tempbuff,"OK",50);//查询下链接状态
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	//AT+CIPSTATUS
	for(i=0;i<3;i++)  
	{
		pRet=SendATCommand("AT+CIPSTATUS\r","OK",50);//查询下链接状态
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	
}	
/*
关闭TCP连接
*/	

void CloseTCP(void)
{
	u8 i=0;
	//AT+CIPCLOSE
	for(i=0;i<3;i++)
	{
		pRet=SendATCommand("AT+CIPCLOSE\r","OK",50);//查询下链接状态
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
}
/*
解包
*/

u8 _4GCat1_UnPacketRecv(u8 *dataPtr,char * cmdtopic)
{
	char  * tempPtr1;
	char  * tempPtr2;
	char tempBuff1[150]={0};
	tempPtr1=strstr((char *)dataPtr,"+MSUB");
	if(tempPtr1==NULL)
	{
		return 1;
	}
	else 
	{
		//+MSUB: "$sys/290724/test1/cmd/request/2496930a-50e2-4474-8abb-20dc988bda53",1 byte,2
		tempPtr2=strchr(tempPtr1,'\"'); //查找引号
		tempPtr2++;		
		tempPtr1=strchr(tempPtr2,'\"'); //查找下一个引号
		if(tempPtr1==NULL||tempPtr2==NULL)
		{
			return 2;
		}
		else
		{
			if(strstr(tempPtr2,"cmd/response")!=NULL)
			{
				return 3;
			}
			//0123=
			// 1  4
			memcpy(tempBuff1,tempPtr2,tempPtr1-tempPtr2); //复制出主题
			strcpy(cmdtopic,tempBuff1);
			
		}
	}
	return 0;

}
u8 _4GCat1_ChangeTopic(char * cmdtopic)
{
	char  * tempPtr1;
	char  * tempPtr2;
	char tempBuff[150]={0};
	// 012345+
	// 01    6
	// $sys/290724/test1/cmd/request/2496930a-50e2-4474-8abb-20dc988bda53
	//                       response
	tempPtr1=strstr(cmdtopic,"request");  //找到
	if(tempPtr1==NULL)
	{
		return 1;
	}
	memcpy(tempBuff,cmdtopic,tempPtr1-cmdtopic); //复制
	strcat(tempBuff,"response");
	strcat(tempBuff,tempPtr1+7);
	strcpy(cmdtopic,tempBuff);
	return 0;
}
u8 _4GCat1_PacketCmdResp(char *cmdid,u8 qos,u8 retain,char * mess)
{
	char tempBuff[150];
	//AT+MPUB=<topic>,1,<retain> ,<message>
	sprintf(tempBuff,"AT+MPUB=\"%s\",%d,%d,%s\r",cmdid,qos,retain,mess);
	_4GCat1_write((u8 *)tempBuff,strlen(tempBuff)); 
}
//发送GPRS数据（唤醒连接用）
u8 GPRS_connect(char* IP,char* port)
{
	char admin[100];
   	
	SendATCommand("AT\r","OK",5);

	SendATCommand("AT\r","OK",5);

	SendATCommand("AT+CIPSHUT\r","OK",5); //关闭移动场景

	SendATCommand("AT\r","OK",5);

	SendATCommand("AT+CIPMUX=0\r","OK",5);  //关闭多 IP 连接

	SendATCommand("AT+CIPMODE=0\r","OK",5); //选择 TCPIP 应用模式 非透明传输模式
	pRet=SendATCommand("AT+CCID\r","AT+CCID",8); //查询 SIM 卡 ICCID 号码：AT+CCID(/ICCID)
	SendATCommand("AT+CREG?\r","OK",5);
	SendATCommand("AT+CFUN?\r","OK",5);
	SendATCommand("AT+CSQ\r","OK",5);

	//SendATCommand("AT+CIPCLOSE=1\r","CLOSE OK",2); //快速关闭 TCP 或 UDP 连接
//	SendATCommand("AT+CIPSTATUS\r","OK",5);
//	SendATCommand("AT+CSTT\r","OK",5);
//	SendATCommand("AT+CIPSTATUS\r","OK",5);
//	SendATCommand("AT+CIICR\r","OK",5);
//	SendATCommand("AT+CIPSTATUS\r","OK",5);
	strcpy(admin,"AT+CIPSTART=");   //建立 TCP 连接或注册 UDP 端口号：AT+CIPSTART
	strcat(admin,"\"TCP\",\"");     //"TCP","IP","port"\r
	strcat(admin,IP);
	strcat(admin,"\",\"");
	strcat(admin,port);
	strcat(admin,"\"\r");  

	memset(prxBuffer,0,300);
	SendATCommand(admin,"OK",20);
	
	if(strstr(prxBuffer,"CONNECT OK"))   
	{
		return 1;
	}
	else if(GPRSWaitA("CONNECT OK",40))   //连接成功
	{
		return 1;
	}
	else
	{
		SendATCommand("AT\r","OK",5);
    	SendATCommand("AT\r","OK",5);

    	SendATCommand("AT+CIPCLOSE=1\r","CLOSE OK",5); //快速关闭TCP连接

		SendATCommand("AT+CIPSHUT\r","OK",5);
		return 0;
	}
}
u8 GPRSWaitA(char * pEcho,u32 outTime)
{
	u32 relen;
	char* pRet=0;
	//等待应答 
	relen=_4GCat1_read(sms_ResaveBuffer,SMS_BUFFER_SIZE,outTime);
	sms_ResaveBuffer[relen]=0;
	if(relen>0)
	{
		pRet=strstr((char *)sms_ResaveBuffer, pEcho);
	}		
	if(pRet>0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
u8 InitdataGSM()
{
	u8 i=0;
	char *pRet=0;
	
	for(i=0;i<10;i++)
	{
		_4GCat1_read((u8 *)prxBuffer,300,5);  //?????????????????  
	}
	SendATCommand("AT\r","OK",5); 
	delay_ms (500);	
 
	SendATCommand("AT\r","OK",20);

	delay_ms (500);	

	SendATCommand("AT\r","OK",20);

	delay_ms (500);	

	SendATCommand("AT\r","OK",20);

	delay_ms (500);	

	SendATCommand("AT\r","OK",5);

	delay_ms (500);	
	
	SendATCommand("AT+IPR=115200\r","OK",5);

	SendATCommand("AT\r","OK",5);
 
	SendATCommand("ATE0\r","OK",5);					
 
	SendATCommand("AT+CMEE=1\r","OK",5);             //设置设备错误提示
 
	delay_ms (500);	
	
	//SendATCommand("ATD18501306153;\r","OK",5);
	//SendATCommand("ATD18500152109;\r","OK",5);
	//OSTimeDlyHMSM (0, 0, 0, 500);	
	for(i=0;i<5;i++)
	{ 
		pRet=SendATCommand("AT+CSMS=1\r","+CSMS",8);        //选择短消息服务
		if(pRet>0) break;        
		delay_ms (1000);	
	} 
	for(i=0;i<5;i++) 
	{ 
		pRet=SendATCommand("AT+CNMI=2,1,0,0,1\r","OK",8);   //设置新消息提示 
		if(pRet>0) break;     
		delay_ms (500);		
	} 
	
//	SendATCommand("AT+CIPSHUT\r","OK",5); 
	if(i==5) return 0;
	else return 1;
}
void SendString(char * ptr)
{
	u32 i;
	u32 num;
	num=strlen(ptr);
	
}

u8 OneNET_SendData_Ack(char *pCommand, char* pEcho, u32 outTime)
{
	u8 ret=0;
	char *pRet=0;
	//传输大量数据点时，避免模块丢指令
	_4GCat_OpenMSUB();
	for(u8 i=0;i<5;i++)
	{
		pRet=SendATCommand(pCommand,pEcho,outTime);
		if(pRet!=NULL)
		{
			break;
		}
		else if(i==4)
		{
			ret = -1;
		}
		delay_ms(500);
	}
	_4GCat_CloseMSUB();
	return ret;
}

u32 te;
char* SendATCommand(char *pCommand, char* pEcho, u32 outTime)
{
  char *pRet=0;  
	u8 i=0;
	
	memset(_4GCat1_Usart.rxbuffer,0,sizeof(_4GCat1_Usart.rxbuffer));
	memset(_4GCat1_Usart.txbuffer,0,sizeof(_4GCat1_Usart.txbuffer));
	te=strlen(pCommand);
	_4GCat1_write((u8 *)pCommand,strlen(pCommand));  
	memset(prxBuffer,0,sizeof(prxBuffer));
	for(i=0;i<3;i++)
	{
		if(_4GCat1_read((u8 *)prxBuffer,UART_BUFFER_LEN,outTime)>0)  
		{
			pRet=strstr(prxBuffer, pEcho); 
			if(pRet==0)	
			{
				continue;
			}
			SEGGER_RTT_printf(0,RTT_CTRL_TEXT_MAGENTA"%s=%s\r\nsuccess\r\n",pCommand,prxBuffer);
			return pRet; 
		}
		else
		{
			SEGGER_RTT_printf(0,RTT_CTRL_TEXT_MAGENTA"%s=%s\r\nfault\r\n",pCommand,prxBuffer);
			continue;
		}   
	}
	return 0;
}

char * _4GCat1_SendBuff(u8 * datbuff,u32 len)	
{
	_4GCat1_write((u8 *)datbuff,len); 
	
}
void SendImgData(char *pro_id, char *dev_name,uint16_t data_name,char * imgData,uint16_t len,u8 outTime)
{
	char tempbuff[1300]={0};
	char data_buf[1150];
	unsigned short i;
	snprintf(data_buf, sizeof(data_buf), "$sys/%s/%s/dp/post/json,", pro_id, dev_name);
	strcpy(tempbuff,"AT+MPUB=");
	strcat(tempbuff,data_buf);
	strcat(tempbuff,"1,");
	strcat(tempbuff,"0,");
	strcat(tempbuff,"\"{\\22id\\22:123,\\22dp\\22:{");


	snprintf(data_buf, sizeof(data_buf), "\\22imd%d\\22:[{\\22v\\22:\\22%s\\22}]", data_name,imgData);
	strcat(tempbuff,data_buf);
	
	strcat(tempbuff,"}}\"\r");
	SendATCommand(tempbuff,"OK",outTime);//训练波特率
}
/*


*/
void SendImgData2(char * imgData,uint16_t len,u16 num,u8 outTime)
{
	char databuff[1300]={0};
	char tempbuff[100]={0};
	char cmdbuff[30]={0};
	unsigned short i;
	u32 lenlast=0;
	//AT+CIPSEND
	databuff[0]='y';
	databuff[1]='g';
	databuff[2]='t';
	databuff[3]=',';
	strcat(databuff,onenet_info.dev_id);
	snprintf(tempbuff,sizeof(tempbuff),",%d-%d-%d:%d:%d:%d,",g_rtc.years,g_rtc.months,g_rtc.days,g_rtc.hours,g_rtc.minutes,g_rtc.secs);
	strcat(databuff,tempbuff);
	snprintf(tempbuff,sizeof(tempbuff),"%d,",num);
	strcat(databuff,tempbuff);
	lenlast=strlen(databuff);
	memcpy(databuff+lenlast,imgData,len);
	lenlast=lenlast+len;
	while(1)
	{
		snprintf(cmdbuff,sizeof(cmdbuff),"AT+CIPSEND=%d\r",lenlast);
		pRet=SendATCommand(cmdbuff,cmdbuff,50);//查询下链接状态
		
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	_4GCat1_SendBuff((u8 *)databuff,lenlast);
		
	
}
/*
4G写入
*/
//发送len个字节.
//buf:发送区首地址
//len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)

s32 _4GCat1_write(const u8 *buf,u32 len)
{
	u32 i=0;
	u32 retry=0;
	if(len==0)	return 0;
	_4GCat1_Usart.rxpointer=0;
	_4GCat1_Usart.rxlen=0;
	_4GCat1_Usart.txpointer=0;
	_4GCat1_Usart.txlen=len; 
	_4GCat1_Usart.rxflag=0;
	for(i=0;i<len;i++)
	{
		_4GCat1_Usart.txbuffer[i]=buf[i];
	}
	while(usart_flag_get(UART7, USART_FLAG_TC) == RESET)
	{
		retry++;
		//delay_ms(1);
		delay_us(50);
		if(retry>100)return 0;
	}	retry=0; 
	usart_data_transmit(UART7,buf[_4GCat1_Usart.txpointer]);//写入第一个字节
	while(usart_flag_get(UART7, USART_FLAG_TC) == RESET)
	{
		retry++;
		//delay_ms(1);
		delay_us(50);
		if(retry>100)return 0;
	} 
	usart_interrupt_enable(UART7, USART_INT_TBE); 				//打开发送中断
	testa=testa;
	testa=testa;
	xSemaphoreTake(_4GCat1_Usart.semTx,RS485_xBlockTime);  //申请互斥信号量//帧头
	
	
//	for(i=0;i<len;i++)
//	{
//		while(usart_flag_get (UART7 ,USART_FLAG_TBE )!=SET ){}
//		usart_data_transmit(UART7,buf[i]);
//		while(usart_flag_get (UART7 ,USART_FLAG_TC )!=SET ){}
//	}

}
//断开GPRS
u8 GPRS_disconnet()     
{
	SendATCommand("AT\r","OK",5);
	SendATCommand("AT\r","OK",5);

	SendATCommand("AT+CIPCLOSE=1\r","CLOSE OK",5);

	SendATCommand("AT+CIPSHUT\r","OK",5);//??????

	return 1;
}

u8 GSmReset(void)
{
	_4GCat1_RESET=1;	
	delay_ms(1000);
	_4GCat1_RESET=0;	
}
//--------------------------------------------------------------------------
// 将单字节数字转换为10进制数字的字符串
// 返回转换后的字符串长度
//--------------------------------------------------------------------------

u8 bytetostring(int ndata,char *pString)
{
	int t,datu;
	u8 len;
	
	//////////////////	2012?6??
	if(ndata>=1000)
	{
		t=1000;
		len = 4;
	}
	//////////////////
	else if(ndata>=100)
	{
		t=100;
		len = 3;
	}
	else if(ndata>=10)
	{
		t=10;
		len = 2;
	}
	else
	{
		t=1;
		len = 1; 
	}
	
	while(1)
	{
		datu=ndata/t; 
		
		*pString=datu+0x30;
		pString++;
				
		if(t==1) break;
			
		ndata%=t;
			
		t/=10;
	}
	
	*pString = 0;
	
	return len;
}

u8 GPRS_send_len(int len)
{
	u32 i = 0;
	char templen[10];
	u8 tmp;
	u8 sms_buffer[20]; 
	
	tmp=bytetostring(len,templen);
	templen[tmp]='\r';
	templen[tmp+1]=0;
  
	// SendATCommand("AT+CIPSEND=?\r","OK",5);
	while(1)
	{
		i = 0;
		strcpy((char *)sms_buffer,"AT+CIPSEND=");
		strcat((char *)sms_buffer,templen); //本命令用于在单链接模式下发送长度不可变的数据
		while(SendATCommand((char *)sms_buffer,">",15)==0)
		{
			i++;
			if(i >=4)
			{
				while(GPRS_connect(onenet_info.ip,onenet_info.port) != 1)
				{
					InitdataGSM();
				}
				OneNET_DevLink(onenet_info.dev_name, onenet_info.pro_id, onenet_info.key);	//登录设备
			}
			break;
			
		}
		
		/*
		// ???">"
		if(SendATCommand((char *)sms_buffer,">",15)==0)  //
		{
			if(++i>2)
			{
				SendATCommand("AT","OK",5);
    		SendATCommand("AT","OK",5);
				SendATCommand("AT+CIPCLOSE=1\r","CLOSE OK",5);
				SendATCommand("AT+CIPSHUT\r","OK",5);
				return 0;                           // ????3??
			}
		}
		else break; 
			*/
		break;
	}	
	return 1;

}

void delay_test(u32 a)
{
	while(a--);
}

/*
4G读取
*/
s32 _4GCat1_read(u8 *buffer, s32 buffersize, s32 timeout)
{
	u8 err;
	s32 rxlen;
	s32 i;
	u32 timecount=0;
	gd32_wdgt_feed_dog();
	if(buffersize==0)	return -1;
	
	if(timeout<_4GCat1_xBlockTime)timeout=_4GCat1_xBlockTime;
	 xSemaphoreTake(_4GCat1_Usart.semRx,timeout);  //申请互斥信号量
	_4GCat1_Usart.rxflag=0;

	rxlen=(buffersize<_4GCat1_Usart.rxlen)?buffersize:_4GCat1_Usart.rxlen;
	memcpy(buffer,_4GCat1_Usart.rxbuffer,rxlen);
	if (rxlen<_4GCat1_Usart.rxlen)
	{
		for (i=0;i<_4GCat1_Usart.rxlen-rxlen;i++) //处理未读取的数据
		{
			_4GCat1_Usart.rxbuffer[i]=_4GCat1_Usart.rxbuffer[i+rxlen];
		} 
	
	} 
	_4GCat1_Usart.rxlen-=rxlen; 
	_4G_Usart_Rxlen=rxlen;	
	return rxlen;
}

void CIPSEND_Test()
{
	char databuff[200]={0};
	u8 txBuff[256];
	u32 len;
	u32 i;
	len=20;
	u8 timeout=0;
	for(i=0;i<len;i++)
	{
		txBuff[i]=i;
	}
	//AT+CIPSEND
	while(1)   
	{
		snprintf(databuff,sizeof(databuff),"AT+CIPSEND=%d\r",len);
		pRet=SendATCommand(databuff,databuff,50);//查询下链接状态
		
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	_4GCat1_SendBuff(txBuff,len);
}


/*
POST方法
*/
void ReportVersion(void )
{
	char * pRet;
	u8 i=0;
	//ReportVersion
	SEGGER_RTT_printf(0,RTT_CTRL_TEXT_MAGENTA"ReportVersion*************\r\n");
	//查询网络附着状态
	for(i=0;i<5;i++)
	{
		pRet=SendATCommand("AT+CGATT?\r","+CGATT: 1",50); 
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
 //初始化HTTP 服务
	for(i=0;i<3;i++)
	{
		pRet=SendATCommand("AT+HTTPINIT\r","OK",50);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	
	//设置URL
	for(i=0;i<3;i++)
	{
		snprintf(tempbuff,sizeof(tempbuff),"AT+HTTPPARA=\"URL\",\"http://iot-api.heclouds.com/fuse-ota/%s/%s/version\"\r",onenet_info.pro_id,onenet_info.dev_name);
		pRet=SendATCommand(tempbuff,"OK",5);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	

	//查询时间
	for(i=0;i<3;i++)
	{
		pRet=SendATCommand("AT+CCLK?\r","+CCLK:",50); //设置使用的PDP的<cid>=1
		if(pRet!=NULL)
		{
			_4G_rtc.years=(*(pRet+8)-0x30)*10+(*(pRet+9)-0x30);
			_4G_rtc.months=(*(pRet+11)-0x30)*10+(*(pRet+12)-0x30);
			_4G_rtc.days=(*(pRet+14)-0x30)*10+(*(pRet+15)-0x30);
			_4G_rtc.hours=(*(pRet+17)-0x30)*10+(*(pRet+18)-0x30);
			_4G_rtc.minutes=(*(pRet+20)-0x30)*10+(*(pRet+21)-0x30);
			_4G_rtc.secs=(*(pRet+23)-0x30)*10+(*(pRet+24)-0x30);
//			sscanf(pRet,"+CCLK: \"%2d/%2d/%2d,%2d:%2d:%2d+",(int*)&_4G_rtc.years,(int*)&_4G_rtc.months,(int*)&_4G_rtc.days,(int*)&_4G_rtc.hours,(int*)&_4G_rtc.minutes,(int*)&_4G_rtc.secs);
			break;
		}
		delay_ms(500);
	}
	Authorization_V2(authorization_buf,sizeof(authorization_buf),_4G_rtc,net_user_info.user_id,net_user_info.user_access_key);
	//设置Headers中的Authorization
	for(i=0;i<3;i++)
	{
		snprintf(tempbuff,sizeof(tempbuff),"AT+HTTPPARA=\"USER_DEFINED\",\"Authorization: %s\"\r",authorization_buf);
		//AT+HTTPPARA="USER_DEFINED","Authorization: version=2020-05-29&res=userid%2F154787&et=1697532276&method=sha1&sign=xq%2BF31uka96lTTAPqwO1LIs%2BhX8%3D"
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_YELLOW"%s\r\n",tempbuff);
		pRet=SendATCommand(tempbuff,"OK",5);;//查询下链接状态
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	//设置Headers中的Content-Type
	for(i=0;i<3;i++)
	{
		pRet=SendATCommand("AT+HTTPPARA=\"USER_DEFINED\",\"Content-Type: application/json\"\r","OK",5);;//查询下链接状态
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	
	//发送body
	snprintf(chbuff,sizeof(chbuff),"{\"s_version\":\"%s\",\"f_version\":\"v1.0.0\"}\r",myota_info.nowVersion);//"{\"s_version\":\"v1.9\",\"f_version\":\"v1.9\"}\r"
	for(i=0;i<3;i++)
	{
		sprintf(chbuff2,"AT+HTTPDATA=%d,10000\r",strlen(chbuff));
		pRet=SendATCommand(chbuff2,"DOWNLOAD",5);;//查询下链接状态
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	
	_4GCat1_SendBuff((u8 *)chbuff,strlen(chbuff));
	for(i=0;i<3;i++)
	{
		if(_4GCat1_read((u8 *)prxBuffer,300,300)>0)  
		{
			pRet=strstr(prxBuffer, "OK"); 
			if(pRet!=0)
			{
				break;
			}
			
		}
	}
	
	//HTTP 方式激活 =1:POST,=0:GET

//	pRet=SendATCommand("AT+HTTPACTION=1\r","OK",5);

	pRet=SendATCommand("AT+HTTPACTION=1\r","+HTTPACTION:",5000);
	
	//查询HTTP服务响应
	for(i=0;i<3;i++)
	{
		pRet=SendATCommand("AT+HTTPREAD\r","OK",5);//查询 HTTP 服务响应： AT+HTTPREAD
		if(pRet!=NULL)
		{
			SEGGER_RTT_printf(0,RTT_CTRL_TEXT_WHITE"%s\r\n",prxBuffer);
			break;
		}
		delay_ms(500);
	}
	
	//终止HTTP 任务
	for(i=0;i<3;i++)
	{
		pRet=SendATCommand("AT+HTTPTERM\r","OK",5);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	
}

//绑定设备编码与onenet设备
s32 Bind_Devcode(void )
{
	int ret = 0;
	char * pRet;
	u8 i=0;
	
	//查询网络附着状态
	for(i=0;i<5;i++)
	{
		pRet=SendATCommand("AT+CGATT?\r","+CGATT: 1",50); 
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	//初始化HTTP 服务
	for(i=0;i<3;i++)
	{
		pRet=SendATCommand("AT+HTTPINIT\r","OK",50);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	
	//设置URL
	for(i=0;i<3;i++)
	{
		snprintf(tempbuff,sizeof(tempbuff),"AT+HTTPPARA=\"URL\",\"http://47.117.126.245:8010/api/device/bind-onenet\"\r");
		pRet=SendATCommand(tempbuff,"OK",5);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	
	//设置Headers中的Content-Type
	for(i=0;i<3;i++)
	{
		pRet=SendATCommand("AT+HTTPPARA=\"USER_DEFINED\",\"Content-Type: application/json\"\r","OK",5);;//查询下链接状态
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	
	//发送body
	snprintf(chbuff,sizeof(chbuff),"{\"randomNumber\":\"%s\",\"oneNetId\":\"%s\"}\r",dev_SavePar.dev_code,onenet_info.dev_id);
	for(i=0;i<3;i++)
	{
		//AT+HTTPDATA=<size>,<time ms>		
		sprintf(chbuff2,"AT+HTTPDATA=%d,10000\r",strlen(chbuff));
		//返回DOWNLOAD准备好输入数据
		pRet=SendATCommand(chbuff2,"DOWNLOAD",5);	
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	_4GCat1_SendBuff((u8 *)chbuff,strlen(chbuff));
	for(i=0;i<3;i++)
	{
		if(_4GCat1_read((u8 *)prxBuffer,300,300)>0)  
		{
			pRet=strstr(prxBuffer, "OK"); 
			if(pRet!=0)
			{
				break;
			}
			
		}
	}
	
	//HTTP 方式激活 =1:POST,=0:GET
//	pRet=SendATCommand("AT+HTTPACTION=1\r","OK",5);
	pRet=SendATCommand("AT+HTTPACTION=1\r","+HTTPACTION:",5000);
	
	//查询HTTP服务响应
	for(i=0;i<3;i++)
	{
		pRet=SendATCommand("AT+HTTPREAD\r","OK",5);//查询 HTTP 服务响应： AT+HTTPREAD
		if(pRet!=NULL)
		{
			SEGGER_RTT_printf(0,RTT_CTRL_TEXT_WHITE"%s\r\n",prxBuffer);
			//从prxBuffer查找到返回的设备编码--JSON格式
			ret = Parse_BindDevCode((prxBuffer+18));
			break;
		}
		delay_ms(500);
	}
	
	//终止HTTP 任务
	for(i=0;i<3;i++)
	{
		pRet=SendATCommand("AT+HTTPTERM\r","OK",5);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	
	return ret;
}

//申请设备编码
s32 Apply_Devcode(void )
{
	int ret =0;
	char * pRet;
	u8 i=0;
	
	//查询网络附着状态
	for(i=0;i<5;i++)
	{
		pRet=SendATCommand("AT+CGATT?\r","+CGATT: 1",50); 
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	//初始化HTTP 服务
	for(i=0;i<3;i++)
	{
		pRet=SendATCommand("AT+HTTPINIT\r","OK",50);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	
	//设置URL
	for(i=0;i<3;i++)
	{
		snprintf(tempbuff,sizeof(tempbuff),"AT+HTTPPARA=\"URL\",\"http://47.117.126.245:8010/api/device/register\"\r");
		pRet=SendATCommand(tempbuff,"OK",5);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	
	//设置Headers中的Content-Type
	for(i=0;i<3;i++)
	{
		pRet=SendATCommand("AT+HTTPPARA=\"USER_DEFINED\",\"Content-Type: application/json\"\r","OK",5);;//查询下链接状态
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	
	//发送body
	snprintf(chbuff,sizeof(chbuff),"{\"deviceModel\":\"GW-001\",\"simCardNumber\":\"%s\"}\r",dev_SavePar.sim_number);
	for(i=0;i<3;i++)
	{
		//AT+HTTPDATA=<size>,<time ms>		
		sprintf(chbuff2,"AT+HTTPDATA=%d,10000\r",strlen(chbuff));
		//返回DOWNLOAD准备好输入数据
		pRet=SendATCommand(chbuff2,"DOWNLOAD",5);	
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	
	_4GCat1_SendBuff((u8 *)chbuff,strlen(chbuff));
	for(i=0;i<3;i++)
	{
		if(_4GCat1_read((u8 *)prxBuffer,300,300)>0)  
		{
			pRet=strstr(prxBuffer, "OK"); 
			if(pRet!=0)
			{
				break;
			}
			
		}
	}
	
	//HTTP 方式激活 =1:POST,=0:GET
//	pRet=SendATCommand("AT+HTTPACTION=1\r","OK",5);
	pRet=SendATCommand("AT+HTTPACTION=1\r","+HTTPACTION:",5000);
	
	//查询HTTP服务响应
	for(i=0;i<3;i++)
	{
		pRet=SendATCommand("AT+HTTPREAD\r","OK",5);//查询 HTTP 服务响应： AT+HTTPREAD
		if(pRet!=NULL)
		{
			SEGGER_RTT_printf(0,RTT_CTRL_TEXT_WHITE"%s\r\n",prxBuffer);
			//从prxBuffer查找到返回的设备编码--JSON格式
			ret = Parse_ApplyDevCode((prxBuffer+18));
			break;
		}
		delay_ms(500);
	}
	
	//终止HTTP 任务
	for(i=0;i<3;i++)
	{
		pRet=SendATCommand("AT+HTTPTERM\r","OK",5);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	
	return ret;
	
}

/*
请求设备编码返回格式
{
  "success": true,
  "message": "设备注册成功",
  "data": {
    "id": 1,
    "deviceModel": "GW-001",
    "simCardNumber": "89860118801012345678",
    "randomNumber": "1234567890123",
    "oneNetId": null,
    "status": "REGISTERED",
    "createTime": "2023-10-01T10:00:00",
    "bindTime": null
  }
}
*/
s32 Parse_ApplyDevCode(char * Buf_Devcode)
{
	int ret = 0;
	cJSON * cJSON_cmd;
	cJSON * root ;

	//将JSON字符串转换成JSON结构体
	root = cJSON_Parse(Buf_Devcode);
	if(root==NULL)
	{
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_RED"%s\r\n",Cmd_Buff);
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_RED"Invalid JSON!\r\n");
		cJSON_Delete(root);
		return -1;
	}
	
	//申请设备编码
	cJSON_cmd=cJSON_GetObjectItem(root,"success");
	if((cJSON_cmd!=NULL) && (cJSON_cmd->type==cJSON_True))
	{
		cJSON_cmd=cJSON_GetObjectItem(root,"data");
		if(cJSON_cmd!=NULL)
		{
			cJSON_cmd=cJSON_GetObjectItem(cJSON_cmd,"randomNumber");		
			if((cJSON_cmd!=NULL) && (cJSON_cmd->type==cJSON_String))
			{
				memcpy(dev_SavePar.dev_code,cJSON_cmd->valuestring,sizeof(dev_SavePar.dev_code));
				ret = 1 ;
			}
		}
	}
	
	cJSON_Delete(root);
	return ret;
}


/*
{
  "success": true,
  "message": "OneNet ID绑定成功",
  "data": {
    "id": 1,
    "deviceModel": "GW-001",
    "simCardNumber": "89860118801012345678",
    "randomNumber": "1234567890123",
    "oneNetId": "onenet_device_001",
    "status": "BOUND",
    "createTime": "2023-10-01T10:00:00",
    "bindTime": "2023-10-01T11:00:00"
  }
}
*/
s32 Parse_BindDevCode(char * Buf_Devcode)
{
	int ret = 0;
	cJSON * cJSON_cmd;
	cJSON * root ;

	//将JSON字符串转换成JSON结构体
	root = cJSON_Parse(Buf_Devcode);
	if(root==NULL)
	{
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_RED"%s\r\n",Cmd_Buff);
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_RED"Invalid JSON!\r\n");
		cJSON_Delete(root);
		return -1;
	}
	
	//申请设备编码
	cJSON_cmd=cJSON_GetObjectItem(root,"success");
	if((cJSON_cmd!=NULL) && (cJSON_cmd->type==cJSON_True))
	{
		ret = 1;
	}
	
	cJSON_Delete(root);
	return ret;
}


/*
使用get方法
*/

void CheckUpdate(void)
{
	char * pRet;
	char * pRet2;
	u8 i=0;
	SEGGER_RTT_printf(0,RTT_CTRL_TEXT_MAGENTA"CheckUpdate*************\r\n");
	//初始化HTTP 服务
	for(i=0;i<5;i++)   
	{
		pRet=SendATCommand("AT+HTTPINIT\r","OK",50);//查询下链接状态
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	//查询下链接状态
	for(i=0;i<5;i++)   
	{
		pRet=SendATCommand("AT+SAPBR=2,1\r","OK",50);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	

	//设置URL
	for(i=0;i<5;i++)
	{
		
		snprintf(tempbuff,sizeof(tempbuff),"AT+HTTPPARA=\"URL\",\"http://iot-api.heclouds.com/fuse-ota/%s/%s/check?type=2&version=%s\"\r",onenet_info.pro_id,onenet_info.dev_name,myota_info.nowVersion);
		pRet=SendATCommand(tempbuff,"OK",5);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	//查询时间
	for(i=0;i<3;i++)
	{
		pRet=SendATCommand("AT+CCLK?\r","+CCLK:",50); //设置使用的PDP的<cid>=1
		if(pRet!=NULL)
		{
			_4G_rtc.years=(*(pRet+8)-0x30)*10+(*(pRet+9)-0x30);
			_4G_rtc.months=(*(pRet+11)-0x30)*10+(*(pRet+12)-0x30);
			_4G_rtc.days=(*(pRet+14)-0x30)*10+(*(pRet+15)-0x30);
			_4G_rtc.hours=(*(pRet+17)-0x30)*10+(*(pRet+18)-0x30);
			_4G_rtc.minutes=(*(pRet+20)-0x30)*10+(*(pRet+21)-0x30);
			_4G_rtc.secs=(*(pRet+23)-0x30)*10+(*(pRet+24)-0x30);
//			sscanf(pRet,"+CCLK: \"%2d/%2d/%2d,%2d:%2d:%2d+",(int*)&_4G_rtc.years,(int*)&_4G_rtc.months,(int*)&_4G_rtc.days,(int*)&_4G_rtc.hours,(int*)&_4G_rtc.minutes,(int*)&_4G_rtc.secs);
			break;
		}
		delay_ms(500);
	}
	Authorization_V2(authorization_buf,sizeof(authorization_buf),_4G_rtc,net_user_info.user_id,net_user_info.user_access_key);
	//设置Headers中的Authorization
	for(i=0;i<5;i++)  
	{
		snprintf(tempbuff,sizeof(tempbuff),"AT+HTTPPARA=\"USER_DEFINED\",\"Authorization: %s\"\r",authorization_buf);
		//AT+HTTPPARA="USER_DEFINED","Authorization: version=2020-05-29&res=userid%2F154787&et=1697532276&method=sha1&sign=xq%2BF31uka96lTTAPqwO1LIs%2BhX8%3D"
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_YELLOW"%s\r\n",tempbuff);
		pRet=SendATCommand(tempbuff,"OK",5);;//查询下链接状态
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}


	//发起请求
	pRet=SendATCommand("AT+HTTPACTION=0\r","+HTTPACTION:",5000);
//	for(i=0;i<3;i++)
//	{
//		
//		if(_4GCat1_read((u8 *)prxBuffer,UART_BUFFER_LEN,100)>0)  
//		{
//			pRet=strstr(prxBuffer, "+HTTPACTION:"); 
//			if(pRet==0)	
//			{
//				continue;
//			}
//			break; 
//		}
//		else
//		{
//			continue;
//		}   
//	}
	//清空Update_par
	memset(Update_Par.target,0,sizeof(Update_Par.target));
	memset(Update_Par.tid,0,sizeof(Update_Par.tid));
	memset(Update_Par.size,0,sizeof(Update_Par.size));
	memset(Update_Par.md5,0,sizeof(Update_Par.md5));
	//查询响应
	for(i=0;i<5;i++)    
	{
		pRet=SendATCommand("AT+HTTPREAD\r","OK",5);//查询 HTTP 服务响应： AT+HTTPREAD
		if(pRet!=NULL)
		{
			SEGGER_RTT_printf(0,RTT_CTRL_TEXT_WHITE"%s\r\n",prxBuffer);
			//pRet2=strstr((char *)pRet,"\"msg\": \"succ\"");
			pRet2=strstr((char *)_4GCat1_Usart.rxbuffer,"{");
			analysisCheck(pRet2);
			break;
		}
		
		delay_ms(500);
	}
	//关闭http
	for(i=0;i<5;i++)    
	{
		pRet=SendATCommand("AT+HTTPTERM\r","OK",5);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}

}
void _4GCat_CloseMSUB(void)
{
	u8 i=0;
	for(i=0;i<3;i++)    
	{
		pRet=SendATCommand("AT+MQTTMSGSET=1\r","OK",5);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	//AT+MQTTMSGSET=
}
void _4GCat_OpenMSUB(void)
{
	u8 i=0;
	for(i=0;i<3;i++)    
	{
		pRet=SendATCommand("AT+MQTTMSGSET=0\r","OK",5);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	//AT+MQTTMSGSET=
}
void _4GCat_PrintMSUB(void)
{
	u8 i=0;
	for(i=0;i<3;i++)    
	{
		pRet=SendATCommand("AT+MQTTMSGGET\r","OK",5);
		if(pRet!=NULL)
		{
			SEGGER_RTT_SetTerminal(1); 
			SEGGER_RTT_printf(0,RTT_CTRL_TEXT_GREEN"%s\r\n",pRet);
			SEGGER_RTT_SetTerminal(0); 
			break;
		}
		delay_ms(500);
	}
}
void analysisCheck(char * str)
{
	cJSON* root;
	cJSON * cJSON_code;
	cJSON * cJSON_msg;
	cJSON * cJSON_request_id;
	
	cJSON * cJSON_data;
	
	cJSON * cJSON_target;
	cJSON * cJSON_tid;
	cJSON * cJSON_size;
	cJSON * cJSON_md5;
	cJSON * cJSON_status;
	cJSON * cJSON_type;
	u32 i=0;
	memset(strtemp,'\0',sizeof(strtemp));
	for(i=0;i<200;i++)
	{
		if(str[i]=='\r')
		{
			break;
		}
		if(str[i]=='\n')
		{
			break;
		}
		strtemp[i]=str[i];
		
	}
	
	
	root = cJSON_Parse(strtemp);
	if(root==NULL)
	{
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_RED"%s\r\n",strtemp);
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_RED"Invalid JSON!\r\n");
		return;
		//Invalid JSON.
	}
	cJSON_code=cJSON_GetObjectItem(root,"code");
	cJSON_msg=cJSON_GetObjectItem(root,"msg");
	cJSON_data=cJSON_GetObjectItem(root,"data");
	cJSON_request_id=cJSON_GetObjectItem(root,"request_id");


	if(cJSON_data!=NULL)
	{
		cJSON_target=cJSON_GetObjectItem(cJSON_data,"target");
		
		cJSON_tid=cJSON_GetObjectItem(cJSON_data,"tid");
		cJSON_size=cJSON_GetObjectItem(cJSON_data,"size");
		cJSON_md5=cJSON_GetObjectItem(cJSON_data,"md5");
		cJSON_status=cJSON_GetObjectItem(cJSON_data,"status");
		cJSON_type=cJSON_GetObjectItem(cJSON_data,"type");
		snprintf(Update_Par.target,sizeof(Update_Par.target),"%s",cJSON_target->valuestring);
		snprintf(Update_Par.tid,sizeof(Update_Par.tid),"%d",(int)cJSON_tid->valuedouble);
		snprintf(Update_Par.size,sizeof(Update_Par.size),"%d",(int)cJSON_size->valuedouble);
		snprintf(Update_Par.md5,sizeof(Update_Par.md5),"%s",cJSON_md5->valuestring);
//		snprintf(Update_Par.tidstr,sizeof(Update_Par.tidstr),"");
	}
	cJSON_Delete(root);
}
/*
查询任务状态
*/
void CheckUpdateTask(void)
{
	
}

//IP应用设置：AT+SAPBR 
void InitSAPBR(void)
{
	u8 i=0;
	pRet=SendATCommand("AT+CGSN\r","AT+CGSN",8); 	//查询IMEI号
	delay_ms(500);
	//查询网络附着状态
	for(i=0;i<5;i++)
	{
		pRet=SendATCommand("AT+CGATT?\r","+CGATT: 1",50); 
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}

	for(i=0;i<5;i++)
	{
		pRet=SendATCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r","OK",50); //设置承载类型为GPRS
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	/*
		AT+SAPBR=3,1,"APN",""
		模块注册网络后会从网络自动获取<apn>并激活一个PDP上下文，用于RNDIS上网使用。
	*/
	for(i=0;i<5;i++)
	{
		pRet=SendATCommand("AT+SAPBR=3,1,\"APN\",\"\"\r","OK",50); //设置PDP承载之APN参数 
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	//AT+SAPBR=1,1
	for(i=0;i<5;i++)
	{
		pRet=SendATCommand("AT+SAPBR=1,1\r","OK",50); //激活该承载的GPRS PDP上下文
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
}
void InitNTP(void)
{
	u8 i=0;
	
	//查询网络附着状态
	for(i=0;i<5;i++)
	{
		pRet=SendATCommand("AT+CGATT?\r","+CGATT: 1",50); 
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	//设置GPRS承载场景ID：AT+CNTPCID  取值同+SAPBR命令的<cid>
	for(i=0;i<5;i++)
	{
		pRet=SendATCommand("AT+CNTPCID=1\r","OK",50); //设置使用的PDP的<cid>=1
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	//同步网络时间：AT+CNTP
	for(i=0;i<5;i++)
	{
		pRet=SendATCommand("AT+CNTP\r","OK",50); 
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}

	//查询时间
	for(i=0;i<3;i++)
	{
		pRet=SendATCommand("AT+CCLK?\r","+CCLK:",50); //设置使用的PDP的<cid>=1
		if(pRet!=NULL)
		{
			_4G_rtc.years=(*(pRet+8)-0x30)*10+(*(pRet+9)-0x30);
			_4G_rtc.months=(*(pRet+11)-0x30)*10+(*(pRet+12)-0x30);
			_4G_rtc.days=(*(pRet+14)-0x30)*10+(*(pRet+15)-0x30);
			_4G_rtc.hours=(*(pRet+17)-0x30)*10+(*(pRet+18)-0x30);
			_4G_rtc.minutes=(*(pRet+20)-0x30)*10+(*(pRet+21)-0x30);
			_4G_rtc.secs=(*(pRet+23)-0x30)*10+(*(pRet+24)-0x30);
			DS3231_SetRTC(_4G_rtc);
			
//			sscanf(pRet,"+CCLK: \"%2d/%2d/%2d,%2d:%2d:%2d+",(int*)&_4G_rtc.years,(int*)&_4G_rtc.months,(int*)&_4G_rtc.days,(int*)&_4G_rtc.hours,(int*)&_4G_rtc.minutes,(int*)&_4G_rtc.secs);
			break;
		}
		delay_ms(500);
	}
}
/*
下载升级包
https://iot-api.heclouds.com/fuse-ota/{pro_id}/{dev_name}/{tid}/download
*/

u8 DownLoadPackages(void)
{
	
	
	u32 downSize;
	u32 downStart;
	u32 downStop;
	u32 secremain;	
	u32 downNum;
	u32 i=0;
	u16 step;
	u8 tempu8;
	u8 j=0;
	
	sscanf(Update_Par.size,"%d",&downSize);
	downNum=downSize/1024;         //一共的下载次数
	if(downSize%1024!=0)   //如果不能整除
	{
		downNum+=1;
	}
	downStart=0;
	MD5_Init(&md5_ctx);
	for(i=0;i<downNum;i++)
	{
		downStop=(downStart+1023>downSize)?downSize:(downStart+1023); //	取最小值
		step=((i+1)*100/downNum);
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_GREEN"all bytes:%d,step:%d\r\n",downSize,step);
		for(j=0;j<5;j++)
		{
			tempu8=DownLoadPack(downStart,downStop);
			if(tempu8==0)
			{
				break;
			}
			if(tempu8!=0)
			{
				SEGGER_RTT_printf(0,RTT_CTRL_TEXT_RED"Download failed code:%d\r\n",tempu8);
				
			}
		}
		
		downStart=downStop+1;
		SendStatus(step);
	}
	memset(md5_result, 0, sizeof(md5_result));
	MD5_Final(&md5_ctx, md5_t);
	for(i = 0; i < 16; i++)
	{
		if(md5_t[i] <= 0x0f)
			sprintf(md5_t1, "0%x", md5_t[i]);
		else
			sprintf(md5_t1, "%x", md5_t[i]);
		
		strcat(md5_result, md5_t1);
	}
	if(strcmp(md5_result, Update_Par.md5) == 0)																			//MD5校验比对
	{
		return 0;
	}
	else
	{
		return 1;
	}
	
}
/*
GET方法
*/
u8 DownLoadPack(u32 downStart,u32 downStop)
{
	u8 i=0;
	char * pRet;
	char * pRet2;
	int tempint;
	u16 tempu16;
	u8 result=0;
	snprintf(rangeStr,sizeof(rangeStr),"%d-%d",downStart,downStop);
	SEGGER_RTT_printf(0,RTT_CTRL_TEXT_BLUE"download Range:%s\r\n",rangeStr);
	while(1)   
	{
		pRet=SendATCommand("AT+HTTPINIT\r","OK",50);//查询下链接状态
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}

	while(1)   
	{
//		snprintf(tempbuff,sizeof(tempbuff),"AT+HTTPPARA=\"URL\",\"http://iot-api.heclouds.com/fuse-ota/%s/%s/version\"\r",onenet_info.pro_id,onenet_info.dev_name);
		snprintf(tempbuff,sizeof(tempbuff),"AT+HTTPPARA=\"URL\",\"http://iot-api.heclouds.com/fuse-ota/%s/%s/%s/download\"\r",onenet_info.pro_id,onenet_info.dev_name,Update_Par.tid);
		
		pRet=SendATCommand(tempbuff,"OK",5);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	Authorization_V2(authorization_buf,sizeof(authorization_buf),_4G_rtc,net_user_info.user_id,net_user_info.user_access_key);
	while(1)   
	{
		snprintf(tempbuff,sizeof(tempbuff),"AT+HTTPPARA=\"USER_DEFINED\",\"Authorization: %s\"\r",authorization_buf);
		pRet=SendATCommand(tempbuff,"OK",5);;//查询下链接状态
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	while(1)   
	{
		snprintf(tempbuff,sizeof(tempbuff),"AT+HTTPPARA=\"USER_DEFINED\",\"Range: %s\"\r",rangeStr);
		pRet=SendATCommand(tempbuff,"OK",5);;//查询下链接状态
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}

	while(1)   
	{
		pRet=SendATCommand("AT+HTTPACTION=0\r","OK",5);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	for(i=0;i<3;i++)
	{
		
		if(_4GCat1_read((u8 *)prxBuffer,UART_BUFFER_LEN,100)>0)  
		{
			pRet=strstr(prxBuffer, "+HTTPACTION:"); 
			if(pRet==0)	
			{
				continue;
			}
			break; 
		}
		else
		{
			continue;
		}   
	}
	while(1)   
	{
		pRet=SendATCommand("AT+HTTPREAD\r","OK",5000);//查询 HTTP 服务响应： AT+HTTPREAD
		if(pRet!=NULL)
		{
			pRet=strstr(prxBuffer,"+HTTPREAD:");
			memset(tempbuff,0,sizeof(tempbuff));
			sscanf(pRet,"+HTTPREAD: %[^\r]",tempbuff);//找到数据位置
			sscanf(tempbuff,"%d",&tempint);
			if(tempint!=(downStop+1-downStart))
			{
				result=2;
				break;
			}
			//1062-(1023+1-0)=1062-1024=38
			//
//			if(_4G_Usart_Rxlen<(1062-(downStop+1-downStart)))
//			{
//				result=3;
//				break;
//			}
			pRet2=strstr(pRet,"\r");
			pRet2+=2;
			memcpy(GD32_wrdata,pRet2,tempint);
			tempu16=(tempint>>2);
			if(tempint%4)
			{
				tempu16+=1;
			}
			MD5_Update(&md5_ctx, GD32_wrdata, tempint);
			Download_WriteFlash(downStart,tempu16); //GD32_wrdata
			SEGGER_RTT_printf(0,RTT_CTRL_TEXT_WHITE"download bytes:%d,gd32 flash write words:%d,_4G_Usart_Rxlen:%d\r\n",tempint,tempu16,_4G_Usart_Rxlen);
			result=0;
			break;
		}
		else
		{
			result=1;
			break;
		}
		
		delay_ms(500);
	}
//	GD32_WriteFlash_Word()
	while(1)   
	{
		pRet=SendATCommand("AT+HTTPTERM\r","OK",5);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	return result;
}
/*
POST方法
*/
void SendStatus(u16 step)
{
	char * pRet;
	u8 i=0;
	while(1)   
	{
		pRet=SendATCommand("AT+HTTPINIT\r","OK",50);//查询下链接状态
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	

	while(1)   
	{
		snprintf(tempbuff,sizeof(tempbuff),"AT+HTTPPARA=\"URL\",\"https://iot-api.heclouds.com/fuse-ota/%s/%s/%s/status\"\r",onenet_info.pro_id,onenet_info.dev_name,Update_Par.tid);
		pRet=SendATCommand(tempbuff,"OK",5);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	Authorization_V2(authorization_buf,sizeof(authorization_buf),_4G_rtc,net_user_info.user_id,net_user_info.user_access_key);
	//"USER_DEFINED"
	while(1)
	{
		snprintf(tempbuff,sizeof(tempbuff),"AT+HTTPPARA=\"USER_DEFINED\",\"Authorization: %s\"\r",authorization_buf);
		//AT+HTTPPARA="USER_DEFINED","Authorization: version=2020-05-29&res=userid%2F154787&et=1697532276&method=sha1&sign=xq%2BF31uka96lTTAPqwO1LIs%2BhX8%3D"
		pRet=SendATCommand(tempbuff,"OK",5);;//查询下链接状态
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	while(1)   
	{
		pRet=SendATCommand("AT+HTTPPARA=\"USER_DEFINED\",\"Content-Type: application/json\"\r","OK",5);;//查询下链接状态
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	
	snprintf(chbuff,sizeof(chbuff),"{\"step\":%d} \r",step);//"{\"s_version\":\"v1.9\",\"f_version\":\"v1.9\"}\r"
	while(1)   
	{
		sprintf(chbuff2,"AT+HTTPDATA=%d,5000\r",strlen(chbuff));
		pRet=SendATCommand(chbuff2,"DOWNLOAD",5);;//查询下链接状态
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	_4GCat1_SendBuff((u8 *)chbuff,strlen(chbuff));
	while(1)
	{
		if(_4GCat1_read((u8 *)prxBuffer,300,300)>0)  
		{
			pRet=strstr(prxBuffer, "OK"); 
			if(pRet!=0)
			{
				break;
			}
			
		}
	}
	
	while(1)   
	{
		pRet=SendATCommand("AT+HTTPACTION=1\r","OK",5);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	for(i=0;i<3;i++)
	{
		
		if(_4GCat1_read((u8 *)prxBuffer,UART_BUFFER_LEN,100)>0)  
		{
			pRet=strstr(prxBuffer, "+HTTPACTION:"); 
			if(pRet==0)	
			{
				continue;
			}
			break; 
		}
		else
		{
			continue;
		}   
	}
	while(1)   //查询HTTP服务响应
	{
		pRet=SendATCommand("AT+HTTPREAD\r","OK",5);//查询 HTTP 服务响应： AT+HTTPREAD
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	
	//AT+HTTPTERM
	while(1)   //终止HTTP 任务
	{
		pRet=SendATCommand("AT+HTTPTERM\r","OK",5);
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	
}
void cJSON_test(void)
{
	char* json_str = "{\"code\":12012,\"msg\":\"not exist\",\"request_id\":\"3080be42ad52442294ae06e1b9c9c8b2\"}";
	cJSON* root = cJSON_Parse(json_str);
	const cJSON* code = cJSON_GetObjectItem(root, "code");
//	printf("name: %s\n", name->valuestring);

	const cJSON* msg = cJSON_GetObjectItem(root, "msg");
//	printf("age: %d\n", age->valueint);

	const cJSON* request_id = cJSON_GetObjectItem(root, "request_id");
//	printf("gender: %s\n", gender->valuestring);
	code=code;
}

/*
4G模块进入低功耗指令
4G模块上电后默认  关闭模块睡眠功能AT+CSCLK=0
进入低功耗后  无论是串口还是从云平台接收数据  都会丢失第一次通信数据(唤醒4G模块)    
*/
u8 GPRS_LowPower(void)
{
	char *pRet=0;
	for(u8 i=0;i<3;i++)   
	{
		//数据业务（包括TCPIP,HTTP,MQTT,FTP）到来时的URC上报，会产生一个120ms低脉冲
		pRet=SendATCommand("AT+CFGRI=1\r","OK",50); 
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	
	for(u8 i=0;i<3;i++)   
	{
		//POWERMODE
		//pRet=SendATCommand("AT+POWERMODE=\"PSM\"\r","OK",50); //查询网络附着状态
		pRet=SendATCommand("AT+CSCLK=3\r","OK",50); 
		if(pRet!=NULL)
		{
			break;
		}
		delay_ms(500);
	}
	
	
}

void Set_DevCode(void)
{
	if(nosave_par.Devcode_SetF != 1)
		return ;
	
	//开机连接onenet平台
	GsmShutDown();
	PowerUpGSM();
	InitSAPBR();
	InitNTP();
	GPRS_connect2(onenet_info.ip,onenet_info.port);
	
	//申请设备编码
	int ret = Apply_Devcode();
	if(ret == 1)
	{
		//将onenet设备与设备编码绑定  ret==1 绑定成功
		ret = Bind_Devcode();
		if(ret == 1)
		{
			nosave_par.Devcode_SetF=0;
		}
	}
}

u8 GPRS_LowPower_wake(void)
{
	char *pRet=0;
	
	//模块休眠后发送7次能唤醒
	for(u8 i=0;i<GPRS_WAKECOUNT;i++)
	{
		pRet=SendATCommand("AT\r","OK",50); 
		if(pRet!=NULL)
		{
			break;
		}
		else if((i+1)==GPRS_WAKECOUNT)
		{
			Statlog_Par._4G=2;    //4G连接故障
			SEGGER_RTT_printf(0,RTT_CTRL_TEXT_GREEN"GPRS_WAKECOUNT has arrived,No Response!\r\n");
		}
		delay_ms(500);
	}
}

