#ifndef _ONENET_H_
#define _ONENET_H_


#include "dstream.h"
#include "main.h"



#define GPRS_send_data(a,b,c)  _4GCat1_write(b,c)
typedef struct
{
	
	char *cmd_resp;					//命令回复指针。比如获取命令返回的数据，可以提取此指针内的数据
	char *cmd_ipd;					//在接入onenet前通过网络获取的数据，比如网络时间、获取接入ip等
	char *cmd_hdl;					//命令处理。在发送命令后，会在返回内容里搜索用户指定的返回数据
	
/*************************发送队列*************************/
	struct NET_SEND_LIST *head, *end;
	
	unsigned int net_time;			//网络时间
	
	int data_bytes;					//接收到的数据长度
	
	signed char signal;				//信号值
	
	unsigned short send_time;		//模组最短数据稳定发送时间间隔发送
	
	unsigned short err : 4; 		//错误类型
	unsigned short init_step : 4;	//初始化步骤
	unsigned short reboot : 1;		//死机重启标志
	unsigned short net_work : 1;	//网络访问OK
	unsigned short device_ok : 1;	//设备检测
	unsigned short send_count : 3;	//网络设备层的发送成功
	unsigned short reverse : 2;		//保留

} NET_DEVICE_INFO;

typedef struct
{
	
    char dev_id[16];    //设备id
    char key[48];
	
	char dev_name[31];
	
	char pro_id[12];   //{pid}表示产品的产品id
	char pro_name[16];   //设备id
	char access_key[48];
	
	char ip[16];
	char port[8];
	
	unsigned short net_work : 1;	//1-OneNET接入成功		0-OneNET接入失败
	unsigned short send_data : 3;
	unsigned short err_count : 3;	//错误计数
	unsigned short heart_beat : 1;	//心跳
	unsigned short get_ip : 1;		//获取到最优登录IP
	unsigned short lbs : 1;			//1-已获取到了位置坐标
	unsigned short lbs_count : 3;	//获取计数
	unsigned short connect_ip : 1;	//连接了IP
	unsigned short err_check : 1;	//错误检测
	unsigned short set_flag;


} ONETNET_INFO;


struct NET_SEND_LIST
{

	unsigned short dataLen;			//数据长度
	unsigned char *buf;				//数据指针
	
	struct NET_SEND_LIST *next;		//下一个

};



extern ONETNET_INFO onenet_info;
extern ONETNET_INFO onenet_info2;
extern ONETNET_INFO onenet_info3;
extern ONETNET_INFO onenet_info4;
extern ONETNET_INFO onenet_info5;
extern NET_DEVICE_INFO net_device_info;
void Delete_Space(char *str);
void Mqqt_OneNetlogin(unsigned short informnetwork);


#define SEND_TYPE_OK			0	//
#define SEND_TYPE_DATA			1	//
#define SEND_TYPE_HEART			2	//
#define SEND_TYPE_PUBLISH		3	//
#define SEND_TYPE_SUBSCRIBE		4	//
#define SEND_TYPE_UNSUBSCRIBE	5	//
#define NET_MallocBuffer	malloc
#define NET_FreeBuffer		free

typedef enum
{

	NET_EVENT_Timer_Check_Err = 0,			//网络定时检查超时错误
	NET_EVENT_Timer_Send_Err,				//网络发送失败错误
	
	NET_EVENT_Send_HeartBeat,				//即将发送心跳包
	NET_EVENT_Send_Data,					//即将发送数据点
	NET_EVENT_Send_Subscribe,				//即将发送订阅数据
	NET_EVENT_Send_UnSubscribe,				//即将发送取消订阅数据
	NET_EVENT_Send_Publish,					//即将发送推送数据
	
	NET_EVENT_Send,							//开始发送数据
	NET_EVENT_Recv,							//Modbus用-收到数据查询指令
	
	NET_EVENT_Check_Status,					//进入网络模组状态检查
	
	NET_EVENT_Device_Ok,					//网络模组检测Ok
	NET_EVENT_Device_Err,					//网络模组检测错误
	
	NET_EVENT_Initialize,					//正在初始化网络模组
	NET_EVENT_Init_Ok,						//网络模组初始化成功
	
	NET_EVENT_Auto_Create_Ok,				//自动创建设备成功
	NET_EVENT_Auto_Create_Err,				//自动创建设备失败
	
	NET_EVENT_Connect,						//正在连接、登录OneNET
	NET_EVENT_Connect_Ok,					//连接、登录成功
	NET_EVENT_Connect_Err,					//连接、登录错误
	
	NET_EVENT_Fault_Process,				//进入错误处理

} NET_EVENT;

_Bool OneNET_RegisterDevice(const char *access_key, const char *pro_id, const char *serial, char *devid, char *key);

_Bool OneNET_ConnectIP(char *ip, char *port);

void OneNET_DevLink(const char *dev_name, const char *pro_id, const char *key);

_Bool OneNET_DisConnect(void);

//unsigned char OneNET_SendData(char *pro_id, char *dev_name, DATA_STREAM *streamArray, unsigned short streamArrayCnt);

unsigned char OneNET_Subscribe(const char *topics[], unsigned char topic_cnt);

unsigned char OneNET_UnSubscribe(const char *topics[], unsigned char topic_cnt);

unsigned char OneNET_Publish(const char *topic, const char *msg);

unsigned char OneNET_SendData_Heart(void);

_Bool OneNET_Check_Heart(void);

void OneNET_CmdHandle(void);

void OneNET_RevPro(unsigned char *dataPtr);

unsigned char NET_DEVICE_AddDataSendList(unsigned char *buf ,unsigned short data_len, _Bool mode);

void Judge_SenDdata(unsigned short Send_Status);
 unsigned char OneNET_Authorization(char *ver, char *res, unsigned int et, char *access_key, char *dev_name,
											char *authorization_buf, unsigned short authorization_buf_len, _Bool flag);

unsigned char OneNET_SendData2(char *dataName, void *dataBuf,DATA_TYPE dataType);
 void OneNET_SendRun_Par(void);
 unsigned char OneNET_SendCMD(char *topic);
 void OneNET_Sendev_SavePar(void);
 void OneNET_SenControl_Par(void);
 void OneNET_SenAuthor_Par(void);
#endif

 
