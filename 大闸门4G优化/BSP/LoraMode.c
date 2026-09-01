#include "LoraMode.h"		

#include "tool.h" 
#include "main.h"

u8 USART2rxbuffer[17];
u8 USART2txbuffer[17];

//u16  LockCoilChargeData;


//***********************************************************************

//************************************************************************
s8 LoraConf(u16 addr,u8 channel)
{
	u8 timeCount=0;
	
//	u8 result,;
//	u16 i;
	//00 00 00 62 00 17 03 00 00
	//00 00 00 62 00 17 03 00 00
	USART2txbuffer[0]=0xC0;  //设置寄存器指令
	USART2txbuffer[1]=0x00;  //起始地址
	USART2txbuffer[2]=0x09;  //长度
	/*00H*/
	USART2txbuffer[3]=(addr>>8);  //[00]模块地址高字节
	/*01H*/
	USART2txbuffer[4]=(addr&0xff);  //[01]模块地址低字节
	/*02H*/
	USART2txbuffer[5]=NETID00;  //[02]网络地址 
	/*03H*/
	USART2txbuffer[6]=Rate9600|Check8N1|Air9D6K;  //[03]REG0  波特率(011 9600)，串口校验位(00 8N1),无线空中速率(010 2.4k)  0x62
	/*04H*/
	USART2txbuffer[7]=PACK240|RSSIDIS|TRANS22dBm;  //[04]REG1  分包设定(00 240字节) RSSI环境噪声使能(0 禁用),(000 保留),发射功率(00 22dBm 最大功率)
	/*05H*/
	USART2txbuffer[8]=channel;  //[05]REG2  信道控制
	/*06H*/
	USART2txbuffer[9]=RSSIByteDIS|TransFixedPoint|RepeatDIS|LBTDIS|WORSender|WORCycle500;  //[06]REG3  (0101 禁用RSSI,定点传输，关闭中继功能，启用LBT),(0000 WOR接收 500ms周期)
	/*07USART2txbufferH*/
	USART2txbuffer[10]=(LocalSecretKey>>8);  //[07]CRYPT_H 字节(0000)
	/*08H*/
	USART2txbuffer[11]=(LocalSecretKey&0xff);  //[08]CRYPY_L 低字节(0000)
	LoraSetMode(2);	
	delay_ms(500); //延时等待切换
	
//	USART_write(COM_PORT_2,(u8 *)USART2txbuffer,12);
	USART_Send((u8 *)USART2txbuffer,12);
	while(1)
	{
		if(g_MainSignal&MSI_UART2_RECV_OVER)
		{
			g_MainSignal&=~MSI_UART2_RECV_OVER;
			if((g_Usart[COM_PORT_2].rxbuffer[0]==0xC1)&&(g_Usart[COM_PORT_2].rxbuffer[1]==0x00)&&(g_Usart[COM_PORT_2].rxbuffer[2]==0x09))
			{
				delay_ms(20); //延时等待切换
				LoraSetMode(1);
				return 0;
			}
		}
		timeCount++;
		if(timeCount>30)
		{
			delay_ms(20); //延时等待切换
			LoraSetMode(1);
			return -1;
		}
		delay_ms(20);
	}
	
	
}
//************************************************************
//参数
//	0：传输模式
//	1：WOR模式
//	2：配置模式
//	3：深度休眠
//*************************************************************
void LoraSetMode(uint8_t mode) //设置工作模式
{
	while(LoraAUX==1);
	delay_ms(10);
	LoraM1=mode>>1;
	LoraM0=mode&0x01;
	while(LoraAUX==1);
	delay_ms(50);
}
void Lora_Wait(void) //等待发送完毕
{
	while(LoraAUX==1);
	delay_ms(30);
}




/*
操作阀控器的函数为如下：
参数：
Addr:Lora地址
Point:1 or 2 第一个还是第二个
state:0 or 1 开还是关

*/
s8 Lora_OperaValve(char * devCode,u8 Point,u8 state)
{
	u8 i=0;
	s32 sRet;
	u16 timeout=0;
//	u16 Addr;  //lora
//	u8 Valve;  //阀门编号
	
	
	for(i=0;i<4;i++)
	{
		Lora_SendValve(devCode,Point,state); //控制阀门的打开
		
		while(1)
		{
			vTaskDelay(100);
			timeout++;
			if(timeout>30)
			{
				timeout=0;
				break;
			}
			if(g_MainSignal&MSI_UART2_RECV_OVER)
			{
				g_MainSignal&=~MSI_UART2_RECV_OVER;
				//将接收到的数据复制出
				memcpy(g_databuffer,g_Usart[COM_PORT_2].rxbuffer,g_Usart[COM_PORT_2].rxlen);
				
				
				if(Lora_Analysis(g_databuffer,g_Usart[COM_PORT_2].rxlen,devCode,Point,state)==1)
				{
					
					if(state==0)
					{
						vTaskDelay(800);
						return 0;  //成功关闭
					}
					else
					{
						vTaskDelay(800);
						return 1;  //成功打开
					}
				}
				
				
				
			}
		}
		
	}
	if(state==0)  //关闭失败
	{
		
		return 2;
	}
	else          //打开失败
	{
		
		return 2;
	}
}
/*
查询灌溉指令
*/
void Lora_SendQueryIrr(char * devCode)
{
	char tempu8[20];
	char SendBuff[100]={0};
	u16 crcCalc;
	u16 strl=0;
	SendBuff[0]=(AllAddr>>8);
	SendBuff[1]=(AllAddr&0xff);
	SendBuff[2]=AllChannel;
/***********************************/
	strcat(SendBuff,"C:");
	strcat(SendBuff,IrriApplyCmd);  //连接字符串
	strcat(SendBuff,";");
/***********************************/
	strcat(SendBuff,"R:");
	strcat(SendBuff,devCode);      //水源的设备编号
	strcat(SendBuff,";");
/***********************************/
	strcat(SendBuff,"T:");
	strcat(SendBuff,(char *)OtherSet_Par.ProjectNo);
	strcat(SendBuff,";");
/***********************************/
	strcat(SendBuff,"D:");

	strcat(SendBuff,"query;");
	
/***********************************/	
	strl=strlen(SendBuff);
	crcCalc=crc((u8 *)SendBuff+3,strl-3);  
	SendBuff[strl++]=(crcCalc>>8);
	SendBuff[strl++]=(crcCalc&0xff);


	USART_Send((u8 *)SendBuff,strl);
//	Lora_Wait();
}
/*
查询气象站
*/
void Lora_SendQueryWeather(DateTimeDef time1)
{
	char tempu8[50];
	char SendBuff[100]={0};
	u16 crcCalc;
	u16 strl=0;
	SendBuff[0]=(WeatherAddr>>8);
	SendBuff[1]=(WeatherAddr&0xff);
	SendBuff[2]=AllChannel;
/***********************************/
	SendBuff[3]=0x05;   //查询气象站
/***********************************/
	//Time:年/月/日 时:分:秒
	sprintf(tempu8,"Time:%d/%d/%d %d:%d:%d\r\n",time1.years,time1.months,time1.days,
	time1.hours,time1.minutes,time1.secs); //时间
	strcat(SendBuff,tempu8); //连接字符串

/***********************************/	
	strl=strlen(SendBuff);
	crcCalc=crc((u8 *)SendBuff+3,strl-3);  
	SendBuff[strl++]=(crcCalc>>8);
	SendBuff[strl++]=(crcCalc&0xff);


	USART_Send((u8 *)SendBuff,strl);
//	Lora_Wait();
}
/*
发送灌溉指令
*/
void Lora_SendIrr(char * devCode,u8 opera,u8 IrrMode, float IrrPara,u8 clearFlag)
{
	char tempu8[50];
	char SendBuff[100]={0};
	u16 crcCalc;
	u16 strl=0;
	SendBuff[0]=(AllAddr>>8);
	SendBuff[1]=(AllAddr&0xff);
	SendBuff[2]=AllChannel;
/***********************************/
	strcat(SendBuff,"C:");
	strcat(SendBuff,IrriApplyCmd);  //连接字符串
	strcat(SendBuff,";");
/***********************************/
	strcat(SendBuff,"R:");
	strcat(SendBuff,devCode);      //水源的设备编号
	strcat(SendBuff,";");
/***********************************/
	strcat(SendBuff,"T:");
	strcat(SendBuff,(char *)OtherSet_Par.ProjectNo);
	strcat(SendBuff,";");
/***********************************/
	strcat(SendBuff,"D:");
	strcat(SendBuff,"irr-"); //灌溉指令
	//第一个参数为灌溉方式,第二个参数为灌溉参数
	sprintf(tempu8,"%d-%d-%f-%d;",opera,IrrMode,IrrPara,clearFlag); 
	strcat(SendBuff,tempu8);
	
/***********************************/	
	strl=strlen(SendBuff);
	crcCalc=crc((u8 *)SendBuff+3,strl-3);  
	SendBuff[strl++]=(crcCalc>>8);
	SendBuff[strl++]=(crcCalc&0xff);


	USART_Send((u8 *)SendBuff,strl);
//	Lora_Wait();
}
/**/

/*
2023年1月10日14:38:12
查询灌溉
*/
s8 Lora_QueryIrr(void)
{
	u8 i=0;
	s32 sRet;
	u16 timeout=0;
	float para1;
//	u16 Addr;  //lora
//	u8 Valve;  //阀门编号
	
	xSemaphoreTake(Lora_Mutex,Lora_xBlockTime);  //申请互斥信号量
	for(i=0;i<4;i++)
	{
/*
		2023年1月10日10:40:06
		进行参数计算
		*/
		//手动时序控制
		Lora_SendQueryIrr(IrriCode);
		while(1)
		{
			vTaskDelay(100);
			timeout++;
			if(timeout>30)
			{
				timeout=0;
				break;
			}
			if(g_MainSignal&MSI_UART2_RECV_OVER)
			{
				g_MainSignal&=~MSI_UART2_RECV_OVER;
				//将接收到的数据复制出
				memcpy(g_databuffer,g_Usart[COM_PORT_2].rxbuffer,g_Usart[COM_PORT_2].rxlen);
				
				
				if(Lora_Analysis(g_databuffer,g_Usart[COM_PORT_2].rxlen,IrriCode,0,0)==2)
				{
					xSemaphoreGive(Lora_Mutex);
					return 1;
				}
				
				
				
			}
		}
		
	}
	xSemaphoreGive(Lora_Mutex);
	return -1;
}
/*
查询气象站
*/
s8 Lora_QueryWeather(void)
{
	u8 i=0;
	s32 sRet;
	u16 timeout=0;
	float para1;
//	u16 Addr;  //lora
//	u8 Valve;  //阀门编号
	
	xSemaphoreTake(Lora_Mutex,Lora_xBlockTime);  //申请互斥信号量
	for(i=0;i<3;i++)
	{
/*
		2023年1月10日10:40:06
		进行参数计算
		*/
		//手动时序控制
		Lora_SendQueryWeather(LocalTime);
		while(1)
		{
			vTaskDelay(100);
			timeout++;
			if(timeout>10)
			{
				timeout=0;
				break;
			}
			if(g_MainSignal&MSI_UART2_RECV_OVER)
			{
				g_MainSignal&=~MSI_UART2_RECV_OVER;
				//将接收到的数据复制出
				memcpy(g_databuffer,g_Usart[COM_PORT_2].rxbuffer,g_Usart[COM_PORT_2].rxlen);
				
				
				if(Lora_AnalysisWeather(g_databuffer,g_Usart[COM_PORT_2].rxlen)==2)
				{
					Flash_SaveWeatherLog(FlashMag_Par.WeatherLogPtr++);
					
					if(FlashMag_Par.WeatherLogNum<WeatherLogLen)
					{
						FlashMag_Par.WeatherLogNum++;
					}
					else 
					{
						FlashMag_Par.WeatherLogNum=WeatherLogLen;
					}
					if(FlashMag_Par.WeatherLogPtr>WeatherLogLen)
					{
						FlashMag_Par.WeatherLogPtr=0;
					}
					Flash_SaveFlashMag();
					xSemaphoreGive(Lora_Mutex);
					return 1;
				}
				
				
				
			}
		}
		
	}
	xSemaphoreGive(Lora_Mutex);
	return -1;
}
/*
n 墒情站编号*/
s8 Lora_QueryMoisture(u8 n)
{
	u8 i=0;
	s32 sRet;
	u16 timeout=0;
	float para1;
//	u16 Addr;  //lora
//	u8 Valve;  //阀门编号
	
	xSemaphoreTake(Lora_Mutex,Lora_xBlockTime);  //申请互斥信号量
	for(i=0;i<3;i++)
	{
/*
		2023年1月10日10:40:06
		进行参数计算
		*/
		//手动时序控制
		Lora_SendQueryMoisture(n);
		while(1)
		{
			vTaskDelay(100);
			timeout++;
			if(timeout>10)
			{
				timeout=0;
				break;
			}
			if(g_MainSignal&MSI_UART2_RECV_OVER)
			{
				g_MainSignal&=~MSI_UART2_RECV_OVER;
				//将接收到的数据复制出
				memcpy(g_databuffer,g_Usart[COM_PORT_2].rxbuffer,g_Usart[COM_PORT_2].rxlen);
				
				
				if(Lora_AnalysisMoisture(g_databuffer,g_Usart[COM_PORT_2].rxlen,n)==2)
				{
					Flash_SaveMoistureLog(FlashMag_Par.MoistureLogPtr[n]++,n);
					if(FlashMag_Par.MoistureLogNum[n]<MoistureLogLen)
					{
						FlashMag_Par.MoistureLogNum[n]++;
					}
					else 
					{
						FlashMag_Par.MoistureLogNum[n]=MoistureLogLen;
					}
					if(FlashMag_Par.MoistureLogPtr[n]>MoistureLogLen)
					{
						FlashMag_Par.MoistureLogPtr[n]=0;
					}
					Flash_SaveFlashMag();
					xSemaphoreGive(Lora_Mutex);
					return 1;
				}
			}
		}
		
	}
	xSemaphoreGive(Lora_Mutex);
	return -1;
}
s8 Lora_SendQueryMoisture(u8 n)
{
	char tempu8[20];
	char SendBuff[100]={0};
	u16 crcCalc;
	u16 strl=0;
	SendBuff[0]=(AllAddr>>8);
	SendBuff[1]=(AllAddr&0xff);
	SendBuff[2]=AllChannel;
/***********************************/
	strcat(SendBuff,"C:");
	strcat(SendBuff,MoisQueryCmd);  //连接字符串
	strcat(SendBuff,";");
/***********************************/
	sprintf(tempu8,"R:%d;",(101+n));
	strcat(SendBuff,tempu8);
/***********************************/
	strcat(SendBuff,"T:");
	strcat(SendBuff,(char *)OtherSet_Par.ProjectNo);
	strcat(SendBuff,";");
/***********************************/
	strcat(SendBuff,"D:");
	strcat(SendBuff,"query;");
	strl=strlen(SendBuff);
	crcCalc=crc((u8 *)SendBuff+3,strl-3);  
	SendBuff[strl++]=(crcCalc>>8);
	SendBuff[strl++]=(crcCalc&0xff);


	USART_Send((u8 *)SendBuff,strl);
//	Lora_Wait();
}
/*
操作灌溉水泵

*/
s8 Lora_OperaIrr(u8 opera,u8 clearFlag)
{
	u8 i=0;
	s32 sRet;
	u16 timeout=0;
	float para1;
//	u16 Addr;  //lora
//	u8 Valve;  //阀门编号
	
	xSemaphoreTake(Lora_Mutex,Lora_xBlockTime);  //申请互斥信号量
	for(i=0;i<4;i++)
	{
/*
		2023年1月10日10:40:06
		进行参数计算
		*/
		//手动时序控制
		if(OtherSet_Par.IrrMode==0)
		{
//			para1=IrrGroup_Par[realtimedata.RunGroupPtr].TimingPara;
			para1=1;
		}
		else if(OtherSet_Par.IrrMode==1)
		{
//			para1=IrrGroup_Par[realtimedata.RunGroupPtr].IrrigationArea*IrrGroup_Par[realtimedata.RunGroupPtr].RationPara;
			para1=1;
		}
		else if(OtherSet_Par.IrrMode==2)
		{
			
//			para1=IrrGroup_Par[realtimedata.RunGroupPtr].TimingPara;
			para1=1;
		}
		else if(OtherSet_Par.IrrMode==3)
		{
//			para1=IrrGroup_Par[realtimedata.RunGroupPtr].IrrigationArea*IrrGroup_Par[realtimedata.RunGroupPtr].RationPara;
			para1=1;
		}
		Lora_SendIrr(IrriCode,opera,OtherSet_Par.IrrMode,para1,clearFlag); //控制阀门的打开
		while(1)
		{
			vTaskDelay(100);
			timeout++;
			if(timeout>30)
			{
				timeout=0;
				break;
			}
			if(g_MainSignal&MSI_UART2_RECV_OVER)
			{
				g_MainSignal&=~MSI_UART2_RECV_OVER;
				//将接收到的数据复制出
				memcpy(g_databuffer,g_Usart[COM_PORT_2].rxbuffer,g_Usart[COM_PORT_2].rxlen);
				
				
				if(Lora_Analysis(g_databuffer,g_Usart[COM_PORT_2].rxlen,IrriCode,opera,0)>0)
				{
					
					if(opera==0)
					{
						xSemaphoreGive(Lora_Mutex);
						return 0;
					}
					else
					{
						xSemaphoreGive(Lora_Mutex);
						return 1;
					}
				}
				
				
				
			}
		}
		
	}
	if(opera==0)  //关闭失败
	{
		xSemaphoreGive(Lora_Mutex);
		return 2;
	}
	else          //打开失败
	{
		xSemaphoreGive(Lora_Mutex);
		return 3;
	}
}
/*
发送指令关闭全部的电磁阀
*/
void Lora_SendValveAllClose(void)
{
	char tempu8[20];
	char SendBuff[100]={0};
	u16 crcCalc;
	u16 strl=0;
	SendBuff[0]=(AllAddr>>8);
	SendBuff[1]=(AllAddr&0xff);
	SendBuff[2]=AllChannel;
/***********************************/
	strcat(SendBuff,"C:");
	strcat(SendBuff,ValvRadioCmd);  //连接字符串
	strcat(SendBuff,";");
/***********************************/
	strcat(SendBuff,"R:all");
	strcat(SendBuff,";");
/***********************************/
	strcat(SendBuff,"T:");
	strcat(SendBuff,(char *)OtherSet_Par.ProjectNo);
	strcat(SendBuff,";");
/***********************************/
	strcat(SendBuff,"D:");
	strcat(SendBuff,"allclose;");
	strl=strlen(SendBuff);
	crcCalc=crc((u8 *)SendBuff+3,strl-3);  
	SendBuff[strl++]=(crcCalc>>8);
	SendBuff[strl++]=(crcCalc&0xff);


	USART_Send((u8 *)SendBuff,strl);
//	Lora_Wait();
}
/*
解析气象站数据
*/
s8 Lora_AnalysisWeather(u8 * E22_RxBuff,u16 len)
{
	u16 i=0;
	u16 u16temp;
	s8 result;
	uint16_t crcCalc; //存储计算出来的crc结果
	uint16_t crcRx;   //接收到的crc
	char * ptrn,* ptrm;
	if(len<4)     //如果数据过短则退出
	{
		return -1;
	}
	crcCalc=crc(E22_RxBuff,len-2);                       //计算的crc
	crcRx=(E22_RxBuff[len-2]<<8)+E22_RxBuff[len-1]; //接收到的crc
	if(crcCalc!=crcRx)   //如果数据crc校验失败则退出
	{
		return -1;    
	}
	ptrn=strstr((char *)E22_RxBuff,"Time:");    //查找H:
	result=sscanf((char *)ptrn,"Time:%d/%d/%d,",&WeatherData_Temp.time.years,&WeatherData_Temp.time.months,&WeatherData_Temp.time.days);
	if(result!=3)
	{
		return -1;
	}
	ptrn=strstr((char *)E22_RxBuff,"TMeanLast:");
	result=sscanf((char *)ptrn,"TMeanLast:%f,",&WeatherData_Temp.TMeanLast);
	if(result!=1)
	{
		return -1;
	}
	ptrn=strstr((char *)E22_RxBuff,"TMean:");
	result=sscanf((char *)ptrn,"TMean:%f,",&WeatherData_Temp.TMean);
	if(result!=1)
	{
		return -1;
	}
	ptrn=strstr((char *)E22_RxBuff,"TMax:");
	result=sscanf((char *)ptrn,"TMax:%f,",&WeatherData_Temp.TMax);
	if(result!=1)
	{
		return -1;
	}
	ptrn=strstr((char *)E22_RxBuff,"TMin:");
	result=sscanf((char *)ptrn,"TMin:%f,",&WeatherData_Temp.TMin);
	if(result!=1)
	{
		return -1;
	}
	ptrn=strstr((char *)E22_RxBuff,"RHMean:");
	result=sscanf((char *)ptrn,"RHMean:%f,",&WeatherData_Temp.RHMean);
	if(result!=1)
	{
		return -1;
	}
	ptrn=strstr((char *)E22_RxBuff,"Wind:");
	result=sscanf((char *)ptrn,"Wind:%f,",&WeatherData_Temp.Wind);
	if(result!=1)
	{
		return -1;
	}
	ptrn=strstr((char *)E22_RxBuff,"Sunhour:");
	result=sscanf((char *)ptrn,"Sunhour:%f,",&WeatherData_Temp.Sunhour);
	if(result!=1)
	{
		return -1;
	}
	ptrn=strstr((char *)E22_RxBuff,"Rain:");
	result=sscanf((char *)ptrn,"Rain:%f,",&WeatherData_Temp.Rain);
	if(result!=1)
	{
		return -1;
	}
	ptrn=strstr((char *)E22_RxBuff,"J:");
	result=sscanf((char *)ptrn,"J:%f,",&WeatherData_Temp.J);
	if(result!=1)
	{
		return -1;
	}
	ptrn=strstr((char *)E22_RxBuff,"ET0:");
	result=sscanf((char *)ptrn,"ET0:%f\r\n",&WeatherData_Temp.ET0);
	if(result!=1)
	{
		return -1;
	}
	return 2;
}
/*
解析墒情站数据
*/
s8 Lora_AnalysisMoisture(u8 * E22_RxBuff,u16 len,u8 n)
{
	u8 Voltagedat[4];
	u16 i=0;
	u16 u16temp;
	
	uint16_t crcCalc; //存储计算出来的crc结果
	uint16_t crcRx;   //接收到的crc
	char * ptrn,* ptrm;
	char operaCode[5]={0};   //操作码
	char slaveCode[20]={0};  //从机编码
	char hostCode[20]={0};   //主机编码
	char datas[100]={0};      //数据
	char rxValve[20]={0};    //阀门
	char rxState[20]={0};    //状态
	char rxPress[20]={0};    //压力
	char  devCode[10];
	int res=0;
	sprintf(devCode,"%d",101+n);
	u8 rxValveU8;
	u8 rxStateU8;
	if(len<4)     //如果数据过短则退出
	{
		return -1;
	}
	crcCalc=crc(E22_RxBuff,len-2);                       //计算的crc
	crcRx=(E22_RxBuff[len-2]<<8)+E22_RxBuff[len-1]; //接收到的crc
	if(crcCalc!=crcRx)   //如果数据crc校验失败则退出
	{
		return -1;    
	}
	/******************************************************************/
	ptrm= strstr((char *)E22_RxBuff,"C:");  //查找C:
	ptrn=strchr(ptrm,';');    //查找;
	if((ptrn-ptrm-2>=sizeof(operaCode))||ptrm==NULL||ptrn==NULL)
	{
		return -1;
	}
	
	memcpy(operaCode,ptrm+2,ptrn-ptrm-2); //复制
/******************************************************************/

	ptrm=strstr(E22_RxBuff,"R:");     //查找S:
	ptrn=strchr(ptrm,';');            //查找;
	if(ptrn-ptrm-2>=sizeof(slaveCode)||ptrm==NULL||ptrn==NULL)
	{
		return -1;
	}
	memcpy(slaveCode,ptrm+2,ptrn-ptrm-2);  //复制
/******************************************************************/
	ptrm=strstr(E22_RxBuff,"T:");    //查找H:
	ptrn=strchr(ptrm,';');           //查找;
	if(ptrn-ptrm-2>sizeof(hostCode)||ptrm==NULL||ptrn==NULL)
	{
		return -1;
	}
	memcpy(hostCode,ptrm+2,ptrn-ptrm-2);
/******************************************************************/	
	ptrm=strstr(E22_RxBuff,"D:");   //查找D:
	ptrn=strchr(ptrm,';');          //查找;
	if(ptrn-ptrm-2>sizeof(datas)||ptrm==NULL||ptrn==NULL)
	{
		return -1;
	}
	memcpy(datas,ptrm+2,ptrn-ptrm-2); //复制
/******************************************************************/	
	if(strcmp(hostCode,devCode)!=0) 
	{
		return -1;
	}
	if(strcmp(slaveCode,OtherSet_Par.ProjectNo)!=0)
	{
		return -1;
	}
	//C:09;R:01;T:101;D:010.59,009.52,006.70,005.45,005.33,006.90,010.74,009.86,3855;V>
	if(strcmp(operaCode,MoisQueryCmd)==0) //请求墒情数据指令
	{
		res=sscanf((char *)datas,"%f,%f,%f,%f,%f,%f,%f,%f,%s",&MoistureLog_Temp.water[0],&MoistureLog_Temp.water[1],&MoistureLog_Temp.water[2],&MoistureLog_Temp.water[3],
			&MoistureLog_Temp.water[4],&MoistureLog_Temp.water[5],&MoistureLog_Temp.water[6],&MoistureLog_Temp.water[7],rxPress);
		if(res!=9)
		{
			return -1;
		}
		for(i=0;i<FlashMag_Par.DeviceMagNum;i++)
		{
			if(strcmp(DeviceMag_Par[i].DevCode,devCode)==0)
			{
				u16temp=atoi(rxPress);
				DeviceMag_Par[i].Power=(float)u16temp/1000;
				break;
//				Flash_SaveDeviceMag();
			}
		}
		MoistureLog_Temp.dateTime.years=LocalTime.years;
		MoistureLog_Temp.dateTime.months=LocalTime.months;
		MoistureLog_Temp.dateTime.days=LocalTime.days;
		MoistureLog_Temp.dateTime.hours=LocalTime.hours;
		MoistureLog_Temp.dateTime.minutes=LocalTime.minutes;
		MoistureLog_Temp.dateTime.secs=LocalTime.secs;
		return 2;
	}
	
	
	
	return -1;
}
/*
解析田间电磁阀数据
小于0校验失败
*/
u16 testu16;
s8 Lora_Analysis(u8 * E22_RxBuff,u16 len,char * devCode,u8 valve,u8 state)
{
	u8 Voltagedat[4];
	u16 i=0;
	u16 u16temp;
	
	uint16_t crcCalc; //存储计算出来的crc结果
	uint16_t crcRx;   //接收到的crc
	char * ptrn,* ptrm;
	char operaCode[5]={0};   //操作码
	char slaveCode[20]={0};  //从机编码
	char hostCode[20]={0};   //主机编码
	char datas[50]={0};      //数据
	char rxValve[20]={0};    //阀门
	char rxState[20]={0};    //状态
	char rxPress[20]={0};    //压力
	u8 rxValveU8;
	u8 rxStateU8;
	if(len<4)     //如果数据过短则退出
	{
		return -1;
	}
	crcCalc=crc(E22_RxBuff,len-2);                       //计算的crc
	crcRx=(E22_RxBuff[len-2]<<8)+E22_RxBuff[len-1]; //接收到的crc
	if(crcCalc!=crcRx)   //如果数据crc校验失败则退出
	{
		return -1;    
	}
	/******************************************************************/
	ptrm= strstr((char *)E22_RxBuff,"C:");  //查找C:
	ptrn=strchr(ptrm,';');    //查找;
	if((ptrn-ptrm-2>=sizeof(operaCode))||ptrm==NULL||ptrn==NULL)
	{
		return -1;
	}
	
	memcpy(operaCode,ptrm+2,ptrn-ptrm-2); //复制
/******************************************************************/

	ptrm=strstr(E22_RxBuff,"R:");     //查找S:
	ptrn=strchr(ptrm,';');            //查找;
	if(ptrn-ptrm-2>=sizeof(slaveCode)||ptrm==NULL||ptrn==NULL)
	{
		return -1;
	}
	memcpy(slaveCode,ptrm+2,ptrn-ptrm-2);  //复制
/******************************************************************/
	ptrm=strstr(E22_RxBuff,"T:");    //查找H:
	ptrn=strchr(ptrm,';');           //查找;
	if(ptrn-ptrm-2>sizeof(hostCode)||ptrm==NULL||ptrn==NULL)
	{
		return -1;
	}
	memcpy(hostCode,ptrm+2,ptrn-ptrm-2);
/******************************************************************/	
	ptrm=strstr(E22_RxBuff,"D:");   //查找D:
	ptrn=strchr(ptrm,';');          //查找;
	if(ptrn-ptrm-2>sizeof(datas)||ptrm==NULL||ptrn==NULL)
	{
		return -1;
	}
	memcpy(datas,ptrm+2,ptrn-ptrm-2); //复制
/******************************************************************/	
	if(strcmp(hostCode,devCode)!=0) 
	{
		return -1;
	}
	if(strcmp(slaveCode,OtherSet_Par.ProjectNo)!=0)
	{
		return -1;
	}
	if(strcmp(operaCode,ValvOperaCmd)==0) //电磁阀操作指令
	{
		//C:13;S:01;H:11;D:01-01-3735;潼
		//01-01-3735
		ptrm=strchr(datas,'-'); //-01-3735
		if(ptrm-datas>sizeof(rxValve)||ptrm==NULL)
		{
			return -1;
		}
		memcpy(rxValve,datas,ptrm-datas); //复制
		/*===========================*/
		ptrn=strchr(ptrm+1,'-');  //-3735
		if(ptrn-ptrm-1>sizeof(rxState)||ptrn==NULL)
		{
			return -1;
		}
		memcpy(rxState,ptrm+1,ptrn-ptrm-1);
		/*===========================*/
//		ptrn=strchr(ptrn+1,'-');  //01-4048
		if(strlen(ptrn+1)>sizeof(rxState)||ptrn==NULL) //3735
		{
			return -1;
		}
		memcpy(rxPress,ptrn+1,strlen(ptrn+1));
		/*===========================*/
		rxValveU8=atoi(rxValve);
		rxStateU8=atoi(rxState);
		for(i=0;i<FlashMag_Par.DeviceMagNum;i++)
		{
			if(strcmp(DeviceMag_Par[i].DevCode,devCode)==0)
			{
				u16temp=atoi(rxPress);
				DeviceMag_Par[i].Power=(float)u16temp/1000;
				
				break;
//				Flash_SaveDeviceMag();
			}
		}
		if(rxValveU8==valve&&rxStateU8==state)
		{
			return 1;
		}
	}
	else if(strcmp(operaCode,IrriApplyCmd)==0)
	{
		//C:01;R:01;T:21;D:irr|0|0.000000|0.000000|50.247898;5?
		ptrm=strstr(datas,"irr"); //irr|0|0.000000|0.000000|50.247898;5?
		
		if(ptrm!=NULL)
		{
			if(*(ptrm+4)=='1')
			{
				irrDevice.OutFreq=50;
			}
			else if(*(ptrm+4)=='0')
			{
				irrDevice.OutFreq=0;
			}
			ptrm+=4;   //0|0.000000|0.000000|50.247898;5?
			ptrn=strchr(ptrm,'|'); //|0.000000|0.000000|50.247898;5?
			if(ptrn==NULL)
			{
				return -1;
			}
			ptrn+=1; //0.000000|0.000000|50.247898;5?
			ptrm=strchr(ptrn,'|');  //|0.000000|50.247898;5?
			if(ptrm-ptrn>sizeof(rxPress)||ptrm==NULL)
			{
				return -1;
			}
			
			memcpy(rxPress,ptrn,ptrm-ptrn); //
			
			
		/*===============================*/
			ptrm+=1;   //0.000000|50.247898;5?
			ptrn=strchr(ptrm,'|');  //|50.247898;5?
			if(ptrn==NULL)
			{
				return -1;
			}
//			ptrn+=1; //0.000000-50.247898;5
//			ptrm=strchr(ptrn,'|');//-50.247898;5
			if(ptrn-ptrm>sizeof(rxValve)||ptrn==NULL)
			{
				return -1;
			}
			
			memcpy(rxValve,ptrm,ptrn-ptrm); //
			
			
		/*===============================*/
			ptrn+=1;   //50.247898;5
			if(strlen(ptrn)>sizeof(rxState))
			{
				return -1;
			}
			memcpy(rxState,ptrn,strlen(ptrn));
//			testu16=strlen(ptrn);
			irrDevice.Press=atof(rxPress);
			IrrDevicePar.instant=atof(rxValve);
			IrrDevicePar.acculat=atof(rxState);
		}
		return 2;
	}
	
	return -1;
	
	/*
	SendBuff[3]=ValvOperaCmd;
	
	SendBuff[4]=(LocalAddr>>8);
	SendBuff[5]=(LocalAddr&0xff);
	
	SendBuff[6]=valve;
	SendBuff[7]=state;
	//发送电压数据
	SendBuff[8]='V';
	SendBuff[9]=':';
	SendBuff[10]=VStr[0];
	SendBuff[11]=VStr[1];
	SendBuff[12]=VStr[2];
	SendBuff[13]=VStr[3];	
	*/
//	if(data[0]!=ValvOperaCmd)
	////////////////////////////////////
//	{
//		return -1;
//	}
////	if((data[1]<<8)+data[2]!=addr) //如果地址不正确
//	{
//		return -2;
//	}
//	if(data[3]!=valve)
//	{
//		return -3;
//	}
//	if(data[4]!=state)
//	{
//		return -4;
//	}
//	Voltagedat[0]=data[7];
//	Voltagedat[1]=data[8];
//	Voltagedat[2]=data[9];
//	Voltagedat[3]=data[10];
//	u16temp=atoi((const char *)Voltagedat);
//	//存储：标志位+地址+点位+状态+电压2
////	ValvState[Ptr][4]=(u16temp>>8);
////	ValvState[Ptr][5]=(u16temp&0xff);
//	return 1;  //如果成功，则返回1
}


/*
操作田间电磁阀
*/
void Lora_SendValve(char * devCode,u8 valve, u8 state)
{
	char tempu8[20];
	char SendBuff[100]={0};
	u16 crcCalc;
	u16 strl=0;
	SendBuff[0]=(AllAddr>>8);
	SendBuff[1]=(AllAddr&0xff);
	SendBuff[2]=AllChannel;
/***********************************/
	strcat(SendBuff,"C:");
	strcat(SendBuff,ValvOperaCmd);  //连接字符串
	strcat(SendBuff,";");
/***********************************/
	strcat(SendBuff,"R:");
	strcat(SendBuff,devCode);
	strcat(SendBuff,";");
/***********************************/
	strcat(SendBuff,"T:");
	strcat(SendBuff,(char *)OtherSet_Par.ProjectNo);
	strcat(SendBuff,";");
/***********************************/
	strcat(SendBuff,"D:");
	sprintf(tempu8,"%02d-%02d;",valve,state); 
	strcat(SendBuff,tempu8);
	strl=strlen(SendBuff);
	crcCalc=crc((u8 *)SendBuff+3,strl-3);  
	SendBuff[strl++]=(crcCalc>>8);
	SendBuff[strl++]=(crcCalc&0xff);


	USART_Send((u8 *)SendBuff,strl);
//	Lora_Wait();
}



