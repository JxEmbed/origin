#include "onenetdata.h"
#include "string.h"
#include "stdio.h"
#include "MqttKit.h"

char Cmd_Buff[200]={0};
//==========================================================
//	函数名称：	OneNETAdd_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：		
//==========================================================
u8 OneNETAdd_RevPro(unsigned char *cmd)
{
	u8 ret = 0;
	char strtemp[100];
	char strtemp2[50];

	char * strPtrtemp;
	char * strPtrtemp2;
	memset(strtemp,0,sizeof(strtemp));
	/*
		系统向设备下发命令  $sys/{pid}/{device-name}/cmd/request/{cmdid}
		+MSUB: "$sys/mWLGM6Wq1p/test1/cmd/request/112f6af1-cb92-4a5f-95ef-5bbd0c304ea9",10 byte,{"test":1}
		
		系统回复"设备命令应答成功" $sys/{pid}/{device-name}/cmd/response/{cmdid}/accepted
		+MSUB: "$sys/mWLGM6Wq1p/test1/cmd/response/112f6af1-cb92-4a5f-95ef-5bbd0c304ea9/accepted",0 byte,
	*/
	snprintf(strtemp,sizeof(strtemp),"$sys/%s/%s/cmd/request/",onenet_info.pro_id,onenet_info.dev_name);
	//从打印的cmd消息寻找 系统向设备下发的命令
	strPtrtemp=strstr((char *)cmd,strtemp);
	if(strPtrtemp!=NULL)
	{
		//strPtrtemp2   收到的主题内容
		strPtrtemp2=strstr(strPtrtemp,"request/");
		strPtrtemp=strstr(strPtrtemp2,"\"");
		memset(strtemp2,0,sizeof(strtemp2));
		//cmdid 为平台为该命令自动创建的唯一标识  strtemp2=112f6af1-cb92-4a5f-95ef-5bbd0c304ea9
		memcpy(strtemp2,(strPtrtemp2+8),(strPtrtemp-strPtrtemp2-8));
		sprintf(strtemp,"$sys/%s/%s/cmd/response/%s",onenet_info.pro_id,onenet_info.dev_name,strtemp2);
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_CYAN"topic=%s\r\n",strtemp);
		
		//切割主题内容  strPtrtemp=byte,{"test":1}
		strPtrtemp=strstr((char *)cmd,"byte");
		if(strPtrtemp!=NULL)
		{
			strPtrtemp=strstr(strPtrtemp,","); 
			//strPtrtemp=strPtrtemp;
			strPtrtemp2=strstr(strPtrtemp,"\r");
			if(strPtrtemp2-strPtrtemp-1<200)		//检测消息内容长度		接收消息内容最大4100字节
			{
				//Cmd_Buff 只有200字节
				memcpy(Cmd_Buff,(strPtrtemp+1),(strPtrtemp2-strPtrtemp-1));
			}
			else
			{
				SEGGER_RTT_printf(0,RTT_CTRL_TEXT_GREEN"Message beyond 200 byte\r\n");
				return -2;
			}
		}
		
		//回复平台   回复5次无回复则ret = -1 回复则ret = 0
		ret = OneNET_SendCMD(strtemp);
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_GREEN"Cmd_Buff=%s\r\n",Cmd_Buff);
		return ret;
	}
}


