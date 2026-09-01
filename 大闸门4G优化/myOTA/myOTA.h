#ifndef __MYOTA_H
#define __MYOTA_H
#include "main.h"
#include "md5.h"
typedef struct
{
	char target[20];
	char tid[20];
	char size[20];
	char md5[40];
	char status[20];
	char type[10];
	
}UpdateSTRUCT;
typedef struct
{
	char user_id[10];          //{pid}表示产品的产品id
	char user_access_key[100];  //用户级access_key
}NET_USERINFO;
#define VERSION_LEN 10
typedef struct
{
	char nowVersion[VERSION_LEN];//现在的版本
	char lastVersion[VERSION_LEN]; //上次的版本
	u8   Update_flag;            //升级标志位
}MYOTA_INFO;
//OTA_INFO
typedef void (*load_a)(void);

extern UpdateSTRUCT Update_Par;

extern MYOTA_INFO myota_info;
extern MD5_CTX md5_ctx;											//MD5相关变量
extern char md5_result[40];
extern unsigned char md5_t[16];
extern char md5_t1[4];
extern NET_USERINFO net_user_info;
void LOAD_A(uint32_t addr);
void LOAD_A_Clear(void);
void GD32_Write_Version(void);
void GD32_Read_Version(void);
void JumpToApp(void);
void Perpare_Erase(void);
void Download_WriteFlash(uint32_t downStart,uint32_t wnum);
void Update_myota_info(void);
void md5_test(void);
void VersionFallback(void);
void Set_NVIC_Offset(void);
#endif
