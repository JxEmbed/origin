#ifndef __LORACONF_H
#define __LORACONF_H


//定义统一的参数
#define AllAddr    0x0a0a
#define AllChannel  0x21

#define WeatherAddr    0x0201
#define WeatherChannel 0x21

/*--定义指令-------4,5,6,7-8,9,10,11-12,13,14,15-----------------------------------------*/
/*
1,2,3,4     为操作施肥机指令
5,6,7,8     为操作气象站指令
9,10,11,12  为操作墒情站指令
13,14,15,16 为操作田间电磁阀指令
*/

//#define WeatQueryCmd 0x05  //请求气象站数据
//#define WeatAsyncCmd 0x06  //同步气象站数据 

//#define MoisQueryCmd 0x09  //请求墒情站数据
//#define MoisAsyncCmd 0x0A  //同步墒情站数据

//#define ValvOperaCmd 0x0D  //操作田间电磁阀
//#define ValvVoltaCmd 0x0E  //读取田间电磁阀电池电压值
//#define ValvTotalCmd 0x0F  //广播操作指令

/*
施肥机发送：
(从机地址+信道)+指令+数据
气象站应答：
(主机地址+信道)+指令+数据
墒情站应答：
(主机地址+信道)+指令+数据
田间电磁阀应答:
(主机地址+信道)+从机地址+指令+数据
*/
#endif


