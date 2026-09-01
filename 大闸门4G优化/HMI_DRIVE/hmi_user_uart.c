/************************************版权申明********************************************
**                             广州大彩光电科技有限公司
**                             http://www.gz-dc.com
**-----------------------------------文件信息--------------------------------------------
** 文件名称:   hmi_user_uart.c
** 修改时间:   2018-05-18
** 文件说明:   用户MCU串口驱动函数库
** 技术支持：  Tel: 020-82186683  Email: hmi@gz-dc.com Web:www.gz-dc.com
--------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------
使用必读
hmi_user_uart.c中的串口发送接收函数共3个函数：串口初始化Uartinti()、发送1个字节SendChar()、
发送字符串SendStrings().若移植到其他平台，需要修改底层寄
存器设置,但禁止修改函数名称，否则无法与HMI驱动库(hmi_driver.c)匹配。
--------------------------------------------------------------------------------------



----------------------------------------------------------------------------------------
1. 基于STM32平台串口驱动
----------------------------------------------------------------------------------------*/
#include "hmi_user_uart.h"
#include "hw_config.h"
#include "hmi_driver.h"
/*!
*   \brief   串口初始化
*   \param   BaudRate-波特率设置
*/


/*!
*   \brief  发送1个字节
*   \param  t 发送的字节

*/
void  SendChar(uchar t)
{
	lcd_txbuff[lcd_txptr++]=t;
//	while (usart_flag_get(UART6, USART_FLAG_TBE)== RESET); /* 获取缓冲区是否为空 */
//		usart_data_transmit(UART6, *buff); /* transmit */
//		while (usart_flag_get(UART6, USART_FLAG_TC)== RESET); /* 获取缓冲区是否为空 */
//    usart_data_transmit(UART6,t);
//    while(usart_flag_get(UART6, USART_FLAG_TBE) == RESET);
//    while((usart_flag_get(UART6, USART_FLAG_TC) == RESET));//等待串口发送完毕
}
