#include "RS485.h"
#include "logdata.h"
u8 RS485_TxBuff[RS485_TX_SIZE];
u32 RS485_RxPtr;                 //接收指针
volatile u8 RS485_RxState;                //接收状态
u8 RS485_RxBuff[RS485_MAX_SIZE]; //接收数组
RS485_Struct RS485_Sensor;
void USART6_Config(uint32_t baudval)
{
    /* 初始化GPIO外设 */
    rcu_periph_clock_enable(RCU_GPIOE);
    /* TX管脚，PA9，复用推挽输出，速度50MHz */
    gpio_init(GPIOE, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    /* RX管脚，PA10，下拉输入，速度50MHz */
    gpio_init(GPIOE, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
	  //DE_EN485
	gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
	gpio_bit_reset(GPIOE, GPIO_PIN_9);
	  //EN_RS485
		gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
		gpio_bit_reset(GPIOE, GPIO_PIN_10);
    /* 初始化USART外设 */
    rcu_periph_clock_enable(RCU_UART6);  // 使能串口0时钟
		usart_deinit(UART6);
    usart_baudrate_set(UART6, baudval);  // 波特率115200
    usart_parity_config(UART6, USART_PM_NONE);  // 无校检
    usart_word_length_set(UART6, USART_WL_8BIT);  // 8位数据位
    usart_stop_bit_set(UART6, USART_STB_1BIT);  // 1位停止位
	  usart_hardware_flow_rts_config(UART6, USART_RTS_DISABLE); /* 禁用rts */
    
    usart_hardware_flow_cts_config(UART6, USART_CTS_DISABLE); /* 无硬件数据流控制 */

    usart_transmit_config(UART6, USART_TRANSMIT_ENABLE);  // 使能串口发送
    usart_receive_config(UART6, USART_RECEIVE_ENABLE);  // 使能串口接收
    usart_enable(UART6);  // 使能串口
		
	nvic_irq_enable(UART6_IRQn,0U, 0U);
	usart_interrupt_enable(UART6, USART_INT_RBNE);  /* 参数：读数据缓冲区非空中断和过载错误中断 */
	usart_interrupt_enable(UART6, USART_INT_IDLE);  /* 参数：读数据缓冲区非空中断和过载错误中断 */
}
void RS485_power_init(void)
{
	
	rcu_periph_clock_enable(RCU_GPIOC);
	//PG1  CHSEL_A
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
	PCout(5)=1;
}
void RS485_Other_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	
	  //DE_EN485
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
	gpio_bit_reset(GPIOA, GPIO_PIN_1);
	rcu_periph_clock_enable(RCU_GPIOC);
	  //EN_RS485
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
	gpio_bit_reset(GPIOC, GPIO_PIN_4);
}
void RS485_init(uint32_t baudval)
{
	RS485_power_init();
	USART1_Config(baudval);
	RS485_Other_init();
}
void RS485_SendBuff(u8 * buff,u32 len)
{
	u32 i;
	DE_EN485=1;
	for(i=0;i<len;i++)
	{
		//USART_FLAG_TC
		while (usart_flag_get(USART1, USART_FLAG_TBE)== RESET); /* 获取缓冲区是否为空 */
		usart_data_transmit(USART1, *buff); /* transmit */
		while (usart_flag_get(USART1, USART_FLAG_TC)== RESET); /* 获取缓冲区是否为空 */
		
		buff++;
	}
	DE_EN485=0;
}
void RS485_MakeTxBuff(u8 addr,u8 fun,u16 offset,u16 data)
{
	u16 crcTx;
	RS485_TxBuff[0]=addr;
	RS485_TxBuff[1]=fun;
	
	RS485_TxBuff[2]=(offset>>8);
	RS485_TxBuff[3]=(offset&0xff);
	RS485_TxBuff[4]=(data>>8);
	RS485_TxBuff[5]=(data&0xff);
	crcTx=crc(RS485_TxBuff,6);
	RS485_TxBuff[6]=crcTx>>8;
	RS485_TxBuff[7]=crcTx&0xff;
	
}
/*
01 06 02 00 00 05 48 71
*/
void RS485_WriteREG(void)
{
	RS485_MakeTxBuff(0x02,0x06,0x0200,0x0001);

	RS485_SendBuff(RS485_TxBuff,8);
}


s8 RS485_ReadKeepReg(u8 dev_addr,u16 reg_addr,u16 num)
{
	u8 i=0;
	s8 res; 

	if(dev_SavePar.TypeLevel==0)
	{
		RS485_MakeTxBuff(dev_addr,Code_ReadKeepReg,reg_addr,num);
	}
	else if(dev_SavePar.TypeLevel==1)
	{
		RS485_MakeTxBuff(dev_addr,Code_ReadInputReg,reg_addr,2);
	}
	
//	xSemaphoreTake(RS485_Mutex,RS485_xBlockTime);  //申请互斥信号量
	for(i=0;i<4;i++)
	{
		RS485_RxState=0;  //发送查询前，先复位接收状态
		RS485_RxPtr=0;    //发送查询前，先恢复接收指针
		RS485_SendBuff(RS485_TxBuff,8);
		xSemaphoreTake(RS485_semRx,RS485_xBlockTime);  //申请互斥信号量
		
		if(RS485_RxState==1) //接收状态
		{
			if(dev_SavePar.TypeLevel==0)
			{
				res=RS485_RxProcess(dev_addr,Code_ReadKeepReg,reg_addr,num);  //如果接收到数据，则进行数据解析
			}	
			else if(dev_SavePar.TypeLevel==1)
			{
				res=RS485_RxProcess(dev_addr,Code_ReadInputReg,reg_addr,2);  //如果接收到数据，则进行数据解析
			}
			RS485_RxState=0; //复位接收状态
			if(res==1)
			{
//					xSemaphoreGive(RS485_Mutex);
				return 1;
			}
			
		}
	}
	return -1;
		
	
}	
/*
2021年11月16日15:53:25
解析接收到的数据
01 03 02 00 00 B8 44
*/
s8 RS485_RxProcess(u8 addr,u8 fun,u16 offset,u16 dat)
{
	uint16_t crcCalc; //存储计算出来的crc结果
	uint16_t crcRx;   //接收到的crc
	uint16_t tempu16; //传感器数据
	u16 reg_addr;
	u8 tempu8[4]="";
	
	if(RS485_RxPtr<2)
	{
		return -1; //如果接收到的数据小于2，则退出
	}
	crcRx=(RS485_RxBuff[RS485_RxPtr-2]<<8)+(RS485_RxBuff[RS485_RxPtr-1]); //
	crcCalc=crc(RS485_RxBuff,RS485_RxPtr-2);  //存储crc结果
	if(crcCalc!=crcRx)  //如果crc校验不通过，则退出
	{
		return -2; 
	}
	if(addr==FRO_H_ADDR)		
	{
		if(fun==Code_ReadKeepReg)
		{
			if(offset==ULTRA_VALUEREG && dat==1)
			{
				tempu16=RS485_RxBuff[3];
				tempu16=(tempu16<<8)+RS485_RxBuff[4];   
				RS485_Sensor.Fro_hs=(float)tempu16/1000;
				return 1;
			}
		}
		else if(fun==Code_ReadInputReg)
		{
			if(offset==RADAR_VALUEREG && dat==2)
			{
//				tempu8[0]=0x00;
//				tempu8[1]=0x00;
//				tempu8[2]=0x30;
//				tempu8[3]=0x41;
				
				//RS485_RxBuff返回数据低16位在前
				tempu8[0]=RS485_RxBuff[4]; //2D
				tempu8[1]=RS485_RxBuff[3]; //1C
				tempu8[2]=RS485_RxBuff[6]; //4B
				tempu8[3]=RS485_RxBuff[5]; //3A   
				
				RS485_Sensor.Fro_hs=*(float *)tempu8;		//单位为m
		
				return 1;
			}
		}
	}
	if(addr==AFT_H_ADDR)
	{
		if(fun==Code_ReadKeepReg)
		{
			if(offset==ULTRA_VALUEREG&&dat==1)
			{
				tempu16=RS485_RxBuff[3];
				tempu16=(tempu16<<8)+RS485_RxBuff[4];   
				RS485_Sensor.Aft_hs=(float)tempu16/1000;
				return 1;
			}
		}
		else if(fun==Code_ReadInputReg)
		{
			if(offset==RADAR_VALUEREG && dat==2)
			{
				//RS485_RxBuff返回数据低字节在前
				tempu8[0]=RS485_RxBuff[4];
				tempu8[1]=RS485_RxBuff[3];
				tempu8[2]=RS485_RxBuff[6];
				tempu8[3]=RS485_RxBuff[5];
				
				RS485_Sensor.Aft_hs=*(float *)tempu8;
				return 1;
			}
		}
	}
	return -3;
}
#define h_Buff_Len 10
float Fro_h_Buff[h_Buff_Len];
u8    Fro_h_BuffPtr=0;
u8    Fro_h_first=0;

float Aft_h_Buff[h_Buff_Len];
u8    Aft_h_BuffPtr;
u8    Aft_h_first=0;
/*
取十个数，去掉3最大去掉3最小，剩下4个求平均
*/
float Fro_h_Process(float f_re_in)
{
	float result;
	float tempbuff[h_Buff_Len];
	float tempf;
	u8 i=0,j=0;
	if(Fro_h_first==0)
	{
		Fro_h_first=1;
		for(i=0;i<h_Buff_Len;i++)
		{
			Fro_h_Buff[i]=f_re_in;
		}
	}
	Fro_h_Buff[Fro_h_BuffPtr++]=f_re_in;
	if(Fro_h_BuffPtr>h_Buff_Len)
	{
		Fro_h_BuffPtr=0;
	}
	for(i=0;i<h_Buff_Len;i++)
	{
		tempbuff[i]=Fro_h_Buff[i];
	}
	for(i=0;i<(h_Buff_Len-1);i++)
	{
		for(j=(i+1);j<h_Buff_Len;j++)
		{
			if(tempbuff[i]<tempbuff[j])
			{
				tempf=tempbuff[i];
				tempbuff[i]=tempbuff[j];
				tempbuff[j]=tempf;
			}
		}
	}
	result=0;
	//0,1,2,3,4,5,6,7,8,9
	for(i=3;i<(3+4);i++)
	{
		result+=tempbuff[i];
	}
	result/=4;
	return result;
}
float Aft_h_Process(float f_re_in)
{
	float result;
	float tempbuff[h_Buff_Len];
	float tempf;
	u8 i=0,j=0;
	if(Aft_h_first==0)
	{
		Aft_h_first=1;
		for(i=0;i<h_Buff_Len;i++)
		{
			Aft_h_Buff[i]=f_re_in;
		}
	}
	Aft_h_Buff[Aft_h_BuffPtr++]=f_re_in;
	if(Aft_h_BuffPtr>h_Buff_Len)
	{
		Aft_h_BuffPtr=0;
	}
	for(i=0;i<h_Buff_Len;i++)
	{
		tempbuff[i]=Aft_h_Buff[i];
	}
	for(i=0;i<(h_Buff_Len-1);i++)
	{
		for(j=(i+1);j<h_Buff_Len;j++)
		{
			if(tempbuff[i]<tempbuff[j])
			{
				tempf=tempbuff[i];
				tempbuff[i]=tempbuff[j];
				tempbuff[j]=tempf;
			}
		}
	}
	result=0;
	//0,1,2,3,4,5,6,7,8,9
	for(i=3;i<(3+4);i++)
	{
		result+=tempbuff[i];
	}
	result/=4;
	return result;
}

s8 RS485_QueryVersion(void)
{
	//56 00 11 00
	RS485_TxBuff[0]=0x56;
	RS485_TxBuff[1]=0x00;
	RS485_TxBuff[2]=0x11;
	RS485_TxBuff[3]=0x00;
	RS485_RxState=0;  //发送查询前，先复位接收状态
	RS485_RxPtr=0;    //发送查询前，先恢复接收指针
	RS485_SendBuff(RS485_TxBuff,4);
	//portMAX_DELAY
	xSemaphoreTake(RS485_semRx,RS485_xBlockTime);  //申请互斥信号量
//	xSemaphoreTake(RS485_semRx,portMAX_DELAY);  //申请互斥信号量
	if(RS485_RxState==1)
	{
		//返回: : 76 00 11 00 0B 50 54 43 32 4D 30 20 31 2E	30 30
		if(RS485_RxBuff[0]!=0x76)
		{
			return -1;
		}
		if(RS485_RxBuff[1]!=0x00)
		{
			return -1;
		}
		if(RS485_RxBuff[2]!=0x11)
		{
			return -1;
		}
		if(RS485_RxBuff[3]!=0x00)
		{
			return -1;
		}
		
		return 1;
	}
	else
	{
		return -1;
	}
}
//	发送: : 56 00 36 01 00 拍照成功返回: : 76 00 36 00 00
s8 RS485_StartPhoto(void)
{

	RS485_TxBuff[0]=0x56;
	RS485_TxBuff[1]=0x00;
	RS485_TxBuff[2]=0x36;
	RS485_TxBuff[3]=0x01;
	RS485_TxBuff[4]=0x00;
	RS485_RxState=0;  //发送查询前，先复位接收状态
	RS485_RxPtr=0;    //发送查询前，先恢复接收指针
	RS485_SendBuff(RS485_TxBuff,5);
	xSemaphoreTake(RS485_semRx,RS485_xBlockTime);  //申请互斥信号量
	if(RS485_RxState==1)
	{
		// 76 00 36 00 00
		if(RS485_RxBuff[0]!=0x76)
		{
			return -1;
		}
		if(RS485_RxBuff[1]!=0x00)
		{
			return -1;
		}
		if(RS485_RxBuff[2]!=0x36)
		{
			return -1;
		}
		if(RS485_RxBuff[3]!=0x00)
		{
			return -1;
		}
		if(RS485_RxBuff[4]!=0x00)
		{
			return -1;
		}
		return 1;
	}
	else
	{
		return -1;
	}
	
}
/*
56 00 34 01 00 
76 00 34 00 04 XX XX XX XX
76 00 34 00 04 00 00 13 DB
读取所拍图片长度指令
*/
uint32_t ReadImglen(void)
{
	uint32_t result=0;
	//56 00 34 01 00 
	RS485_TxBuff[0]=0x56;
	RS485_TxBuff[1]=0x00;
	RS485_TxBuff[2]=0x34;
	RS485_TxBuff[3]=0x01;
	RS485_TxBuff[4]=0x00;
	RS485_RxState=0;  //发送查询前，先复位接收状态
	RS485_RxPtr=0;    //发送查询前，先恢复接收指针
	RS485_SendBuff(RS485_TxBuff,5);
	
	xSemaphoreTake(RS485_semRx,RS485_xBlockTime);  //申请互斥信号量
	if(RS485_RxState==1)
	{
		/*
		56 00 34 01 00 
		76 00 34 00 04 XX XX XX XX
		76 00 34 00 04 00 00 13 DB
		*/
		//76 00 34 00 04 XX XX XX XX
		if(RS485_RxBuff[0]!=0x76)
		{
			return 0;
		}
		if(RS485_RxBuff[1]!=0x00)
		{
			return 0;
		}
		if(RS485_RxBuff[2]!=0x34)
		{
			return 0;
		}
		if(RS485_RxBuff[3]!=0x00)
		{
			return 0;
		}
		if(RS485_RxBuff[4]!=0x04)
		{
			return 0;
		}
		if(RS485_RxPtr<9)
		{
			return 0;
		}
		result=RS485_RxBuff[5];
		result=(result<<8)+RS485_RxBuff[6];
		result=(result<<8)+RS485_RxBuff[7];
		result=(result<<8)+RS485_RxBuff[8];
		return result;
	}
	else
	{
		return 0;
	}
}
/*
56 00 32 0C 00 0A 00 00 00 00 00 00 2B 6E 00 FF 
*/
s8 ReadImg(uint32_t addr,uint32_t len)
{
	//56 00 32 0C 00 0A 00 00 00 00 00 00 2B 6E 00 FF 
	RS485_TxBuff[0]=0x56;
	RS485_TxBuff[1]=0x00;
	RS485_TxBuff[2]=0x32;
	RS485_TxBuff[3]=0x0C;
	RS485_TxBuff[4]=0x00;
	RS485_TxBuff[5]=0x0A;
	RS485_TxBuff[6]=(addr>>24);
	RS485_TxBuff[7]=(addr>>16)&0xff;
	RS485_TxBuff[8]=(addr>>8)&0xff;
	RS485_TxBuff[9]=addr&0xff;
	RS485_TxBuff[10]=(len>>24)&0xff;
	RS485_TxBuff[11]=(len>>16)&0xff;
	RS485_TxBuff[12]=(len>>8)&0xff;
	RS485_TxBuff[13]=len&0xff;
	RS485_TxBuff[14]=0x00;
	RS485_TxBuff[15]=0xff;
	RS485_RxState=0;  //发送查询前，先复位接收状态
	RS485_RxPtr=0;    //发送查询前，先恢复接收指针
	RS485_SendBuff(RS485_TxBuff,16);
	xSemaphoreTake(RS485_semRx,RS485_xBlockTime);  //申请互斥信号量
	if(RS485_RxState==1)
	{
		//返回: : 76 00 32 00 00 FF D8 。。。。。。 FF D9 76 00 32 00 00
		//0,1,2,3,4,5,6,7   ---8
		if(RS485_RxBuff[0]!=0x76)
		{
			return -1;
		}
		if(RS485_RxBuff[1]!=0x00)
		{
			return -1;
		}
		if(RS485_RxBuff[2]!=0x32)
		{
			return -1;
		}
		if(RS485_RxBuff[3]!=0x00)
		{
			return -1;
		}
		if(RS485_RxBuff[4]!=0x00)
		{
			return -1;
		}
		
		imgTrans(imgData,&RS485_RxBuff[5],RS485_RxPtr-10);
		
	}
}
s8 RS485_Resume()
{
	//56 00 36 01 02
	RS485_TxBuff[0]=0x56;
	RS485_TxBuff[1]=0x00;
	RS485_TxBuff[2]=0x36;
	RS485_TxBuff[3]=0x01;
	RS485_TxBuff[4]=0x02;
	RS485_RxState=0;  //发送查询前，先复位接收状态
	RS485_RxPtr=0;    //发送查询前，先恢复接收指针
	RS485_SendBuff(RS485_TxBuff,5);
	xSemaphoreTake(RS485_semRx,RS485_xBlockTime);  //申请互斥信号量
	if(RS485_RxState==1)
	{
		// 76 00 36 00 00
		if(RS485_RxBuff[0]!=0x76)
		{
			return -1;
		}
		if(RS485_RxBuff[1]!=0x00)
		{
			return -1;
		}
		if(RS485_RxBuff[2]!=0x36)
		{
			return -1;
		}
		if(RS485_RxBuff[3]!=0x00)
		{
			return -1;
		}
		if(RS485_RxBuff[4]!=0x00)
		{
			return -1;
		}
		return 1;
	}
	else
	{
		return -1;
	}
}
void RS485_TakePhoto(void)
{
	uint16_t imgRxLen=0;
	uint16_t i=0;
	uint16_t N=0;
	RS485_init(115200);  //01 03 01 00 00 01 85 F6
	delay_ms(500);
	RS485_StartPhoto();
	delay_ms(500);
	imgRxLen=ReadImglen();
	delay_ms(500);
	N=imgRxLen/512;
	for(i=0;i<N;i++)
	{
		ReadImg(i*512,512);
		delay_ms(1000);
		SendImgData2(imgData,512*2,i,50);
//		SendImgData(onenet_info.pro_id, onenet_info.dev_name,i,imgData,imgData_Len,50);
	}
	if(imgRxLen%512)
	{
		ReadImg(N*512-1,imgRxLen-N*512);
		SendImgData2(imgData,(imgRxLen-N*512)*2,99,50);
//		SendImgData(onenet_info.pro_id, onenet_info.dev_name,i,imgData,imgData_Len,50);
	}
	RS485_Resume();
	RS485_init(9600);  //01 03 01 00 00 01 85 F6
	
}
/*
读取水位
主机： 01 03 01 00 00 01 85 F6
从机： 01 03 02 02 F2 38 A1
*/

void RS485_ReadLevel(void)
{
	//打开水位功能
	if(dev_SavePar.Level_Fun == 0)
		return ;
	
	s8 state;
	
	if(dev_SavePar.TypeLevel==0)
	{
		RS485_Sensor.Addr_ValueReg=ULTRA_VALUEREG;
	}
	else if(dev_SavePar.TypeLevel==1)
	{
		RS485_Sensor.Addr_ValueReg=RADAR_VALUEREG;
	}
	
	//前水位实时读取
	if(RS485_ReadKeepReg(FRO_H_ADDR,RS485_Sensor.Addr_ValueReg,1)==1)	//读取前水位空高
	{
		if(nosave_par.Auto_airh==1)
		{
			dev_SavePar.FroHeight_Air=RS485_Sensor.Fro_hs;
			if(dev_SavePar.TypeUltra == 1 )
			{
				Flash_SaveDevData();
				nosave_par.Auto_airh=0;
			}
		}
		
		RS485_Sensor.Fro_state=1;
		RS485_Sensor.Fro_h=dev_SavePar.FroHeight_Air-RS485_Sensor.Fro_hs;
		if(RS485_Sensor.Fro_h<0)
		{
			RS485_Sensor.Fro_h=0;
		}
		Statlog_Par.fro_level=1;
	}
	else
	{
		RS485_Sensor.Fro_state=0;
		Statlog_Par.fro_level=2;
	}
	
	//后水位闸门开 实时读取 		闸门关  间隔读取
	if(dev_SavePar.TypeUltra == 1)		//绑定巴歇槽  以其前水位作为后水位
	{
		nosave_par.Read_AftLF=0;
	}
	else 
	{
		if(nosave_par.Read_AftLF==1)  
		{
			if(RS485_ReadKeepReg(AFT_H_ADDR,RS485_Sensor.Addr_ValueReg,1)==1)	//读取后水位空高	
			{
				if(nosave_par.Auto_airh==1)
				{
					dev_SavePar.AftHeight_Air=RS485_Sensor.Aft_hs;
					Flash_SaveDevData();
					nosave_par.Auto_airh=0;
				}
				RS485_Sensor.Aft_state=1;
				RS485_Sensor.Aft_h=dev_SavePar.AftHeight_Air-RS485_Sensor.Aft_hs;		//液位=安装高度-水位空高
				if(RS485_Sensor.Aft_h<0)
				{
					RS485_Sensor.Aft_h=0;
				}
				Statlog_Par.aft_level=1;
			}
			else
			{
				RS485_Sensor.Aft_state=0;
				Statlog_Par.aft_level=2;
			}
		}
	}
	
}
//读取实时值
void RS485_ReadRealLevel(void)
{
	s8 state;
	
	if(RS485_ReadKeepReg(FRO_H_ADDR,RS485_Sensor.Addr_ValueReg,1)==1)
	{
		
		RS485_Sensor.Fro_state=1;
		RS485_Sensor.Fro_h=dev_SavePar.FroHeight_Air-RS485_Sensor.Fro_hs;
		if(RS485_Sensor.Fro_h<0)
		{
			RS485_Sensor.Fro_h=0;
		}
		Statlog_Par.fro_level=1;
	}
	else
	{
		RS485_Sensor.Fro_state=0;
		Statlog_Par.fro_level=2;
	}
	if(RS485_ReadKeepReg(AFT_H_ADDR,RS485_Sensor.Addr_ValueReg,1)==1)
	{
		RS485_Sensor.Aft_state=1;
		RS485_Sensor.Aft_h=dev_SavePar.AftHeight_Air-RS485_Sensor.Aft_hs;
		if(RS485_Sensor.Aft_h<0)
		{
			RS485_Sensor.Aft_h=0;
		}
		Statlog_Par.aft_level=1;
	}
	else
	{
		RS485_Sensor.Aft_state=0;
		Statlog_Par.aft_level=2;
	}
	Calc_Wat_head();
}
void Calc_Wat_head(void)
{
	if(RS485_Sensor.Aft_h<0.05||RS485_Sensor.Fro_h<0.05)
	{
		RS485_Sensor.Wat_head=0;
	}
	else
	{
		//水头 = 	闸前水深		+ 	地板高差		-	闸后水位
		RS485_Sensor.Wat_head=RS485_Sensor.Fro_h+dev_SavePar.base_heightDiff-RS485_Sensor.Aft_h;
	}
}


/*
6+4+4+4+4+8+8+8+1+1
48bype




512Mbyte=
512*1000Kbyte=
512*1000*1000byte


10666666
*/

	

