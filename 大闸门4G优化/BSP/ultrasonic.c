#include "ultrasonic.h"
#include "math.h"
#include "TDC1000Conf.h"
PARAMETER_FLOW_CALCU Ultra_CALCU_Par; //超声波计算参数
PARAMETER_FROM_CALCU CACLU_Parameter;
DEVICE_SETTING_STRUCT Dev_Setting_Par;


float Aver_FlowV; //平均流速
void Ultra_Power_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOC);
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
	Power_5V_EN=1;
}
void Power_5V_init(void)
{
	//LM22670电源控制
	rcu_periph_clock_enable(RCU_GPIOE);
	gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
	gpio_bit_set(GPIOE,GPIO_PIN_5);
	//LM22670的EN引脚
	rcu_periph_clock_enable(RCU_GPIOE);
	gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
	gpio_bit_set(GPIOE,GPIO_PIN_6);
}
void SPI_Software_init(void)
{
	    /* 初始化GPIO外设 */
    rcu_periph_clock_enable(RCU_GPIOF);
	rcu_periph_clock_enable(RCU_GPIOC);
	//PD12 MISO
    gpio_init(GPIOF, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
	//PD13 MOSI
	gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
	//PD14 SCLK
	gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
	//PF14 SPI_CSB_A  
	gpio_init(GPIOF,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_3);
	//TDC7200_CS
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
	Ultra_CSB_A=1;
	TDC7200_CSB_A=1;
	Ultra_SPI_SCLK=1;
}
void TDC7200_Interrupt(void)
{

}
/*

*/
void TDC7200_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOC);
	//TDC7200 使能
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
	TDC7200_EN_A=1;
	//TDC7200 INT
	
	
	gpio_init(GPIOC, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
	
}


void Ultra_MUX_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOF);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOG);
	//U_D0~U_D3
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
	gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
	gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
/*************************************************************************/
	//Ultra_TX1_A_EN
    gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
	//Ultra_RX2_A_EN
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
	//Ultra_TX2_A_EN
    gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
	//Ultra_RX1_A_EN
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
	
	
}
void UCC27537_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOG);
	//TX1_AT
    gpio_init(GPIOG, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
	//TX2_AB
    gpio_init(GPIOG, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
}
void TDC1000_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOF);
	//RESET_A  
    gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
	//PG0 TRIGGER_A
//    gpio_init(GPIOF, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
	
	//PG1  CHSEL_A
    gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);

	
}
void Power_30V_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOC);
	//PG1  CHSEL_A
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
	Power_30V_DIS;
}
void Ultra_All_init(void)
{
	Power_5V_init();
	Power_30V_init();
	SPI_Software_init();
	Ultra_MUX_init();
	TDC1000_init();
	TDC7200_init();
	UCC27537_init();
}
/*
软件模拟SPI
*/

static uint8_t SPI_Software_SendData2(uint8_t ByteSend)
{
	uint8_t i, ByteReceive = 0x00;
	
	for (i = 0; i < 8; i ++)
	{
//		if(ByteSend& 0x80)
//		{
//			Ultra_SPI_MOSI= 1;
//		}
//		else
//		{
//			Ultra_SPI_MOSI= 0;
//		}
//		ByteSend<<=1;
//		ByteReceive<<=1;
		Ultra_SPI_MOSI= ((ByteSend & (0x80 >> i))>0);
		Ultra_SPI_SCLK=0;
		SPI_Delay(0x01);
//		if (Ultra_SPI_MISO == 1){ByteReceive |=1;}
		if (Ultra_SPI_MISO == 1)
		{
			ByteReceive |= (0x80 >> i);
		}
		Ultra_SPI_SCLK=1;
		SPI_Delay(0x01);
		
	}
	
	return ByteReceive;
}
//通道选择
/*
0-------0000 --1111
1-------0001 --1110
2-------0010 --1101
3-------0011 --1100
4 ------0100 --1011

Channel 值0就是1通道  1就是2通道    值0-15对应1-16通道
*/
void Ultra_channelSwitch(u16 Channel)
{
	if(Channel>15)
	{
		return;
	}

//	Channel=15-Channel;
	
	Ultra_D0=(Channel&0x01);
	Ultra_D1=(Channel>>1)&0x01;
	Ultra_D2=(Channel>>2)&0x01;
	Ultra_D3=(Channel>>3)&0x01;
}
void SPI_Delay(__IO uint32_t count)
{
	uint32_t i;
	for(i=0;i<count;i++)
	{
//		uint8_t uc =5;while(uc--);
	}
}
/*********************************************************************************************************
// 初始化任务: TDC1000_Write
// 功能：
// 
*********************************************************************************************************/
u8 test;
void TDC1000_Write(unsigned char addr,unsigned char data)
{
	
  unsigned char inst;
 	inst = 0x40 | addr;  //0100 0000 写入                                             
  inst = 0x7F & inst;    //0111 0000 清零最高位
	Ultra_CSB_A=0;
//	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_TXE);
//	delay_us(2);
	test=SPI_Software_SendData2(inst);
//	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
//	delay_us(2);
	test=SPI_Software_SendData2(data);
//	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
//	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_TXE);
//	delay_us(2);
	Ultra_CSB_A=1;
//	delay_us(2);
	//return i;
}
void TDC1000_Read(unsigned char addr,unsigned char data)
{
  unsigned char inst;                                             
  inst = 0x3F & addr;   //0011 1111
	Ultra_CSB_A=0;
//	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_TXE);
//	delay_us(2);
	test=SPI_Software_SendData2(inst);
//	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
//	delay_us(2);
	test=SPI_Software_SendData2(0);
//	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
//	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_TXE);
//	delay_us(2);
	Ultra_CSB_A=1;
//	delay_us(2);
	//return i;	
}
//
void TDC7200_Write(unsigned char addr,unsigned char data)
{
	
  unsigned char inst;
 	inst = 0x40 | addr;                                               
  inst = 0x7F & inst;
	TDC7200_CSB_A=0;
//	delay_us(2);
//	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_TXE);
	test=SPI_Software_SendData2(inst);
//	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
//	delay_us(2);
	test=SPI_Software_SendData2(data);
//	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
//	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_TXE);
//	delay_us(2);
	TDC7200_CSB_A=1;
//	delay_us(2);
	//return i;
}
uint8_t TDC7200_Read(unsigned char addr,unsigned char data)
{
	uint8_t result;
  unsigned char inst;
 	 inst = 0x3F & addr;   //0011 1111
	TDC7200_CSB_A=0;
//	delay_us(2);
//	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_TXE);
	test=SPI_Software_SendData2(inst);
//	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
//	delay_us(2);
	test=SPI_Software_SendData2(data);
//	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
//	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_TXE);
//	delay_us(2);
	TDC7200_CSB_A=1;
//	delay_us(2);
	result=test;
	return result;
}
uint32_t TDC7200_ReadBuff(unsigned char addr,unsigned char data,uint8_t num)
{
	uint8_t i;
  unsigned char inst;
	uint32_t result=0;
 	 inst = 0x3F & addr;   //0011 1111
	TDC7200_CSB_A=0;
	delay_us(2);
//	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_TXE);
	test=SPI_Software_SendData2(inst);
	for(i=num;i>0;i--)
	{
			delay_us(2);
			result|=(SPI_Software_SendData2(data)<<(8*(i-1)));
	}
//	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

//	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
//	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_TXE);
	delay_us(2);
	TDC7200_CSB_A=1;
	delay_us(2);
	return result;
}
uint32_t TDC7200_Read24(unsigned char addr,unsigned char data)
{
	uint8_t i;
  unsigned char inst;
	uint32_t result=0;
 	 inst = 0x3F & addr;   //0011 1111
	inst = 0x80 | inst;   //0011 1111
	TDC7200_CSB_A=0;
//	delay_us(2);
//	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_TXE);
	test=SPI_Software_SendData2(inst);
//	delay_us(2);
	result+=(SPI_Software_SendData2(data)<<16);
	result+=(SPI_Software_SendData2(data)<<8);
	result+=SPI_Software_SendData2(data);
//	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

//	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
//	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_TXE);
//	delay_us(2);
	TDC7200_CSB_A=1;
//	delay_us(2);
	return result;
}
void TDC7200_REG_Init(void)
{
	//1000 0 01 0
	TDC7200_Write(0x00,0x82);  //1000 0 01 0
	delay_ms(10);
	TDC7200_Read(0x00,0x00);  //00 000 001
	delay_ms(10);
	//00 000 000  测量两个时钟周期 750 000/1000/1000
	TDC7200_Write(0x01,0x00);  //00 000 000
	delay_ms(10);	
	TDC7200_Read(0x01,0x00);  //00 000 001
	delay_ms(10);
	//00000 111
	TDC7200_Write(0x03,0x07);  //0000 0 111
	delay_ms(10);
	TDC7200_Read(0x03,0x00);  //00 000 001
	delay_ms(10);
//	TDC7200_Write(0x04,0x00);  //00 000 001
//	delay_ms(10);
//	TDC7200_Read(0x04,0x00);  //00 000 001
//	delay_ms(10);
//	TDC7200_Write(0x05,0x04);  //00 000 001
//	delay_ms(10);
//	TDC7200_Read(0x05,0x00);  //00 000 001
//	delay_ms(10);
//	
//	TDC7200_Write(0x06,0xFF);  //00 000 001
//	delay_ms(10);
//	TDC7200_Read(0x06,0xFF);  //00 000 001
//	delay_ms(10);
//	TDC7200_Write(0x07,0xFF);  //00 000 001
//	delay_ms(10);
//	TDC7200_Read(0x07,0xFF);  //00 000 001
//	delay_ms(10);
//	
//	TDC7200_Write(0x07,0xFF);  //00 000 001
//	delay_ms(10);
//	TDC7200_Read(0x07,0xFF);  //00 000 001
//	delay_ms(10);
//	TDC7200_Write(0x08,0x00);  //00 000 001
//	delay_ms(10);
//	TDC7200_Read(0x08,0x00);  //00 000 001
//	delay_ms(10);

}
/*********************************************************************************************************
// 初始化任务: TDC1000_REG_Init
// 功能：
//   Pin CH_SEL = 1  TX2     CH_SEL = 0   TX1
*********************************************************************************************************/
//4000k
void TDC1000_REG_Init(void)
{
    //010 2 01110  Divide by 8                   100 01110 4     101   5     110   6       111 7
	TDC1000_Write(0x00,0x4E);    //超声波频率500K   8E  250k   AE  125K		CE  62.5K   EE  31.25K
	
	//01 000 001 一个测量周期 一个STOP信号
	TDC1000_Write(0x01,0x41);		//默认为40      采用自动切换通道功能  需要将NUM_AVG > 0  则  为48  41:只触发一个STOP信号
	
	//0000 1010 启用外部通道选择  2h： 模式 2
	TDC1000_Write(0x02,0x0A);     // 12：CH_SWP= 1,mode = 2；02:CH_SWP= 0,mode = 2；
	
	//0000 0 110
	TDC1000_Write(0x03,BLANKING_DIS|ECHO_QUAL_THLD_125);	    // BLANKING = 0；

	// 110 0 0 0 11
	TDC1000_Write(0x05,PGA_GAIN_21|PGA_CTRL_EN|LNA_CTRL_EN|3);    //Gain: 21db  

	TDC1000_Write(0x06,0xFF);		//  TIMIN_REG filed 设置为1023   (1024-30)*8*t0 =   start TOF Stand measure mode
	//0001 1 0 00 启用回波超时（ 默认设置）
	TDC1000_Write(0x08,0x18|ECHO_TIMEOUT_DIS);		//  enable echo and TOF_TIMOUT_CTL= 512*t0 = 128us
//    TDC1000_Write(0x08,0x18);
	// 0000 0111
	TDC1000_Write(0x09,0x00);		//CLOCCK_DIV = 1   f0 = 0.25uS      AUTOZERO_PERIOD = 512*f0 = 
	TDC1000_REG_InitRead();
}
/*
弃用
*/
void TDC1000_REG_InitRead(void)
{
	//45 0100 0101 0100 1110
    //010 2 01110  Divide by 8                   100 01110 4     101   5     110   6       111 7
	TDC1000_Read(0x00,0x4E);    //超声波频率500K   8E  250k   AE  125K		CE  62.5K   EE  31.25K

	//01 000 001 一个测量周期 一个STOP信号
	TDC1000_Read(0x01,0x41);		//默认为40      采用自动切换通道功能  需要将NUM_AVG > 0  则  为48  41:只触发一个STOP信号

	//0000 1010  启用外部通道选择 模式2
	TDC1000_Read(0x02,0x0A);     // 12：CH_SWP= 1,mode = 2；02:CH_SWP= 0,mode = 2；
	
	//0000 0110
	TDC1000_Read(0x03,0x06);	    // BLANKING = 0；

	// 111 0 0 0 11
	TDC1000_Read(0x05,0xE3);    //Gain: 21db  
	
	TDC1000_Read(0x06,0xff);		//  TIMIN_REG filed 设置为1023   (1024-30)*8*t0 =   start TOF Stand measure mode
	
	TDC1000_Read(0x08,0x18);		//  enable echo and TOF_TIMOUT_CTL= 512*t0 = 128us
	
	TDC1000_Read(0x09,0x00);		//CLOCCK_DIV = 1   f0 = 0.25uS      AUTOZERO_PERIOD = 512*f0 = 
}
/*********************************************************************************************************
// 初始化任务: getOneChannelValue
// 功能：获得一个通道的飞行时间  成功则返回1 否则返回0
// 输入参数：channel 需要采集的通道序号（0-15）
*********************************************************************************************************/
u8 test_tdc;
u32 TDCtimecount=0;
u8 tempu8;
s8 getOneChannelValue(u8 channel,u8 filter,u8 dir)
{	u32 TEMPA[10];
	u32 TEMPB[10];
	u32 tempSort;
//	TDC1000_Write(0x07,0x03);
	TDC1000_REG_Init();
	u32 timecount=0;
//	u8 tempu8;
	Ultra_channelSwitch(channel);		//进入采集前已配置通道
	if(dir==0) //TX2
	{
		CHSEL_A_H;       //外部通道选择
		Ultra_TX2_A_EN;  //使能多路复用开关
		Ultra_RX2_A_EN;  //使能多路复用开关
		UCC_EN_TX2_AB;   //使能Ucc27537
		
		Ultra_TX1_A_DIS; //关闭多路复用器
		Ultra_RX1_A_DIS; //关闭多路复用器
		UCC_DIS_TX1_AT;  //UCC27537关闭
	}
	else       //TX1
	{
		CHSEL_A_L;       //外部通道选择
		Ultra_TX2_A_DIS;  //使能
		Ultra_RX2_A_DIS;  //使能
		UCC_DIS_TX2_AB;   //使能
		
		Ultra_TX1_A_EN; //关闭多路复用器
		Ultra_RX1_A_EN; //关闭多路复用器
		UCC_EN_TX1_AT;  //UCC27537关闭
	}
	delay_ms(3);
	TDC7200_EN_A=1;
	///00 000 000
	/*
		//COMMAND DEFINE 
parameter   CONFIG1_COMMAND =   16'b0100_0000_1000_0000;    //0x00-0x80
parameter   CONFIG1_READING =   16'b0000_0000_0000_0000;

parameter   CONFIG2_COMMAND =   16'b0100_0001_0000_0000;    //0x01-0x00
parameter   CONFIG2_READING =   16'b0000_0001_0000_0000;
	
parameter   INTMASK_COMMAND =   16'b0100_0011_0000_0001;    //0x03-0x01
parameter   INTMASK_READING =   16'b0000_0011_0000_0000;

parameter   CNTRO_H_COMMAND =   16'b0100_0100_1111_1111;    //0x04-0xff
parameter   CNTRO_H_READING =   16'b0000_0100_0000_0000;

parameter   CNTRO_L_COMMAND =   16'b0100_0101_0000_0000;    //0x05-0x00
parameter   CNTRO_L_READING =   16'b0000_0101_0000_0000;

parameter   CONFIG1_TO_STAR =   16'b0100_0000_1000_0001;    //0x00-0x81

parameter   READING_TIME1   =   16'b1001_0000_0000_0000;    //0x10-0x00
parameter   READING_CALI1   =   16'b1001_1011_0000_0000;    //0x1b-0x00
parameter   READING_CALI2   =   16'b1001_1100_0000_0000;    //0x1c-0x00

parameter   READING_SUPP    =   16'b0000_0000_0000_0000;    //0x00-0x00

	*/
/*
0x01-0x00
0x02-0x1f
*/
	TDC7200_Write(0x00,0x82);  //00 000 001
	test_tdc=TDC7200_Read(0x00,0x00);  //00 000 001
	TDC7200_Write(0x01,0x00);  //00 000 001
	test_tdc=TDC7200_Read(0x01,0x00);  //00 000 001
	TDC7200_Write(0x02,0x1f);  //000 1 1111
	test_tdc=TDC7200_Read(0x02,0x00);  //00 000 001
	TDC7200_Write(0x03,0x01);  //000 1 1111
	test_tdc=TDC7200_Read(0x03,0x00);  //00 000 001
	TDC7200_Write(0x04,0xff);  //000 1 1111
	test_tdc=TDC7200_Read(0x04,0x00);  //00 000 001
	TDC7200_Write(0x05,0x00);  //000 1 1111
	test_tdc=TDC7200_Read(0x05,0x00);  //00 000 001
	//1000 
	TDC7200_Write(0x00,0x83);  //00 000 001
//			while(PFin(12)==1);
	
	while(TDC7200_INT)
	{
		/*
		如果超过6ms都没收到则超时
		*/
		delay_us(1);
		timecount++;
		if(timecount>6000) 
		{
			break;
		}
	}
	TDCtimecount=timecount;
	tempu8=TDC7200_Read(0x02,0x00);
	//if(!(TDC7200_Read(0x02,0x00)&0x01))  //00 011 001)
	if(((((tempu8>>4) & 1)!=1) || ((tempu8 & 1)!=1)))  //00 011 001)
//	if(0)
	{
		Ultra_CALCU_Par.Error_Flag2[filter]=1;
		return 0;
	}
	else
	{
		calc();
		if(isnan(TOF1))
		{
			Ultra_CALCU_Par.tofData_Temp=0;
			Ultra_CALCU_Par.Error_Flag2[filter]=1;		//Ultra_CALCU_Par.Error_Flag2每一次采集是否正常标志位
			return 0;
		}
		
		Ultra_CALCU_Par.tofData_Temp=TOF1;
		
		if(dir==0)
		{
			Ultra_CALCU_Par.distence_for2[channel][filter]=TOF1*1500;
			Ultra_CALCU_Par.tofData_for[channel][filter]=Ultra_CALCU_Par.tofData_Temp;
		}
		else
		{
			Ultra_CALCU_Par.distence_rev2[channel][filter]=TOF1*1500;
			Ultra_CALCU_Par.tofData_rev[channel][filter]=Ultra_CALCU_Par.tofData_Temp;
		}
		
//		//19H  0001 1001
//		Ultra_CALCU_Par.tofData_Temp=TOF1;
		return 1;
	}
}
void ultra_test(void)
{
	Ultra_channelSwitch(0);
	getOneChannelValue(0,0,1);
	vTaskDelay(500);
//	getOneChannelValue(0,0,0);
//	vTaskDelay(500);

}
u8 getAllChannelValue(u8 channelNUM)
{
	Power_30V_EN;
	uint8_t i,j;

	for(i=0;i<channelNUM;i++)		//channelNUM		探头数量
	{
		for(j=0;j<FILTER_NUM;j++)	//FILTER_NUM采集次数
		{
			Ultra_CALCU_Par.Error_Flag2[j]=0;	
		}
		Ultra_channelSwitch(i);
		delay_ms(3);
		for(j=0;j<FILTER_NUM;j++)
		{
			getOneChannelValue(i,j,0);
			delay_ms(20);
			getOneChannelValue(i,j,1);
			delay_ms(20);
			
		}
		Ultra_CALCU_Par.V[i]=Get_Channel_Speed(i);
		if(i==0)
		{
			i=0;
		}
			
	}
	
	Power_30V_DIS;
//	Ultra_test();	

//	Filter_Smooth2(channelNUM);
//	for(i=0;i<channelNUM;i++)
//	{
////		if(Ultra_CALCU_Par.Error_Flag[i]!=1)
//		{
//			Ultra_CALCU_Par.T_for[i]=Flow_Man_Fun1(Ultra_CALCU_Par.tofData_for[i],FILTER_NUM,3);
//			Ultra_CALCU_Par.T_rev[i]=Flow_Man_Fun1(Ultra_CALCU_Par.tofData_rev[i],FILTER_NUM,3);
//		}
//		
//	}
}
/*
	
*/
float Get_Channel_Speed(u8 channel)
{
	uint8_t j;
	float result;
	for(j=0;j<FILTER_NUM;j++)
	{
		//Ultra_CALCU_Par.tofData_for[channel][filter]
		Ultra_CALCU_Par.Delta_T=Ultra_CALCU_Par.tofData_for[channel][j]-Ultra_CALCU_Par.tofData_rev[channel][j];
		Dev_Setting_Par.L=Dev_Setting_Par.b/sin(Dev_Setting_Par.angle)+0.00764;  //计算声波传播的通道的长度
		Dev_Setting_Par.L2=Dev_Setting_Par.b/sin(Dev_Setting_Par.angle);  //计算声波传播的通道的长度
		Ultra_CALCU_Par.Average_T=(Ultra_CALCU_Par.tofData_for[channel][j]+Ultra_CALCU_Par.tofData_rev[channel][j])/2;
//			Ultra_CALCU_Par.Delta_T[i]=Ultra_CALCU_Par.T_for[i]-Ultra_CALCU_Par.T_rev[i];
		Ultra_CALCU_Par.V_Filter[j]=2*Dev_Setting_Par.L*Ultra_CALCU_Par.Delta_T
		/cos(Dev_Setting_Par.angle)/(4.0f*Ultra_CALCU_Par.Average_T*Ultra_CALCU_Par.Average_T-Ultra_CALCU_Par.Delta_T*Ultra_CALCU_Par.Delta_T)*Dev_Setting_Par.L2/Dev_Setting_Par.L;
		if(isnan(Ultra_CALCU_Par.V_Filter[j]))
		{
			Ultra_CALCU_Par.V_Filter[j]=0;
		}
	}
	result=Flow_Man_Fun1(Ultra_CALCU_Par.V_Filter,channel,3);
	
	
	
}
void Ultra_test(void)
{
	TDC7200_EN_A=0;
	Ultra_TX1_A_DIS; //关闭多路复用器
	Ultra_RX1_A_DIS; //关闭多路复用器
	UCC_DIS_TX1_AT;  //UCC27537关闭
	Ultra_TX2_A_DIS;  
	Ultra_RX2_A_DIS;  
	UCC_DIS_TX2_AB;  
}
/*

----------------------------
|           |
|           |
|           h
|           |
h0          |   
|      ---------------------
|              
|      ---------------------
|           
|           
----------------------------
*/



/*
25cm
第一层0.2m/s
第二层0.3m/s
*/


//void flow_test(void)
//{
//	Water_Level_Par.hs=0.7-0.25;
//	Ultra_CALCU_Par.V[0]=0.2;
//	Ultra_CALCU_Par.V[1]=0.3;
//}
//void Flow_Process(void)
//{
//	static float flow[5];
//	static u8 ptr=0;
//	float result=0;
//	u8 i=0;
//	flow[ptr]=Ultra_CALCU_Par.Q_Total;  //
//	ptr++;
//	if(ptr>=5)
//	{
//		ptr=0;
//	}
//	for(i=0;i<5;i++)
//	{
//		result+=flow[i];
//	}
//	result=result/5;
//	Ultra_CALCU_Par.Q_Total2=result;
//}


u8 Get_Flow_data(u8 channelNUM)
{
	int8_t i;
	uint8_t N;
	float h_top;
	Ultra_CALCU_Par.h=RS485_Sensor.Fro_h;

//	flow_test();
	if(Ultra_CALCU_Par.hc==0)
	{
		return 0;
	}
	N=Ultra_CALCU_Par.h/Ultra_CALCU_Par.hc;  //读取到的最顶层的流量
	
	h_top=Ultra_CALCU_Par.h-Ultra_CALCU_Par.hc*N;
	if(N<1)
	{
		//b为底宽，闸后水位h，n为糙率，p为底坡=1/1000
		//Q=(b*h*((b*h/(b+2h))^(2/3))*p^0.5)/n
		Ultra_CALCU_Par.Q_Total=(Dev_Setting_Par.b*Ultra_CALCU_Par.h*pow(Dev_Setting_Par.b*Ultra_CALCU_Par.h/(Dev_Setting_Par.b+2*Ultra_CALCU_Par.h),2.0f/3)*pow(Dev_Setting_Par.p,0.5))/Dev_Setting_Par.n;
	}
	else
	{
		//最顶层速度
		Ultra_CALCU_Par.V_Top=(Ultra_CALCU_Par.V[N-1]-Ultra_CALCU_Par.V[N-2])*(h_top/Ultra_CALCU_Par.hc)+Ultra_CALCU_Par.V[N-1];
		//最底层速度
		Ultra_CALCU_Par.V_Low=Ultra_CALCU_Par.V[0]*Ultra_CALCU_Par.Side;
		//最底层流量
		Ultra_CALCU_Par.Q_Low=(Ultra_CALCU_Par.V_Low+Ultra_CALCU_Par.V[0])*Ultra_CALCU_Par.hc*Dev_Setting_Par.b/2;  
		//最顶层流量
		Ultra_CALCU_Par.Q_Top=(Ultra_CALCU_Par.V_Top+Ultra_CALCU_Par.V[N-1])*h_top*Dev_Setting_Par.b/2;
		Ultra_CALCU_Par.Q_Total=Ultra_CALCU_Par.Q_Low+Ultra_CALCU_Par.Q_Top;
		/*如果只测量了一层速度，即N=1则不会进入下面的循环*/
		for(i=0;i<N-1;i++)
		{
			Ultra_CALCU_Par.Q[i]=(Ultra_CALCU_Par.V[i]+Ultra_CALCU_Par.V[i+1])*Ultra_CALCU_Par.hc*Dev_Setting_Par.b/2;
			Ultra_CALCU_Par.Q_Total+=Ultra_CALCU_Par.Q[i];
		}
		Ultra_CALCU_Par.Q_Total=Ultra_CALCU_Par.Q_Total*Ultra_CALCU_Par.k;
	}
	//		if(Ultra_CALCU_Par.Q_Total<0)
//		{
//			Ultra_CALCU_Par.Q_Total=0;
//		}
		
	if(Ultra_CALCU_Par.Q_Total<dev_SavePar.flow_min)
	{
		Ultra_CALCU_Par.Q_Total=0;
	}

	if(isnan(Ultra_CALCU_Par.Q_Total))
	{
		Ultra_CALCU_Par.Q_Total=0;
	}
	if(isinf(Ultra_CALCU_Par.Q_Total))
	{
		Ultra_CALCU_Par.Q_Total=0;
	}
//	Flow_Process();
	return 0;
}

s8 Get_Flow_data2(u8 channelNUM)
{
	int8_t i;
	uint8_t N=0;
	float h_top;
	float ftemp=0;
	Ultra_CALCU_Par.h=RS485_Sensor.Aft_h;		//闸后水位

//	flow_test();
	if(Ultra_CALCU_Par.hc==0)		//探头间距
	{
		return 0;
	}
	/*
	5  4   5
	4  3   4
	3  2   3
	2  1   3
	1  0   1
	*/
	for(i=dev_SavePar.Ultra_Num;i>0;i--)  //dev_SavePar.Ultra_Num值为16-1
	{
		if(Ultra_CALCU_Par.Error_Flag[i-1]==0)
		{
			N=i;  //N读取到的最顶层的流量
			break;
		}
	}
	if(N>=2)	//N=3   Ultra_CALCU_Par.V[2]/Ultra_CALCU_Par.V[1] >2	//不明白什么意思
	{
		//最高一层流速是次高一层流速的2倍以上  则最高一层探头忽视
		if(Ultra_CALCU_Par.V[N-1]/Ultra_CALCU_Par.V[N-2]>2)		
		{
			N-=1;			//N=2
		}
		//否则还是N=3
	}
	else if(N==1)
	{
		//一层探头	流速>2m/s  则忽视
		if(Ultra_CALCU_Par.V[0]>2)
		{
			N=0;
		}
	}
//	N=Ultra_CALCU_Par.h/Ultra_CALCU_Par.hc;  //读取到的最顶层的流量
	
	//未淹没更高一层探头的高度=后水位高度-探头间距*水位淹没探头数量
	h_top=Ultra_CALCU_Par.h-Ultra_CALCU_Par.hc*N;	
	
	//水位未淹没第一层探头，但有水头>0.05m			根据后水位计算瞬时流量
	if(N==0)
	{
		//b为底宽，闸后水位h，n为糙率，p为底坡=1/1000
		//Q=(b*h*((b*h/(b+2h))^(2/3))*p^0.5)/n
		if(RS485_Sensor.Wat_head<0.05)		
		{
			Ultra_CALCU_Par.Q_Calc=0;
		}
		else		
		{
			Ultra_CALCU_Par.Q_Calc=(Dev_Setting_Par.b*Ultra_CALCU_Par.h*pow(Dev_Setting_Par.b*Ultra_CALCU_Par.h/(Dev_Setting_Par.b+2*Ultra_CALCU_Par.h),2.0f/3)*pow(Dev_Setting_Par.p,0.5))/Dev_Setting_Par.n;
		}
		
	}
	else if(N==1)
	{
		
		//最底层速度
		Ultra_CALCU_Par.V_Low=Ultra_CALCU_Par.V[0]*Ultra_CALCU_Par.Side;
		//最顶层速度
		Ultra_CALCU_Par.V_Top=(Ultra_CALCU_Par.V[0]-Ultra_CALCU_Par.V_Low)*(h_top/Ultra_CALCU_Par.hc)+Ultra_CALCU_Par.V[0];
		//最底层流量
		Ultra_CALCU_Par.Q_Low=(Ultra_CALCU_Par.V_Low+Ultra_CALCU_Par.V[0])*Ultra_CALCU_Par.hc*Dev_Setting_Par.b/2;  
		//最顶层流量
		Ultra_CALCU_Par.Q_Top=(Ultra_CALCU_Par.V_Top+Ultra_CALCU_Par.V[N-1])*h_top*Dev_Setting_Par.b/2;
		ftemp=Ultra_CALCU_Par.Q_Low+Ultra_CALCU_Par.Q_Top;
		/*如果只测量了一层速度，即N=1则不会进入下面的循环*/
		for(i=0;i<N-1;i++)
		{
			Ultra_CALCU_Par.Q[i]=(Ultra_CALCU_Par.V[i]+Ultra_CALCU_Par.V[i+1])*Ultra_CALCU_Par.hc*Dev_Setting_Par.b/2;
			ftemp+=Ultra_CALCU_Par.Q[i];
		}
		Ultra_CALCU_Par.Q_Calc=ftemp*Ultra_CALCU_Par.k;
	
	}
	else
	{
		//最顶层速度
		Ultra_CALCU_Par.V_Top=(Ultra_CALCU_Par.V[N-1]-Ultra_CALCU_Par.V[N-2])*(h_top/Ultra_CALCU_Par.hc)+Ultra_CALCU_Par.V[N-1];
		//最底层速度
		Ultra_CALCU_Par.V_Low=Ultra_CALCU_Par.V[0]*Ultra_CALCU_Par.Side;
		//最底层流量
		Ultra_CALCU_Par.Q_Low=(Ultra_CALCU_Par.V_Low+Ultra_CALCU_Par.V[0])*Ultra_CALCU_Par.hc*Dev_Setting_Par.b/2;  
		//最顶层流量
		Ultra_CALCU_Par.Q_Top=(Ultra_CALCU_Par.V_Top+Ultra_CALCU_Par.V[N-1])*h_top*Dev_Setting_Par.b/2;
		ftemp=Ultra_CALCU_Par.Q_Low+Ultra_CALCU_Par.Q_Top;
		/*如果只测量了一层速度，即N=1则不会进入下面的循环*/
		for(i=0;i<N-1;i++)
		{
			Ultra_CALCU_Par.Q[i]=(Ultra_CALCU_Par.V[i]+Ultra_CALCU_Par.V[i+1])*Ultra_CALCU_Par.hc*Dev_Setting_Par.b/2;
			ftemp+=Ultra_CALCU_Par.Q[i];
		}
		Ultra_CALCU_Par.Q_Calc=ftemp*Ultra_CALCU_Par.k;
	}

	
	//检查Ultra_CALCU_Par.Q_Calc 
	if(Ultra_CALCU_Par.Q_Calc<dev_SavePar.flow_min)
	{
		Ultra_CALCU_Par.Q_Calc=0;
	}
	
	if(Ultra_CALCU_Par.Q_Calc>dev_SavePar.flow_max)
	{
		return 1;
	}
	else if(isnan(Ultra_CALCU_Par.Q_Calc))
	{
		return 1;
	}
	else if(isinf(Ultra_CALCU_Par.Q_Calc))
	{
		return 1;
	}
	else
	{
		Ultra_CALCU_Par.Q_Total=Ultra_CALCU_Par.Q_Calc;
		return 0;
	}
}
/*
参数准备
*/
void Ultra_ParPrePare(void)
{
	Dev_Setting_Par.b=dev_SavePar.water_inletWidth ;  //底宽
//	Dev_Setting_Par.angle=dev_SavePar.Channel_Angle;
	Dev_Setting_Par.angle=dev_SavePar.Channel_Angle*3.1415926/180; //声道角度
	Ultra_CALCU_Par.h=RS485_Sensor.Aft_h;           //闸后水位
	Ultra_CALCU_Par.hc=dev_SavePar.Ultra_Spacing;   //探头间距
	Ultra_CALCU_Par.k=dev_SavePar.Correct_K;          //修正系数
	Ultra_CALCU_Par.Side=dev_SavePar.Side_K;         //边壁系数
	Dev_Setting_Par.p=dev_SavePar.Gradient;          //底坡
	Dev_Setting_Par.n=dev_SavePar.Roughness;         //糙率
	
}
void Ultra_total(void)
{
	u8 i=0;
	
	//测流功能(0关闭)   					测流设备为测流槽(0)					读取标志位成立
	if((dev_SavePar.Flow_Fun == 0) || (dev_SavePar.TypeUltra != 0) || (nosave_par.Read_AftLF!=1))
	{
		return ;
	}
	
	Ultra_ParPrePare();					//测流参数赋值
	
	getAllChannelValue(dev_SavePar.Ultra_Num);		//采集计算V
	
	//重置闸门关闭  后水位,测流槽读取标志位
	if(PDin(11)==1)
	{
		nosave_par.Read_AftLF=0;
	}
	
	
//	Get_Flow_data(dev_SavePar.Ultra_Num);
	Get_Flow_data2(dev_SavePar.Ultra_Num);			//各种情况计算Ultra_CALCU_Par.Q_Total
	
	//开度绝对值 <=0.001m		闸门关闭Ultra_CALCU_Par.Q_Total置0
	if(fabs(nosave_par.openingm)<=0.001)		
	{
		Ultra_CALCU_Par.Q_Total=0;
	}
	
//	flow_Process();
	
	//检查Ultra_CALCU_Par.Q_Total是否为数值或无穷大		是则清零
	if(isnan(Ultra_CALCU_Par.Q_Total))
	{
		Ultra_CALCU_Par.Q_Total=0;
	}
	if(isinf(Ultra_CALCU_Par.Q_Total))
	{
		Ultra_CALCU_Par.Q_Total=0;
	}
	
	
	//Aver_FlowV平均流速计算
	//Ultra_CALCU_Par.h后水位的液位高度
	if(Ultra_CALCU_Par.h<=0)
	{
		Aver_FlowV=0;
	}
	else
	{
		Aver_FlowV=Ultra_CALCU_Par.Q_Total/Ultra_CALCU_Par.h;
	}
	nosave_par.flow_flag=1;		//未使用
}
//sensor_Read
void RS485_Level_Read(void)
{
	
	RS485_ReadLevel();
	
}










