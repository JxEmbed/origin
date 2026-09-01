#ifndef __MYONENET_H
#define __MYONENET_H

#include "main.h"
#include "onenet.h"
/*********************************************************************
 
	                   _ooOoo_
	                  o8888888o
	                  88" . "88
	                  (| -_- |)
	                  O\  =  /O
	               ____/`---'\____
	             .'  \\|     |//  `.
	            /  \\|||  :  |||//  \
	           /  _||||| -:- |||||-  \
	           |   | \\\  -  /// |   |
	           | \_|  ''\-/''  |   |
	           \  .-\__  `-`  ___/-. /
	         ___`. .'  /-.-\  `. . __
	      ."" '<  `.___\_<|>_/___.'  >'"".
	     | | :  `- \`.;`\ _ /`;.`/ - ` : | |
	     \  \ `-.   \_ __\ /__ _/   .-` /  /
	======`-.____`-.___\_____/___.-`____.-'======
	                   `=-='
 

********************************************************************/
//#define	ONENET_NUM		9020					//一干沙池出口
//#define	ONENET_NUM		8881					//二干麻家卓段
//#define	ONENET_NUM			8748					//二干白鹿段
//#define		ONENET_NUM		7251         //test1
#define   ONENET_NUM    8888          //test1
/*
我们的服务器的ip和端口
*/
#define OURS_SERVER_IP   "47.117.126.245"
#define OURS_SERVER_PORT "9070"

unsigned char OneNET_Authorization_V1(char *res, char *access_key, char *dev_name,
											char *authorization_buf, unsigned short authorization_buf_len, _Bool flag);
unsigned char OneNET_SendCMD(char *topic);
void OneNET_CmdHandle(void);
void MyOnenet_ProcessCmd(void);
void myOnenet_infoSet(void); 
u8 myOnenet_CheckLink(void);
void OneNETAdd_RevPro2(char * strTopic,char *strLen,char *strMessage);
void OneNET_CmdHandle2(void);


extern ONETNET_INFO onenet_info_test1;
extern ONETNET_INFO onenet_info_tui;
extern ONETNET_INFO onenet_info_mao;
extern ONETNET_INFO onenet_info_zhang;
#endif