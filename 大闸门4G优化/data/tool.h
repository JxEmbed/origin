/****************************************Copyright (c)**************************************************
**                               		NERCITA
**                                 http://www.nercita.org.cn
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			tool.h
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

#ifndef _TOOL_H_
#define _TOOL_H_
 
#include "main.h"

char * s_IntToString(int   nSrc,char   *pStr,int   nMode);
int    s_SearchToken(const unsigned char* pToken,int tokenlen,const unsigned char *pFindBuffer,int len);
int s_strSearch(const char *source,const char *substring);

unsigned char* s_Int32ToBuffer(unsigned int datu,unsigned char* pBuffer);
unsigned int s_BufferToInt32(unsigned char *pBuffer);

unsigned char* s_Int16ToBuffer(unsigned short datu,unsigned char* pBuffer);
unsigned char* s_cardInt16ToBuffer(unsigned short datu,unsigned char* pBuffer);
unsigned short s_BufferToInt16(unsigned char *pBuffer);
unsigned short s_cardBufferToInt16(unsigned char *pBuffer);

unsigned char* s_Int24ToBuffer(unsigned int datu,unsigned char* pBuffer);
unsigned int s_BufferToInt24(unsigned char *pBuffer);

unsigned char* s_CharToBuffer(char datu,unsigned char* pBuffer);
char s_BufferToChar(unsigned char *pBuffer);


unsigned char* s_Int8UToBuffer(unsigned char datu,unsigned char* pBuffer);
unsigned char s_BufferToInt8U(unsigned char *pBuffer);

unsigned char* s_FloatToBuffer(float datu,unsigned char* pBuffer);
float s_BufferToFloat(unsigned char *pBuffer);

u16 crc(u8 *puchMsg , u16 usDataLen);

unsigned char IntToBCD(unsigned char data);
unsigned char BCDToInt(unsigned char data);

void FloatToStr(float data,char* pBuffer,u8 num);

void ArrayToHexStr(unsigned char *sdata,char *str,int num);

unsigned char IntToBCD(unsigned char sdata);
unsigned char BCDToInt(unsigned char sdata);

void FloatToStr(float sdata,char* pBuffer,unsigned char num);
int strtobyten(char *a, int n);

unsigned char strtobyte(const char *pString);
unsigned char btos(unsigned char ndata,char *pString,unsigned char tlen);

unsigned int strtoword(const char *pString);
unsigned char MB2S(int ndata,char *pString);

#endif

