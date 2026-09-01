#ifndef _OTA_H_
#define _OTA_H_


#define APP_LOADED_ADDR                    0x08040000

#define FLASH_APP1_ADDR        0x08040000

//0x40000
typedef  void (*iapfun)(void); 			//定义一个函数类型的参数


typedef struct
{

	char authorization[128];			//检测是用到的签名
	
	char version[24];					//版本
	char token[32];						//Token
	unsigned int size;					//固件大小(字节)
	char md5[40];						//MD5校验
	
	unsigned int addr;					//跳转地址
	
	unsigned char ota_check : 1;		//OTA任务检测
	unsigned char ota_download_ok : 1;	//下载成功
	unsigned char ota_report_ok : 1;	//上报状态成功
	unsigned char err_count : 3;
	unsigned char create_ok : 1;		//自动创建设备
	unsigned char reverse : 2;

} OTA_INFO;


extern OTA_INFO ota_info;


unsigned char OTA_Check_OneNETParameter(unsigned char flash_page, _Bool flash_type, char *proid, char *masterkey, char *regcode);

unsigned int OTA_CheckApplication(unsigned char flash_page, _Bool flash_type, unsigned char app_page, _Bool power_on_check);

unsigned char OTA_Authorization(char *ver, char *res, unsigned int et, char *access_key, char *token_buf, unsigned short token_buf_len);

__weak void OTA_Prepare(void);

unsigned char OTA_Process(char *masterkey, char *reg_code, char *dev_name, char *auth_info);

void OTA_Jump(unsigned int addr);
#endif
