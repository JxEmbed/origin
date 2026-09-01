#include "liu.h"
#include "Lora.h"
#include "string.h"
#include "stdio.h"
#include "jiang.h"
#include "LoraProtocol.h"
//#define LORA_HEX_LEN       20        //定义的最大hex长度
//u8   LoraHex[LORA_HEX_LEN];  //lora协议中存储Hex数组
//char LoraStr[LORA_HEX_LEN*2];  //lora协议中存储String
void liu_Lora_ReadTerminal(char *dev_code,char* Dev_type)
{
	//  打开Lora功能	(0关闭)				打开测流功能(0关闭)			测流设备为巴歇尔槽(1)				读取外部设备标志位(0为等待)
	if((dev_SavePar.Lora_Fun == 0) || (dev_SavePar.Flow_Fun==0) || (dev_SavePar.TypeUltra != 1) || (nosave_par.Read_ExterndevF == 0))
	{
		return ;
	}
	
	nosave_par.Read_ExterndevF=0;
	
	uint16_t crcCalc; //存储计算出来的crc结果
	char tempc[300]="";
	u32 tx_len=0;
	memset(Lora_Txbuff,0,sizeof(Lora_Txbuff));
	
	Lora_SendAddr(Dev_type);  					//放入地址和信道
	snprintf((char *)Lora_Txbuff,sizeof(Lora_Txbuff),"%s%s|",Lora_Txbuff,LORA_PROTOCOL_VER); 	//协议版本
	snprintf((char *)Lora_Txbuff,sizeof(Lora_Txbuff),"%s%s|",Lora_Txbuff,dev_SavePar.dev_code); //发送方设备编号
	snprintf((char *)Lora_Txbuff,sizeof(Lora_Txbuff),"%s%s|",Lora_Txbuff,dev_code); 			//接收方设备编号
	
	
	snprintf((char *)Lora_Txbuff,sizeof(Lora_Txbuff),"%s0300000010|",Lora_Txbuff); //读取流量
	
	tx_len=strlen((char *)Lora_Txbuff);
	crcCalc=crc(Lora_Txbuff+3,tx_len-3);                       //计算的crc
//	Lora_Txbuff[tx_len++]=(crcCalc>>8);
//	Lora_Txbuff[tx_len++]=(crcCalc&0xff);
//	Hex2Str((u8 *)&crcCalc,(char *)(Lora_Txbuff+tx_len),2);
	fromCharHex((crcCalc&0xff),(u8 *)(Lora_Txbuff+tx_len+2));
	fromCharHex((crcCalc>>8),(u8 *)(Lora_Txbuff+tx_len));
	tx_len+=4;
	snprintf((char *)Lora_Txbuff,sizeof(Lora_Txbuff),"%s|SSS",Lora_Txbuff);
	tx_len+=4;
	vTaskSuspendAll();													//挂起调度器		
	Lora_Send((u8 *)Lora_Txbuff,tx_len);
	xTaskResumeAll();	
	
	lora_send++;
}

u8 Lora_Rxanalysis_liu(void)
{
	char buff[Lora_RxbuffLen]={0};
	uint16_t crcCalc; //存储计算出来的crc结果
	uint16_t crcRx;   //接收到的crc
	char * tempptr;
	u16 len;
	float tempf;
	s8 ret;
	u8 crcStr[4];
	u8 crcHex[2];
	/*判断接收长度*/
	if(Lora_RxBuffPtr<6)     //如果数据过短则退出
	{
		return 0;
	}
	/*判断结尾是否是SSS*/
	if(!(Lora_Rxbuff[Lora_RxBuffPtr-1]=='S'&&Lora_Rxbuff[Lora_RxBuffPtr-2]=='S'&&Lora_Rxbuff[Lora_RxBuffPtr-3]=='S'&&Lora_Rxbuff[Lora_RxBuffPtr-4]=='|'))
	{
		return 0;
	}
	/*对比CRC*/
//	crcRx=(Lora_Rxbuff[Lora_RxBuffPtr-6]<<8)+Lora_Rxbuff[Lora_RxBuffPtr-5];
	crcStr[0]=Lora_Rxbuff[Lora_RxBuffPtr-8];
	crcStr[1]=Lora_Rxbuff[Lora_RxBuffPtr-7];
	crcStr[2]=Lora_Rxbuff[Lora_RxBuffPtr-6];
	crcStr[3]=Lora_Rxbuff[Lora_RxBuffPtr-5];
	len=Str2Hex(crcHex,(u8 *)crcStr,4);
	crcRx=(crcHex[0]<<8)+crcHex[1];					//收到的CRC
	crcCalc=crc(Lora_Rxbuff,Lora_RxBuffPtr-8);		//計算的CRC	
	
	if(crcRx!=crcCalc)   
	{
		return 0;
	}
	memcpy(buff,Lora_Rxbuff,Lora_RxBuffPtr);
	tempptr = strtok((char *)buff, "|");  			//协议版本
	if(strcmp(tempptr,LORA_PROTOCOL_VER)!=0)
	{
		return 0;  
	}
	tempptr = strtok(NULL, "|"); 					//发送方
	snprintf(lora_Master_Code,sizeof(lora_Master_Code),"%s",tempptr);
	tempptr=strtok(NULL,"|");    					//接收方
	if(strcmp(tempptr,dev_SavePar.dev_code)!=0)
	{
		return 0;
	}
	
	tempptr=strtok(NULL,"|");    //指令
	if(strlen(tempptr)>LORA_HEX_LEN*2) //如果长度超过
	{
		return 0;
	}
	len=Str2Hex(LoraHex,(u8 *)tempptr,strlen(tempptr));
	if(len>0)
	{
//		memset(Lora_Txbuff,0,sizeof(Lora_Txbuff));
//		snprintf((char *)Lora_Txbuff,sizeof(Lora_Txbuff),"%s",LORA_PROTOCOL_VER); //版本
//		snprintf((char *)Lora_Txbuff,sizeof(Lora_Txbuff),"%s|%s",Lora_Txbuff,dev_SavePar.dev_code); 	   //发送方
//		snprintf((char *)Lora_Txbuff,sizeof(Lora_Txbuff),"%s|%s",Lora_Txbuff,dev_SavePar.Externdev_code); //接收方
		//清空标志位
		Statlog_Par.aft_level=2;
		RS485_Sensor.Aft_state=2;
		//解析数据报文
		ret=Slave_Lora_Map(LoraHex,len);
//		if(ret>0)
//		{
//			Hex2Str(LoraHex,LoraStr,ret);
//			snprintf((char *)Lora_Txbuff,sizeof(Lora_Txbuff),"%s|%s|",Lora_Txbuff,LoraStr); //指令
//			crcCalc=crc(Lora_Txbuff,strlen((char *)Lora_Txbuff));
//			memset(LoraStr,0,sizeof(LoraStr));
//			LoraHex[0]=(crcCalc>>8);
//			LoraHex[1]=(crcCalc&0xff);
//			Hex2Str(LoraHex,LoraStr,2);
//			snprintf((char *)Lora_Txbuff,sizeof(Lora_Txbuff),"%s%s",Lora_Txbuff,LoraStr); //crc
//			snprintf((char *)Lora_Txbuff,sizeof(Lora_Txbuff),"%s|SSS",Lora_Txbuff); //结束标志
//			LoraSetMode(0);	
//			Lora_Packet((char *)Lora_Txbuff);
//			LoraSetMode(1);	
//		}
	}
	tempptr=strtok(NULL,"|");    //crc
	
	tempptr=strtok(NULL,"|");    //停止
	
}

u8 Slave_Lora_Map(u8 * rxData,u16 len)
{
	u16 FloNum;
	u16 i=0;
	u8 tempu8s[4];
	float tempf;
	if(rxData[0]!=0x03)
	{
		return 0;
	}
	FloNum=rxData[1]/2;
	for(i=0;i<FloNum;i++)
	{
		tempu8s[0]=rxData[3+i*4];
		tempu8s[1]=rxData[2+i*4];
		tempu8s[2]=rxData[5+i*4];
		tempu8s[3]=rxData[4+i*4];
		tempf=*(float *)tempu8s;
		liu_lora_map_SetHoldReg(i,tempf);
//		map_MGetInputReg(addr-1,offset+i,&value);
	}
	return 1;
}


