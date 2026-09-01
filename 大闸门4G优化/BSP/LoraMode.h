#ifndef __LORAMODE_H
#define __LORAMODE_H	



#include "main.h"
#define RS485_TX_EN   	GPIO_SetBits(GPIOB, GPIO_Pin_9 )  	      
#define RS485_RX_EN    	GPIO_ResetBits(GPIOB, GPIO_Pin_9 ) 	

#define LORACHANNL  03
#define LORABAND    0xFFFF

#define NETID00      	0x0  		//定义本地网络地址

/*定义串口速率*/
#define Rate1200        (0<<5)
#define Rate2400        (1<<5)
#define Rate4800        (2<<5)
#define Rate9600        (3<<5)
#define Rate19200       (4<<5)
#define Rate38400       (5<<5)
#define Rate57600       (6<<5)
#define Rate115200      (7<<5)
/*定义串口校验位*/
#define Check8N1      (0<<3)
#define Check8O1      (1<<3)
#define Check8E1      (2<<3)
/*定义无线空中速率*/
#define Air2D4K        2
#define Air4D8K        3
#define Air9D6K        4
#define Air19D2K       5 
#define Air38D4K       6
#define Air62D5K       7
/*定义分包设定*/
#define PACK240       (0<<6)
#define PACK128       (1<<6)
#define PACK64        (2<<6)
#define PACK32        (3<<6)
/*RSSI环境噪声使能*/
#define RSSIEN        (0<<5)
#define RSSIDIS       (1<<5)
/*发射功率*/
#define TRANS22dBm     0
#define TRANS17dBm     1
#define TRANS13dBm     2
#define TRANS10dBm     3
/*Addr:06H -------------------------- */
/*启用RSSI字节*/
#define RSSIByteDIS   (0<<7)
#define RSSIBybeEN    (1<<7)
/*传输模式*/
#define TransTransparent   (0<<6)
#define TransFixedPoint    (1<<6)
/*中继功能*/
#define RepeatDIS           (0<<5)
#define RepeatEN            (1<<5)
/*LBT使能*/
#define LBTDIS               (0<<4)
#define LBTEN             (1<<4)
/*WOR模式收发控制*/
#define WORReceiver           (0<<3)
#define WORSender             (1<<3)
/*WOR周期 */
#define WORCycle500           0
#define WORCycle1000          1
#define WORCycle1500          2
#define WORCycle2000          3
#define WORCycle2500          4
#define WORCycle3000          5
#define WORCycle3500          6
#define WORCycle4000          7
/*Addr:07H-08H -------------------------- */
/*定义秘钥      addr:07H-08H*/
#define LocalSecretKey             0x0000
/*定义E22-400T22S寄存器地址*/
#define ADDH      0x00
#define ADDL      0x01
#define NETID     0x02
#define REG0      0x03
#define REG1      0x04
#define REG2      0x05
#define REG3      0x06
#define CRYPT_H   0x07
#define CRYPT_L   0x08
#define PID       0x80

#endif
