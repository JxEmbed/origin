/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	iap.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-02-11
	*
	*	版本： 		V1.0
	*
	*	说明： 		在应用编程
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//硬件驱动
#include "iap.h"
//#include "stmflash.h"


iapfun jump2app;


/*
************************************************************
*	函数名称：	IAP_DataConvert
*
*	函数功能：	数据转换
*
*	入口参数：	buffer：存储数据的缓存
*				data：升级源数据
*				pos写入buffer的位置
*				numBytes：此次需要转换多少个字节
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void IAP_DataConvert(unsigned short *buffer, unsigned char *data, unsigned short pos, unsigned short numBytes)
{
	
	unsigned short countSHORT = 0, countCHAR = 0;
	
	//这里可能还需要吧字符转为数值
	
	//这里可能还需要吧字符转为数值

	for(countSHORT = 0, countCHAR = 0; countCHAR < numBytes; countCHAR += 2, countSHORT++)
	{
		buffer[pos + countSHORT] = ((unsigned short)data[countCHAR] << 0) | ((unsigned short)data[countCHAR + 1] << 8);
	}

}

/*
************************************************************
*	函数名称：	IAP_UpdateProgram
*
*	函数功能：	写入Flash
*
*	入口参数：	addr：写入的地址
*				data：需要写入的数据
*
*	返回参数：	0-成功		1-失败
*
*	说明：		想要省事点，就一次性写入2K数据。因为对于大容量
				MCU，一页刚好2K。
************************************************************
*/
unsigned char IAP_UpdateProgram(unsigned int addr, unsigned int offset, unsigned short *data, unsigned short dataLen)
{
	
	STMFLASH_Write(addr + offset, data, dataLen);

	return 0;

}

/*
************************************************************
*	函数名称：	IAP_CheckAPP
*
*	函数功能：	检查是否有程序存在
*
*	入口参数：	addr：地址
*
*	返回参数：	0-有		1-无
*
*	说明：		
************************************************************
*/
unsigned char IAP_CheckAPP(unsigned int addr)
{
    
    unsigned char count = 0, loop = 10;

    while(loop)
    {
        if(STMFLASH_ReadHalfWord(addr) != 0xffff)
            count++;
        
        addr += 2;
        loop--;
    }
    
    if(count == 10)
        return 0;
    else
        return 1;

}

__asm void MSR_MSP(unsigned int addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

/*
************************************************************
*	函数名称：	IAP_Jump
*
*	函数功能：	跳转到应用程序段
*
*	入口参数：	appxaddr:用户代码起始地址
*
*	返回参数：	0-有		1-无
*
*	说明：		
************************************************************
*/
void IAP_Jump(unsigned int appxaddr)
{

	if(((*(volatile unsigned int *)(appxaddr + 4)) & 0xFF000000) == 0x08000000) //判断是否为0X08XXXXXX
	{
		if(((*(volatile unsigned int *)appxaddr) & 0x2FFE0000) == 0x20000000)	//检查栈顶地址是否合法.
		{ 
			jump2app = (iapfun)*(volatile unsigned int *)(appxaddr + 4); //用户代码区第二个字为程序开始地址(复位地址)
			
			MSR_MSP(*(volatile unsigned int *)appxaddr); //初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
			
			jump2app(); //跳转到APP
		}
	}

}
