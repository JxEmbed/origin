#include "DS3231.h"


#define DS3231_ADDRESS          0xD0
#define	DS3231_ADDRESS_Write	0xD0
#define	DS3231_ADDRESS_Read		0xD1

/* DS3231 Registers. Refer Sec 8.2 of application manual */
#define DS3231_SEC_REG        0x00    // 秒
#define DS3231_MIN_REG        0x01    //
#define DS3231_HOUR_REG       0x02
#define DS3231_WDAY_REG       0x03
#define DS3231_MDAY_REG       0x04
#define DS3231_MONTH_REG      0x05
#define DS3231_YEAR_REG       0x06



void DS3231_init(void)
{
#if(DS3231_SOFTWARE_ENABLE)
	DS3231_SoftWare_init();
#endif
}
void DS3231_SoftWare_init(void)
{
	 /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOA);
	gpio_init(GPIOA, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    /* connect I2C_SCL_PIN to I2C_SCL */
    /* connect I2C_SDA_PIN to I2C_SDA */
	rcu_periph_clock_enable(RCU_GPIOC);
	gpio_init(GPIOC, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
	
}
u8 BCD2HEX(u8 bcd)
{
	u8 result;
	result=(bcd>>4)*10+(bcd&0x0f);
	return result;
}

unsigned char HEX2BCD(unsigned char val)    //B码转换为BCD码
{
    unsigned char i,j,k;
    i=val/10;
    j=val%10;
    k=j+(i<<4);
    return k;
}

uint8_t IIC_DS3231_ByteWrite(uint8_t WriteAddr , uint8_t date)
{
	while(i2c_flag_get(I2CX, I2C_FLAG_I2CBSY)){}
	i2c_start_on_bus(I2CX);
	while((!i2c_flag_get(I2CX, I2C_FLAG_SBSEND))){}
	i2c_master_addressing(I2CX, DS3231_ADDRESS_Write, I2C_TRANSMITTER);	
	while((!i2c_flag_get(I2CX, I2C_FLAG_ADDSEND))){}
	i2c_flag_clear(I2CX, I2C_FLAG_ADDSEND); 
	while((!i2c_flag_get(I2CX, I2C_FLAG_TBE))){}
	i2c_data_transmit(I2CX, WriteAddr);	
	while((!i2c_flag_get(I2CX, I2C_FLAG_BTC))){}
	
		//
	i2c_data_transmit(I2CX, date);
 
	/* wait until BTC bit is set */
	while((!i2c_flag_get(I2CX, I2C_FLAG_BTC))){}
	i2c_stop_on_bus(I2CX);
            /* i2c master sends STOP signal successfully */
	while((I2C_CTL0(I2CX) & I2C_CTL0_STOP)){}
}

uint8_t IIC_DS3231_ByteRead(uint8_t ReadAddr,uint8_t* Receive)
{
	
}
uint8_t DS3231_setDate(uint8_t year,uint8_t mon,uint8_t day)
{
	uint8_t temp_H , temp_L;
	temp_L = year%10;
	temp_H = year/10;
	year = (temp_H << 4) + temp_L;
	if(IIC_DS3231_ByteWrite(DS3231_YEAR_REG,year)) //set year
	{
			
			return 1;
	}	
	temp_L = mon%10;
	temp_H = mon/10;
	mon = (temp_H << 4) + temp_L;	
	if(IIC_DS3231_ByteWrite(DS3231_MONTH_REG,mon)) //set mon
	{
		
		return 2;
	}
	temp_L = day%10;
	temp_H = day/10;
	day = (temp_H << 4) + temp_L;		
	if(IIC_DS3231_ByteWrite(DS3231_MDAY_REG,day)) //set day
	{

		return 3;
	}
	return 0;
}
uint8_t DS3231_setTime(uint8_t hour , uint8_t min , uint8_t sec)
{
	uint8_t temp_H , temp_L;
	temp_L = hour%10;
	temp_H = hour/10;
	hour = (temp_H << 4) + temp_L;
	if(IIC_DS3231_ByteWrite(DS3231_HOUR_REG,hour)) //set hour
		return 1;
	temp_L = min%10;
	temp_H = min/10;
	min = (temp_H << 4) + temp_L;
	if(IIC_DS3231_ByteWrite(DS3231_MIN_REG,min)) //SET min
		return 2;	
	temp_L = sec%10;
	temp_H = sec/10;
	sec = (temp_H << 4) + temp_L;	
	if(IIC_DS3231_ByteWrite(DS3231_SEC_REG,sec))		//SET sec
		return 3;
	return 0;
}

#if(DS3231_SOFTWARE_ENABLE)

void DS3231_software_init(void)
{
	//SCL
    rcu_periph_clock_enable(RCU_GPIOA);
	gpio_init(GPIOA, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
	//SDA
   rcu_periph_clock_enable(RCU_GPIOC);
	gpio_init(GPIOC, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
	
}

DateTimeDef Timer={0};

//uint8_t mid_flag;  居中标志,暂时没有使用

extern uint16_t sec;
//uint32_t DS3231_Now_Time=0;

const unsigned char rtc_address[7]={0x00,0x01,0x02,0x03,0x04,0x05,0x06};//秒分时日月周年 最低位读写位

/*设置SDA引脚为输入模式*/
void DS3231_SET_SDA_IN(void)
{
	gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
}
/*设置SDA引脚为输出模式*/
void DS3231_SET_SDA_OUT(void)
{  gpio_init(GPIOC, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
}
/*设置SCL引脚为输入模式*/
void DS3231_SET_SCL_IN(void)
{  gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
}
/*设置SCL引脚为输出模式*/
void DS3231_SET_SCL_OUT(void)
{   gpio_init(GPIOA, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_8);

}


//定义一个软件延时函数，用于对IIC 调速
static void I2C_delay(void)
{
volatile int i=8;
while(i--);
}

//使用软件模拟IIC //
//初始化
void IIC_init(void)
{
    DS3231_SET_SDA_OUT();        //设置SDA引脚为通用开漏输出模式
    DS3231_SET_SCL_OUT();        //设置SCL引脚为通用开漏输出模式
    IIC_SDA_H;      //SDA置高
    IIC_SCL_H;      //SCL置高
//    Read_RTC();    

}

//读取数据


//产生起始信号
void IIC_Start(void)
{
    DS3231_SET_SDA_OUT();
    IIC_SCL_H;//set SDA pin as output high
    I2C_delay();

    IIC_SDA_H;//set SCL pin as output high
    I2C_delay();

    IIC_SDA_L;//set SDA pin as output low
    I2C_delay();
    IIC_SCL_L;//set SCL pin as output low
    I2C_delay();
}

//产生停止信号
void IIC_Stop(void)
{
    DS3231_SET_SDA_OUT();
    IIC_SCL_L;//set SDA pin as output low
    I2C_delay();
    IIC_SDA_L;//set SCL pin as output high
    I2C_delay();
    IIC_SCL_H;//set SDA pin as output high
    I2C_delay();
    IIC_SDA_H;
    I2C_delay();
}

//等待响应信号
//0--接收应答成功 1-接收应答失败
unsigned char IIC_Wait_Ack(void)
{
   int ucErrTime = 0;

    IIC_SDA_H;//set SCL pin as output high
    IIC_SCL_H; //set sda as input with pull up
    
    DS3231_SET_SDA_IN();  //设置SDA引脚为浮空输入模式
    
    while(DS3231_SDA_R)
    {
                ucErrTime++;
                if(ucErrTime > 1024)
                {
                     DS3231_SET_SDA_OUT();      //设置SDA引脚为通用开漏输出模式
                        IIC_Stop();
                        return 1;
            }
    }
    DS3231_SET_SDA_OUT();   //设置SDA引脚为通用开漏输出模式
    IIC_SCL_L; //set SCL pin as output low
    return 0;
}

//产生ACK信号
void IIC_Ack(void)
{

    IIC_SCL_L;//set SCL pin as output low
   I2C_delay();
    IIC_SDA_L;//set SDA pin as output low
   I2C_delay();
    IIC_SCL_H;//set SDA pin as output high
   I2C_delay();
    IIC_SCL_L; //set SCL pin as output low
    I2C_delay();
}

//No ACK
void IIC_NAck(void)
{
    IIC_SCL_L; //set SCL pin as output low
    I2C_delay();
    IIC_SDA_H;//set SDA pin as output high
    I2C_delay();
    IIC_SCL_H;//set SCL pin as output high
   I2C_delay();//延时
    IIC_SCL_L;//set SCL pin as output low
    I2C_delay();
}

//IIC发送一个字节
void IIC_Send_Byte(unsigned char txd)
{
    unsigned char i;

    IIC_SCL_L; //set SCL pin as output low
    for(i = 0; i < 8; i++)
    {
            if((txd & 0x80)>>7)
            {
                IIC_SDA_H;//set SDA pin as output high
            }
            else
            {
                IIC_SDA_L;//set SDA pin as output low
            }
            txd <<= 1;
            I2C_delay();
            IIC_SCL_H;//set SCL pin as output high
            I2C_delay();
 
            IIC_SCL_L; //set SCL pin as output low
            I2C_delay();
    }
}

//读取一个字节
unsigned char IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, res = 0;
    IIC_SDA_H; 
    DS3231_SET_SDA_IN(); //set sda as input with pull up

    for(i = 0; i < 8; i++ )
    {
            IIC_SCL_L;//set SCL pin as output high
          // delay_1us();
                 I2C_delay();

            IIC_SCL_H;//set SCL pin as output high
          // delay_1us();
                  I2C_delay();
            res <<= 1;
            if(DS3231_SDA_R)
            {
              res ++;
            }
    }
   DS3231_SET_SDA_OUT();
    if(!ack)
      IIC_NAck();
    else
      IIC_Ack();
    return res;
}
//-------------------------------------------------------------------
void IIC_single_byte_write(unsigned char Waddr,unsigned char Data)
{
  //指定位置写入一个字节数据函数
    IIC_Start();//产生起始信号
    IIC_Send_Byte(0xd0);//写入设备地址（写）
    IIC_Wait_Ack();//等待设备的应答
    IIC_Send_Byte(Waddr);//写入要操作的单元地址。
    IIC_Wait_Ack();//等待设备的应答。
    IIC_Send_Byte(Data);
    IIC_Wait_Ack();
    IIC_Stop();//产生停止符号。
}
//-------------------------------------------------------------------
unsigned char IIC_single_byte_read(unsigned char Waddr)
{
     //从任意地址读取一个字节数据函数
    unsigned char Data;//定义一个缓冲寄存器。
    IIC_Start();//产生起始信号
    IIC_Send_Byte(0xd0);//写入设备地址（写）
    IIC_Wait_Ack();//等待设备的应答
    IIC_Send_Byte(Waddr);//写入要操作的单元地址。
    IIC_Wait_Ack();//等待设备的应答。
    IIC_Stop();//产生停止符号。

    IIC_Start();//产生起始信号
    IIC_Send_Byte(0xd1);//写入设备地址（写）
    IIC_Wait_Ack();//等待设备的应答
    Data=IIC_Read_Byte(0);//写入数据。
   // i2c_Delay();
    IIC_Stop();//产生停止符号。
    //-------------------返回读取的数据--------------------
    return Data;//返回读取的一个字节数据。 
}

/******************************************
名   称：Read_RTC(）；
功   能：循环读取当前时间；
参   数：i:计数；temp：时间缓存区；p：缓存区指针
rtc_address：时间地址存放缓存区

**********************************************/
void DS3231_GetRTC(void)
{
    unsigned char i;
    unsigned char temp[7]={0};
    const unsigned char *p;
	xSemaphoreTake(DS3231_SemSR,DS3231_xBlockTime);  //申请互斥信号量
      p=rtc_address;         //地址传递
    for(i=0;i<7;i++)            //分6次读取 秒分时日月年
    {
      temp[i]=IIC_single_byte_read(*p);
            
      p++;
    }
	g_rtc.secs=BCD2HEX(temp[0]);
	g_rtc.minutes=BCD2HEX(temp[1]);
	g_rtc.hours=BCD2HEX(temp[2]);
	g_rtc.weeks=BCD2HEX(temp[3]);
	g_rtc.days=BCD2HEX(temp[4]);
	g_rtc.months=BCD2HEX(temp[5]);
	g_rtc.years=BCD2HEX(temp[6]);
    xSemaphoreGive(DS3231_SemSR);       //帧尾
}

/****************************************************************
函数功能：依据服务器GPRS传来时间信息 设置DS3231时间
输入参数：服务器传来信息
返回参数：无
备    注：无
*****************************************************************/
//void Set_RTC(unsigned char *data)
//{
//    unsigned char i;
//    for(i=0;i<7;i++)
//    {  
//     data[i+7]=HEX2BCD(data[i+7]);
//    }
//    IIC_single_byte_write(0x0e,0X8c);
//    for(i=0;i<7;i++)        //6次写入 秒分时周日月年
//    {
//        IIC_single_byte_write(rtc_address[i],data[i+7]);
//    }
//    IIC_single_byte_write(0x0e,0x0c);
//}

void DS1302_Initial(void)
{
      
    IIC_SDA_H;//释放IIC总线的数据线。
    IIC_SCL_H;
    IIC_single_byte_write(0x0e,0x0c);
}

                
 /*******************************************
名   称：void DS3231_Set(uint8_t yea,uint8_t mon,uint8_t da,uint8_t we,uint8_t hou,uint8_t min,uint8_t sec)
功   能：设置DS3231的时间，
参   数：yea:年  mon:月   da:日   we:周   hou:时  min:分  se:秒
说   明：数据格式采用16进制，例如22时56分应写成 hou：0x16；min：0x38
**********************************************/
void DS3231_SetRTC(DateTimeDef dateTime)
{
    uint8_t temp=0;
	xSemaphoreTake(DS3231_SemSR,DS3231_xBlockTime);  //申请互斥信号量
    temp=HEX2BCD(dateTime.years);
     IIC_single_byte_write(DS3231_YEAR_REG,temp);

    temp=HEX2BCD(dateTime.months);
     IIC_single_byte_write(DS3231_MONTH_REG,temp);

    temp=HEX2BCD(dateTime.days);
     IIC_single_byte_write(DS3231_MDAY_REG,temp);
    
    temp=HEX2BCD(dateTime.weeks);
     IIC_single_byte_write(DS3231_WDAY_REG,temp);

    temp=HEX2BCD(dateTime.hours);
     IIC_single_byte_write(DS3231_HOUR_REG,temp);

    temp=HEX2BCD(dateTime.minutes);
     IIC_single_byte_write(DS3231_MIN_REG,temp);

    temp=HEX2BCD(dateTime.secs);
     IIC_single_byte_write(DS3231_SEC_REG,temp);
	 xSemaphoreGive(DS3231_SemSR);       //帧尾
	 nosave_par.time_OK=1;
}
#else

void i2c_init(void)
{
	gpio_config();
	i2c_config();
}
void gpio_config(void)
{
    /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOA);
	gpio_init(GPIOA, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    /* connect I2C_SCL_PIN to I2C_SCL */
    /* connect I2C_SDA_PIN to I2C_SDA */
   rcu_periph_clock_enable(RCU_GPIOC);
	gpio_init(GPIOC, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
	rcu_periph_clock_enable(RCU_AF);
	gpio_pin_remap1_config(GPIO_PCF5,GPIO_PCF5_I2C2_REMAP0,ENABLE);
}

/*!
    \brief      configure the I2CX interface
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_config(void)
{
    /* enable I2C clock */
    rcu_periph_clock_enable(RCU_I2C2);
    /* configure I2C clock */
    i2c_clock_config(I2CX, I2C_SPEED, I2C_DTCY_2);
    /* configure I2C address */
    i2c_mode_addr_config(I2CX, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2CX_SLAVE_ADDRESS7);
    /* enable I2CX */
    i2c_enable(I2CX);
    /* enable acknowledge */
    i2c_ack_config(I2CX, I2C_ACK_ENABLE);
}

/*!
    \brief      reset I2C bus
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_bus_reset(void)
{
    i2c_deinit(I2CX);
    /* configure SDA/SCL for GPIO */
    GPIO_BC(I2C_SCL_PORT) |= I2C_SCL_PIN;
    GPIO_BC(I2C_SDA_PORT) |= I2C_SDA_PIN;
    gpio_init(I2C_SCL_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, I2C_SCL_PIN);
    gpio_init(I2C_SDA_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, I2C_SDA_PIN);
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    GPIO_BOP(I2C_SCL_PORT) |= I2C_SCL_PIN;
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    GPIO_BOP(I2C_SDA_PORT) |= I2C_SDA_PIN;
    /* connect I2C_SCL_PIN to I2C_SCL */
    /* connect I2C_SDA_PIN to I2C_SDA */
    gpio_init(I2C_SCL_PORT, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, I2C_SCL_PIN);
    gpio_init(I2C_SDA_PORT, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, I2C_SDA_PIN);
    /* configure the I2CX interface */
    i2c_config();
}
//S(起始信号）通信的开始
void IIC_Start(void)
{
	while(i2c_flag_get(I2CX, I2C_FLAG_I2CBSY)){}
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2CX);
      /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2CX, I2C_FLAG_SBSEND));
		
		
}
//Slave Addr 7 Bit(设备寻址)
void IIC_SendSlaveAddr(void)
{
	/* send slave address to I2C bus */
    i2c_master_addressing(I2CX, 0x70, I2C_TRANSMITTER);//我这边的设备地址是0c70
    
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2CX, I2C_FLAG_ADDSEND));
    
    /* clear the ADDSEND bit */
    i2c_flag_clear(I2CX,I2C_FLAG_ADDSEND);

}
void IIC_Wait_Ask(void)
{
	    /* wait until the transmit data buffer is empty */
    while( SET != i2c_flag_get(I2CX, I2C_FLAG_TBE));
}
void IIC_SendRegAddr(void)
{
	    /* send the reg internal address to write to */
    i2c_data_transmit(I2CX, 0x1c);//这个地址是读写数据时都要一致
	
    /* wait until BTC bit is set */
    while(!i2c_flag_get(I2CX, I2C_FLAG_BTC));

}
void IIC_SendData(uint8_t writ_byte)
{
		writ_byte = writ_byte << 1;	
    i2c_data_transmit(I2CX, writ_byte);
	
        /* wait until BTC bit is set */
    while(!i2c_flag_get(I2CX, I2C_FLAG_BTC));

}
void IIC_SendByte(uint8_t SlaveAddr,uint8_t WriteAddr , uint8_t writ_byte)
{
//S
	    while(i2c_flag_get(I2CX, I2C_FLAG_I2CBSY));
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2CX);
      /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2CX, I2C_FLAG_SBSEND));
//	 Addr 7 Bit(设备寻址)
  /* send slave address to I2C bus */
    i2c_master_addressing(I2CX, SlaveAddr, I2C_TRANSMITTER);//我这边的设备地址是0c70
    
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2CX, I2C_FLAG_ADDSEND));
    
    /* clear the ADDSEND bit */
    i2c_flag_clear(I2CX,I2C_FLAG_ADDSEND);
//W(等待应答信号)
//A(应答信号)
    /* wait until the transmit data buffer is empty */
    while( SET != i2c_flag_get(I2CX, I2C_FLAG_TBE));
//Reg Addr 8 Bit(寄存器的8位地址)这个地址是读写数据时都要一致
    /* send the reg internal address to write to */
    i2c_data_transmit(I2CX, WriteAddr);//这个地址是读写数据时都要一致
	
    /* wait until BTC bit is set */
    while(!i2c_flag_get(I2CX, I2C_FLAG_BTC));
//W(等待应答信号)
//A(应答信号)
    /* wait until the transmit data buffer is empty */
    while( SET != i2c_flag_get(I2CX, I2C_FLAG_TBE));
//Data 8 Bit(II2SDA数据线上的8位数据）
		writ_byte = writ_byte << 1;	
    i2c_data_transmit(I2CX, writ_byte);
	
        /* wait until BTC bit is set */
    while(!i2c_flag_get(I2CX, I2C_FLAG_BTC));
//W(等待应答信号)
//A(应答信号)
    /* wait until the transmit data buffer is empty */
    while( SET != i2c_flag_get(I2CX, I2C_FLAG_TBE));
//P(停止信号)
    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2CX);
    /* wait until the stop condition is finished */
   while(I2C_CTL0(I2CX)&0x0200);

}

void IIC_ReadByte(uint8_t SlaveAddr,uint8_t ReadAddr,uint8_t* p_buffer)
{
//S(起始信号）通信的开始
	    while(i2c_flag_get(I2CX, I2C_FLAG_I2CBSY));
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2CX);
      /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2CX, I2C_FLAG_SBSEND));
//Slave Addr 7 Bit(设备寻址)
  /* send slave address to I2C bus */
    i2c_master_addressing(I2CX, SlaveAddr, I2C_TRANSMITTER);//我这边的设备地址是0c70
    
    /* wait until ADDSEND bit is set */
	//这里卡死了
//    while(!i2c_flag_get(I2CX, I2C_FLAG_ADDSEND));
    
    /* clear the ADDSEND bit */
    i2c_flag_clear(I2CX,I2C_FLAG_ADDSEND);
//W(等待应答信号)
//A(应答信号)
    /* wait until the transmit data buffer is empty */
//    while( SET != i2c_flag_get(I2CX, I2C_FLAG_TBE));
    /* enable I2CX*/
    i2c_enable(I2CX);
//Reg Addr 8 Bit(寄存器的8位地址)这个地址是读写数据时都要一致
    /* send the reg internal address to write to */
    i2c_data_transmit(I2CX, ReadAddr);//这个地址是读写数据时都要一致
	
    /* wait until BTC bit is set */
    while(!i2c_flag_get(I2CX, I2C_FLAG_BTC));
//S(起始信号)
/* wait until BTC bit is set */
    while(!i2c_flag_get(I2CX, I2C_FLAG_BTC));
    
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2CX);
    
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2CX, I2C_FLAG_SBSEND));
//Slave Addr 7 Bit(设备寻址)
  /* send slave address to I2C bus */
    i2c_master_addressing(I2CX, 0x70, I2C_RECEIVER);//我这边的设备地址是0c70
    
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2CX, I2C_FLAG_ADDSEND));
    
    /* clear the ADDSEND bit */
    i2c_flag_clear(I2CX,I2C_FLAG_ADDSEND);
//Data 8 Bit(II2SDA数据线上的8位数据）
    *p_buffer = i2c_data_receive(I2CX);
            
    /* point to the next location where the byte read will be saved */
     p_buffer++; 
//N(无应答信号)
   /* enable acknowledge */
    i2c_ack_config(I2CX,I2C_ACK_ENABLE);

    i2c_ackpos_config(I2CX,I2C_ACKPOS_CURRENT);
//P(停止信号)
    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2CX);
    /* wait until the stop condition is finished */
   while(I2C_CTL0(I2CX)&0x0200);

}
void DS3231_GetRTC(void)
{
	u8 tempu8;
	uint8_t i2c_buffer_read[7];
//	IIC_ReadByte(DS3231_ADDRESS_Read,DS3231_SEC_REG,g_rtc.secs);
	eeprom_buffer_read_timeout(i2c_buffer_read, DS3231_SEC_REG, 7);
	g_rtc.secs=BCD2HEX(i2c_buffer_read[0]);
	g_rtc.minutes=BCD2HEX(i2c_buffer_read[1]);
	g_rtc.hours=BCD2HEX(i2c_buffer_read[2]);
	g_rtc.weeks=BCD2HEX(i2c_buffer_read[3]);
	g_rtc.days=BCD2HEX(i2c_buffer_read[4]);
	g_rtc.months=BCD2HEX(i2c_buffer_read[5]);
	g_rtc.years=BCD2HEX(i2c_buffer_read[6]);
}
void DS3231_SetRTC(DateTimeDef dateTime)
{
	uint8_t i2c_buffer_write[7];
	DS3231_setDate(dateTime.years,dateTime.months,dateTime.days);
	DS3231_setTime(dateTime.hours,dateTime.minutes,dateTime.secs);
}

#endif



