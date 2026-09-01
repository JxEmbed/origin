#include "Bluetooth.h"
#include "string.h"
#include "stdio.h"
#define BT_RXBUFF_LEN 100
u8 bt_rxbuff[BT_RXBUFF_LEN];
u8 bt_rxbuffPtr;
u8 bt_rxFlag;
#define BT_TxbuffLen 1000
u8 bt_txbuff[BT_TxbuffLen];
void Bt_UartInit(void)
{
	/* 初始化GPIO外设 */
    rcu_periph_clock_enable(RCU_GPIOD);
	rcu_periph_clock_enable(RCU_GPIOC);
    /* TX管脚，PA9，复用推挽输出，速度50MHz */
    gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
    /* RX管脚，PA10，下拉输入，速度50MHz */
    gpio_init(GPIOD, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
	
    /* 初始化USART外设 */
    rcu_periph_clock_enable(RCU_UART4);  // 使能串口0时钟
	usart_deinit(UART4);
    usart_baudrate_set(UART4, 256000);  // 波特率115200
    usart_parity_config(UART4, USART_PM_NONE);  // 无校检
    usart_word_length_set(UART4, USART_WL_8BIT);  // 8位数据位
    usart_stop_bit_set(UART4, USART_STB_1BIT);  // 1位停止位
    usart_hardware_flow_rts_config(UART4, USART_RTS_DISABLE); /* 禁用rts */
    
    usart_hardware_flow_cts_config(UART4, USART_CTS_DISABLE); /* 无硬件数据流控制 */

	usart_transmit_config(UART4, USART_TRANSMIT_ENABLE);  // 使能串口发送
    usart_receive_config(UART4, USART_RECEIVE_ENABLE);  // 使能串口接收
    usart_enable(UART4);  // 使能串口
	nvic_irq_enable(UART4_IRQn,1U, 1U);
	usart_interrupt_enable(UART4, USART_INT_RBNE);  /* 参数：读数据缓冲区非空中断和过载错误中断 */
	usart_interrupt_enable(UART4, USART_INT_IDLE);  /* 参数：读数据缓冲区非空中断和过载错误中断 */
}
void Bluetooth_Init(void)
{
	rcu_periph_clock_enable(RCU_GPIOD);
	rcu_periph_clock_enable(RCU_AF);

	/* SWD remap */
	gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
	/* GPIOB output */
	gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
	
	Bluetooth_PowerEN=0;
	gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
	
	Bt_Wakeup=1;
	Bt_UartInit();
//	delay_ms(200);
//	return;
//	while(1)
//	{
//		delay_ms(1000);
//	}
	delay_ms(200);
	Bt_SendStr("AT\r\n");
//	while(1)
//	{
//		
////		bt_sendstr("at+dch=0\r\n");
////		delay_ms(5000);
//		Bt_SendStr("abcdefg\r\n");
//		delay_ms(1000);
////		delay_ms(5000);
//	}



}

void Bt_SendBuff(u8 * buff,u32 len)
{

	u32 i;
	Bt_Wakeup=0;
	for(i=0;i<len;i++)
	{
		//USART_FLAG_TC
		while (usart_flag_get(UART4, USART_FLAG_TBE)== RESET); /* 获取缓冲区是否为空 */
		usart_data_transmit(UART4, *buff); /* transmit */
		while (usart_flag_get(UART4, USART_FLAG_TC)== RESET); /* 获取缓冲区是否为空 */
		
		buff++;
	}
}
void Bt_SendStr(char * str)
{
	Bt_Wakeup=0;
	delay_ms(1);
	vTaskDelay(1);
	while(*str)
	{
		while(usart_flag_get(UART4,USART_FLAG_TBE)==RESET);
		usart_data_transmit(UART4,*str);
		while(usart_flag_get(UART4,USART_FLAG_TC)==RESET);
		str++;
	}
//	Bt_Wakeup=1;
}
void Bt_SendWatLev(void)
{
	u8 i=0,j=0;
	uint16_t crcCalc; //存储计算出来的crc结果
	u32 tx_len=0;
//	char tempc[100];
	memset(bt_txbuff,0,sizeof(bt_txbuff));
	//01|设备编号|gates| v1:open;tw1:33;w1:330;s1:open;b:12.4;g;20.445,4545.154|OK|CRC16
//	strcat((char *)Lora_Txbuff,"01|")
	snprintf((char *)bt_txbuff,sizeof(bt_txbuff),"%sB,%s,",bt_txbuff,dev_SavePar.dev_code);
	snprintf((char *)bt_txbuff,sizeof(bt_txbuff),"%s%02d-%02d-%02d% 02d:%02d:%02d,",bt_txbuff,lora_rtc.years,lora_rtc.months,lora_rtc.days,lora_rtc.hours,lora_rtc.minutes,lora_rtc.secs);
	
	snprintf((char *)bt_txbuff,sizeof(bt_txbuff),"%s%f,",bt_txbuff,RS485_Sensor.Aft_h);
	snprintf((char *)bt_txbuff,sizeof(bt_txbuff),"%sSSS\r\n",bt_txbuff);
	tx_len=strlen((char *)bt_txbuff);
//	LoraSetMode(0);	
//	delay_ms(500); //延时等待切换
//	vTaskSuspendAll();
	Bt_SendBuff((u8 *)bt_txbuff,tx_len);
//	xTaskResumeAll();
//	delay_ms(500); //延时等待切换
//	LoraSetMode(1);	
}
void Bt_RxProcess(void)
{
	if(bt_rxFlag==1)
	{
		bt_rxbuffPtr=0;
		bt_rxFlag=0;
	}
}
void UART4_IRQHandler(void)
{
	u8 res;
	//串口接收中断
	 if(RESET != usart_interrupt_flag_get(UART4, USART_INT_FLAG_RBNE)) /* receive */
	{
		res =usart_data_receive(UART4); /* 接收 */
		usart_interrupt_flag_clear(UART4,USART_INT_FLAG_RBNE); 
		if(bt_rxFlag==1)
		{
			bt_rxbuffPtr=0;
			bt_rxFlag=0;
		}
		if(bt_rxbuffPtr<BT_RXBUFF_LEN)
		{
			bt_rxbuff[bt_rxbuffPtr++]=res;
		}
	}
	if(RESET != usart_interrupt_flag_get(UART4, USART_INT_FLAG_IDLE))
	{
		res = usart_data_receive(UART4); /* 接收 */
		usart_interrupt_flag_clear(UART4,USART_INT_FLAG_IDLE);
		bt_rxFlag=1;
	}
}

