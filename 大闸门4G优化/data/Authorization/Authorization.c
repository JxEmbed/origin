#include "Authorization.h"
#include "string.h"
#include "stdio.h"
#include "base64.h"
#include "hmac_sha1.h"
#include "onenet.h"
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

*/
void Authorization_V2(char *authorization_buf, unsigned short authorization_buf_len,DateTimeDef time2,char * user_id,char * access_key)
{
	size_t olen = 0;
	char version[20];
	char res[20];//userid/5279560816674
	char et[20];
	char method[20];
	char key[65];
	char hmac_sha1_buf[64];							//保存签名
	char org[50];
	u32 timeStamp;
	char sign_buf[64];								//保存签名的Base64编码结果 和 URL编码结果
	DateTimeDef time1;
	//复制时间
	memcpy(&time1,&time2,sizeof(time1));
	if(time1.years<2000)
	{
		time1.years+=2000;
	}
	//版本
	strcpy(version,"2020-05-29");
	snprintf(res,sizeof(res),"userid/%s",user_id);
	timeStamp=DateTime_ToCounts(time1);
//	timeStamp += 3600*24;
    // 用户自定义token过期时间 一天的有效期
	timeStamp=1956499200;
	snprintf(et,sizeof(et),"%d",timeStamp);
    // 签名方法，支持md5、sha1、sha256
	strcpy(method,"sha1");
    // 对access_key进行decode
    //key = base64.b64decode(access_key)
	memset(key, 0, sizeof(key));
	BASE64_Decode((unsigned char *)key, sizeof(key), &olen, (unsigned char *)access_key, strlen(access_key));
    // 计算sign
	snprintf(org,sizeof(org),"%s\n%s\n%s\n%s",et,method,res,version);
    
	
	memset(hmac_sha1_buf, 0, sizeof(hmac_sha1_buf));
	
	hmac_sha1((unsigned char *)key, strlen(key),
				(unsigned char *)org, strlen(org),
				(unsigned char *)hmac_sha1_buf);
//----------------------------------------------------将加密结果进行Base64编码------------------------------------------------------
	olen = 0;
	memset(sign_buf, 0, sizeof(sign_buf));
	BASE64_Encode((unsigned char *)sign_buf, sizeof(sign_buf), &olen, (unsigned char *)hmac_sha1_buf, strlen(hmac_sha1_buf));
//----------------------------------------------------将Base64编码结果进行URL编码---------------------------------------------------
	OTA_UrlEncode(sign_buf);
//    sign_b = hmac.new(key=key, msg=org.encode(), digestmod=method)
//    sign = base64.b64encode(sign_b.digest()).decode()

    // value 部分进行url编码，method/res/version值较为简单无需编码
//    sign = quote(sign, safe='')
//    res = quote(res, safe='')

    // token参数拼接
//    token = 'version=%s&res=%s&et=%s&method=%s&sign=%s' % (version, res, et, method, sign)
	snprintf(authorization_buf, authorization_buf_len, "version=%s&res=%s&et=%s&method=%s&sign=%s", version, res, et, method, sign_buf);
//    return token
//				SEGGER_RTT_printf(0,RTT_CTRL_TEXT_RED"authorization_buf:%s\r\n",authorization_buf);
}

