#include "L76K.h"
#include "string.h"
#include "stdio.h"
u8 L76K_RxBuff[L76K_REBUFF_LEN];
u16 L76K_RxBuffPtr=0;
u8 L76K_RxFlag=0;
L76K_STRUCT L76K_Par;
//uint8_t L76K_TA[]={
//0xBA, 0xCE, 0x38, 0x00, 0x0B, 0x01 , 0x52 , 0xB8 , 0x1E , 0x85 , 0xEB, 
//0xD1 , 0x3F , 0x40 , 0xD7 , 0xA3 , 0x70 , 0x3D , 0x0A , 0x47 , 0x5D , 0x40 , 0x00 00 00 00 00 00 00 00 00 00
//00 00 7C 2E 11 41 00 00 00 00 00 00 00 00 00 00 00 3F 00 00 00 00 E5 07 00 00 62 08 00 23 19 B4 48
//E7
//};
void L76K_USART5_Config(uint32_t baudval)
{
	/* 初始化GPIO外设 */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* TX管脚，PA9，复用推挽输出，速度50MHz */
    gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    /* RX管脚，PA10，下拉输入，速度50MHz */
    gpio_init(GPIOC, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, GPIO_PIN_7);

    /* 初始化USART外设 */
    rcu_periph_clock_enable(RCU_USART5);  // 使能串口0时钟
		usart_deinit(USART5);
    usart_baudrate_set(USART5, baudval);  // 波特率115200
    usart_parity_config(USART5, USART_PM_NONE);  // 无校检
    usart_word_length_set(USART5, USART_WL_8BIT);  // 8位数据位
    usart_stop_bit_set(USART5, USART_STB_1BIT);  // 1位停止位
	usart_hardware_flow_rts_config(USART5, USART_RTS_DISABLE); /* 禁用rts */
    
    usart_hardware_flow_cts_config(USART5, USART_CTS_DISABLE); /* 无硬件数据流控制 */

    usart_transmit_config(USART5, USART_TRANSMIT_ENABLE);  // 使能串口发送
    usart_receive_config(USART5, USART_RECEIVE_ENABLE);  // 使能串口接收
    usart_enable(USART5);  // 使能串口
		
	nvic_irq_enable(USART5_IRQn,0U, 0U);
	usart_interrupt_enable(USART5, USART_INT_RBNE);  /* 参数：读数据缓冲区非空中断和过载错误中断 */
	usart_interrupt_enable(USART5, USART_INT_IDLE);  /* 参数：读数据缓冲区非空中断和过载错误中断 */
//	usart_interrupt_disable(UART7, USART_INT_TBE);  /*发送器缓冲区空中断 */
}
void L76K_Gpio_Config(void)
{
	//4G模块开机/关机控制脚初始化  --PWR_4G
	rcu_periph_clock_enable(RCU_GPIOG);
	gpio_init(GPIOG, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
	//4G模块供电控制脚初始化  --EN_4G
	rcu_periph_clock_enable(RCU_GPIOG);
	gpio_init(GPIOG, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
	//4G模块供电控制脚初始化  --EN_4G
	rcu_periph_clock_enable(RCU_GPIOG);
	gpio_init(GPIOG, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
	PGout(6)=1;
	PGout(8)=1;
	PGout(7)=1;
}

void L76K_init(uint32_t baudval)
{
	L76K_Gpio_Config();
	L76K_USART5_Config(baudval);
}
void L76K_Start(void)
{
	L76K_EN=0;			//供电
//	delay_ms(1000);
	L76K_WAKEUP=0;		
//	L76K_RESETN=1;
//	delay_ms(30);
	L76K_RESETN=0;
}
void L76K_End(void)
{
	L76K_EN=1;			//断电
	L76K_WAKEUP=1;		
	L76K_RESETN=1;
}
void L76K_Send(u8 * buff,u32 len)
{
	u16 i=0;
	for(i=0;i<len;i++)
	{
		//USART_FLAG_TC
		while (usart_flag_get(USART5, USART_FLAG_TBE)== RESET); /* 获取缓冲区是否为空 */
		usart_data_transmit(USART5, *buff); /* transmit */
		while (usart_flag_get(USART5, USART_FLAG_TC)== RESET); /* 获取缓冲区是否为空 */
		
		buff++;
	}
}
void L76K_Test(void)
{
	//

	//BA CE 00 00 06 00 00 00 06 00
	u8 buff[100];
	buff[0]=0xba;
	buff[1]=0xce;
	buff[2]=0x00;
	buff[3]=0x00;//
	buff[4]=0x06;//
	buff[5]=0x00;///
	buff[6]=0x00;
	buff[7]=0x00;
	buff[8]=0x06;
	buff[9]=0x00;
	L76K_Send(buff,10);
}
/*
WGS84转CGCS2000
纬度：ddmm.mmmmm（度分）
经度：dddmm.mmmmm（度分）


dd.ddddd
转换公式 dd.ddddd = dd + mm.mmmm/60。
*/
double dd;
double mm;
double D_Lat;  //浮点数表示的纬度
double D_Lon;  //浮点数表示的经度
u32    u32_Lat;
u32    u32_Lon;
void L76K_Resend()
{
	char * ptr1,* ptr2;
	
	if(L76K_RxFlag==1)
	{
		L76K_RxFlag=0;
		ptr1=strstr((const char *)L76K_RxBuff,"$GNGGA");
		if((L76K_REBUFF_LEN-((u8 *)ptr1-L76K_RxBuff))<74)
		{
			return;
		}
		ptr2=strstr(ptr1+1,",");
		ptr1=strstr(ptr2+1,",");
		if(ptr1-ptr2-1>0)
		{
			memset(L76K_Par.UTC,0,sizeof(L76K_Par.UTC));
			memcpy(L76K_Par.UTC,ptr2+1,ptr1-ptr2-1);
		}
		
		
		ptr2=strstr(ptr1+1,",");
		if(ptr2-ptr1-1>0)
		{
			memset(L76K_Par.Lat,0,sizeof(L76K_Par.Lat));
			memcpy(L76K_Par.Lat,ptr1+1,ptr2-ptr1-1);
		}

		ptr1=strstr(ptr2+1,",");
		if(ptr1-ptr2-1>0)
		{
			memset(L76K_Par.N_S,0,sizeof(L76K_Par.N_S));
			memcpy(L76K_Par.N_S,ptr2+1,ptr1-ptr2-1);
		}
		
		ptr2=strstr(ptr1+1,",");
		if(ptr2-ptr1-1>0)
		{
			memset(L76K_Par.Lon,0,sizeof(L76K_Par.Lon));
			memcpy(L76K_Par.Lon,ptr1+1,ptr2-ptr1-1);
		}
		
		ptr1=strstr(ptr2+1,",");
		if(ptr1-ptr2-1>0)
		{
			memset(L76K_Par.E_W,0,sizeof(L76K_Par.E_W));
			memcpy(L76K_Par.E_W,ptr2+1,ptr1-ptr2-1);
		}
		
		ptr2=strstr(ptr1+1,",");
		if(ptr2-ptr1-1>0)
		{
			memset(L76K_Par.Quality,0,sizeof(L76K_Par.Quality));
			memcpy(L76K_Par.Quality,ptr1+1,ptr2-ptr1-1);
		}
		if(strstr(L76K_Par.Quality,"1"))
		{
			sscanf(L76K_Par.Lat, "%lf", &D_Lat);
			sscanf(L76K_Par.Lon, "%lf", &D_Lon);
			u32_Lat=D_Lat*100000;
			u32_Lon=D_Lon*100000;
			dd=u32_Lat/10000000;
			mm=u32_Lat%10000000;
			
			L76K_Par.F_Lat=dd+(mm/100000/60);
			
			u32_Lon=D_Lon*100000;
			dd=u32_Lon/10000000;
			mm=u32_Lon%10000000;
			
			L76K_Par.F_Lon=dd+(mm/100000/60);
//			L76K_Par.F_Lon=100;
			if(strstr(L76K_Par.N_S,"S"))
			{
				snprintf(L76K_Par.Union_Lat,sizeof(L76K_Par.Union_Lat),"S%.5f",L76K_Par.F_Lat);
				L76K_Par.F_Lat=-L76K_Par.F_Lat;
			}
			else
			{
				snprintf(L76K_Par.Union_Lat,sizeof(L76K_Par.Union_Lat),"N%.5f",L76K_Par.F_Lat);
			}
			if(strstr(L76K_Par.E_W,"W"))
			{
				snprintf(L76K_Par.Union_Lon,sizeof(L76K_Par.Union_Lon),"W%.5f",L76K_Par.F_Lon);
				L76K_Par.F_Lon=-L76K_Par.F_Lon;
			}
			else
			{
				snprintf(L76K_Par.Union_Lon,sizeof(L76K_Par.Union_Lon),"E%.5f",L76K_Par.F_Lon);
			}
			Statlog_Par.GPSState=1;
			nosave_par.L76K_SendF=3;
		}
		else
		{
			Statlog_Par.GPSState=2;
		}
	
		L76K_RxBuffPtr=0;
	}
}
void L76K_SendStr(char * str)
{
	while(*str)
	{
		//USART_FLAG_TC
		while (usart_flag_get(USART5, USART_FLAG_TBE)== RESET); /* 获取缓冲区是否为空 */
		usart_data_transmit(USART5, *str); /* transmit */
		while (usart_flag_get(USART5, USART_FLAG_TC)== RESET); /* 获取缓冲区是否为空 */
		
		str++;
	}
}

void L76K_LowPower(void)
{
	if(nosave_par.L76K_SendF==1)
	{
		//清空数据
		memset(&L76K_Par,0,sizeof(L76K_Par));
		Statlog_Par.GPSState=0;
		
		L76K_Start();
		nosave_par.L76K_SendF=2;
	}
	else if(nosave_par.L76K_SendF==2)
	{
		L76K_Resend();		
	}
	else if(nosave_par.L76K_SendF==3)
	{
		L76K_End();
		nosave_par.L76K_SendF=0;
	}
}

