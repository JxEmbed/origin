#include "LoraProtocol.h"
#include "string.h"
#include "stdio.h"
#include "Lora.h"

u8   LoraHex[LORA_HEX_LEN];  //lora协议中存储Hex数组
char LoraStr[LORA_HEX_LEN*2];  //lora协议中存储String
char lora_Master_Code[100];
//unsigned char outHex[LORA_PACKET_LEN];
//unsigned char outStr[LORA_PACKET_LEN];
/*
协议版本|发送方唯一编码|接收方唯一编码|指令|crc|SSS
01|A01|101|0123456789ABCDEF|9167
*/

u8 Lora_Rxanalysis(void)
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
	crcRx=(crcHex[0]<<8)+crcHex[1];
	crcCalc=crc(Lora_Rxbuff,Lora_RxBuffPtr-8);
	
//	if(crcRx!=crcCalc)   
//	{
//		return 0;
//	}
	memcpy(buff,Lora_Rxbuff,Lora_RxBuffPtr);
	tempptr = strtok((char *)buff, "|");  //协议版本
	if(strcmp(tempptr,LORA_PROTOCOL_VER)!=0)
	{
		return 0;  
	}
	tempptr = strtok(NULL, "|"); //发送方
	snprintf(lora_Master_Code,sizeof(lora_Master_Code),"%s",tempptr);
	tempptr=strtok(NULL,"|");    //接收方
	if(strcmp(tempptr,LORA_LOCAL_CODE)!=0)
	{
		return 0;
	}
	
	tempptr=strtok(NULL,"|");    //指令
	if(strlen(tempptr)>LORA_HEX_LEN*2) //如果长度超过
	{
		return 0;
	}
	len=Str2Hex(LoraHex,(u8 *)tempptr,strlen(tempptr));
	//Lora_Txbuff
//	snprintf(Lora_Txbuff,sizeof(Lora_Txbuff),"%s");
	if(len>0)
	{
		memset(Lora_Txbuff,0,sizeof(Lora_Txbuff));
		snprintf((char *)Lora_Txbuff,sizeof(Lora_Txbuff),"%s",LORA_PROTOCOL_VER); //版本
		snprintf((char *)Lora_Txbuff,sizeof(Lora_Txbuff),"%s|%s",Lora_Txbuff,LORA_LOCAL_CODE); //发送方
		snprintf((char *)Lora_Txbuff,sizeof(Lora_Txbuff),"%s|%s",Lora_Txbuff,lora_Master_Code); //接收方
		ret=Slave_Check(LoraHex,len);
		if(ret>0)
		{
			Hex2Str(LoraHex,LoraStr,ret);
			snprintf((char *)Lora_Txbuff,sizeof(Lora_Txbuff),"%s|%s|",Lora_Txbuff,LoraStr); //指令
			crcCalc=crc(Lora_Txbuff,strlen((char *)Lora_Txbuff));
			memset(LoraStr,0,sizeof(LoraStr));
			LoraHex[0]=(crcCalc>>8);
			LoraHex[1]=(crcCalc&0xff);
			Hex2Str(LoraHex,LoraStr,2);
			snprintf((char *)Lora_Txbuff,sizeof(Lora_Txbuff),"%s%s",Lora_Txbuff,LoraStr); //crc
			snprintf((char *)Lora_Txbuff,sizeof(Lora_Txbuff),"%s|SSS",Lora_Txbuff); //结束标志
			LoraSetMode(0);	
			Lora_Packet((char *)Lora_Txbuff);
		}
	}
	tempptr=strtok(NULL,"|");    //crc
	
	tempptr=strtok(NULL,"|");    //停止
//	if()
	
	
}
s16 Slave_Check(u8 * rxData,u16 len)
{
	s32 ret;
	u8 func;			// 功能码,0-255
	u8 i=0;
	u16 StartAddress,RegNumber;
	if(len<1)
	{
		return -1; //出错
	}
	func=rxData[0];
	memset(LoraHex,0,sizeof(LoraHex));
	LoraHex[0]=func;
	
	switch(func)
	{
		case 0x03:  // 读保持寄存器
			ret=GetHR(rxData+1,LoraHex+1,len-1);
			break;
		case 0x06:
			ret=SetHR(rxData+1,LoraHex+1,len-1);
			break;
		case 0x10:
			ret=SetMultiHR(rxData+1,LoraHex+1,len-1);
			break;
		default:
			ret=-1;
			break;
	}
	if(ret>0)
	{
		return ret+1;
	}
	else
	{
		return 0;
	}
	
}
s32 GetHR(u8 *rxData,u8 *txData,u32 pDatalen)
{
	u16 StartAddress,RegNumber;
	u8  *pAns;
	u32 temp;
	u32 i,pt;
	s32 ret=-1;
	
	if(pDatalen!=4) return 0;               // 数据长度错误
		
	StartAddress = (rxData[0]<<8)|rxData[1];    // 变量起始地址
		
	RegNumber    = (rxData[2]<<8)|(rxData[3]);  // 变量的个数			
	
	
	if(StartAddress+RegNumber>0xFFFF)
	{
		return -1;
	}
	if(124<RegNumber)
	{
		return -1;                            // 读取的寄存器个数太多（超过了实际的寄存器个数或缓冲区长度）
	}
	pt=0;

	for(i=StartAddress;i<StartAddress+RegNumber;i++)
	{
		ret=lora_map_GetHoldReg(i,&temp);
		if(ret==1)
		{// 获取变量成功
			txData[++pt]=(temp>>8)&0xFF;
			txData[++pt]=temp&0xFF;
		}
		else
		{
		   return -1;				
		}
	}
	txData[0]=pt;                // 重新设置数量
    return pt+1;
}

s32 SetHR(u8 *rxData,u8 *txData,u32 pDatalen)
{
	u32 StartAddress,RegNumber;
	u16 value;
	s32 ret=-1;
	
	if((pDatalen)!=4) return 0;              // 数据长度错误
		
	StartAddress = (rxData[0]<<8)|rxData[1];    // 变量起始地址
		
	value        = (rxData[2]<<8)|(rxData[3]);  // 变量值			
		
	
		
	
	
	ret=lora_map_SetHoldReg(StartAddress,&value);
	txData[0]=rxData[0];
	txData[1]=rxData[1];
	txData[2]=rxData[2];
	txData[3]=rxData[3];
	if(ret==-1)
	{// 无效数据
	}
	else if(ret==-2)
	{// 无效地址
		 ret=-1;
	}
	else ret=4;

  return ret;
}
s32 SetMultiHR(u8 *rxData,u8 *txData,u32 pDatalen)
{
	u32 StartAddress,RegNumber;
	u8 Bytecount;
	u8 i;
	u16 value;
	s32 ret=-1;
	
	if((pDatalen)<5) return 0;              // 数据长度错误
		
	StartAddress = (rxData[0]<<8)|rxData[1];    // 变量起始地址
		
	RegNumber        = (rxData[2]<<8)|(rxData[3]);  // 变量值			
		
	Bytecount    = rxData[4];                  //字节长度
	if(Bytecount!=2*RegNumber) return 0;       // 字节长度错误
	if((Bytecount+5)!=pDatalen) return 0;     // 长度错误
	if(StartAddress+RegNumber>0xFFFF)      // 寄存器个数太多（超过了实际的寄存器个数或缓冲区长度）
	{
		return -1;
	}
	//示例：01 |10 |4E 21| 00 03|  06| 00 01| 00 11| 00 08| BB 05
	for(i=0;i<RegNumber;i++)
	{
		value= (rxData[5+i*2]<<8)|(rxData[6+i*2]);  // 变量值
		ret=lora_map_SetHoldReg(StartAddress+i,&value);
		if(ret==-1)
		{
			 break;
		}
		else if(ret==-2)
		{
			 break;
		}
		else
		{
			ret=4;
		}
	}
	txData[0]=rxData[0];
	txData[1]=rxData[1];
	txData[2]=rxData[2];
	txData[3]=rxData[3];

  return ret;
}
bool fromHexChar(unsigned char c, unsigned char *outChar) 
{
	if('0' <= c && c <= '9')
	{
		*outChar = c - '0';
	}
	else if('a' <= c && c <= 'f')
	{
		*outChar = c - 'a' + 10;
	}
	else if('A' <= c && c <= 'F')
	{
		*outChar = c - 'A' + 10;
	}
	else
	{
		return 0;
	}
	return 1;
}

bool getLen(unsigned char *dst, const unsigned char *src, int srcLen, int *len)
{
	if (srcLen % 2 == 1) 
	{
		return FALSE;
	}
 
	int i = 0, j = 1;
	for (; j < srcLen; j += 2)
	{
		unsigned char a = 0x00;
		unsigned char b = 0x00;
		if(!fromHexChar(src[j-1], &a) || !fromHexChar(src[j], &b))
		{
			return FALSE;
		}
		dst[i] = (a << 4) | b;
		i++;
	}
	*len = i;
	return TRUE;
}

u16 Str2Hex(unsigned char *outHex, unsigned char *inStr, int inStrLen)
{
	int len = 0;
	u8 dst[LORA_HEX_LEN];
	//输入的字符串长度不能超过inStr定义的长度
	if(inStrLen>LORA_HEX_LEN*2)   
	{
		return FALSE;
	} 
    
	if(getLen(dst, inStr, inStrLen, &len))
	{
		memcpy(outHex, dst, len);
		return len;
	}
	return FALSE;
}
bool fromCharHex(unsigned char c, unsigned char *outChar) 
{
	u8 tempu8;
	tempu8=(c>>4);
	if(tempu8<10)
	{
		*outChar=tempu8+'0';
	}
	else
	{
		*outChar=tempu8- 10 + 'A';
	}
	outChar++;
	tempu8=(c&0x0f);
	if(tempu8<10)
	{
		*outChar=tempu8+'0';
	}
	else
	{
		*outChar=tempu8-10+'A';
	}
	return TRUE;
	
}
/*
Hex转换成Str

*/
u16 Hex2Str(unsigned char *inHex, char *outStr,int inHexLen)
{
	int i=0;
	int len=0;
	len=inHexLen;
	//输入的hex长度不能超过hex的定义的最大长度
	if(inHexLen>LORA_HEX_LEN) 
	{
		return FALSE;
	}
	for(i=0;i<inHexLen;i++)
	{
		if(!fromCharHex(inHex[i],(u8 *)(outStr+i*2)))
		{
			return FALSE;
		}
	}
	
	
}

void Hex2StrTest(void)
{
	u8 tempu8[20];
	tempu8[0]=0x01;
	tempu8[1]=0x23;
	tempu8[2]=0x45;
	tempu8[3]=0x67;
	tempu8[4]=0x89;
	tempu8[5]=0xAB;
	tempu8[6]=0xCD;
	tempu8[7]=0xEF;
	char tempstr[20];
	Hex2Str(tempu8,tempstr,8);
	
}
