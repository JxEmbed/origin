#include "Lora.h"
#include "LoraMode.h"
#include "string.h"
#include "stdio.h"
#include "jiang.h"
#include "liu.h"

u8 Lora_Rxbuff[Lora_RxbuffLen];
u8 Lora_Txbuff[Lora_RxbuffLen];
u8 Lora_RxFlag=0;
u16 Lora_RxBuffPtr=0;
void Lora_UART3_Config(uint32_t baudval)
{
	/* 初始化GPIO外设 */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* TX管脚，PA9，复用推挽输出，速度50MHz */
    gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    /* RX管脚，PA10，下拉输入，速度50MHz */
    gpio_init(GPIOC, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, GPIO_PIN_11);

    /* 初始化USART外设 */
    rcu_periph_clock_enable(RCU_UART3);  // 使能串口0时钟
		usart_deinit(UART3);
    usart_baudrate_set(UART3, baudval);  // 波特率9600
    usart_parity_config(UART3, USART_PM_NONE);  // 无校检
    usart_word_length_set(UART3, USART_WL_8BIT);  // 8位数据位
    usart_stop_bit_set(UART3, USART_STB_1BIT);  // 1位停止位
	  usart_hardware_flow_rts_config(UART3, USART_RTS_DISABLE); /* 禁用rts */
    
    usart_hardware_flow_cts_config(UART3, USART_CTS_DISABLE); /* 无硬件数据流控制 */

    usart_transmit_config(UART3, USART_TRANSMIT_ENABLE);  // 使能串口发送
    usart_receive_config(UART3, USART_RECEIVE_ENABLE);  // 使能串口接收
    usart_enable(UART3);  // 使能串口
		
	nvic_irq_enable(UART3_IRQn,0U, 0U);
	usart_interrupt_enable(UART3, USART_INT_RBNE);  /* 参数：读数据缓冲区非空中断和过载错误中断 */
	usart_interrupt_enable(UART3, USART_INT_IDLE);  /* 参数：读数据缓冲区非空中断和过载错误中断 */
}
void Lora_init(void)
{
	if(dev_SavePar.Lora_Fun == 0)
	{
		return ;
	}
	
	Lora_PWR_init();
	Lora_UART3_Config(9600);
	Lora_ModePin_init();
	
	while(LoraConf(BIGGateAddr,BIGGateChannel)!=0)
	{
		delay_ms(20);
	}
}
void Lora_PWR_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOD);
	//lora电源使能引脚
    gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
	Lora_PowerEN=0;
}
void Lora_ModePin_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOD);
	//lora电源使能引脚
    gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
	gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
	rcu_periph_clock_enable(RCU_GPIOA);
	gpio_init(GPIOA, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
}
void Lora_SendStr(char * str)
{
	while(*str)
	{
		while(usart_flag_get (UART3 ,USART_FLAG_TBE )!=SET ){}
		usart_data_transmit(UART3,*str);
		while(usart_flag_get (UART3 ,USART_FLAG_TC )!=SET ){}
		str++;
	}

}
/*
Lora发送
*/
void Lora_Send(u8 * buff, u32 len)
{
	u32 i;
	for(i=0;i<len;i++)
	{
		while(usart_flag_get (UART3 ,USART_FLAG_TBE )!=SET ){}
		usart_data_transmit(UART3,buff[i]);
		while(usart_flag_get (UART3 ,USART_FLAG_TC )!=SET ){}
	}
}
void Lora_Packet(char *str)
{
	u8 buff[5];
	buff[0]=(ALLAddr>>8);
	buff[1]=(ALLAddr&0xff);
	buff[2]=GATECHANNEL;
	Lora_Send(buff,3);
	Lora_SendStr(str);
}
//************************************************************
//参数
//	0：传输模式
//	1：WOR模式
//	2：配置模式
//	3：深度休眠
//*************************************************************
void LoraSetMode(uint8_t mode) //设置工作模式
{

	while(LoraAUX==1);
	delay_ms(10);
	LoraM1=mode>>1;
	LoraM0=mode&0x01;
	while(LoraAUX==1);
	delay_ms(50);
}
s8 LoraConf(u16 addr,u8 channel)
{
	u8 timeCount=0;
	
//	u8 result,;
//	u16 i;
	//00 00 00 62 00 17 03 00 00
	//00 00 00 62 00 17 03 00 00
	Lora_Txbuff[0]=0xC0;  //设置寄存器指令
	Lora_Txbuff[1]=0x00;  //起始地址
	Lora_Txbuff[2]=0x09;  //长度
	/*00H*/
	Lora_Txbuff[3]=(addr>>8);  //[00]模块地址高字节
	/*01H*/
	Lora_Txbuff[4]=(addr&0xff);  //[01]模块地址低字节
	/*02H*/
	Lora_Txbuff[5]=NETID00;  //[02]网络地址 
	/*03H*/
	Lora_Txbuff[6]=Rate9600|Check8N1|Air9D6K;  //[03]REG0  波特率(011 9600)，串口校验位(00 8N1),无线空中速率(010 2.4k)  0x62
	/*04H*/
	Lora_Txbuff[7]=PACK240|RSSIDIS|TRANS22dBm;  //[04]REG1  分包设定(00 240字节) RSSI环境噪声使能(0 禁用),(000 保留),发射功率(00 22dBm 最大功率)
	/*05H*/
	Lora_Txbuff[8]=channel;  //[05]REG2  信道控制
	/*06H*/
	Lora_Txbuff[9]=RSSIByteDIS|TransFixedPoint|RepeatDIS|LBTDIS|WORSender|WORCycle500;  //[06]REG3  (0101 禁用RSSI,定点传输，关闭中继功能，启用LBT),(0000 WOR接收 500ms周期)
	/*07USART2txbufferH*/
	Lora_Txbuff[10]=(LocalSecretKey>>8);  //[07]CRYPT_H 字节(0000)
	/*08H*/
	Lora_Txbuff[11]=(LocalSecretKey&0xff);  //[08]CRYPY_L 低字节(0000)
	LoraSetMode(2);	
	delay_ms(500); //延时等待切换
	
//	USART_write(COM_PORT_2,(u8 *)USART2txbuffer,12);
	Lora_Send((u8 *)Lora_Txbuff,12);
	while(1)
	{
		if(Lora_RxFlag==1)
		{
			
			if((Lora_Rxbuff[0]==0xC1)&&(Lora_Rxbuff[1]==0x00)&&(Lora_Rxbuff[2]==0x09))
			{
				delay_ms(20); //延时等待切换
				LoraSetMode(1);
				Lora_RxFlag=0;
				Lora_RxBuffPtr=0;
				Statlog_Par.loraState=1;
				return 0;
			}
		}
		timeCount++;
		if(timeCount>30)
		{
			delay_ms(20); //延时等待切换
			LoraSetMode(1);
			Statlog_Par.loraState=2;
			return -1;
		}
		delay_ms(20);
	}
	
	
}
void Lora_Process(void)
{
	if(dev_SavePar.Lora_Fun == 0)
		return ;
	
	if(Lora_RxFlag==1)
	{
		Lora_Rxanalysis_liu();
		Lora_RxBuffPtr=0;
		Lora_RxFlag=0;
		lora_rece++;
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_WHITE"lora_send=%u,lora_rece=%u\r\n",lora_send,lora_rece);
	}
}

u32 tx_len=0;
void Lora_SendAllData(void)
{
	
	uint16_t crcCalc; //存储计算出来的crc结果
	char tempc[100];
	memset(Lora_Txbuff,0,sizeof(Lora_Txbuff));
	Lora_Txbuff[0]=(ALLAddr>>8);
	Lora_Txbuff[1]=(ALLAddr&0xff);
	Lora_Txbuff[2]=ALLCHANNEL;
	//01|设备编号|gates| v1:open;tw1:33;w1:330;s1:open;b:12.4;g;20.445,4545.154|OK|CRC16
	strcat((char *)Lora_Txbuff,"01|");
	snprintf((char *)tempc,sizeof(tempc),"%s|gates|",dev_SavePar.dev_code);
	strcat((char *)Lora_Txbuff,tempc);
	//"时间;闸前水位m;闸后水位m;开度%;实时流量m3/s;累计水量m3;
	snprintf((char *)tempc,sizeof(tempc),"%f,%f,%f,%f,%f|",RunLog_Par.fro_level,RunLog_Par.aft_level,RunLog_Par.opening,RunLog_Par.flow,RunLog_Par.accu_water);
	strcat((char *)Lora_Txbuff,tempc);
	tx_len=strlen((char *)Lora_Txbuff);
	crcCalc=crc(Lora_Txbuff+3,tx_len-3);                       //计算的crc
	Lora_Txbuff[tx_len++]=(crcCalc>>8);
	Lora_Txbuff[tx_len++]=(crcCalc&0xff);
	LoraSetMode(0);	
//	delay_ms(500); //延时等待切换
	vTaskSuspendAll();
	Lora_Send((u8 *)Lora_Txbuff,tx_len);
	xTaskResumeAll();
//	delay_ms(500); //延时等待切换
//	LoraSetMode(1);	
}

