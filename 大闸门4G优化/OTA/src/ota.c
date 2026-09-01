/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	ota.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2019-07-31
	*
	*	版本： 		V1.0
	*
	*	说明： 		OTA
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//OTA
#include "ota.h"

//算法
#include "base64.h"
#include "hmac_sha1.h"
#include "md5.h"

//驱动
#include "delay.h"
//#include "usart.h"
//#include "stmflash.h"
#include "gdflash.h"
//#include "GPRS.h"
//网络设备
#include "net_device.h"

//C库
#include <stdio.h>
#include <string.h>


#define OTA_IP		"183.230.40.50"

#define OTA_PORT	"80"


#define OTA_APP_ADDR_PAGE_OFFSET		0		//保存应用代码起始地址的地址

#define OTA_UPDATE_FLAG_PAGE_OFFSET		4		//保存升级标志的地址

#define OTA_FILE_SIZE_PAGE_OFFSET		8		//保存固件大小的地址

#define OTA_FILE_MD5_PAGE_OFFSET		12		//保存MD5校验数据的地址---32字节

#define OTA_FILE_TOKEN_PAGE_OFFSET		44		//保存Token的地址---24字节

#define OTA_PROID_PAGE_OFFSET			68		//保存产品ID的地址---16字节---占用16字节

#define OTA_MASTERKEY_PAGE_OFFSET		84		//保存masterkey的地址---28字节---占用32字节

#define OTA_REGCODE_PAGE_OFFSET			116		//保存regcode的地址---16字节---占用24字节

#define OTA_VERSION_PAGE_OFFSET			140		//保存version的地址---最大24字节


#define OTA_REPORT_DOWNLOAD_PROGRESS	1		//1-上报下载进度	0-不上报


OTA_INFO ota_info = {{0}, {0}, {0}, 0, {0}, 0, 0, 0, 0, 0, 0};


static iapfun jump2app;


//static unsigned char flash_buf[2048];			//flash读写缓存
static unsigned char flash_buf[2560];			//flash读写缓存
//---------------------------------------------------------------------------------//
//static char dev_id[16]="739879661";							//设备id
static char dev_id[16]="566468806";							//设备id
static char *initial_version = "V1.0";			//初始版本


//一共需要更改3处，1、main.c 44-45行;2、net_device.c 207行;3、ota.c 78-79行
//---------------------------------------------------------------------------------//

static char cur_version[24];					//当前版本


static unsigned char save_para_page = 0;		//保存跳转地址的页码

static _Bool mcu_flash_type = 0;				//flash类型

static unsigned short flash_page_size = 0;		//flash页大小

static unsigned char app_start_page = 0;		//应用代码的起始地址所在页码

static unsigned int update_flag = 0;			//正在升级标志

static unsigned int power_on_check_addr = 0;
static _Bool power_on_check_flag = 0;			//1-不管有无app上电连网检测升级状态，如果无app则循环检测
												//0-有app则上电跳转运行，无app则循环检测升级状态


/*
************************************************************
*	函数名称：	OTA_Display
*
*	函数功能：	提示升级进度
*
*	入口参数：	bytes：已下载字节数
*				size：文件总大小
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
static void OTA_Display(unsigned int bytes, unsigned int size)
{

	if(bytes >= size)
	{
//		UsartPrintf(USART_DEBUG, "Update %d / %d   Bytes, 100%%\r\n", size, size);
	}
	else
	{
//		UsartPrintf(USART_DEBUG, "Update %d / %d   Bytes, %0.2f%%\r\n", bytes, size, (float)bytes / size * 100);
	}

}

/*
************************************************************
*	函数名称：	OTA_SaveParameter_Int
*
*	函数功能：	OTA参数保存-数值
*
*	入口参数：	flash_page：页
*				offset：页内偏移---必须是4的正整数倍
*				flash_type：flash的类型，0-1KB	1-2KB
*				addr：要保存的数据
*
*	返回参数：	0-成功	其他-失败
*
*	说明：		
************************************************************
*/
static void OTA_SaveParameter_Int(unsigned char flash_page, unsigned short offset, _Bool flash_type, unsigned int data)
{
	
	if((offset % 4) == 0)
	{
//		unsigned int save_addr = FLASH_GetStartAddr(flash_page, flash_type) + offset;				//读取页码地址+偏移
		
//	Flash_Read(flash_page, (unsigned short *)flash_buf, flash_page_size >> 1, flash_type);		//读出全页数据
		
		__set_PRIMASK(1);
		
//		FLASH_Erase(flash_page, 1, flash_type);														//擦除页
		
		flash_buf[offset + 3] = data >> 24;
		flash_buf[offset + 2] = data >> 16;
		flash_buf[offset + 1] = data >> 8;
		flash_buf[offset] = data;
		
//		Flash_Write(flash_page, (unsigned short *)flash_buf, flash_page_size >> 1, flash_type);		//写入数据
		
		__set_PRIMASK(0);
	}

}

/*
************************************************************
*	函数名称：	OTA_SaveParameter_String
*
*	函数功能：	OTA参数保存-字符串
*
*	入口参数：	flash_page：页
*				offset：页内偏移---必须是4的正整数倍
*				flash_type：flash的类型，0-1KB	1-2KB
*				addr：要保存的数据
*
*	返回参数：	0-成功	其他-失败
*
*	说明：		
************************************************************
*/
static void OTA_SaveParameter_String(unsigned char flash_page, unsigned short offset, _Bool flash_type, char *str)
{
	
	unsigned int str_len = strlen(str);
	
	if(((offset % 4) == 0) && str_len)
	{
//		unsigned int save_addr = FLASH_GetStartAddr(flash_page, flash_type) + offset;				//读取页码地址+偏移
		unsigned int i = 0;
		
//		Flash_Read(flash_page, (unsigned short *)flash_buf, flash_page_size >> 1, flash_type);		//读出全页数据
		
		__set_PRIMASK(1);
		
//		FLASH_Erase(flash_page, 1, flash_type);														//擦除页
		
		for(; i < str_len; i++)
			flash_buf[i + offset] = *str++;
		
//		Flash_Write(flash_page, (unsigned short *)flash_buf, flash_page_size >> 1, flash_type);		//写入数据
		
		__set_PRIMASK(0);
	}

}

/*
************************************************************
*	函数名称：	OTA_Check_OneNETParameter
*
*	函数功能：	OTA参数保存-OneNET相关信息
*
*	入口参数：	flash_page：页
*				flash_type：flash的类型，0-1KB	1-2KB
*				proid：产品ID
*				masterkey：masterkey
*				regcode：注册码
*
*	返回参数：	0-成功	其他-失败
*
*	说明：		
************************************************************
*/
unsigned char OTA_Check_OneNETParameter(unsigned char flash_page, _Bool flash_type, char *proid, char *masterkey, char *regcode)
{
	
	char proid_t[16], masterkey_t[32], regcode_t[24];
	
	if(proid == NULL || masterkey == NULL || regcode == NULL)
		return 1;
	
	memset(proid_t, 0, sizeof(proid_t));
	memset(masterkey_t, 0, sizeof(masterkey_t));
	memset(regcode_t, 0, sizeof(regcode_t));

//	Flash_Read(flash_page, (unsigned short *)flash_buf, flash_type == 1 ? 1024 : 512, flash_type);		//读出全页数据
	
	strncpy(proid_t, (char *)flash_buf + OTA_PROID_PAGE_OFFSET, sizeof(proid_t));
	strncpy(masterkey_t, (char *)flash_buf + OTA_MASTERKEY_PAGE_OFFSET, sizeof(masterkey_t));
	strncpy(regcode_t, (char *)flash_buf + OTA_REGCODE_PAGE_OFFSET, sizeof(regcode_t));
	
	if((strcmp(proid, proid_t) == 0) && (strcmp(masterkey, masterkey_t) == 0) && (strcmp(regcode, regcode_t) == 0))
		;
	else
	{
		__set_PRIMASK(1);
		
//		FLASH_Erase(flash_page, 1, flash_type);															//擦除页
		
		memcpy(flash_buf + OTA_PROID_PAGE_OFFSET, proid, sizeof(proid_t));
		memcpy(flash_buf + OTA_MASTERKEY_PAGE_OFFSET, masterkey, sizeof(masterkey_t));
		memcpy(flash_buf + OTA_REGCODE_PAGE_OFFSET, regcode, sizeof(regcode_t));
		
//		Flash_Write(flash_page, (unsigned short *)flash_buf, flash_type == 1 ? 1024 : 512, flash_type);	//写入数据
		
		__set_PRIMASK(0);
	}
	
	return 0;

}

/*
************************************************************
*	函数名称：	OTA_CheckVersion
*
*	函数功能：	检查version
*
*	入口参数：	flash_page：页
*				flash_type：flash的类型，0-1KB	1-2KB
*
*	返回参数：	0-有		1-无
*
*	说明：		
************************************************************
*/
static _Bool OTA_CheckVersion(unsigned char flash_page, _Bool flash_type)
{
	
//	if(*(volatile unsigned int *)(FLASH_GetStartAddr(flash_page, flash_type) + OTA_VERSION_PAGE_OFFSET) == 0xFFFFFFFF)
		return 1;
//	else
		return 0;

}

/*
************************************************************
*	函数名称：	OTA_WriteVersion
*
*	函数功能：	保存版本
*
*	入口参数：	flash_page：页
*				flash_type：flash的类型，0-1KB	1-2KB
*				ver：版本
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
static void OTA_WriteVersion(unsigned char flash_page, _Bool flash_type, char *ver)
{
	
	OTA_SaveParameter_String(flash_page, OTA_VERSION_PAGE_OFFSET, flash_type, ver);

}

/*
************************************************************
*	函数名称：	OTA_ReadVersion
*
*	函数功能：	读取版本
*
*	入口参数：	flash_page：页
*				flash_type：flash的类型，0-1KB	1-2KB
*				ver：版本
*				len：读取长度
*
*	返回参数：	0-成功	其他-失败
*
*	说明：		
************************************************************
*/
static unsigned char OTA_ReadVersion(unsigned char flash_page, _Bool flash_type, char *ver, unsigned char len)
{
	
	unsigned char i = 0;
	
	if(ver == NULL)
		return 1;
	
	if(OTA_CheckVersion(flash_page, flash_type) == 1)
		return 2;

//	Flash_Read(flash_page, (unsigned short *)flash_buf, flash_type == 1 ? 1024 : 512, flash_type);		//读出全页数据
	
	strncpy(ver, (char *)flash_buf + OTA_VERSION_PAGE_OFFSET, len);
	
	while(i < len)
	{
		if(*ver < 0x20 || *ver > 0x7E)
		{
			*ver = 0;
			break;
		}
		
		ver++;
		i++;
	}
	
	return 0;

}

//__asm void MSR_MSP(unsigned int addr) 
//{
//    MSR MSP, r0 			//set Main Stack value
//    BX r14
//}

/*
************************************************************
*	函数名称：	OTA_Jump
*
*	函数功能：	跳转到应用程序段
*
*	入口参数：	addr：用户代码起始地址
*
*	返回参数：	0-有		1-无
*
*	说明：		
************************************************************
*/
void OTA_Jump(unsigned int addr)
{

	if(((*(volatile unsigned int *)(addr + 4)) & 0xFF000000) == 0x08000000)	//判断是否为0X08XXXXXX
	{
		if(((*(volatile unsigned int *)addr) & 0x2FFE0000) == 0x20000000)	//检查栈顶地址是否合法
		{
			jump2app = (iapfun)*(volatile unsigned int *)(addr + 4);		//用户代码区第二个字为程序开始地址(复位地址)
			
			MSR_MSP(*(volatile unsigned int *)addr);						//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
			
			jump2app();
		}
	}

}

/*
************************************************************
*	函数名称：	OTA_CheckApplication
*
*	函数功能：	检查Flash里是否存有跳转地址
*
*	入口参数：	flash_page：页
*				flash_type：flash的类型，0-1KB	1-2KB
*				app_addr：应用代码的起始页
*				power_on_check：1-不管有无app上电连网检测升级状态，如果无app则循环检测
*								0-有app则上电跳转运行，无app则循环检测升级状态
*
*	返回参数：	无
*
*	说明：		offset必须为4的正整数倍
************************************************************
*/
unsigned int OTA_CheckApplication(unsigned char flash_page, _Bool flash_type, unsigned char app_page, _Bool power_on_check)
{
	unsigned int app_addr;
	unsigned int temp;
	u32 daartemp = 0;
//	daartemp = FLASH_GetStartAddr(flash_page, flash_type) + OTA_UPDATE_FLAG_PAGE_OFFSET;
	update_flag = *(volatile unsigned int *)daartemp;
	//update_flag = *(volatile unsigned int *)(FLASH_GetStartAddr(flash_page, flash_type) + OTA_UPDATE_FLAG_PAGE_OFFSET);
																							//读取升级标志
	if(update_flag == 0xFFFFFFFF)															//如果未处于正在升级状态
	{
//			app_addr = FLASH_GetStartAddr(app_page, flash_type);					//得到跳转地址
			temp = (app_addr + 4) & 0xFF000000;
		//if(((*(volatile unsigned int *)(app_addr + 4)) & 0xFF000000) == 0x08000000)			//检查合理性
		if(temp == 0x08000000)
		{
			if(power_on_check == 0)															//不检测升级状态，直接跳转运行
				OTA_Jump(app_addr);
			else																			//需检测升级状态，记录跳转地址
				power_on_check_addr = app_addr;
		}
	}
	
	save_para_page = flash_page;
	mcu_flash_type = flash_type;
	flash_page_size = flash_type == 1 ? 2048 : 1024;
	app_start_page = app_page;
	power_on_check_flag = power_on_check;
	
	return 0;

}

/*
************************************************************
*	函数名称：	OTA_AutoCreateDevice
*
*	函数功能：	自动创建设备
*
*	入口参数：	masterkey：masterkey
*				reg_code：注册码
*				dev_name：设备名
*				auth_info：鉴权信息
*				devid：保存设备id
*
*	返回参数：	无
*
*	说明：		offset必须为4的正整数倍
************************************************************
*/
unsigned char OTA_AutoCreateDevice(const char *masterkey, const char *reg_code, const char *dev_name, const char *auth_info, char *devid)
{

	_Bool result = 255;
	char send_buf[320];
	unsigned char time_out = 200;
	
	if(!net_device_info.net_work)
		return 1;
	
	if(masterkey == NULL || reg_code == NULL || dev_name == NULL || auth_info == NULL || devid == NULL)
		return 2;
	
	if(NET_DEVICE_Connect("TCP", "183.230.40.33", "80") == 0)
	{
		snprintf(send_buf, sizeof(send_buf), "PUT /register_attr HTTP/1.1\r\napi-key:%s\r\nHost:api.heclouds.com\r\n"
						"Content-Length:19\r\n\r\n"
						"{\"allow_dup\": true}", masterkey);
		
		if(NET_DEVICE_SendData((unsigned char *)send_buf, strlen(send_buf)))
		{
			NET_DEVICE_Close();
			return 2;
		}
		
		net_device_info.cmd_ipd = NULL;
			
		while(--time_out)
		{
			if(net_device_info.cmd_ipd != NULL)
				break;
			
			DelayXms(20);
		}
		
		if(time_out)
		{
			if(strstr(net_device_info.cmd_ipd, "succ"))
			{
				unsigned short send_len = 20 + strlen(dev_name) + strlen(auth_info);
				char *data_ptr = NULL;
				
//				UsartPrintf(USART_DEBUG, "Tips:	OneNET_RepetitionCreateFlag Ok\r\n");
				
				DelayXms(net_device_info.send_time);
				
				snprintf(send_buf, 140 + send_len, "POST /register_de?register_code=%s HTTP/1.1\r\n"
									"Host: api.heclouds.com\r\n"
									"Content-Length:%d\r\n\r\n"
									"{\"sn\":\"%s\",\"title\":\"%s\"}",
					
									reg_code, send_len, auth_info, dev_name);
				
				if(NET_DEVICE_SendData((unsigned char *)send_buf, strlen(send_buf)))
				{
					NET_DEVICE_Close();
					return 3;
				}
				
				net_device_info.cmd_ipd = NULL;
				
				time_out = 200;
				while(--time_out)
				{
					if(net_device_info.cmd_ipd != NULL)
						break;
					
					DelayXms(20);
				}
				
				if(time_out)
				{
					data_ptr = strstr(net_device_info.cmd_ipd, "device_id");
					
					if(strstr(net_device_info.cmd_ipd, "auth failed"))
					{
//						UsartPrintf(USART_DEBUG, "WARN:	正式环境注册码错误\r\n");
					}
				}
				
				if(data_ptr)
				{
					char apikey[32];
					
					if(sscanf(data_ptr, "device_id\":\"%[^\"]\",\"key\":\"%[^\"]\"", devid, apikey) == 2)
					{
//						UsartPrintf(USART_DEBUG, "create device: %s, %s\r\n", devid, apikey);
						ota_info.create_ok = 1;
						result = 0;
					}
				}
			}
			else if(strstr(net_device_info.cmd_ipd, "auth failed"))
			{
//				UsartPrintf(USART_DEBUG, "WARN:	当前使用的不是masterkey 或 apikey错误\r\n");
			}
			else
			{}
//				UsartPrintf(USART_DEBUG, "Tips:	OneNET_RepetitionCreateFlag Err\r\n");
		}
		else
//			UsartPrintf(USART_DEBUG, "Tips:	OneNET_RepetitionCreateFlag Time Out\r\n");
		
		NET_DEVICE_Close();
	}
	
	return result;

}

/*
************************************************************
*	函数名称：	OTA_ConnectIP
*
*	函数功能：	连接服务器
*
*	入口参数：	无
*
*	返回参数：	0-成功	1-失败
*
*	说明：		
************************************************************
*/
_Bool OTA_ConnectIP(void)
{

	_Bool result = 1;
	
	if(!net_device_info.net_work)									//网络模组具备网络连接能力
		return result;
	
	if(GPRS_connect(OTA_IP, OTA_PORT))
	{
		result = 0;
	}
	else
	{
		DelayXms(500);
		
		if(++ota_info.err_count >= 5)
		{
			ota_info.err_count = 0;
//			UsartPrintf(USART_DEBUG, "请检查IP地址和PORT是否正确\r\n");
		}
	}
	
	return result;

}

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
*	函数名称：	OTA_ErrMsg
*
*	函数功能：	打印错误信息
*
*	入口参数：	fun：函数名
*				msg：消息
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
static void OTA_ErrMsg(char *fun, char *msg)
{

	char *data_ptr = data_ptr = strstr(net_device_info.cmd_ipd, "errno");
	
	char err_str[48];
	unsigned int err_code = 0;
	
	if(data_ptr)
	{
		if(sscanf(data_ptr, "errno\":%d,\"error\":\"%[^\"]\"", &err_code, err_str) == 2)
		{}
//			UsartPrintf(USART_DEBUG, "WARN:	%s Err:err_code:%d, err_msg:%s\r\n", fun, err_code, err_str);
	}

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
*				authorization_buf：缓存token的指针
*				authorization_buf_len：缓存区长度(字节)
*
*	返回参数：	0-成功	其他-失败
*
*	说明：		当前仅支持sha1
************************************************************
*/
#define METHOD		"sha1"
unsigned char OTA_Authorization(char *ver, char *res, unsigned int et, char *access_key, char *authorization_buf, unsigned short authorization_buf_len)
{
	
	size_t olen = 0;
	
	char sign_buf[40];								//保存签名的Base64编码结果 和 URL编码结果
	char hmac_sha1_buf[40];							//保存签名
	char access_key_base64[40];						//保存access_key的Base64编码结合
	char string_for_signature[56];					//保存string_for_signature，这个是加密的key

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
	snprintf(string_for_signature, sizeof(string_for_signature), "%d\n%s\nproducts/%s\n%s", et, METHOD, res, ver);
	//UsartPrintf(USART_DEBUG, "string_for_signature: %s\r\n", string_for_signature);
	
//----------------------------------------------------加密-------------------------------------------------------------------------
	memset(hmac_sha1_buf, 0, sizeof(hmac_sha1_buf));
	
	hmac_sha1((unsigned char *)access_key_base64, strlen(access_key_base64),
				(unsigned char *)string_for_signature, strlen(string_for_signature),
				(unsigned char *)hmac_sha1_buf);
	
	//UsartPrintf(USART_DEBUG, "hmac_sha1_buf: %s\r\n", hmac_sha1_buf);
	
//----------------------------------------------------将加密结果进行Base64编码------------------------------------------------------
	olen = 0;
	memset(sign_buf, 0, sizeof(sign_buf));
	BASE64_Encode((unsigned char *)sign_buf, sizeof(sign_buf), &olen, (unsigned char *)hmac_sha1_buf, strlen(hmac_sha1_buf));

//----------------------------------------------------将Base64编码结果进行URL编码---------------------------------------------------
	OTA_UrlEncode(sign_buf);
	//UsartPrintf(USART_DEBUG, "sign_buf: %s\r\n", sign_buf);
	
//----------------------------------------------------计算Token--------------------------------------------------------------------
	snprintf(authorization_buf, authorization_buf_len, "version=%s&res=products%%2F%s&et=%d&method=%s&sign=%s", ver, res, et, METHOD, sign_buf);
	//UsartPrintf(USART_DEBUG, "Token: %s\r\n", token_buf);
	
	return 0;

}

/*
************************************************************
*	函数名称：	OTA_ReportVersion
*
*	函数功能：	上报当前版本
*
*	入口参数：	dev_id：设备ID
*				ver：版本
*				authorization：
*
*	返回参数：	0-成功	其他-失败
*
*	说明：		
************************************************************
*/
static unsigned char OTA_ReportVersion(char *dev_id, char *ver, char *authorization)
{
	
	unsigned char result = 255;
	unsigned char* ptr_temp_data = NULL;
	char *data_ptr = NULL;
	char send_buf[296];
	int time_out = 2000;
	
	if(dev_id == NULL || ver == NULL || authorization == NULL)
		return 1;
	
	memset(send_buf, 0, sizeof(send_buf));
	snprintf(send_buf, sizeof(send_buf), "POST /ota/device/version?dev_id=%s HTTP/1.1\r\n"
											"Authorization:%s\r\n"
											"Host:ota.heclouds.com\r\n"
											"Content-Type:application/json\r\n"
											"Content-Length:%d\r\n\r\n"
											"{\"s_version\":\"%s\"}",
											dev_id, authorization, strlen(ver) + 16, ver);
	
	net_device_info.cmd_ipd = NULL;
	
	NET_DEVICE_SendData((unsigned char *)send_buf, strlen(send_buf));
	
	//ptr_temp_data = NET_DEVICE_Read();
	
	while(--time_out)
	{
		//net_device_info.cmd_ipd = (char*)NET_DEVICE_Read();
		if(net_device_info.cmd_ipd != NULL)
			break;
		
		//DelayXms(20);
	}
	
	if(time_out)
	{
		data_ptr = strstr(net_device_info.cmd_ipd, "succ");
	}
	
	if(data_ptr != NULL)
	{
//		UsartPrintf(USART_DEBUG, "Tips:	OTA_ReportVersion Ok\r\n");
		
		result = 0;
	}
	else
		OTA_ErrMsg("OTA_ReportVersion", net_device_info.cmd_ipd);
	
	return result;

}

/*
************************************************************
*	函数名称：	OTA_Check
*
*	函数功能：	检测是否需要升级
*
*	入口参数：	dev_id：设备ID
*				authorization：
*
*	返回参数：	0-成功	其他-失败
*
*	说明：		
************************************************************
*/
static unsigned char OTA_Check(char *dev_id, char *authorization)
{
	
	unsigned char result = 255;
	char *data_ptr = NULL;
	char send_buf[320];
	int time_out = 1000;
	
	if(dev_id == NULL || authorization == NULL)
		return 1;
	
	memset(send_buf, 0, sizeof(send_buf));
	snprintf(send_buf, sizeof(send_buf), "GET /ota/south/check?"
											"dev_id=%s&manuf=100&model=10001&type=2&version=%s&cdn=false HTTP/1.1\r\n"
											"Authorization:%s\r\n"
											"Host:ota.heclouds.com\r\n\r\n",
											dev_id, cur_version,authorization);
	
	net_device_info.cmd_ipd = NULL;
	
	NET_DEVICE_SendData((unsigned char *)send_buf, strlen(send_buf));
	
	while(--time_out)
	{
		if(net_device_info.cmd_ipd != NULL)
			break;
		
		DelayXms(50);
	}
	
	if(time_out)
	{
		data_ptr = strstr(net_device_info.cmd_ipd, "target");
	}
	
	if(data_ptr != NULL)
	{
		//errno":0,"error":"succ","data":{"target":"1.0","token":"ota_e8J7tlNueb7RICV8p5Ij","size":3868,
		//"md5":"2fcaf36d96f1bdbc849eb7c034957cf3","signal":10,"power":50,"retry":0,"interval":0,"type":1}}
		
//		UsartPrintf(USART_DEBUG, "Tips:	OTA_Check Ok\r\n");
		
		if(sscanf(data_ptr, "target\":\"%[^\"]\",\"token\":\"%[^\"]\",\"size\":%d,\"md5\":\"%[^\"]\",",
								ota_info.version, ota_info.token, &ota_info.size, ota_info.md5) == 4)
		{
//			UsartPrintf(USART_DEBUG, "ver: %s, token: %s, size: %d, md5:%s\r\n",
//										ota_info.version, ota_info.token, ota_info.size, ota_info.md5);
			
			OTA_SaveParameter_Int(save_para_page, OTA_FILE_SIZE_PAGE_OFFSET, mcu_flash_type, ota_info.size);
			OTA_SaveParameter_String(save_para_page, OTA_FILE_MD5_PAGE_OFFSET, mcu_flash_type, ota_info.md5);
			OTA_SaveParameter_String(save_para_page, OTA_FILE_TOKEN_PAGE_OFFSET, mcu_flash_type, ota_info.token);
			
			result = 0;
		}
	}
	else
		OTA_ErrMsg("OTA_Check", net_device_info.cmd_ipd);
	
	return result;

}

/*
************************************************************
*	函数名称：	OTA_CheckToken
*
*	函数功能：	校验Token
*
*	入口参数：	dev_id：设备ID
*				token：平台返回的Token
*				authorization：
*
*	返回参数：	0-成功	其他-失败
*
*	说明：		
************************************************************
*/
static unsigned char OTA_CheckToken(char *dev_id, char *token, char *authorization)
{

	unsigned char result = 255;
	char *data_ptr = NULL;
	char send_buf[320];
	unsigned char time_out = 200;

	if(!net_device_info.net_work)
		return result;
	
	if(dev_id == NULL || token == NULL || authorization == NULL)
		return 1;
	
	memset(send_buf, 0, sizeof(send_buf));
	snprintf(send_buf, sizeof(send_buf), "GET /ota/south/download/"
											"%s/check?dev_id=%s HTTP/1.1\r\n"
											"Authorization:%s\r\n"
											"Host:ota.heclouds.com\r\n\r\n",
											token, dev_id, authorization);
	
	net_device_info.cmd_ipd = NULL;
	
	NET_DEVICE_SendData((unsigned char *)send_buf, strlen(send_buf));
	
	while(--time_out)
	{
		if(net_device_info.cmd_ipd != NULL)
			break;
		
		DelayXms(20);
	}
	
	if(time_out)
	{
		data_ptr = strstr(net_device_info.cmd_ipd, "succ");
	}
	
	if(data_ptr != NULL)
	{
//		UsartPrintf(USART_DEBUG, "Tips:	OTA_CheckToken Ok\r\n");
		
		result = 0;
	}
	else
		OTA_ErrMsg("OTA_CheckToken", net_device_info.cmd_ipd);
	
	return result;

}

#if(OTA_REPORT_DOWNLOAD_PROGRESS == 1)
/*
************************************************************
*	函数名称：	OTA_Report_DownloadProgress
*
*	函数功能：	上报下载进度
*
*	入口参数：	dev_id：设备ID
*				token：平台返回的Token
*				authorization：
*				step：下载进度(0~100)
*
*	返回参数：	0-成功	其他-失败
*
*	说明：		
************************************************************
*/
static unsigned char OTA_Report_DownloadProgress(char *dev_id, char *token, char *authorization, unsigned char step)
{

	unsigned char result = 255;
	char *data_ptr = NULL;
	char send_buf[320];
	unsigned char time_out = 200;
	unsigned char num = 0;

	if(!net_device_info.net_work)
		return result;
	
	if(dev_id == NULL || token == NULL || authorization == NULL || (step < 1 || step > 150))
		return result;
	
	if(step < 10)
		num = 1;
	else if(step >= 10 && step < 100)
		num = 2;
	else if(step >= 100)
		num = 3;
	
	memset(send_buf, 0, sizeof(send_buf));
	snprintf(send_buf, sizeof(send_buf), "POST /ota/south/device/download/"
											"%s/progress?dev_id=%s HTTP/1.1\r\n"
											"Authorization:%s\r\n"
											"Host:ota.heclouds.com\r\n"
											"Content-Type:application/json\r\n"
											"Content-Length:%d\r\n\r\n"
											"{\"step\":%d}",
											token, dev_id, authorization, 9 + num, step);
	
	net_device_info.cmd_ipd = NULL;
	
	NET_DEVICE_SendData((unsigned char *)send_buf, strlen(send_buf));
	
	while(--time_out)
	{
		if(net_device_info.cmd_ipd != NULL)
			break;
		
		DelayXms(20);
	}
	
	if(time_out)
	{
		data_ptr = strstr(net_device_info.cmd_ipd, "succ");
	}
	
	if(data_ptr != NULL)
		result = 0;
	else
		OTA_ErrMsg("OTA_Report_DownloadProgress", net_device_info.cmd_ipd);
	
	return result;

}
#endif

/*
************************************************************
*	函数名称：	OTA_Download_Range
*
*	函数功能：	分片下载固件
*
*	入口参数：	token：平台返回的Token
*				md5：平台返回的MD5
*				size：平台返回的固件大小(字节)
*				bytes_range：分片大小(字节)
*
*	返回参数：	0-成功	其他-失败
*
*	说明：		
************************************************************
*/
static unsigned char OTA_Download_Range(char *token, char *md5, unsigned int size, const unsigned short bytes_range)
{
	
	MD5_CTX md5_ctx;											//MD5相关变量
	unsigned char i = 0, md5_t[16];
	char md5_result[40];
	char md5_t1[4] = {0, 0, 0, 0};

	unsigned char result = 255;									//数据处理相关变量
	char send_buf[320];
	char *data_ptr = NULL;
	unsigned char time_out = 200;
	unsigned int bytes = 0;
	unsigned char err_cnt = 0;
	unsigned short bytes_temp = 0;
	
	
	unsigned int packet_num = 0;								//flash读写相关变量
	unsigned int write_page_start = app_start_page;
	unsigned int write_page_cnt = app_start_page;
	
//----------------------------------------------------MD5初始化、擦除闪存-----------------------------------------------------------
	MD5_Init(&md5_ctx);
	
	packet_num = size / flash_page_size + 1;
//	FLASH_Erase(write_page_start, packet_num, mcu_flash_type);
	
	while(bytes < size)
	{
//----------------------------------------------------发送报文下载一包数据----------------------------------------------------------
		if((size - bytes)>= bytes_range)
		{
			bytes_temp = bytes_range - 1;
		}
		else
		{
			bytes_temp = (size - bytes)-1;
		}
			
		memset(send_buf, 0, sizeof(send_buf));
		snprintf(send_buf, sizeof(send_buf), "GET /ota/south/download/"
												"%s HTTP/1.1\r\n"
												"Range:bytes=%d-%d\r\n"
												"Host:ota.heclouds.com\r\n\r\n",
												token, bytes, bytes + bytes_temp);
		
		net_device_info.cmd_ipd = NULL;
		
		if(NET_DEVICE_SendData((unsigned char *)send_buf, strlen(send_buf)) == 0)
		{
			err_cnt = 0;
//----------------------------------------------------等待数据---------------------------------------------------------------------
			time_out = 200;
			while(--time_out)
			{
				if(net_device_info.cmd_ipd != NULL)
					break;
				
				DelayXms(20);
			}
			
//----------------------------------------------------跳过HTTP报文头、找到固件数据--------------------------------------------------
			if(time_out)
			{
				data_ptr = strstr(net_device_info.cmd_ipd, "Range");
				data_ptr = strstr(data_ptr, "\r\n");
				data_ptr += 4;
			}
			
//----------------------------------------------------将固件数据写入缓存和闪存-----------------------------------------------------
			if(data_ptr != NULL)
			{
				//if(bytes < size - bytes_range)																	
				if((size - bytes) >= bytes_range)																	//判断是否为最后一包。满足则不是
				{
					memcpy(flash_buf + (bytes % flash_page_size), data_ptr, bytes_range);
					//UsartPrintf(USART_DEBUG, "Tips:	MD5: %s\r\n", data_ptr);
//					Usart_SendString(USART_DEBUG, (unsigned char *)data_ptr, bytes_range);
					MD5_Update(&md5_ctx, (unsigned char *)data_ptr, bytes_range);
				}
				else
				{
					memcpy(flash_buf + (bytes % flash_page_size), data_ptr, size - bytes);
					//UsartPrintf(USART_DEBUG, "Tips:	MD5: %s\r\n", data_ptr);
//					Usart_SendString(USART_DEBUG, (unsigned char *)data_ptr, size - bytes);
					MD5_Update(&md5_ctx, (unsigned char *)data_ptr, size - bytes);
				}

				bytes += bytes_temp + 1;
				if((bytes % flash_page_size) == 0)																//缓存是否保存了2KB的数据了
				{
//					Flash_Write(write_page_cnt, (unsigned short *)flash_buf, flash_page_size >> 1, mcu_flash_type);
					write_page_cnt++;
				}
				
				if(bytes >= size)																				//最后一包数据
//					Flash_Write(write_page_cnt, (unsigned short *)flash_buf, (size % flash_page_size) >> 1, mcu_flash_type);
				
				//OTA_Display(bytes, size);
				
#if(OTA_REPORT_DOWNLOAD_PROGRESS == 1)
				DelayXms(net_device_info.send_time+ 100);
				OTA_Report_DownloadProgress(dev_id, ota_info.token, ota_info.authorization, (unsigned char)((float)bytes / size * 100));
#endif
			}
		}
		else
		{
//			if(++err_cnt >= 3)
//			{
//				err_cnt = 0;
//				
//				NET_DEVICE_Close();
//				
//				while(OTA_ConnectIP())
//				{
//					InitdataGSM();
//					DelayXms(5000);
//				}
//			}
		}
		
		DelayXms(net_device_info.send_time + 100);																//为了通信的稳定
	}
	
//----------------------------------------------------MD校验比对------------------------------------------------------------------
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
	
//	UsartPrintf(USART_DEBUG, "Tips:	MD5: %s\r\n", md5_result);
	
	if(strcmp(md5_result, md5) == 0)																			//MD5校验比对
	{
//		UsartPrintf(USART_DEBUG, "Tips:	MD5 Successful Matches\r\n");
		
		ota_info.ota_download_ok = 1;
//		ota_info.addr = FLASH_GetStartAddr(write_page_start, mcu_flash_type);									//得到应用代码的跳转地址
		result = 0;
	}
	
	return result;

}

/*
************************************************************
*	函数名称：	OTA_Report
*
*	函数功能：	升级状态上报
*
*	入口参数：	dev_id：设备ID
*				token：平台返回的Token
*				authorization：
*				status：升级状态
*
*	返回参数：	0-成功	其他-失败
*
*	说明：		101---下载成功-升级包下载成功
*				102---下载失败-空间不足
*				103---下载失败-内存溢出
*				104---下载失败-下载请求超时
*				105---下载失败-电量不足
*				106---下载失败-信号不良
*				107---下载失败-未知异常
*
*				201---升级成功-升级成功
*				202---升级失败-电量不足
*				203---升级失败-内存溢出
*				204---升级失败-版本不一致
*				205---升级失败-MD5校验失败
*				206---升级失败-未知异常
************************************************************
*/
static unsigned char OTA_Report(char *dev_id, char *token, char *authorization, int status)
{

	unsigned char result = 255;
	char *data_ptr = NULL;
	char send_buf[320];
	char status_str[4];
	unsigned char time_out = 200;

	if(!net_device_info.net_work)
		return result;
	
	if(dev_id == NULL || token == NULL || authorization == NULL)
		return result;
	
	snprintf(status_str, sizeof(status_str), "%d", status);
	memset(send_buf, 0, sizeof(send_buf));
	snprintf(send_buf, sizeof(send_buf), "POST /ota/south/report?dev_id=%s&token=%s HTTP/1.1\r\n"
											"Authorization:%s\r\n"
											"Host:ota.heclouds.com\r\n"
											"Content-Type:application/json\r\n"
											"Content-Length:14\r\n\r\n"
											"{\"result\":%s}",
											dev_id, token, authorization, status_str);
	
	net_device_info.cmd_ipd = NULL;
	
	NET_DEVICE_SendData((unsigned char *)send_buf, strlen(send_buf));
	
	while(--time_out)
	{
		if(net_device_info.cmd_ipd != NULL)
			break;
		
		DelayXms(20);
	}
	
	if(time_out)
	{
		data_ptr = strstr(net_device_info.cmd_ipd, "succ");
	}
	
	if(data_ptr != NULL)
	{
//		UsartPrintf(USART_DEBUG, "Tips:	OTA Report Status Ok\r\n");
			
		ota_info.ota_report_ok = 1;
		
		result = 0;
	}
	else
		OTA_ErrMsg("OTA_Report", net_device_info.cmd_ipd);
	
	return result;

}

/*
************************************************************
*	函数名称：	OTA_Clear
*
*	函数功能：	清零化相关变量
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
static void OTA_Clear(void)
{

	ota_info.addr = 0;
	ota_info.ota_check = 0;
	ota_info.ota_download_ok = 0;
	ota_info.ota_report_ok = 0;
	ota_info.err_count = 0;
	ota_info.size = 0;
	
	memset(ota_info.version, 0, sizeof(ota_info.version));
	memset(ota_info.token, 0, sizeof(ota_info.token));
	memset(ota_info.md5, 0, sizeof(ota_info.md5));

}

/*
************************************************************
*	函数名称：	OTA_Prepare
*
*	函数功能：	OTA跳转前的准备工作
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
__weak void OTA_Prepare(void)
{

}

/*
************************************************************
*	函数名称：	OTA_Process
*
*	函数功能：	OTA处理
*
*	入口参数：	masterkey：masterkey
*				reg_code：注册码
*				dev_name：设备名
*				auth_info：鉴权信息
*
*	返回参数：	0-成功	其他-失败
*
*	说明：		
************************************************************
*/
unsigned char OTA_Process(char *masterkey, char *reg_code, char *dev_name, char *auth_info)
{
		char *initial_version = "V1.0";
	//if(ota_info.create_ok == 0)														//如果未创建设备
	//	OTA_AutoCreateDevice(masterkey, reg_code, dev_name, auth_info, dev_id);
	//OTA_WriteVersion(save_para_page, mcu_flash_type, initial_version);
	OTA_ReadVersion(save_para_page, mcu_flash_type, cur_version, sizeof(cur_version));
		
	DelayXms(200);

	OTA_Clear();
	
	while(OTA_ConnectIP())
	{
		InitdataGSM();
		DelayXms(1000);
	}
	DelayXms(3000);
	if(OTA_CheckVersion(save_para_page, mcu_flash_type) == 1)
	{
		OTA_WriteVersion(save_para_page, mcu_flash_type, initial_version);
		strncpy(cur_version, initial_version, strlen(initial_version));
	}
	else
		OTA_ReadVersion(save_para_page, mcu_flash_type, cur_version, sizeof(cur_version));
	
	//UsartPrintf(USART_DEBUG, "Version: %s\r\n", cur_version);
	//DelayXms(1000);
	OTA_ReportVersion(dev_id, cur_version, ota_info.authorization);
	DelayXms(1000);
	if(update_flag == 0xFFFFFFFF)													//如果之前未处于升级状态，则重新校验token
	{
		if(OTA_Check(dev_id, ota_info.authorization))
		{
			DelayXms(1000);
			
			if((power_on_check_flag == 1) && (power_on_check_addr > 0x08000000))
			//if(power_on_check_flag == 1)
			{
				OTA_Prepare();
				
				OTA_Jump(power_on_check_addr);
			}
			
			return 1;
		}
		
		if(OTA_CheckToken(dev_id, ota_info.token, ota_info.authorization))
			return 2;
		
		OTA_SaveParameter_Int(save_para_page, OTA_UPDATE_FLAG_PAGE_OFFSET, mcu_flash_type, 1);
																					//保存正在升级标志，如果断电或死机，重启后可重新升级固件
	}
	else
	{
//		Flash_Read(save_para_page, (unsigned short *)flash_buf, flash_page_size >> 1, mcu_flash_type);
		
		ota_info.size = flash_buf[OTA_FILE_SIZE_PAGE_OFFSET] | flash_buf[OTA_FILE_SIZE_PAGE_OFFSET + 1] << 8
						| flash_buf[OTA_FILE_SIZE_PAGE_OFFSET + 2] << 16 | flash_buf[OTA_FILE_SIZE_PAGE_OFFSET + 3] << 24;
		
		strncpy(ota_info.md5, (char *)flash_buf + OTA_FILE_MD5_PAGE_OFFSET, 32);	//读取MD5
		strncpy(ota_info.token, (char *)flash_buf + OTA_FILE_TOKEN_PAGE_OFFSET, 24);//读取Token
		
		if((ota_info.size < 3072 || ota_info.size > 473088) || (strlen(ota_info.md5) != 32) || strlen(ota_info.token) != 24)
		{
			update_flag = 0xFFFFFFFF;
			
			return 3;
		}
		
//		UsartPrintf(USART_DEBUG, "*********token: %s, size: %d, md5:%s\r\n",
//										ota_info.token, ota_info.size, ota_info.md5);
	}
	
	if(OTA_Download_Range(ota_info.token, ota_info.md5, ota_info.size, 1024))
	{
//		OTA_Report(dev_id, ota_info.token, ota_info.authorization, 107);
//		DelayXms(net_device_info.send_time);
//		OTA_Report(dev_id, ota_info.token, ota_info.authorization, 206);
		
//			UsartPrintf(USART_DEBUG, "fail-ota\r\n");
	}
	else
	{
		OTA_Report(dev_id, ota_info.token, ota_info.authorization, 101);
		DelayXms(net_device_info.send_time);
		OTA_Report(dev_id, ota_info.token, ota_info.authorization, 201);
//		UsartPrintf(USART_DEBUG, "success-ota\r\n");
	}
	
	OTA_SaveParameter_Int(save_para_page, OTA_UPDATE_FLAG_PAGE_OFFSET, mcu_flash_type, 0xFFFFFFFF);	//保存升级结束标志
	
	OTA_WriteVersion(save_para_page, mcu_flash_type, ota_info.version);
	
	if(ota_info.ota_download_ok == 1 && ota_info.ota_report_ok == 1)
	{
		OTA_Prepare();
		
		OTA_Jump(ota_info.addr);
	}
	else
		OTA_Clear();
	
	return 0;

}
