#ifndef _IAP_H_
#define _IAP_H_


typedef  void (*iapfun)(void); 			//定义一个函数类型的参数


#define FLASH_APP_1		0x0800C000		//从flash的49K地址开始写入APP1
#define FLASH_APP_2		0x08046000		//从flash的281k地址开始写入APP2




unsigned char IAP_UpdateProgram(unsigned int addr, unsigned int offset, unsigned short *data, unsigned short dataLen);

void IAP_DataConvert(unsigned short *buffer, unsigned char *data, unsigned short pos, unsigned short numBytes);

unsigned char IAP_CheckAPP(unsigned int addr);

void IAP_Jump(unsigned int appxaddr);


#endif
