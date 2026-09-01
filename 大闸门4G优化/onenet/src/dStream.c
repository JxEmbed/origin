/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	dStream.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-09-11
	*
	*	版本： 		V1.1
	*
	*	说明： 		cJson格式数据流通用封装
	*
	*	修改记录：	V1.1：修复当数据流flag全为0时封装错误的bug。
	************************************************************
	************************************************************
	************************************************************
**/

//协议封装文件
#include "dStream.h"

//C库
#include <string.h>
#include <stdio.h>


//==========================================================
//	函数名称：	DSTREAM_toString
//
//	函数功能：	将数值转为字符串
//
//	入口参数：	StreamArray：数据流
//				buf：转换后的缓存
//				pos：数据流中的哪个数据
//				bufLen：缓存长度
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void DSTREAM_toString(DATA_STREAM *streamArray, char *buf, unsigned short pos, unsigned short bufLen)
{
	
	memset(buf, 0, bufLen);

	switch((unsigned char)streamArray[pos].dataType)
	{
		case TYPE_BOOL:
			snprintf(buf, bufLen, "%d", *(_Bool *)streamArray[pos].dataPoint);
		break;
		
		case TYPE_CHAR:
			snprintf(buf, bufLen, "%d", *(signed char *)streamArray[pos].dataPoint);
		break;
		
		case TYPE_UCHAR:
			snprintf(buf, bufLen, "%d", *(unsigned char *)streamArray[pos].dataPoint);
		break;
		
		case TYPE_SHORT:
			snprintf(buf, bufLen, "%d", *(signed short *)streamArray[pos].dataPoint);
		break;
		
		case TYPE_USHORT:
			snprintf(buf, bufLen, "%d", *(unsigned short *)streamArray[pos].dataPoint);
		break;
		
		case TYPE_INT:
			snprintf(buf, bufLen, "%d", *(signed int *)streamArray[pos].dataPoint);
		break;
		
		case TYPE_UINT:
			snprintf(buf, bufLen, "%d", *(unsigned int *)streamArray[pos].dataPoint);
		break;
		
		case TYPE_LONG:
			snprintf(buf, bufLen, "%ld", *(signed long *)streamArray[pos].dataPoint);
		break;
		
		case TYPE_ULONG:
			snprintf(buf, bufLen, "%ld", *(unsigned long *)streamArray[pos].dataPoint);
		break;
			
		case TYPE_FLOAT:
			snprintf(buf, bufLen, "%f", *(float *)streamArray[pos].dataPoint);
		break;
		
		case TYPE_DOUBLE:
			snprintf(buf, bufLen, "%f", *(double *)streamArray[pos].dataPoint);
		break;
		
		case TYPE_GPS:
			snprintf(buf, bufLen, "{\"lon\":%s,\"lat\":%s}", (char *)streamArray[pos].dataPoint, (char *)(streamArray[pos].dataPoint) + 24);
		break;
		
		case TYPE_LBS:
			snprintf(buf, bufLen, "{\"cid\":\"%s\",\"lac\":\"%s\",\"networktype\":%d,\"flag\":%d}",
					(char *)streamArray[pos].dataPoint, (char *)(streamArray[pos].dataPoint) + 8,
					*((unsigned char *)streamArray[pos].dataPoint + 16), *((unsigned char *)streamArray[pos].dataPoint + 17));
		break;
		
		case TYPE_STRING:
			snprintf(buf, bufLen, "\"%s\"", (char *)streamArray[pos].dataPoint);
		break;
	}

}

//==========================================================
//	函数名称：	 DSTREAM_GetDataStream_Body
//
//	函数功能：	获取数据流格式消息体
//
//	入口参数：	streamArray：数据流结构
//				streamArrayCnt：数据流个数
//				buffer：缓存
//				maxLen：最大缓存长度
//				offset：偏移
//
//	返回参数：	Body的长度，0-失败
//
//	说明：		
//==========================================================
short DSTREAM_GetDataStream_Body(DATA_STREAM *streamArray, unsigned short streamArrayCnt, unsigned char *buffer, short maxLen, short offset)
{
	
	short count = 0, num_bytes = 0;								//count-循环计数。numBytes-记录数据装载长度
	char stream_buf[112];
	char data_buf[56];
	short cBytes = 0;
	
	unsigned char *data_ptr = buffer + offset;
	
	for(; count < streamArrayCnt; count++)
	{
		if(streamArray[count].flag)
			break;
	}
	
	if(count == streamArrayCnt)
		return -1;
	
	count = 0;
	
	maxLen -= 1;												//预留结束符位置
	
	sprintf((char *)data_ptr, "{\"id\":123,\"dp\":{");
	num_bytes += strlen((char *)data_ptr);
	
	for(; count < streamArrayCnt; count++)
	{
		if(streamArray[count].flag) 						//如果使能发送标志位
		{
			DSTREAM_toString(streamArray, data_buf, count, sizeof(data_buf));
			snprintf(stream_buf, sizeof(stream_buf), "\"%s\":[{\"v\":%s}],", streamArray[count].name, data_buf);

			cBytes = strlen(stream_buf);
			if(cBytes >= maxLen - num_bytes)
			{
				//UsartPrintf(USART_DEBUG, "dStream_Get_dFormatBody Load Failed %d\r\n", numBytes);
				return 0;
			}
			
			memcpy(data_ptr + num_bytes, stream_buf, cBytes);
			
			num_bytes += cBytes;
			if(num_bytes > maxLen)						//内存长度判断
				return 0;
		}
	}
	
	num_bytes += 2;
	data_ptr[num_bytes - 3] = '}';
	data_ptr[num_bytes - 2] = '}';
	data_ptr[num_bytes - 1] = '\0';							//将最后的','替换为结束符
	
	//UsartPrintf(USART_DEBUG, "Body Len: %d\r\n", numBytes);
	return num_bytes;

}

//==========================================================
//	函数名称：	 DSTREAM_GetDataStream_Body_Measure
//
//	函数功能：	测量当前使能的数据流长度
//
//	入口参数：	streamArray：数据流结构
//				streamArrayCnt：数据流个数
//				flag：1-测量全部数据流长度	0-测量当前需要发送的数据流长度
//
//	返回参数：	Body的长度
//
//	说明：		
//==========================================================
short DSTREAM_GetDataStream_Body_Measure(DATA_STREAM *streamArray, unsigned short streamArrayCnt, _Bool flag)
{

	short count = 0, num_bytes = 0;						//count-循环计数。numBytes-记录数据装载长度
	char stream_buf[112];
	char data_buf[56];
	
	for(; count < streamArrayCnt; count++)
	{
		if(streamArray[count].flag)
			break;
	}
	
	if(count == streamArrayCnt)
		return -1;
	
	count = 0;
	
	snprintf(data_buf, sizeof(data_buf), "{\"id\":123,\"dp\":{");
	num_bytes += strlen(data_buf);
	
	for(; count < streamArrayCnt; count++)
	{
		if(streamArray[count].flag || flag)
		{
			DSTREAM_toString(streamArray, data_buf, count, sizeof(data_buf));
			snprintf(stream_buf, sizeof(stream_buf), "\"%s\":[{\"v\":%s}],", streamArray[count].name, data_buf);

			num_bytes += strlen(stream_buf);
		}
	}
	
	num_bytes += 2;
	
	return num_bytes;

}
