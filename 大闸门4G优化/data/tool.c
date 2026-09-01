/****************************************Copyright (c)**************************************************
**                               		NERCITA
**                                 http://www.nercita.org.cn
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			tool.c
** Last modified Date:	2007-03-09
** Last Version:		1.0
** Descriptions:		常用工具类函数，包括字符的转换；数字字符间的转换等。
**						
**------------------------------------------------------------------------------------------------------
** Created by:			scj
** Created date:		2007-03-09
** Version:				0.1
** Descriptions:		建立文件
**
**------------------------------------------------------------------------------------------------------
** Modified by:			
** Modified date:		
** Version:				
** Descriptions:		
**
**------------------------------------------------------------------------------------------------------
** Modified by: 
** Modified date:
** Version:	
** Descriptions: 
**
********************************************************************************************************/

#include "tool.h" 
#include <string.h>
#include "main.h" 
/********************************************************************************************************
* 函数名称：s_InitToString
* 功能：将int类型整数nSrc转换为指定的nMode进制的字符串，保存到pStr中
* 参数: 
*        nSrc,int:     欲转换的整数
*        pStr,char*:   转换后的字符串保存空间
*        nMode,int:    转换的进制,1-16,如果为<=0，将直接返回错误
* 输出:  char *:转换后字符串指针,如果正确完成转换,该指针等于pStr;如果出现错误，则返回0
* Note:
*********************************************************************************************************/
char  * s_IntToString(int   nSrc,char   *pStr,int   nMode)   
  {   
          char   *p,*q,temp; 
            
          char   bFlag=1;
             
          p=q=pStr;   
          if(nSrc<0)
          {   
              bFlag=0;
              nSrc=-nSrc;   
          }
          
          
          do  
          {   
               if(nSrc%nMode>=10)   
               {
                    *p++ = (char)(nSrc%nMode-10+'A');
               }
               else
               {   
                     *p++= (char)(nSrc%nMode+'0');   
                }
                
                nSrc/=nMode;   
          }while(nSrc!=0);
             
          if(bFlag==0)  *p++='-';   
          
          *p--='\0';
          
          while(p>q)   
          {   
          		temp=*p;   
                *p--=*q;   
                *q++=temp;   
          }
          
          return   pStr;   
  }   



//-------------------------------------------------------------------------------------------------------------------
// s_SearchToken:从指定的缓冲区中搜索有效地应答，？
//                比如接收到的应答信息为FF01FF02，其中01才是有效的应答字符，其前面的字符FF是Ncr时间，SD卡并没有应答
//                本函数用来分析SD卡的返回数据
// 参数：
//        pToken,     INT8U *:   搜索的有效标志缓冲区
//        tokenlen,   INT32U:    标志区长度
//        pFindBuffer,INT8U *:   欲搜索的缓冲区
//        len,        INT32S:    欲搜索的缓冲区的长度
// 返回值：
//        INT32S,     >=0:搜索到指定的标志，其起始位置为返回值
//                    -1:未搜索到指定的标志
//---------------------------------------------------------------------------------------------------------------------
int s_SearchToken(const unsigned char* pToken,int tokenlen,const unsigned char *pFindBuffer,int len)
{
	int i,j;
	
	if(pToken==0)         return -1;
	if(tokenlen==0)       return -1;			
	if(pFindBuffer==0)    return -1;
	if(len==0)            return -1;
	if(len<tokenlen)      return -1;
	
	len-=tokenlen;        // 只需要搜索比被搜索的缓冲区内容长的部分即可
	
	for(i=0;i<len;i++)
	{		
		if(*pFindBuffer==*pToken)
		{
			for(j=0;j<tokenlen;j++)
			{
				if(pFindBuffer[j]!=pToken[j]) break;
			}
			
			if(j==tokenlen)
			{// 搜索成功
				return i;
			}
			
		}
				
		pFindBuffer++;		
		
	}
	
	return -1;               // 失败，没有搜索到指定的内容
}

/********************************************************************************************************
** Function Name:strinclude
** Description: 判断source中是否从开始位置包含substring
** Input:       source:源串；substring:子串
**              
** Output:      
** Return Value: unsigned int:0:不包含;65535:字符串太长；其它返回
** global variables: 无
** Calling modules:  无
*********************************************************************************************************/

int s_strSearch(const char *source,const char *substring)
{
	unsigned int alllen,sublen;
	
	alllen=strlen(source);
	sublen=strlen(substring);
	
	if(alllen<sublen) return -1;
	
	
	return s_SearchToken((const unsigned char *)substring,sublen,(const unsigned char *)source,alllen);	
	
	
}




// 低位在前
unsigned char* s_Int32ToBuffer(unsigned int datu,unsigned char* pBuffer)
{
	if(pBuffer==0) return 0;
	
	*pBuffer++=(datu&0xFF);
	*pBuffer++=((datu>>8)&0xFF);
	*pBuffer++=((datu>>16)&0xFF);
	*pBuffer++=((datu>>24)&0xFF);
	
	return pBuffer;
}

unsigned int s_BufferToInt32(unsigned char *pBuffer)
{
	unsigned int datu=0;
	if(pBuffer==0) return 0;
	
	datu=*(pBuffer+3);
	datu<<=8;
	datu&=0xFFFFFF00;
	
	datu|=*(pBuffer+2);	
	datu<<=8;
	datu&=0xFFFFFF00;
	
	datu|=*(pBuffer+1);
	datu<<=8;
	datu&=0xFFFFFF00;
	
	
	datu|=*pBuffer;
	
	return datu;
}

unsigned char* s_Int16ToBuffer(unsigned short datu,unsigned char* pBuffer)
{
	if(pBuffer==0) return 0;
	
	*pBuffer++=(datu&0xFF);
	*pBuffer++=((datu>>8)&0xFF);
		
	return pBuffer;
}

unsigned char* s_cardInt16ToBuffer(unsigned short datu,unsigned char* pBuffer)
{
	if(pBuffer==0) return 0;
	
	*pBuffer++=((datu>>8)&0xFF);
	*pBuffer++=(datu&0xFF);
		
	return pBuffer;
}

unsigned short s_BufferToInt16(unsigned char *pBuffer)
{
	unsigned short datu=0;
	if(pBuffer==0) return 0;
	
	datu=*(pBuffer+1);
	datu<<=8;
	datu|=(*pBuffer);	
	
	
	return datu;
}

unsigned short s_cardBufferToInt16(unsigned char *pBuffer)
{
	unsigned short datu=0;
	if(pBuffer==0) return 0;
	
	datu=*pBuffer;
	datu<<=8;
	datu|=*(pBuffer+1);	
	
	
	return datu;
}

unsigned char* s_Int24ToBuffer(unsigned int datu,unsigned char* pBuffer)
{
	if(pBuffer==0) return 0;
	
	*pBuffer++=(datu&0xFF);
	*pBuffer++=((datu>>8)&0xFF);
	*pBuffer++=((datu>>16)&0xFF);
		
	return pBuffer;
}

unsigned int s_BufferToInt24(unsigned char *pBuffer)
{
	unsigned int datu=0;
	if(pBuffer==0) return 0;
	
	datu=*(pBuffer+2);	
	datu<<=8;
	datu&=0xFFFFFF00;
	
	datu|=*(pBuffer+1);
	datu<<=8;
	datu&=0xFFFFFF00;
	
	datu|=(*pBuffer);	
	
	return datu;
}


unsigned char* s_CharToBuffer(char datu,unsigned char* pBuffer)
{
	if(pBuffer==0) return 0;
	
	*pBuffer++=datu;
		
	return pBuffer;
}

char s_BufferToChar(unsigned char *pBuffer)
{
	char datu=0;
	if(pBuffer==0) return 0;
	
	datu=*pBuffer++;
		
	return datu;
}

unsigned char* s_Int8UToBuffer(unsigned char datu,unsigned char* pBuffer)
{
	if(pBuffer==0) return 0;
	
	*pBuffer++=datu;
		
	return pBuffer;
}

unsigned char s_BufferToInt8U(unsigned char *pBuffer)
{
	unsigned char datu=0;
	if(pBuffer==0) return 0;
	
	datu=*pBuffer++;
		
	return datu;
}

unsigned char* s_FloatToBuffer(float datu,unsigned char* pBuffer)
{
	memcpy(pBuffer,&datu,4);
	
	return pBuffer+4;
}

float s_BufferToFloat(unsigned char *pBuffer)
{
	float datu=0;
	
	memcpy(&datu,pBuffer,4);
	
	return datu;
}

const unsigned char auchCRCHi[] =
{
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,0x40,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01,0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41,0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81,0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
} ;

const unsigned  char auchCRCLo[] = 
{
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,0x04,
	0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,0x08, 0xC8, 
	0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,0x1D, 0x1C, 0xDC, 
	0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,0x11, 0xD1, 0xD0, 0x10, 
	0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,0x37, 0xF5, 0x35, 0x34, 0xF4, 
	0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 
	0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 
	0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 
	0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 
	0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 
	0x78, 0xB8, 0xB9, 0x79, 0xBB,0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 
	0xB4, 0x74, 0x75, 0xB5,0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 
	0x50, 0x90, 0x91,0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 
	0x9C, 0x5C,0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 
	0x88,0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,0x40
} ;

u16 crc(u8 *puchMsg , u16 usDataLen)
{
   	u8 uchCRCHi = 0xFF ; /* high byte of CRC initialized */
   	u8 uchCRCLo = 0xFF ; /* low byte of CRC initialized */
   	u8 uIndex ;          /* will index into CRC lookup table */
   	
   	while (usDataLen--)  /* pass through message buffer */
   	{
				uIndex = uchCRCHi ^ *puchMsg++ ; /* calculate the CRC */
				uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
				uchCRCLo = auchCRCLo[uIndex] ;
   	}
	
		return (uchCRCHi << 8 | uchCRCLo) ;
}

const char HexStr[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void ArrayToHexStr(u8 *data,char *str,s32 num)
{
	int i;
	for(i=0;i<num;i++)
	{
		str[i*2]=HexStr[*(data+i)/16];
		str[i*2+1]=HexStr[*(data+i)%16];
	}
	str[num*2]=0;
}

unsigned char IntToBCD(unsigned char data)
{
	unsigned char ret;
	ret=(data/10)*16+data%10;
	return ret;
}

unsigned char BCDToInt(unsigned char data)
{
	unsigned char ret;
	ret=(data/16)*10+data%16;
	return ret;
}

void FloatToStr(float data,char* pBuffer,u8 num)
{
	int itemp,i;
	int dtemp,dec;
	dec=0;
	if(data<0)
	{
		data*=-1;
		dec=1;
	}
	itemp=1;
	for(i=0;i<num;i++)
	{
		itemp*=10;
	}
	
	dtemp=(int)(data*itemp);
	
	if(num==1)
	{
		if(dec==1)
		{
			sprintf(pBuffer,"-%d.%01d",dtemp/itemp,dtemp%itemp);
		}
		else
		{
			sprintf(pBuffer,"%d.%01d",dtemp/itemp,dtemp%itemp);
		}
	}
	else if(num==2)
	{
		if(dec==1)
		{
			sprintf(pBuffer,"-%d.%02d",dtemp/itemp,dtemp%itemp);
		}
		else
		{
			sprintf(pBuffer,"%d.%02d",dtemp/itemp,dtemp%itemp);
		}
			
	}
	else if(num==3)
	{
		if(dec==1)
		{
			sprintf(pBuffer,"-%d.%03d",dtemp/itemp,dtemp%itemp);
		}
		else
		{
			sprintf(pBuffer,"%d.%03d",dtemp/itemp,dtemp%itemp);
		}
			
	}
	else
	{
		if(dec==1)
		{
			sprintf(pBuffer,"-%d.%04d",dtemp/itemp,dtemp%itemp);
		}
		else
		{
			sprintf(pBuffer,"%d.%04d",dtemp/itemp,dtemp%itemp);
		}
	}	
}


int strtobyten(char *a, int n)
{
	int i,ret;
	ret=0;
	for(i=0;i<n;i++)
	{
		ret=ret*10+(*(a+i)-'0');
	}

	return ret;
}

// 字符串->数组
unsigned char strtobyte(const char *pString)
{
	unsigned char datu,len;
	datu = 0;
	
	len = 0;
	while(pString[len]!=0)
	{
		datu = pString[len];
		if((datu<'0')||(datu>'9')) break;
		len++;
	}
	
	if((len>3)||(len==0)) return 0;
	
	if(len==3) datu=(pString[0]-0x30)*100+(pString[1]-0x30)*10+(pString[2]-0x30);
	else if(len==2) datu=(pString[0]-0x30)*10+(pString[1]-0x30);
	else if(len==1) datu=pString[0]-0x30;
	else datu=0;
	
	return datu;
	
}

unsigned char btos(unsigned char ndata,char *pString,unsigned char tlen)	   
{
	unsigned char t,datu;
	unsigned char len;
	
	if(ndata>=100)
	{
		t=100;
		len = 3;
	}
	else if(ndata>=10)
	{
		t=10;
		len = 2;
		if(tlen>2) *pString++='0';
	}
	else
	{
		
		t=1;
		len = 1; 
		
		if(tlen==3)
		{
			*pString++='0';
			*pString++='0';
		}
		else if(tlen==2)
		{
			*pString++='0';
		}
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

unsigned int strtoword(const char *pString)
{
	unsigned int result,datu;
	unsigned char i,j,len;
	unsigned char pos;
	
	result = 0;
	
	pos = 0;
	len = 0;
	i=0;
	while(pString[i]!=0)
	{
		if((pString[i]<'0')||(pString[i]>'9'))
		{
			if(result!=0) break;
		}
		else
		{
			if(result==0)
			{
				pos=i;
				result = 1;
			}
		}
		
		if(result==1) len++;
		
		i++;
	}
	
	if((len>5)||(len==0)) return 0;
	
	result = 0;
		
	
	for(i=0;i<len;i++)
	{
		datu = pString[i+pos]-0x30;
		for(j=0;j<(len-i-1);j++)	datu*=10;
		
		result+=datu;
		
	}
	
	return result;
}

unsigned char MB2S(int ndata,char *pString)
{
	unsigned t,datu;
	unsigned j;
	unsigned int temp[5]={1,10,100,1000,10000};
		
	t = 4;
	if(ndata<0)
	{
		*pString++='-';
		ndata = -ndata;
	}
	else *pString++=' ';
	
	if(ndata<10000)
	{
		t --;
		//*pString++=' ';
	}	
	
	if(ndata<1000)
	{
		t --;
		*pString++='0';
	}	
	if(ndata<100)
	{
		t--;
		*pString++='0';
	}	
	if(ndata<10)
	{
		t--;
		*pString++='0';
	}	
	
	for(j=0;j<=t;j++)
	{
		datu=ndata/temp[t-j]; 
		
		*pString=datu+0x30;
		pString++;
				
		ndata%=temp[t-j];
	}
	
	*pString = 0;
	
	return 5;
}
