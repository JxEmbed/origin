/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	onenet.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-05-27
	*
	*	版本： 		V1.0
	*
	*	说明： 		OneNET平台应用示例
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
//#include "stm32f10x.h"

//网络设备
//#include "net_device.h"

//协议文件
#include "onenet.h"
#include "fault.h"
#include "mqttkit.h"
#include <stdio.h>
//协议分析
#include "protocol_parser.h"

//命令回调
#include "cmd_callback.h"

//算法
#include "base64.h"
#include "hmac_sha1.h"

#include "StallGuard.h"

//硬件驱动
//#include "usart.h"
//#include "delay.h"
//#include "GPRS.h"
//C库
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "data.h"


unsigned char *Temp_Buffer;
extern DATA_STREAM data_stream[];
extern DATA_STREAM data_stream_temperture[];//
extern DATA_STREAM data_stream_moisture[];
extern DATA_STREAM data_stream_Water_Flow[];
extern DATA_STREAM data_stream_Water_Speed[];
extern DATA_STREAM data_stream_Water_Level[];

extern unsigned char data_stream_cnt;
extern unsigned char data_stream_cnt_st;
extern unsigned char data_stream_cnt_sm;		
extern unsigned char data_stream_cnt_Water_Flow;	
extern unsigned char data_stream_cnt_Water_Speed;	
extern unsigned char data_stream_cnt_Water_Level;	

extern char *topics[];
extern unsigned char topics_num;
NET_DEVICE_INFO net_device_info = {NULL, NULL, NULL, NULL, NULL,
									0, 0, 0, 60,
									0, 0, 0, 0, 0, 1, 0};
						
		
ONETNET_INFO onenet_info;

ONETNET_INFO onenet_info2 = {"2268614692", "RmU0amNXYVdQaks4U1VsQ29xM080Vm80ZlZxSVpSZEk=", "8389267285345",
							"mWLGM6Wq1p","GUAN_1", "rwuRP8k3+0kNT5lD6e8TzdsrdxvrXb89E5Mtl38LSc8=",
							"183.230.40.96", "1883",1, 0, 0, 1, 0, 0, 0, 0, 0, 0};	
ONETNET_INFO onenet_info3 = {"2282028087", "S1JmVkZkaVRMUzhxTnJXMzBYM1p5VmE3YTU1ZUxadkU=", "4765149792440",
							"mWLGM6Wq1p","GUAN_1", "rwuRP8k3+0kNT5lD6e8TzdsrdxvrXb89E5Mtl38LSc8=",
							"183.230.40.96", "1883",1, 0, 0, 1, 0, 0, 0, 0, 0, 0};	
ONETNET_INFO onenet_info4 = {"2356490353", "QVZNVm9GMGFZYUo3d2dpZmF0VHFlV3hvSGlDQk9wRVc=", "2786254476478",
							"mWLGM6Wq1p","GUAN_1", "rwuRP8k3+0kNT5lD6e8TzdsrdxvrXb89E5Mtl38LSc8=",
							"183.230.40.96", "1883",1, 0, 0, 1, 0, 0, 0, 0, 0, 0};	
ONETNET_INFO onenet_info5 = {"2356852323", "NFliYVRnWGNKbkhONmZrbjlicDFmNXhsVDVMOXJWaHM=", "5386641057941",
							"mWLGM6Wq1p","GUAN_1", "rwuRP8k3+0kNT5lD6e8TzdsrdxvrXb89E5Mtl38LSc8=",
							"183.230.40.96", "1883",1, 0, 0, 1, 0, 0, 0, 0, 0, 0};	

char *topics[4] = {NULL, NULL, NULL,NULL};

unsigned char topics_num = 0;

#define TOPIC_BYTES			56
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

/*
************************************************************
*	函数名称：	OTA_Authorization
*
*	函数功能：	计算Authorization
*
*	入口参数：	ver：参数组版本号，日期格式，目前仅支持格式"2018-10-31"
*				res：产品id
*				et：过期时间，UTC秒值
*				access_key：访问密钥
*				dev_name：设备名
*				authorization_buf：缓存token的指针
*				authorization_buf_len：缓存区长度(字节)
*
*	返回参数：	0-成功	其他-失败
*
*	说明：		当前仅支持sha1
************************************************************
*/

#define METHOD		"sha1"
char sign_buf[64];								//保存签名的Base64编码结果 和 URL编码结果
char hmac_sha1_buf[64];							//保存签名
char access_key_base64[64];						//保存access_key的Base64编码结合
char string_for_signature[72];					//保存string_for_signature，这个是加密的key
//sign = base64(hmac_<method>(base64decode(key), utf-8(StringForSignature)))
 unsigned char OneNET_Authorization(char *ver, char *res, unsigned int et, char *access_key, char *dev_name,
											char *authorization_buf, unsigned short authorization_buf_len, _Bool flag)
{
	
	size_t olen = 0;
	
	

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

//==========================================================
//	函数名称：	OneNET_RegisterDevice
//
//	函数功能：	在产品中注册一个设备
//
//	入口参数：	access_key：访问密钥
//				pro_id：产品ID
//				serial：唯一设备号
//				devid：保存返回的devid
//				key：保存返回的key
//
//	返回参数：	0-成功		1-失败
//
//	说明：		
//==========================================================
_Bool OneNET_RegisterDevice(const char *access_key, const char *pro_id, const char *serial, char *devid, char *key)
{

	_Bool result = 1;
	unsigned short send_len = 11 + strlen(serial);
	unsigned char time_out = 200;
	char *send_ptr = NULL, *data_ptr = NULL;
	
	char authorization_buf[144];													//加密的key
	
	if(!net_device_info.net_work)
		return result;
	
	send_ptr = NET_MallocBuffer(send_len + 240);
	if(send_ptr == NULL)
		return result;
	
	//NET_DEVICE_Close();
	GPRS_disconnet();
	
	//if(NET_DEVICE_Connect("TCP", "183.230.40.33", "80") == 0) GPRS_connect("183.230.40.42","987");
	if(GPRS_connect("183.230.40.96","1883") == 0)
	{
		OneNET_Authorization("2018-10-31", onenet_info.pro_id, 1956499200, onenet_info.access_key, NULL,
								authorization_buf, sizeof(authorization_buf), 1);
		
		snprintf(send_ptr, 240 + send_len, "POST /mqtt/v1/devices/reg HTTP/1.1\r\n"
						"Authorization:%s\r\n"
						"Host:ota.heclouds.com\r\n"
						"Content-Type:application/json\r\n"
						"Content-Length:%d\r\n\r\n"
						"{\"name\":\"%s\"}",
		
						authorization_buf, 11 + strlen(serial), serial);
		
		if(!GPRS_send_data((unsigned char *)send_ptr,(unsigned char *)send_ptr, strlen(send_ptr)))
		{
			//{"device_id":"553740003","name":"mcu_id_43118237","pid":282932,"key":"OAyPH9GfODmxVvUoWviFfEGkbSkZxDzRaPQBLs5gcO0="}
			
			net_device_info.cmd_ipd = NULL;
			
			while(--time_out)
			{
				if(net_device_info.cmd_ipd != NULL)
					break;
				
				delay_ms(20);
				
			}
			
			if(time_out)
			{
				data_ptr = strstr(net_device_info.cmd_ipd, "device_id");
			}
			
			if(data_ptr)
			{
				char name[16];
				int pid = 0;
				
				if(sscanf(data_ptr, "device_id\":\"%[^\"]\",\"name\":\"%[^\"]\",\"pid\":%d,\"key\":\"%[^\"]\"", devid, name, &pid, key) == 4)
				{
					//UsartPrintf(USART_DEBUG, "create device: %s, %s, %d, %s\r\n", devid, name, pid, key);
					result = 0;
				}
			}
		}
		
		NET_FreeBuffer(send_ptr);
		//NET_DEVICE_Close();
		GPRS_disconnet();
	}
	
	return result;

}

//==========================================================
//	函数名称：	OneNET_ConnectIP
//
//	函数功能：	连接IP
//
//	入口参数：	ip：IP地址缓存指针
//				port：端口缓存指针
//
//	返回参数：	返回初始化结果
//
//	说明：		0-成功		1-失败
//==========================================================
_Bool OneNET_ConnectIP(char *ip, char *port)
{

	_Bool result = 1;
	
	if(!net_device_info.net_work)									//网络模组具备网络连接能力
		return result;
	
	if(onenet_info.connect_ip)										//已经连接了IP
		return !result;
	
	if(!GPRS_connect(ip, port))
	{
		result = 0;
		net_fault_info.net_fault_count = 0;
		onenet_info.connect_ip = 1;
	}
	else
	{
		//RTOS_TimeDly(100);
		delay_ms(100);
		
		if(++onenet_info.err_count >= 5)
		{
			onenet_info.err_count = 0;
			//UsartPrintf(USART_DEBUG, "请检查IP地址和PORT是否正确\r\n");
		}
	}
	
	return result;

}

//==========================================================
//	函数名称：	OneNET_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	dev_name：创建设备的设备名
//				pro_id：产品ID
//				key：注册设备时获取的key
//
//	返回参数：	无
//
//	说明：		与onenet平台建立连接，成功或会标记oneNetInfo.netWork网络状态标志
//==========================================================
static char authorization_buf[160];
void OneNET_DevLink(const char *dev_name, const char *pro_id, const char *key)
{
	
	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};					//协议包
	
	unsigned char timeOut = 200;
	
	
	//计算鉴权
	OneNET_Authorization("2018-10-31", (char *)pro_id, 1956499200, (char *)key, (char *)dev_name,
								authorization_buf, sizeof(authorization_buf), 0);
	
	//UsartPrintf(USART_DEBUG, "OneNET_DevLink\r\n"
							//"NAME: %s,	PROID: %s,	KEY:%s\r\n"
                       // , dev_name, pro_id, authorization_buf);
	
	if(MQTT_PacketConnect(pro_id, authorization_buf, dev_name, 256, 1, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqtt_packet) == 0)
	{
		Protocol_Parser_Print(mqtt_packet._data);
		GPRS_send_len(mqtt_packet._len);
		GPRS_send_data(mqtt_packet._data,mqtt_packet._data, mqtt_packet._len);
		//NET_DEVICE_SendData(mqtt_packet._data, mqtt_packet._len);			//上传平台
		//NET_DEVICE_AddDataSendList(mqtt_packet._data, mqtt_packet._len, 1);//加入链表
		
		MQTT_DeleteBuffer(&mqtt_packet);									//删包
		
		while(!onenet_info.net_work && --timeOut)
			//RTOS_TimeDly(2);
			delay_ms(2);
	}
	else
		//UsartPrintf(USART_DEBUG, "WARN:	MQTT_PacketConnect Failed\r\n");
	;
	
	if(onenet_info.net_work)											//如果接入成功
	{
		onenet_info.err_count = 0;
	}
	else
	{
		if(++onenet_info.err_count >= 5)								//如果超过设定次数后，还未接入平台
		{
			onenet_info.net_work = 0;
			onenet_info.err_count = 0;
			
			onenet_info.err_check = 1;
		}
	}
	
}

//==========================================================
//	函数名称：	OneNET_DisConnect
//
//	函数功能：	与平台断开连接
//
//	入口参数：	无
//
//	返回参数：	0-成功		1-失败
//
//	说明：		
//==========================================================
_Bool OneNET_DisConnect(void)
{

	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};							//协议包

	if(!onenet_info.net_work)
		return 1;
	
	if(MQTT_PacketDisConnect(&mqtt_packet) == 0)
	{
		Protocol_Parser_Print(mqtt_packet._data);
		GPRS_send_data(mqtt_packet._data,mqtt_packet._data, mqtt_packet._len);
		//NET_DEVICE_SendData(mqtt_packet._data, mqtt_packet._len);				//向平台发送订阅请求
		NET_DEVICE_AddDataSendList(mqtt_packet._data, mqtt_packet._len, 1);		//加入链表
		
		MQTT_DeleteBuffer(&mqtt_packet);											//删包
	}
	
	return 0;

}

//==========================================================
//	函数名称：	OneNET_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	pro_id：产品ID
//				dev_name：设备名
//				streamArray：数据流
//				streamArrayNum：数据流个数
//
//	返回参数：	SEND_TYPE_OK-发送成功	SEND_TYPE_DATA-需要重送
//
//	说明：		
//==========================================================
unsigned char OneNET_SendData(char *pro_id, char *dev_name, DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{
	
	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};											//协议包
	
	_Bool status = SEND_TYPE_OK;
	short body_len = 0;
	
	if(!onenet_info.net_work)
		return SEND_TYPE_DATA;
	body_len = DSTREAM_GetDataStream_Body_Measure(streamArray, streamArrayCnt, 0);		//获取当前需要发送的数据流的总长度
	if(body_len > 0)
	{
		if(MQTT_PacketSaveData(pro_id, dev_name, body_len, NULL, &mqtt_packet) == 0)
		{
			body_len = DSTREAM_GetDataStream_Body(streamArray, streamArrayCnt, mqtt_packet._data, mqtt_packet._size, mqtt_packet._len);
			
			if(body_len > 0)
			{
				mqtt_packet._len += body_len;
				//UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", mqtt_packet._len);
				
				//Protocol_Parser_Print(mqtt_packet._data);
				GPRS_send_len(mqtt_packet._len);
				delay_ms(500);
				GPRS_send_data(mqtt_packet._data,mqtt_packet._data, mqtt_packet._len);
				//NET_DEVICE_SendData(mqtt_packet._data, mqtt_packet._len);						//上传数据到平台
				//NET_DEVICE_AddDataSendList(mqtt_packet._data, mqtt_packet._len, 1);				//加入链表
			}
			else
				//UsartPrintf(USART_DEBUG, "WARN:	DSTREAM_GetDataStream_Body Failed\r\n");
			{
				;
			}
			MQTT_DeleteBuffer(&mqtt_packet);														//删包
		}
		else
		{
			//UsartPrintf(USART_DEBUG, "WARN:	MQTT_NewBuffer Failed\r\n");
			;
		}
	}
	else if(body_len < 0)
		status = SEND_TYPE_OK;
	else
		status = SEND_TYPE_DATA;
	
	net_fault_info.net_fault_level_r = NET_FAULT_LEVEL_0;										//发送之后清除标记
	
	return status;
	
}
//onenet_info.dev_id,onenet_info.dev_name
/*
	dat_name
	dat_buf
*/
u8 OneNET_SendDataGPS(L76K_STRUCT L76K_Dat)
{
	char tempbuff[300]={0};
	char data_buf[48];
	char ch_buf[20];
	unsigned short i;
	if(strstr(L76K_Dat.Quality,"0"))
	{
		return 1;
	}
	strcpy(tempbuff,"AT+MPUB=");                    //onenet_info.dev_id,onenet_info.dev_name
	snprintf(data_buf, sizeof(data_buf), "$sys/%s/%s/dp/post/json,", onenet_info.pro_id, onenet_info.dev_name);
	strcat(tempbuff,data_buf);
	strcat(tempbuff,"1,");
	strcat(tempbuff,"0,");
	strcat(tempbuff,"\"{\\22id\\22:123,\\22dp\\22:{");
	
	snprintf(data_buf,sizeof(data_buf),"\\22%s\\22:[{\\22v\\22:\\22%f,%f\\22}]","GPS",L76K_Dat.F_Lat,L76K_Dat.F_Lon);
	
	strcat(tempbuff,data_buf);
	
	strcat(tempbuff,"}}\"\r");
	SEGGER_RTT_printf(0,RTT_CTRL_TEXT_YELLOW"tempbuff%s",tempbuff);
	SendATCommand(tempbuff,"OK",50);//训练波特率
}
unsigned char OneNET_SendData2(char *dataName, void *dataBuf,DATA_TYPE dataType)
{
	u8 ret=0;
	char tempbuff[300]={0};
	char data_buf[128]="";
	char ch_buf[20]="";
	unsigned short i;
	DateTimeDef time;
	strcpy(tempbuff,"AT+MPUB=");                    //onenet_info.dev_id,onenet_info.dev_name
	snprintf(data_buf, sizeof(data_buf), "$sys/%s/%s/dp/post/json,", onenet_info.pro_id, onenet_info.dev_name);
	strcat(tempbuff,data_buf);
	strcat(tempbuff,"1,");
	strcat(tempbuff,"0,");
	strcat(tempbuff,"\"{\\22id\\22:123,\\22dp\\22:{");
	switch((u8)dataType)
	{
	
		case TYPE_BOOL:
			snprintf(data_buf,sizeof(data_buf),"\\22%s\\22:[{\\22v\\22:\\22%d\\22}]",dataName,*(_Bool *)dataBuf);
		break;
		case TYPE_CHAR:
			snprintf(data_buf,sizeof(data_buf),"\\22%s\\22:[{\\22v\\22:\\22%d\\22}]",dataName,*( signed char *)dataBuf);
		break;
		case TYPE_UCHAR:
			snprintf(data_buf,sizeof(data_buf),"\\22%s\\22:[{\\22v\\22:\\22%d\\22}]",dataName,*( unsigned char *)dataBuf);
		break;
		case TYPE_SHORT:
			snprintf(data_buf,sizeof(data_buf),"\\22%s\\22:[{\\22v\\22:\\22%d\\22}]",dataName,*(short *)dataBuf);
		break;
		case TYPE_USHORT:
			snprintf(data_buf,sizeof(data_buf),"\\22%s\\22:[{\\22v\\22:\\22%d\\22}]",dataName,*(unsigned short *)dataBuf);
		break;
		case TYPE_INT:
			snprintf(data_buf,sizeof(data_buf),"\\22%s\\22:[{\\22v\\22:\\22%d\\22}]",dataName,*(int *)dataBuf);
		break;
		case TYPE_UINT:
			snprintf(data_buf,sizeof(data_buf),"\\22%s\\22:[{\\22v\\22:\\22%d\\22}]",dataName,*(unsigned int *)dataBuf);
		break;
		case TYPE_LONG:
			snprintf(data_buf,sizeof(data_buf),"\\22%s\\22:[{\\22v\\22:\\22%ld\\22}]",dataName,*(long *)dataBuf);
		break;
		case TYPE_ULONG:
			snprintf(data_buf,sizeof(data_buf),"\\22%s\\22:[{\\22v\\22:\\22%ld\\22}]",dataName,*(unsigned long *)dataBuf);
		break;
		case TYPE_FLOAT:
			snprintf(data_buf,sizeof(data_buf),"\\22%s\\22:[{\\22v\\22:\\22%f\\22}]",dataName,*(float *)dataBuf);
		break;
		case TYPE_DOUBLE:
				snprintf(data_buf,sizeof(data_buf),"\\22%s\\22:[{\\22v\\22:\\22%lf\\22}]",dataName,*(double *)dataBuf);
		break;
		case TYPE_STRING:
			snprintf(data_buf,sizeof(data_buf),"\\22%s\\22:[{\\22v\\22:\\22%s\\22}]",dataName,(char *)dataBuf);
		break;
		case TYPE_DATETIME:
		{
			time=*(DateTimeDef * )dataBuf;
			 snprintf(data_buf,sizeof(data_buf),"\\22%s\\22:[{\\22v\\22:\\22%d-%d-%d\\22}]",dataName,time.years,time.months,time.days);
			break;
		}
		default:
		break;
			
		
	}
//	snprintf(data_buf,sizeof(data_buf),"\\22%s\\22:[{\\22v\\22:\\22%f\\22}]",dat_name,Datalog_Par_new.fro_level);
	strcat(tempbuff,data_buf);
	
	strcat(tempbuff,"}}\"\r");
	SEGGER_RTT_printf(0,RTT_CTRL_TEXT_YELLOW"tempbuff=%s",tempbuff);
	/*
		设备上报数据点后  解析到  系统通知"设备上传数据点成功"  
		当上传数据处理成功时
		返回消息至：$sys/${pid}/${device-name}/dp/post/json/accepted
		当上传数据处理失败时
		返回消息至：$sys/${pid}/${device-name}/dp/post/json/rejected
		ret==0  系统通知"设备上传数据点成功"
		ret==-1 设备上报数据点后  最多五次都未收到系统通知上传成功
	*/
	ret = OneNET_SendData_Ack(tempbuff,"accepted",50);
	return ret;
}

unsigned char OneNET_SendCMD(char *topic)
{
	char tempbuff[300]={0};
	char data_buf[48];
	char ch_buf[20];
	unsigned short i;
	
	strcpy(tempbuff,"AT+MPUB=");                    //onenet_info.dev_id,onenet_info.dev_name
//	snprintf(data_buf, sizeof(data_buf), "$sys/%s/%s/dp/post/json,", onenet_info.pro_id, onenet_info.dev_name);
	strcat(tempbuff,topic);
	strcat(tempbuff,",1,");
	strcat(tempbuff,"0,");
	strcat(tempbuff,"abcd");

//	snprintf(data_buf,sizeof(data_buf),"\\22%s\\22:[{\\22v\\22:\\22%f\\22}]",dat_name,Datalog_Par_new.fro_level);
//	strcat(tempbuff,data_buf);
	
	strcat(tempbuff,"\r");
	SEGGER_RTT_printf(0,RTT_CTRL_TEXT_YELLOW"tempbuff=%s",tempbuff);
	OneNET_SendData_Ack(tempbuff,"accepted",50);
}

void OneNET_SendRun_Par(void)
{
	char temp[120]="";
	OneNET_SendData2("dev_code",dev_SavePar.dev_code,TYPE_STRING);
	
	snprintf(temp,sizeof(temp),"%hu-%hhu-%hhu %hhu:%hhu:%hhu",g_rtc.years,g_rtc.months,g_rtc.days,g_rtc.hours,g_rtc.minutes,g_rtc.secs);
	OneNET_SendData2("time",temp,TYPE_STRING);
	
	if(strstr(L76K_Par.Quality,"1"))
	{
		OneNET_SendData2("longitude",&L76K_Par.F_Lon,TYPE_FLOAT);          		//经度
		OneNET_SendData2("latitude",&L76K_Par.F_Lat,TYPE_FLOAT);            	//纬度
	}
	else
	{
		OneNET_SendData2("longitude"," ",TYPE_STRING);          				
		OneNET_SendData2("latitude"," ",TYPE_STRING);            				
	}
	
	OneNET_SendData2("fro_level",&RS485_Sensor.Fro_h,TYPE_FLOAT); 					//闸前水位
	OneNET_SendData2("aft_level",&RS485_Sensor.Aft_h,TYPE_FLOAT); 					//闸后水位
	
	OneNET_SendData2("floatball_state",&Statlog_Par.FloatBall_Safe,TYPE_UCHAR);		//浮球开关
	
	OneNET_SendData2("target_opening",&Control_Par.set_openingm,TYPE_FLOAT); 		//设定开度
	OneNET_SendData2("target_fro_level",&Control_Par.set_level,TYPE_FLOAT); 		//设定前水头
	OneNET_SendData2("target_aft_level"," ",TYPE_STRING); 							//设定后水位
	OneNET_SendData2("target_flow",&Control_Par.set_flow,TYPE_FLOAT); 				//设定流量
	OneNET_SendData2("target_rotation_wv"," ",TYPE_STRING);							//设定本次水量
	OneNET_SendData2("target_data_interval"," ",TYPE_STRING);						//设定数据间隔
	OneNET_SendData2("max_opening_degree",&dev_SavePar.water_inletHeight,TYPE_FLOAT);	//最大开度(进水高度)
	OneNET_SendData2("opening_degree",&nosave_par.openingm,TYPE_FLOAT);				//开度
	
	if(dev_SavePar.TypeUltra==0)
		OneNET_SendData2("flow",&Ultra_CALCU_Par.Q_Total,TYPE_FLOAT);				//实时流量
	else
		OneNET_SendData2("flow",&Log_Par.Q_current,TYPE_FLOAT);				
	
	OneNET_SendData2("day_wv",&Log_Par.day_water,TYPE_DOUBLE); 						//当日水量
	OneNET_SendData2("rotation_wv",&Log_Par.this_water,TYPE_DOUBLE); 				//本次水量
	OneNET_SendData2("year_wv",&Log_Par.year_water,TYPE_DOUBLE); 		   	   		//本年流量
	OneNET_SendData2("accu_wv",&Log_Par.accu_water,TYPE_DOUBLE);  			   		//累计流量
	OneNET_SendData2("current_voltage",&Statlog_Par_new.voltage,TYPE_FLOAT);    	//板载电压(电路板电源电压)
	OneNET_SendData2("I"," ",TYPE_STRING);											//电流
	OneNET_SendData2("P"," ",TYPE_STRING);											//功率
	OneNET_SendData2("4G_state",&Statlog_Par._4G,TYPE_UCHAR);						//4G状态
	OneNET_SendData2("lora_state",&Statlog_Par.loraState,TYPE_UCHAR);				//lora状态
	OneNET_SendData2("gps_state",&Statlog_Par.GPSState,TYPE_UCHAR);					//GPS状态
	OneNET_SendData2("StallGuard_flag",&StallGuard_Par.StallGuard_flag,TYPE_UCHAR);	//堵转
	OneNET_SendData2("gate_function"," ",TYPE_STRING);								//闸门功能
	
	OneNET_SendData2("controlmode",&Control_Par.control_mode,TYPE_SHORT);  			//控制模式
	OneNET_SendData2("Wat_head",&RS485_Sensor.Wat_head,TYPE_FLOAT); 				//水头
	OneNET_SendData2("sim_number",&dev_SavePar.sim_number,TYPE_STRING);             //sim卡号
}

/*
发送设备存储设置
OneNET_SendData2 的第一次参数最大长度为16
*/
void OneNET_Sendev_SavePar(void)
{
	OneNET_SendData2("dev_name",&dev_SavePar.dev_name,TYPE_STRING); //产品名称
	OneNET_SendData2("factory_time",&dev_SavePar.factory_time,TYPE_DATETIME); //出厂时间
	OneNET_SendData2("dev_code",&dev_SavePar.dev_code,TYPE_STRING); //设备编号
	
	OneNET_SendData2("software_version",&myota_info.nowVersion,TYPE_STRING); //软件版本
	OneNET_SendData2("sim_number",&dev_SavePar.sim_number,TYPE_STRING);             //sim卡号
	OneNET_SendData2("form_factor",&dev_SavePar.form_factor,TYPE_STRING);           //外形尺寸
	
	OneNET_SendData2("motor_power",&dev_SavePar.motor_power,TYPE_STRING);      //电机功率

	OneNET_SendData2("w_inletWidth",&dev_SavePar.water_inletWidth,TYPE_FLOAT); //进水宽度
	OneNET_SendData2("w_inletHeight",&dev_SavePar.water_inletHeight,TYPE_FLOAT);//进水高度
	OneNET_SendData2("base_heightDiff",&dev_SavePar.base_heightDiff,TYPE_FLOAT);   //底板高差
	OneNET_SendData2("Ultra_initDist",&dev_SavePar.Ultra_initialDist,TYPE_FLOAT);//探头初距
	OneNET_SendData2("Ultra_Spacing",&dev_SavePar.Ultra_Spacing,TYPE_FLOAT);      //探头间距
	OneNET_SendData2("Ultra_Num",&dev_SavePar.Ultra_Num,TYPE_FLOAT);          //探头数量
	OneNET_SendData2("FroHeight_Air",&dev_SavePar.FroHeight_Air,TYPE_FLOAT);   //闸前净空
	OneNET_SendData2("AftHeight_Air",&dev_SavePar.AftHeight_Air,TYPE_FLOAT);   //闸后净空
	OneNET_SendData2("Gradient",&dev_SavePar.Gradient,TYPE_FLOAT);             //渠道比降
	
	OneNET_SendData2("flow_min",&dev_SavePar.flow_min,TYPE_FLOAT);              //最小流量
	OneNET_SendData2("flow_max",&dev_SavePar.flow_max,TYPE_FLOAT);               //最大流量
	OneNET_SendData2("flow_deviation",&dev_SavePar.flow_deviation,TYPE_FLOAT);   //流量偏差
	OneNET_SendData2("level_min",&dev_SavePar.level_min,TYPE_FLOAT);             //最小水位
	OneNET_SendData2("level_max",&dev_SavePar.level_max,TYPE_FLOAT);              //最大水位
	OneNET_SendData2("level_deviation",&dev_SavePar.level_deviation,TYPE_FLOAT);  //水位偏差
	OneNET_SendData2("data_interval",&dev_SavePar.data_interval,TYPE_FLOAT);        //数据间隔
	OneNET_SendData2("send_interval",&dev_SavePar.send_interval,TYPE_FLOAT);   //状态间隔
	OneNET_SendData2("Channel_Angle",&dev_SavePar.Channel_Angle,TYPE_FLOAT);      //声道角度
	OneNET_SendData2("Side_K",&dev_SavePar.Side_K,TYPE_FLOAT);                   //边壁系数
	OneNET_SendData2("Correct_K",&dev_SavePar.Correct_K,TYPE_FLOAT);   //流量系数
	OneNET_SendData2("Roughness",&dev_SavePar.Roughness,TYPE_FLOAT);   //明渠系数
}	

/*
发送控制参数
*/
void OneNET_SenControl_Par(void)
{
	OneNET_SendData2("set_openingm",&Control_Par.set_openingm,TYPE_FLOAT); //设定开度
	OneNET_SendData2("control_run",&Control_Par.control_run,TYPE_UCHAR); //运行模式
	OneNET_SendData2("opening",&Datalog_Par_new.opening,TYPE_FLOAT); //开度

}
/*
发送权限信息
*/
void OneNET_SenAuthor_Par(void)
{
//	OneNET_SendData2("admin_pass",&Author_Par.admin_pass,TYPE_STRING); //管理员密码
//	OneNET_SendData2("opera_pass",&Author_Par.opera_pass,TYPE_STRING); //操作员密码
//	OneNET_SendData2("maint_pass",&Author_Par.maint_pass,TYPE_STRING); //维护员密码
//	OneNET_SendData2("tempo_pass",&Author_Par.tempo_pass,TYPE_STRING); //维护员密码
//	OneNET_SendData2("admin_phone",&Author_Par.admin_phone,TYPE_STRING); //维护员密码
//	OneNET_SendData2("opera_phone",&Author_Par.opera_phone,TYPE_STRING); //维护员密码

}
//==========================================================
//	函数名称：	OneNET_SendData_Heart
//
//	函数功能：	心跳检测
//
//	入口参数：	无
//
//	返回参数：	SEND_TYPE_OK-发送成功	SEND_TYPE_DATA-需要重送
//
//	说明：		
//==========================================================
unsigned char OneNET_SendData_Heart(void)
{
	
	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};				//协议包
	
	if(!onenet_info.net_work)										//如果网络为连接
		return SEND_TYPE_HEART;
	
	if(MQTT_PacketPing(&mqtt_packet))
		return SEND_TYPE_HEART;
	
	onenet_info.heart_beat = 0;
	
	Protocol_Parser_Print(mqtt_packet._data);
	GPRS_send_data(mqtt_packet._data,mqtt_packet._data, mqtt_packet._len);
	//NET_DEVICE_SendData(mqtt_packet._data, mqtt_packet._len);		//向平台上传心跳请求
	NET_DEVICE_AddDataSendList(mqtt_packet._data, mqtt_packet._len, 0);//加入链表
	
	MQTT_DeleteBuffer(&mqtt_packet);									//删包
	
	return SEND_TYPE_OK;

}

//==========================================================
//	函数名称：	OneNET_Check_Heart
//
//	函数功能：	发送心跳后的心跳检测
//
//	入口参数：	无
//
//	返回参数：	0-成功	1-等待
//
//	说明：		基于调用时基，runCount每隔此函数调用一次的时间自增
//				达到设定上限检测心跳标志位是否就绪
//				上限时间可以不用太精确
//==========================================================
_Bool OneNET_Check_Heart(void)
{
	
	static unsigned char runCount = 0;
	
	if(!onenet_info.net_work)
		return 1;

	if(onenet_info.heart_beat == 1)
	{
		runCount = 0;
		onenet_info.err_count = 0;
		
		return 0;
	}
	
	if(++runCount >= 40)
	{
		runCount = 0;
		
		//UsartPrintf(USART_DEBUG, "HeartBeat TimeOut: %d\r\n", onenet_info.err_count);
		onenet_info.send_data = SEND_TYPE_HEART;		//发送心跳请求
		
		if(++onenet_info.err_count >= 3)
		{
			onenet_info.err_count = 0;
			
			onenet_info.err_check = 1;
		}
	}
	
	return 1;

}

//==========================================================
//	函数名称：	OneNET_Publish
//
//	函数功能：	发布消息
//
//	入口参数：	topic：发布的主题
//				msg：消息内容
//
//	返回参数：	SEND_TYPE_OK-成功	SEND_TYPE_PUBLISH-需要重送
//
//	说明：		
//==========================================================
unsigned char OneNET_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};							//协议包

	if(!onenet_info.net_work)
		return SEND_TYPE_PUBLISH;
	
	//UsartPrintf(USART_DEBUG, "Publish Topic: %s, Msg: %s\r\n", topic, msg);
	
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL1, 0, 1, &mqtt_packet) == 0)
	{
		Protocol_Parser_Print(mqtt_packet._data);
		GPRS_send_data(mqtt_packet._data,mqtt_packet._data, mqtt_packet._len);
		//NET_DEVICE_SendData(mqtt_packet._data, mqtt_packet._len);				//向平台发送订阅请求
		NET_DEVICE_AddDataSendList(mqtt_packet._data, mqtt_packet._len, 1);		//加入链表
		
		MQTT_DeleteBuffer(&mqtt_packet);											//删包
	}
	
	return SEND_TYPE_OK;

}

//==========================================================
//	函数名称：	OneNET_Subscribe
//
//	函数功能：	订阅
//
//	入口参数：	topics：订阅的topic
//				topic_cnt：topic个数
//
//	返回参数：	SEND_TYPE_OK-成功	SEND_TYPE_SUBSCRIBE-需要重发
//
//	说明：		
//==========================================================
unsigned char OneNET_Subscribe(const char *topics[], unsigned char topic_cnt)
{
	
	unsigned char i = 0;
	
	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};							//协议包

	if(!onenet_info.net_work)
		return SEND_TYPE_SUBSCRIBE;
	
	//for(; i < topic_cnt; i++)
		//UsartPrintf(USART_DEBUG, "Subscribe Topic: %s\r\n", topics[i]);
	
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL1, topics, topic_cnt, &mqtt_packet) == 0)
	{
		Protocol_Parser_Print(mqtt_packet._data);
		GPRS_send_len(mqtt_packet._len);
		GPRS_send_data(mqtt_packet._data,mqtt_packet._data, mqtt_packet._len);
		//NET_DEVICE_SendData(mqtt_packet._data, mqtt_packet._len);				//向平台发送订阅请求
		//NET_DEVICE_AddDataSendList(mqtt_packet._data, mqtt_packet._len, 1);		//加入链表
		
		MQTT_DeleteBuffer(&mqtt_packet);											//删包
	}
	
	return SEND_TYPE_OK;

}

//==========================================================
//	函数名称：	OneNET_UnSubscribe
//
//	函数功能：	取消订阅
//
//	入口参数：	topics：订阅的topic
//				topic_cnt：topic个数
//
//	返回参数：	SEND_TYPE_OK-发送成功	SEND_TYPE_UNSUBSCRIBE-需要重发
//
//	说明：		
//==========================================================
unsigned char OneNET_UnSubscribe(const char *topics[], unsigned char topic_cnt)
{
	
	unsigned char i = 0;
	
	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};							//协议包

	if(!onenet_info.net_work)
		return SEND_TYPE_UNSUBSCRIBE;
	
	for(; i < topic_cnt; i++)
		//UsartPrintf(USART_DEBUG, "UnSubscribe Topic: %s\r\n", topics[i]);
	
	if(MQTT_PacketUnSubscribe(MQTT_UNSUBSCRIBE_ID, topics, topic_cnt, &mqtt_packet) == 0)
	{
		Protocol_Parser_Print(mqtt_packet._data);
		GPRS_send_data(mqtt_packet._data,mqtt_packet._data, mqtt_packet._len);
		//NET_DEVICE_SendData(mqtt_packet._data, mqtt_packet._len);				//向平台发送取消订阅请求
		NET_DEVICE_AddDataSendList(mqtt_packet._data, mqtt_packet._len, 1);		//加入链表
		
		MQTT_DeleteBuffer(&mqtt_packet);											//删包
	}
	
	return SEND_TYPE_OK;

}

//==========================================================
//	函数名称：	OneNET_CmdHandle
//
//	函数功能：	读取平台rb中的数据
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================

void OneNET_CmdHandle(void)
{
		MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};										//协议包
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short topic_len = 0;
	unsigned short req_len = 0;
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	unsigned char dataPtr[SMS_BUFFER_SIZE];
	
	unsigned char *ipdPtr = NULL;		//数据指针
	
	s32 rxlen;
	//dataPtr = NET_DEVICE_Read();						//等待数据
		memset(dataPtr,0,sizeof(dataPtr));
	rxlen =_4GCat1_read((u8 *)dataPtr,SMS_BUFFER_SIZE,3);
//	rxlen = USART_read(GPRS_UART,dataPtr, SMS_BUFFER_SIZE, 3);
	if(rxlen>0)									//数据有效
	{
		OneNETAdd_RevPro(dataPtr);					//集中处理
		
		MyOnenet_ProcessCmd();    //处理命令
	}
}

//==========================================================
//	函数名称：	OneNET_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNET_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};										//协议包
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short topic_len = 0;
	unsigned short req_len = 0;
	
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	Protocol_Parser_Print(cmd);
	
	switch(MQTT_UnPacketRecv(cmd))
	{
		case MQTT_PKT_CONNACK:
		
			switch(MQTT_UnPacketConnectAck(cmd))
			{
				case 0:
					onenet_info.net_work = 1;
				break;
				
				//case 1:UsartPrintf(USART_DEBUG, "WARN:	连接失败：协议错误\r\n");break;
				//case 2:UsartPrintf(USART_DEBUG, "WARN:	连接失败：非法的clientid\r\n");break;
				//case 3:UsartPrintf(USART_DEBUG, "WARN:	连接失败：服务器失败\r\n");break;
				//case 4:UsartPrintf(USART_DEBUG, "WARN:	连接失败：用户名或密码错误\r\n");break;
				//case 5:UsartPrintf(USART_DEBUG, "WARN:	连接失败：非法链接(比如token非法)\r\n");break;
				
				//default:UsartPrintf(USART_DEBUG, "ERR:	连接失败：未知错误\r\n");break;
				default:;break;
			}
		
		break;
		
		case MQTT_PKT_PINGRESP:
		
			//UsartPrintf(USART_DEBUG, "Tips:	HeartBeat OK\r\n");
			onenet_info.heart_beat = 1;
		
		break;
		
		case MQTT_PKT_CMD:																	//命令下发
			
			if(MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len) == 0)			//解出topic和消息体
			{
				//UsartPrintf(USART_DEBUG, "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);
				
				//执行命令回调------------------------------------------------------------
				//CALLBACK_Execute(req_payload);
				
				if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqtt_packet) == 0)			//命令回复组包
				{
					//UsartPrintf(USART_DEBUG, "Tips:	Send CmdResp\r\n");
					
					Protocol_Parser_Print(mqtt_packet._data);
					GPRS_send_data(mqtt_packet._data,mqtt_packet._data, mqtt_packet._len);
					//NET_DEVICE_SendData(mqtt_packet._data, mqtt_packet._len);				//回复命令
					NET_DEVICE_AddDataSendList(mqtt_packet._data, mqtt_packet._len, 1);		//加入链表
					MQTT_DeleteBuffer(&mqtt_packet);											//删包
				}
				
				MQTT_FreeBuffer(cmdid_topic);
				MQTT_FreeBuffer(req_payload);
				onenet_info.send_data = SEND_TYPE_DATA;
			}
		
		break;
			
		case MQTT_PKT_PUBLISH:																//接收的Publish消息
		
			if(MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id) == 0)
			{
				char *data_ptr = NULL;
				
				//UsartPrintf(USART_DEBUG, "topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
																	//cmdid_topic, topic_len, req_payload, req_len);
				
				//执行命令回调------------------------------------------------------------
				//CALLBACK_Execute(req_payload);
				
				data_ptr = strstr(cmdid_topic, "request/");									//查找cmdid
				if(data_ptr)
				{
					char topic_buf[80], cmdid[40];
					
					data_ptr = strchr(data_ptr, '/');
					data_ptr++;
					
					memcpy(cmdid, data_ptr, 36);											//复制cmdid
					cmdid[36] = 0;
					
					snprintf(topic_buf, sizeof(topic_buf), "$sys/%s/%s/cmd/response/%s",
															onenet_info.pro_id, onenet_info.dev_name, cmdid);
					OneNET_Publish(topic_buf, "ojbk");										//回复命令
				}
				
				switch(qos)
				{
					case 1:																	//收到publish的qos为1，设备需要回复Ack
					
						if(MQTT_PacketPublishAck(pkt_id, &mqtt_packet) == 0)
						{
							//UsartPrintf(USART_DEBUG, "Tips:	Send PublishAck\r\n");
							
							Protocol_Parser_Print(mqtt_packet._data);
							GPRS_send_data(mqtt_packet._data,mqtt_packet._data, mqtt_packet._len);
							//NET_DEVICE_SendData(mqtt_packet._data, mqtt_packet._len);
							NET_DEVICE_AddDataSendList(mqtt_packet._data, mqtt_packet._len, 1);//加入链表
							MQTT_DeleteBuffer(&mqtt_packet);
						}
					
					break;
					
					case 2:																	//收到publish的qos为2，设备先回复Rec
																							//平台回复Rel，设备再回复Comp
						
					
					break;
					
					default:
						break;
				}
				
				MQTT_FreeBuffer(cmdid_topic);
				MQTT_FreeBuffer(req_payload);
				onenet_info.send_data = SEND_TYPE_DATA;
			}
		
		break;
			
		case MQTT_PKT_PUBACK:																//发送Publish消息，平台回复的Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
			{
				//UsartPrintf(USART_DEBUG, "Tips:	MQTT Publish Send OK\r\n");
				;
				
#if(LBS_EN == 1)
				if(lbs_info.lbs_ok == 1 && !onenet_info.lbs && onenet_info.lbs_count < 4)	//如果获取到了基站信息 且 未获取到位置坐标 且 获取次数小于一定值
					onenet_info.net_work = 0;												//则重新获取一下位置信息
#endif
			}
			
		break;
			
		case MQTT_PKT_SUBACK:																//发送Subscribe消息的Ack
		
			if(MQTT_UnPacketSubscribe(cmd) == 0)
			//UsartPrintf(USART_DEBUG, "Tips:	MQTT Subscribe OK\r\n");
			;
			else
				//UsartPrintf(USART_DEBUG, "Tips:	MQTT Subscribe Err\r\n");
			;
		
		break;
			
		case MQTT_PKT_UNSUBACK:																//发送UnSubscribe消息的Ack
		
			if(MQTT_UnPacketUnSubscribe(cmd) == 0)
				//UsartPrintf(USART_DEBUG, "Tips:	MQTT UnSubscribe OK\r\n");
			;
			else
				//UsartPrintf(USART_DEBUG, "Tips:	MQTT UnSubscribe Err\r\n");
			;
		
		break;
		
		default:
			
		break;
	}

}

//==========================================================
//	函数名称：	NET_DEVICE_AddDataSendList
//
//	函数功能：	在发送链表尾新增一个发送链表
//
//	入口参数：	buf：需要发送的数据
//				data_len：数据长度
//				mode：0-添加到头部	1-添加到尾部
//
//	返回参数：	0-成功	其他-失败
//
//	说明：		异步发送方式
//==========================================================
unsigned char NET_DEVICE_AddDataSendList(unsigned char *buf ,unsigned short data_len, _Bool mode)
{
	
	struct NET_SEND_LIST *current = (struct NET_SEND_LIST *)NET_MallocBuffer(sizeof(struct NET_SEND_LIST));
																//分配内存
	
	if(current == NULL)
		return 1;
	
	current->buf = (unsigned char *)NET_MallocBuffer(data_len);	//分配内存
	if(current->buf == NULL)
	{
		NET_FreeBuffer(current);								//失败则释放
		return 2;
	}
	
	if(net_device_info.head == NULL)							//如果head为NULL
		net_device_info.head = current;							//head指向当前分配的内存区
	else														//如果head不为NULL
	{
		if(mode)
			net_device_info.end->next = current;				//则end指向当前分配的内存区
		else
		{
			struct NET_SEND_LIST *head_t = NULL;
			
			head_t = net_device_info.head;
			net_device_info.head = current;
			current->next = head_t;
		}
	}
	
	memcpy(current->buf, buf, data_len);						//复制数据
	current->dataLen = data_len;
	current->next = NULL;										//下一段为NULL
	
	net_device_info.end = current;								//end指向当前分配的内存区
	
	return 0;

}

/*****************************************************************************************
 SEND_TYPE_OK			0	//
 SEND_TYPE_DATA			1	//
 SEND_TYPE_HEART			2	//
 SEND_TYPE_PUBLISH		3	//
 SEND_TYPE_SUBSCRIBE		4	//
 SEND_TYPE_UNSUBSCRIBE	5	//
******************************************************************************************/
void Judge_SenDdata(unsigned short Send_Status)
{
//			//switch(onenet_info.send_data)
//			switch(Send_Status)
//		{
//					
//			case 6:  /////////aaaaa
//			
//				onenet_info.send_data = OneNET_SendData(onenet_info.pro_id, onenet_info.dev_name,
//																data_stream_Water_Speed, data_stream_cnt_Water_Speed);				//上传数据到平台
//			break;
//			
//			
//			case 7:  /////////aaaaa
//			
//				onenet_info.send_data = OneNET_SendData(onenet_info.pro_id, onenet_info.dev_name,
//																data_stream_Water_Level, data_stream_cnt_Water_Level);				//上传数据到平台
//			break;
//			
//			case 8:  /////////aaaaa
//			
//				onenet_info.send_data = OneNET_SendData(onenet_info.pro_id, onenet_info.dev_name,
//																data_stream_Water_Flow, data_stream_cnt_Water_Flow);				//上传数据到平台
//			break;
//			
//			case 1:  /////////aaaaa
//			
//				onenet_info.send_data = OneNET_SendData(onenet_info.pro_id, onenet_info.dev_name,
//																data_stream_moisture, data_stream_cnt_sm);				//上传数据到平台
//			break;
//			
//			case SEND_TYPE_SUBSCRIBE:
//			
//				onenet_info.send_data = OneNET_Subscribe((const char **)topics, topics_num);				//订阅主题
//			
//			break;
//			
//			case SEND_TYPE_UNSUBSCRIBE:
//			
//				onenet_info.send_data = OneNET_UnSubscribe((const char **)topics, topics_num);				//取消订阅的主题
//			
//			break;
//			
//			case SEND_TYPE_PUBLISH:
//			
//				//onenet_info.send_data = OneNET_Publish("pc_topic", "Publish Test");						//发布主题
//			
//			break;
//			
//			case SEND_TYPE_HEART:
//			
//				onenet_info.send_data = OneNET_SendData_Heart();
//			
//			break;
//		}
//		OneNET_Check_Heart();
//		
//		//RTOS_TimeDly(10);												//挂起任务50ms	

}

void Mqqt_OneNetlogin(unsigned short informnetwork)
{

		if(informnetwork)
					{
						if(!topics[0]) topics[0] = (char *)NET_MallocBuffer(TOPIC_BYTES);
						if(!topics[1]) topics[1] = (char *)NET_MallocBuffer(TOPIC_BYTES);
						if(!topics[2]) topics[2] = (char *)NET_MallocBuffer(TOPIC_BYTES);
						if(!topics[3]) topics[3] = (char *)NET_MallocBuffer(TOPIC_BYTES);
						if(topics[0] && topics[1] && topics[2]&&topics[3])
						{
							topics_num = 0;
							
							snprintf(topics[topics_num], TOPIC_BYTES, "$sys/%s/%s/dp/post/json/accepted",		//系统通知"设备上传数据点成功"
																		onenet_info.pro_id, onenet_info.dev_name);topics_num++;
							
							snprintf(topics[topics_num], TOPIC_BYTES, "$sys/%s/%s/dp/post/json/rejected",		//系统通知"设备上传数据点失败"
																		onenet_info.pro_id, onenet_info.dev_name);topics_num++;
							
							snprintf(topics[topics_num], TOPIC_BYTES, "$sys/%s/%s/cmd/#",						//设备接收命令Topic
																		onenet_info.pro_id, onenet_info.dev_name);topics_num++;
//							//$creq
//							snprintf(topics[topics_num],TOPIC_BYTES,"$crsp/cmd/%s/%s",
//																		onenet_info.pro_id, onenet_info.dev_name);topics_num++;
							onenet_info.send_data = SEND_TYPE_SUBSCRIBE;		//连接成功则订阅相关系统功能
							Judge_SenDdata(onenet_info.send_data);
						}
						else
						{
							if(topics[0]) {NET_FreeBuffer(topics[0]);topics[0] = NULL;}
							if(topics[1]) {NET_FreeBuffer(topics[1]);topics[1] = NULL;}
							if(topics[2]) {NET_FreeBuffer(topics[2]);topics[2] = NULL;}
							if(topics[3]) {NET_FreeBuffer(topics[3]);topics[3] = NULL;}
						}
						
						//NET_Event_CallBack(NET_EVENT_Connect_Ok);
					}
					else
					{
						//NET_Event_CallBack(NET_EVENT_Connect_Err);
						;
					}

}