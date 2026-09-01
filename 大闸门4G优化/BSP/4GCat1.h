#ifndef __4GCAT1_H
#define __4GCAT1_H

#include "main.h"

#define GPRS_WAKECOUNT 20
#define COUNT_CMD_4G	3
#define UART_BUFFER_LEN 1300
#define SMS_BUFFER_SIZE 300
typedef struct
{
	u16 rxtimer;

	u32 txpointer;
	u32 txlen;

	u32 rxpointer;
	u32 rxlen;
	
	u8 rxflag;
	u8 txbuffer[UART_BUFFER_LEN];
	u8 rxbuffer[UART_BUFFER_LEN];
	QueueHandle_t semRx;					//本串口是否收到一个数据包
	QueueHandle_t semTx;					//本串口是否发送完毕 
}S_UART;



u8 testGSM(void);
u8 testGSM1(void);
u8 StartGSM(void);
u8 InitdataGSM(void);
void _4GCat1_init(uint32_t BaudRate);

char* SendATCommand(char *pCommand, char* pEcho, u32 outTime);
s32 _4GCat1_write(const u8 *buf,u32 len);
s8 PowerUpGSM(void);
s32 _4GCat1_read(u8 *buffer, s32 buffersize, s32 timeout);
u8 GPRSWaitA(char * pEcho,u32 outTime);
u8 GPRS_connect(char* IP,char* port);
u8 GPRS_disconnet(void);
u8 GsmShutDown(void);
u8 setCat1Sleep(void);
u8 GPRS_send_len(int len);
u8 GPRS_connect2(char* IP,char* port);
u8 _4GCat1_UnPacketRecv(u8 *dataPtr,char * cmdtopic);
u8 _4GCat1_ChangeTopic(char * cmdtopic);
u8 _4GCat1_PacketCmdResp(char *cmdid,u8 qos,u8 retain,char * mess);
void SendString(char * ptr);
void SendImgData(char *pro_id, char *dev_name,uint16_t data_name,char * imgData,uint16_t len,u8 outTime);
u8 _4GCat1_GPS(void);
u8 GPRS_TCPconnect2(char* IP,char* port);
u8 GPRS_connectTCP(char* IP,char* port);
char * _4GCat1_SendBuff(u8 * datbuff,u32 len);
void SendImgData2(char * imgData,uint16_t len,u16 num,u8 outTime);
void CIPSEND_Test(void);
void ReportVersion(void);
void CheckUpdate(void);
void analysisCheck(char * str);
void cJSON_test(void);
u8 GSmReset(void);
u8 DownLoadPack(u32 downStart,u32 downStop);
u8 DownLoadPackages(void);
void SendStatus(u16 step);
void InitSAPBR(void);
void InitNTP(void);
u8 MuxLinkMode(void);
void CloseTCP(void);
char* SendATCommand2(char *pCommand);
char *ReadATCommand2(char* pEcho, u32 outTime);
void _4GCat_CloseMSUB(void);
void _4GCat_OpenMSUB(void);
void _4GCat_PrintMSUB(void);
#define _4GCat1_POWEREN   PCout(13)
//4G的这个引脚拉低后实现上电开机
#define _4GCat1_PWRKEY    PFout(0)  //这里进行了取反
#define _4GCat1_RESET     PEout(4)  //这里进行了取反
#define	GPRS_UART	2
#define _4GCat1_xBlockTime 1000
extern char prxBuffer[UART_BUFFER_LEN];
extern S_UART _4GCat1_Usart;
extern char authorization_buf[160];
//#define GPS_POWER_EN _4GCat1_POWEREN=1
//#define GPS_POWER_DIS	_4GCat1_POWEREN=0

u8 GPRS_LowPower(void);
u8 GPRS_ReadLowPower(void);
u8 GPRS_LowPower_wake(void);

s32 Apply_Devcode(void );
s32 Parse_ApplyDevCode(char * Buf_Devcode);
s32 Bind_Devcode(void );
s32 Parse_BindDevCode(char * Buf_Devcode);
void Set_DevCode(void);
u8 OneNET_SendData_Ack(char *pCommand, char* pEcho, u32 outTime);
#endif

