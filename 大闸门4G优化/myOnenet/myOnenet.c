#include "myOnenet.h"
#include "mqttkit.h"
#include "string.h"
#include "base64.h"
#include "hmac_sha1.h"
#include "stdio.h"
#include "cJSON.h"
#include "StallGuard.h"
#include "TMC5160A.h"
#include "ff_user.h"
/*   
设备id-设备key-设备名称
产品id-产品名称-产品key
ip地址-端口
*/
/*5832197465123*/
ONETNET_INFO onenet_info_dev1 = {"2469453573", "WjJSeXp4TFlQNjJveE91bWdMajBzRDZlTGxrYzlIajk=", "5832197465123",
							"mWLGM6Wq1p","GUAN_1", "rwuRP8k3+0kNT5lD6e8TzdsrdxvrXb89E5Mtl38LSc8=",
							"183.230.40.96", "1883",1, 0, 0, 1, 0, 0, 0, 0, 0, 0};	
/*3628501749365*/
ONETNET_INFO onenet_info_dev2 = {"2469452975", "aXVVckNIN25oTE81d2lSY2NSbjJyQkp0RTlydkswOHI=", "3628501749365",
							"mWLGM6Wq1p","GUAN_1", "rwuRP8k3+0kNT5lD6e8TzdsrdxvrXb89E5Mtl38LSc8=",
							"183.230.40.96", "1883",1, 0, 0, 1, 0, 0, 0, 0, 0, 0};
/*4957281038462*/
ONETNET_INFO onenet_info_dev3 = {"2469453111", "ZWw4V0ZJbkZmSlk1c1BnSldkVW5CMVlUMjBpRWJKZU8=", "4957281038462",
							"mWLGM6Wq1p","GUAN_1", "rwuRP8k3+0kNT5lD6e8TzdsrdxvrXb89E5Mtl38LSc8=",
							"183.230.40.96", "1883",1, 0, 0, 1, 0, 0, 0, 0, 0, 0};
/*6543210987654*/
ONETNET_INFO onenet_info_dev4 = {"2469453199", "Tm5UdFdhMzg0TVhNTjhlbzVDWk1BbVA3RlowVnh1VGw=", "6543210987654",
							"mWLGM6Wq1p","GUAN_1", "rwuRP8k3+0kNT5lD6e8TzdsrdxvrXb89E5Mtl38LSc8=",
							"183.230.40.96", "1883",1, 0, 0, 1, 0, 0, 0, 0, 0, 0};
/*3628501749365*/
ONETNET_INFO onenet_info_dev5 = {"2282028087", "S1JmVkZkaVRMUzhxTnJXMzBYM1p5VmE3YTU1ZUxadkU=", "4765149792440",
							"mWLGM6Wq1p","GUAN_1", "rwuRP8k3+0kNT5lD6e8TzdsrdxvrXb89E5Mtl38LSc8=",
							"183.230.40.96", "1883",1, 0, 0, 1, 0, 0, 0, 0, 0, 0};
ONETNET_INFO onenet_info_gaizao = {"2472437758", "R25WMWM4T0tmNTNFc3hycmVCUjBrdmNHaUlBYTNGRXk=", "6123456789017",
							"mWLGM6Wq1p","GUAN_1", "rwuRP8k3+0kNT5lD6e8TzdsrdxvrXb89E5Mtl38LSc8=",
							"183.230.40.96", "1883",1, 0, 0, 1, 0, 0, 0, 0, 0, 0};			
/*测试账号1*/
ONETNET_INFO onenet_info_test1 = {"2125830313", "UGxwUFVsVW4ya09aZjJNMnhaZ3F0d0JFUGoxV0ZjWDM=", "test1",
							"mWLGM6Wq1p","GUAN_1", "rwuRP8k3+0kNT5lD6e8TzdsrdxvrXb89E5Mtl38LSc8=",
							"183.230.40.96", "1883",1, 0, 0, 1, 0, 0, 0, 0, 0, 0};
/*测试账号3*/
ONETNET_INFO onenet_info_test3 = {"2501969121", "UXVLNVJKY2RLaUJ2UGFHdEVEYktjWlZkeE1MU216aHg=", "test3",
							"mWLGM6Wq1p","GUAN_1", "rwuRP8k3+0kNT5lD6e8TzdsrdxvrXb89E5Mtl38LSc8=",
							"183.230.40.96", "1883",1, 0, 0, 1, 0, 0, 0, 0, 0, 0};
/*退水渠*/
ONETNET_INFO onenet_info_tui   = {"2126603765", "RXNZeHU1MEVnZ0IxTVZiN3BFUWNhRk5CN1o0bWw1Z3I=", "6963174682409",
							"mWLGM6Wq1p","GUAN_1", "rwuRP8k3+0kNT5lD6e8TzdsrdxvrXb89E5Mtl38LSc8=",
							"183.230.40.96", "1883",1, 0, 0, 1, 0, 0, 0, 0, 0, 0};
/*毛渠*/
ONETNET_INFO onenet_info_mao   = {"2126603793", "bUMwT1V4VVY0OE0wN04xdXJVUlE0OE5ISUhOaG4yOVI=", "1492607234613",
							"mWLGM6Wq1p","GUAN_1", "rwuRP8k3+0kNT5lD6e8TzdsrdxvrXb89E5Mtl38LSc8=",
							"183.230.40.96", "1883",1, 0, 0, 1, 0, 0, 0, 0, 0, 0};
/*张宝渠*/
ONETNET_INFO onenet_info_zhang = {"2126603816", "ejNSYW1POEVDblUxWVJhakViNWU3aUFpTTQ5SXF1UlI=", "5279560816674",
							"mWLGM6Wq1p","GUAN_1", "rwuRP8k3+0kNT5lD6e8TzdsrdxvrXb89E5Mtl38LSc8=",
							"183.230.40.96", "1883",1, 0, 0, 1, 0, 0, 0, 0, 0, 0};

/*北孙闸站*/
ONETNET_INFO onenet_info_beisunzha   = {"2501334307", "UjhLMENrN0daejlPajZ1WTdMVUhSeE9aN09wSlBWV0I=", "7123456789018",
							"mWLGM6Wq1p","GUAN_1", "rwuRP8k3+0kNT5lD6e8TzdsrdxvrXb89E5Mtl38LSc8=",
							"183.230.40.96", "1883",1, 0, 0, 1, 0, 0, 0, 0, 0, 0};
/*九支渠分渠*/
ONETNET_INFO onenet_info_jiuzhiqu   = {"2501335408", "VVhxTFMyWHFqdVNETDFaUktUeW5JTjl5NGRzSGhRZDU=", "3123456789023",
							"mWLGM6Wq1p","GUAN_1", "rwuRP8k3+0kNT5lD6e8TzdsrdxvrXb89E5Mtl38LSc8=",
							"183.230.40.96", "1883",1, 0, 0, 1, 0, 0, 0, 0, 0, 0};
/*三支分渠*/
ONETNET_INFO onenet_info_sanzhifenqu   = {"250133563", "SDNKdUxwRFhPTUdVVlZyWmhlU2ZVTTk4djZyZnBabDk=", "1123456789025",
							"mWLGM6Wq1p","GUAN_1", "rwuRP8k3+0kNT5lD6e8TzdsrdxvrXb89E5Mtl38LSc8=",
							"183.230.40.96", "1883",1, 0, 0, 1, 0, 0, 0, 0, 0, 0};
/*四支分渠*/
ONETNET_INFO onenet_info_sizhifenqu   = {"2501335796", "cDVJakJLaEF5dnlCUHJMRUd2THNIYjdhUFdDMHZWcko=", "6123456789026",
							"mWLGM6Wq1p","GUAN_1", "rwuRP8k3+0kNT5lD6e8TzdsrdxvrXb89E5Mtl38LSc8=",
							"183.230.40.96", "1883",1, 0, 0, 1, 0, 0, 0, 0, 0, 0};
/*石川河闸口*/
ONETNET_INFO onenet_info_shichuanhezha  = {"2501336327", "dkZsandoOWxqSU5MdXBPYVJpd0FIR1RCdkpqRHU0MzM=", "4623456789022",
							"mWLGM6Wq1p","GUAN_1", "rwuRP8k3+0kNT5lD6e8TzdsrdxvrXb89E5Mtl38LSc8=",
							"183.230.40.96", "1883",1, 0, 0, 1, 0, 0, 0, 0, 0, 0};

/********************************************************************************************************************/
/*
设备信息赋值						//更改ONENET_CHANGED_FLAG后，拷贝的onenet_info结构体也应跟着更改
*/
#define ONENET_CHANGED_FLAG 6
void myOnenet_infoSet(void)								
{
	if(onenet_info.set_flag!=ONENET_CHANGED_FLAG)
	{
		memcpy(&onenet_info,&onenet_info_sanzhifenqu,sizeof(onenet_info));
		memcpy(dev_SavePar.Upload_URL,onenet_info.ip,sizeof(onenet_info.ip));
		onenet_info.set_flag=ONENET_CHANGED_FLAG;
		Flash_SaveONENET_Par();
		//onenet_info.dev_name onenet的设备名称需要以设备编码命名
		memcpy(dev_SavePar.dev_code,onenet_info.dev_name,sizeof(dev_SavePar.dev_code));
		Flash_SaveDevData();
	}
}
/*
检查与onenet连接
*/
u8 myOnenet_CheckLink(void)
{
	u8 ret=0;
	u8 i=0;
	char *pRet=0;
	
	for(i=0;i<3;i++)
	{
		pRet=SendATCommand("AT+MQTTSTATU\r","OK",50); 		//查询网络附着状态
		if(pRet!=NULL)
		{
			if(strstr(prxBuffer,"+MQTTSTATU :1")!=NULL)
			{
				char temp[64]="";
				snprintf(temp,sizeof(temp),"%hu-%hhu-%hhu %hhu:%hhu:%hhu",g_rtc.years,g_rtc.months,g_rtc.days,g_rtc.hours,g_rtc.minutes,g_rtc.secs);
				ret = OneNET_SendData2("Online_Time",temp,TYPE_STRING);                       //发送本地时间
				if(ret == 0)
				{
					Statlog_Par._4G=1;
					SEGGER_RTT_printf(0,RTT_CTRL_TEXT_GREEN"MQTT ONENET:CONNNECT\r\n");
				}
				else
				{
					Statlog_Par._4G=2;    //4G连接故障--->与平台丢失连接
					SEGGER_RTT_printf(0,RTT_CTRL_TEXT_GREEN"MQTT ONENET:DISCONNNECT MQTTSTATU=1\r\n");
				}
				return 0;
			}
			else if(strstr(prxBuffer,"+MQTTSTATU :0")!=NULL)
			{
				Statlog_Par._4G=2;    //4G连接故障
				SEGGER_RTT_printf(0,RTT_CTRL_TEXT_GREEN"MQTT ONENET:DISCONNNECT MQTTSTATU=0\r\n");
				return 1;
			}
			else if(strstr(prxBuffer,"+MQTTSTATU :2")!=NULL)
			{
				Statlog_Par._4G=2;    //4G连接故障
				SEGGER_RTT_printf(0,RTT_CTRL_TEXT_GREEN"MQTT ONENET:DISCONNNECT MQTTSTATU=2\r\n");
				return 1;
			}
			break;
		}
		delay_ms(500);
	}
}
/*
重新连接
*/

/*
************************************************************
*	函数名称：	OTA_UrlEncode
*
*	函数功能：	sign需要进行URL编码
*
*	入口参数：	sign：加密结果
*
*	返回参数：	0-成功	其他-失败
*
*	说明：		+			%2B
*				空格		%20
*				/			%2F
*				?			%3F
*				%			%25
*				#			%23
*				&			%26
*				=			%3D
************************************************************
*/
static unsigned char OTA_UrlEncode(char *sign)
{

	char sign_t[40];
	unsigned char i = 0, j = 0;
	unsigned char sign_len = strlen(sign);
	
	if(sign == (void *)0 || sign_len < 28)
		return 1;
	
	for(; i < sign_len; i++)
	{
		sign_t[i] = sign[i];
		sign[i] = 0;
	}
	sign_t[i] = 0;
	
	for(i = 0, j = 0; i < sign_len; i++)
	{
		switch(sign_t[i])
		{
			case '+':
				strcat(sign + j, "%2B");j += 3;
			break;
			
			case ' ':
				strcat(sign + j, "%20");j += 3;
			break;
			
			case '/':
				strcat(sign + j, "%2F");j += 3;
			break;
			
			case '?':
				strcat(sign + j, "%3F");j += 3;
			break;
			
			case '%':
				strcat(sign + j, "%25");j += 3;
			break;
			
			case '#':
				strcat(sign + j, "%23");j += 3;
			break;
			
			case '&':
				strcat(sign + j, "%26");j += 3;
			break;
			
			case '=':
				strcat(sign + j, "%3D");j += 3;
			break;
			
			default:
				sign[j] = sign_t[i];j++;
			break;
		}
	}
	
	sign[j] = 0;
	
	return 0;

}

#define METHOD		"sha1"
static char sign_buf[64];								//保存签名的Base64编码结果 和 URL编码结果
static char hmac_sha1_buf[64];							//保存签名
static char access_key_base64[64];						//保存access_key的Base64编码结合
static char string_for_signature[72];					//保存string_for_signature，这个是加密的key
/*
OneNET_Authorization_V1("2018-10-31", (char *)onenet_info.pro_id, 1956499200, (char *)onenet_info.key, (char *)onenet_info.dev_name,
								authorization_buf, sizeof(authorization_buf), 0);
*/
 unsigned char OneNET_Authorization_V1(char *res, char *access_key, char *dev_name,
											char *authorization_buf, unsigned short authorization_buf_len, _Bool flag)
{
	
	size_t olen = 0;
	char *ver;
	unsigned int et;
	ver="2018-10-31";
//et=2524579200;
	et=1956499200;
//----------------------------------------------------参数合法性--------------------------------------------------------------------
	if(ver == (void *)0 || res == (void *)0 || et < 1564562581 || access_key == (void *)0
		|| authorization_buf == (void *)0 || authorization_buf_len < 120)
		return 1;
	
//----------------------------------------------------将access_key进行Base64解码----------------------------------------------------
	memset(access_key_base64, 0, sizeof(access_key_base64));
	BASE64_Decode((unsigned char *)access_key_base64, sizeof(access_key_base64), &olen, (unsigned char *)access_key, strlen(access_key));
	//UsartPrintf(USART_DEBUG, "access_key_base64: %s\r\n", access_key_base64);
//----------------------------------------------------计算string_for_signature-----------------------------------------------------
	memset(string_for_signature, 0, sizeof(string_for_signature));
	if(flag==1)
		snprintf(string_for_signature, sizeof(string_for_signature), "%d\n%s\nproducts/%s\n%s", et, METHOD, res, ver);
	else
		snprintf(string_for_signature, sizeof(string_for_signature), "%d\n%s\nproducts/%s/devices/%s\n%s", et, METHOD, res, dev_name, ver);

	
//----------------------------------------------------加密-------------------------------------------------------------------------
	memset(hmac_sha1_buf, 0, sizeof(hmac_sha1_buf));
	
	hmac_sha1((unsigned char *)access_key_base64, strlen(access_key_base64),
				(unsigned char *)string_for_signature, strlen(string_for_signature),
				(unsigned char *)hmac_sha1_buf);

	//UsartPrintf(USART_DEBUG, "hmac_sha1_buf: %s\r\n", hmac_sha1_buf);
	
//----------------------------------------------------将加密结果进行Base64编码------------------------------------------------------
	olen = 0;
	memset(sign_buf, 0, sizeof(sign_buf));
//	base64_encode( (unsigned char *)hmac_sha1_buf, strlen(hmac_sha1_buf),(unsigned char *)sign_buf, &olen);
	BASE64_Encode((unsigned char *)sign_buf, sizeof(sign_buf), &olen, (unsigned char *)hmac_sha1_buf, buf_getlen(hmac_sha1_buf,sizeof(hmac_sha1_buf)));


//----------------------------------------------------将Base64编码结果进行URL编码---------------------------------------------------
	OTA_UrlEncode(sign_buf);
	//UsartPrintf(USART_DEBUG, "sign_buf: %s\r\n", sign_buf);


//----------------------------------------------------计算Token--------------------------------------------------------------------
		if(flag)
		snprintf(authorization_buf, authorization_buf_len, "version=%s&res=products%%2F%s&et=%d&method=%s&sign=%s", ver, res, et, METHOD, sign_buf);
	else
		snprintf(authorization_buf, authorization_buf_len, "version=%s&res=products%%2F%s%%2Fdevices%%2F%s&et=%d&method=%s&sign=%s", ver, res, dev_name, et, METHOD, sign_buf);
	//UsartPrintf(USART_DEBUG, "Token: %s\r\n", authorization_buf);
	
	return 0;

}
/*
处理数据
发送 reset 重启
*/
void MyOnenet_ProcessCmd(void)
{
	cJSON * cJSON_cmd;
	cJSON * root ;
	root = cJSON_Parse(Cmd_Buff);
	if(root==NULL)
	{
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_RED"%s\r\n",Cmd_Buff);
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_RED"Invalid JSON!\r\n");
		return;
	}
	
	/*控制模式  control_mode*/
	cJSON_cmd=cJSON_GetObjectItem(root,"control_mode");
	if((cJSON_cmd!=NULL) && (cJSON_cmd->type==cJSON_Number))
	{
		if(((cJSON_cmd->valueint) < 0) || ((cJSON_cmd->valueint) > 2))
		{
			SaveOPERA(REMOTO_CONTROL_MODE,&Control_Par.control_mode,"Error_Mode");
			cJSON_Delete(root);
			return;
		}
			
		SaveOPERA(REMOTO_CONTROL_MODE,&Control_Par.control_mode,&(cJSON_cmd->valueint));
		Control_Par.control_mode=cJSON_cmd->valueint;
		
		switch (Control_Par.control_mode)
		{
			case 0:
					/*设定开度  set_opening*/
					cJSON_cmd=cJSON_GetObjectItem(root,"set_opening");
					if((cJSON_cmd!=NULL) && (cJSON_cmd->type==cJSON_Number))
					{
						SaveOPERA(REMOTO_CONTROL_OPENING,&Control_Par.set_openingm,&(cJSON_cmd->valuedouble));
						Control_Par.set_openingm=cJSON_cmd->valuedouble;
						
						//校验数据合法值
						if(Control_Par.set_openingm>dev_SavePar.water_inletHeight)
						{
							Control_Par.set_openingm=dev_SavePar.water_inletHeight;
						}
						if(Control_Par.set_openingm<0)
						{
							Control_Par.set_openingm=0;
						}
					}
				break;
			case 1:
					/*设定水位  set_level*/
					cJSON_cmd=cJSON_GetObjectItem(root,"set_level");
					if((cJSON_cmd!=NULL) && (cJSON_cmd->type==cJSON_Number))
					{
						SaveOPERA(REMOTO_CONTROL_LEVEL,&Control_Par.set_level,&cJSON_cmd->valuedouble);
						Control_Par.set_level=cJSON_cmd->valuedouble;
						
						//校验数据合法值
						if(Control_Par.set_level>dev_SavePar.level_max)
						{
							Control_Par.set_level=dev_SavePar.level_max;
						}
						if(Control_Par.set_level<0)
						{
							Control_Par.set_level=0;
						}
					}
				break;
			case 2:
					/*设定流量  set_level*/
					cJSON_cmd=cJSON_GetObjectItem(root,"set_flow");
					if((cJSON_cmd!=NULL) && (cJSON_cmd->type==cJSON_Number))
					{
						SaveOPERA(REMOTO_CONTROL_FLOW,&Control_Par.set_flow,&(cJSON_cmd->valuedouble));
						Control_Par.set_flow=cJSON_cmd->valuedouble;
						
						//校验数据合法值
						if(Control_Par.set_flow>dev_SavePar.flow_max)
						{
							Control_Par.set_flow=dev_SavePar.flow_max;
						}
						if(Control_Par.set_flow<0)
						{
							Control_Par.set_flow=0;
						}
					}
				break;
			default:
				break;
		}
		
		/*运行开关  control_run*/
		cJSON_cmd=cJSON_GetObjectItem(root,"control_run");
		if((cJSON_cmd!=NULL) && (cJSON_cmd->type==cJSON_Number))
		{
			SaveOPERA(REMOTO_CONTROL_RUN,&Control_Par.control_run,&(cJSON_cmd->valueint));
			Control_Par.control_run=cJSON_cmd->valueint;
			
			if(Control_Par.control_run==1)
			{
				Control_Par.setState=GATE_STOP;
				StallGuardResetCheck();
			}
			else if(Control_Par.control_run==0)
			{
				Control_Par.setState=GATE_STOP;
				TMC5160A_STOP();
			}
		}
	}
	
	/*重启*/
	cJSON_cmd=cJSON_GetObjectItem(root,"reset");
	if((cJSON_cmd!=NULL) && (cJSON_cmd->type==cJSON_Number))
	{
		if(cJSON_cmd->valueint==1)
		{
			__set_FAULTMASK(1); // 关闭所有中端
			NVIC_SystemReset(); // 复位
		}
	}
	
	cJSON_Delete(root);
}
void ATimerCallback(TimerHandle_t xTimer)
{
	
}
void OneNET_CmdHandle2(void)
{
	if(nosave_par.URC_ComF != 1)
		return ;
	nosave_par.URC_ComF=0;
	
	u8 ret = 0;
	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};										//协议包
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short topic_len = 0;
	unsigned short req_len = 0;
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	unsigned char *ipdPtr = NULL;		//数据指针
	
	char strRxBuff[300];
	char strTopic[100];
	char strLen[10];
	char strMessage[100];
	s32 rxlen;
	u8 i=0;
	char * pRet;
	char * pRet2;
/**********************************************************/	
	//防止模块睡眠 导致 无回复
	GPRS_LowPower_wake();
	//打印收到的所有的订阅消息：AT+MQTTMSGGET
	pRet=SendATCommand("AT+MQTTMSGGET\r","OK",50); 
	/*
		+MSUB: "$sys/mWLGM6Wq1p/test1/cmd/request/112f6af1-cb92-4a5f-95ef-5bbd0c304ea9",10 byte,{"test":1}
	*/
	//对收到的消息prxBuffer进行切割
	pRet=strstr(prxBuffer,"+MSUB:");
	if(pRet!=NULL)
	{
		//打印切割的消息
//		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_GREEN"%s\r\n",_4GCat1_Usart.rxbuffer);	
		memset(Cmd_Buff,0,sizeof(Cmd_Buff));
		ret = OneNETAdd_RevPro((u8 *)prxBuffer);					//回复平台和提取消息内容
		
		if((strlen(Cmd_Buff)>0) && (ret==0))
		{
			MyOnenet_ProcessCmd();    //处理命令
		}
	}
	return;
}
void OneNETAdd_RevPro2(char * strTopic,char *strLen,char *strMessage)
{
	char strtemp[100];
	char strtemp2[50];

	char * strPtrtemp;
	char * strPtrtemp2;
	memset(strtemp,0,sizeof(strtemp));
	snprintf(strtemp,sizeof(strtemp),"$sys/%s/%s/cmd/request/",onenet_info.pro_id,onenet_info.dev_name);
	strPtrtemp=strstr((char *)strTopic,strtemp);
	if(strPtrtemp!=NULL) //找到主题
	{
		strPtrtemp2=strstr(strPtrtemp,"request/");
		strPtrtemp=strstr(strPtrtemp2,"\"");
		memset(strtemp2,0,sizeof(strtemp2));
		memcpy(strtemp2,(strPtrtemp2+8),(strPtrtemp-strPtrtemp2-8));
		sprintf(strtemp,"$sys/%s/%s/cmd/response/%s",onenet_info.pro_id,onenet_info.dev_name,strtemp2);
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_CYAN"topic:%s\r\n",strtemp);
		

		OneNET_SendCMD(strtemp);
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_GREEN"%s\r\n",Cmd_Buff);
		//+MSUB: "mqtt/topic",9 byte,SSSSddddd
		
	}
}

