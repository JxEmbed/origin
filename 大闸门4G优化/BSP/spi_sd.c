#include "spi_sd.h"
#include "string.h"
u8  SD_Type=0;//SD卡的类型 
u8 SD_BUF[512];	 		//一页大小
u32 SD_SectorNum=0;
spi_parameter_struct spi_struct;
void SD_init(void)
{
	u8 err;
	u8 res;
	u8 timeout=0;
	err=SD_Initialize();
	 while(err!=0)					//检测SD卡
	{
		timeout++;
		if(timeout>5)
		{
			Statlog_Par.SDState=1;
			break;
		}
		delay_ms(200);
		err=SD_Initialize();
	}
	if(err==0xff)
	{
		SD_State.SD_exist=0;
		Statlog_Par.SDState=SD_NO_EXIST;
	}
	if(err==0)
	{
		SD_State.SD_exist=1;
		SD_Type=0x04;
		SD_SectorNum=SD_GetSectorCount();
	}
	
}

extern 	u8 spi0_send_array;
extern 	u8 spi0_receive_array;
u32 testb=0;
u8 SD_SPI_ReadWriteByte(u8 ByteSend)
{
//	uint8_t i, ByteReceive = 0x00;
//	while(spi_i2s_flag_get(SPI0, SPI_FLAG_TBE) == RESET){ // 等待SPI发送缓冲器为空
//	;
//	}
//	spi_i2s_data_transmit(SPI0, ByteSend);            // 把数据放到发生缓冲器
////	while(spi_i2s_flag_get(SPI0, SPI_FLAG_TRANS) == SET){ // 等待通信结束
////	;
////	}

//	while(spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE) == RESET){ // 等待SPI接收缓冲器非空
//	;
//	}
//	return spi_i2s_data_receive(SPI0); /* 把接收到的数据返回（从接收缓冲器里拿出） */
//	
	/******************************************************/
//	spi_dma_disable(SPI0, SPI_DMA_RECEIVE);
//	spi_dma_disable(SPI0, SPI_DMA_TRANSMIT);
	dma_channel_disable(DMA0, DMA_CH2);
//	dma_flag_clear(DMA0, DMA_CH2, DMA_FLAG_FTF);
	dma_channel_disable(DMA0, DMA_CH1);
//	dma_flag_clear(DMA0, DMA_CH1, DMA_FLAG_FTF);
	spi0_send_array=ByteSend;
	dma_memory_address_config(DMA0,DMA_CH2,(u32)(&spi0_send_array));
	dma_memory_address_config(DMA0,DMA_CH1,(u32)(&spi0_receive_array));
	dma_transfer_number_config(DMA0,DMA_CH2,1);
	dma_transfer_number_config(DMA0,DMA_CH1,1);
//	spi_dma_A();

	spi_dma_enable(SPI0, SPI_DMA_RECEIVE);
	spi_dma_enable(SPI0, SPI_DMA_TRANSMIT);
	
	dma_channel_enable(DMA0, DMA_CH1);

	
	dma_channel_enable(DMA0, DMA_CH2);
	

	

	

//	spi_nss_output_enable(SPI0);
	while(!dma_flag_get(DMA0, DMA_CH2, DMA_FLAG_FTF)){
		testb++;
	}
	dma_flag_clear(DMA0, DMA_CH2, DMA_FLAG_FTF);

//	while(spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE) == RESET){ // 等待SPI接收缓冲器非空
//	;
//	}
	while(!dma_flag_get(DMA0, DMA_CH1, DMA_FLAG_FTF)){
		testb++;
    }
	dma_flag_clear(DMA0, DMA_CH1, DMA_FLAG_FTF);
	
	return spi0_receive_array;
}
void SPI0_SetSpeed(uint32_t SpeedSet)
{
	spi_struct.prescale = SpeedSet;                          /*!< SPI prescaler factor 8分频*/
	spi_init(SPI0, &spi_struct);
	spi_enable(SPI0);
} 
//SD卡初始化的时候,需要低速
void SD_SPI_SpeedLow(void)
{
	SPI0_SetSpeed(SPI_PSC_256);//设置到低速模式	
}
//SD卡正常工作的时候,可以高速了
void SD_SPI_SpeedHigh(void)
{
	SPI0_SetSpeed(SPI_PSC_4);//设置到高速模式	
}
//SD_SPI硬件层初始化
void SD_SPI_Init(void)
{
	//时钟初始化
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_AF);    //使能AF时钟
	rcu_periph_clock_enable(RCU_SPI0);
	//GPIO初始化
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);       //CS
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);        //CLK
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);        //MISO
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING,GPIO_OSPEED_50MHZ, GPIO_PIN_6);   //MOSI
	SD_CS=1;
	//SPI初始化
	spi_i2s_deinit(SPI0);
//	/* configure I2S1 */
//    i2s_init(SPI0, I2S_MODE_MASTERTX, I2S_STD_PHILLIPS, I2S_CKPL_LOW);
//    i2s_psc_config(SPI0, I2S_AUDIOSAMPLE_44K, I2S_FRAMEFORMAT_DT16B_CH16B, I2S_MCKOUT_DISABLE);
	spi0_init();
	spi_enable(SPI0);
}
// SPI0初始化
void spi0_init(void)
{
	spi_struct.device_mode = SPI_MASTER;                    /*!< SPI master  做主机*/
	spi_struct.trans_mode = SPI_TRANSMODE_FULLDUPLEX;         /*!< SPI transfer type 全双工 */
	spi_struct.frame_size =  SPI_FRAMESIZE_8BIT;              /*!< SPI frame size  一次8字节 */
	spi_struct.nss = SPI_NSS_SOFT;                            /*!< SPI NSS control by software 软件CS */
	spi_struct.endian = SPI_ENDIAN_MSB;                       /*!< SPI big endian or little endian  传输高字节在前*/
	spi_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE; /*!< SPI clock phase and polarity 空闲低电平 第一个边沿进行采样*/
	spi_struct.prescale = SPI_PSC_256;                          /*!< SPI prescaler factor 8分频*/
	
	spi_init(SPI0, &spi_struct);
}
///////////////////////////////////////////////////////////////////////////////////
//取消选择,释放SPI总线
void SD_DisSelect(void)
{
	SD_CS=1;
 	SD_SPI_ReadWriteByte(0xff);//提供额外的8个时钟
}
//选择sd卡,并且等待卡准备OK
//返回值:0,成功;1,失败;
u8 SD_Select(void)
{
	SD_CS=0;
	if(SD_WaitReady()==0)return 0;//等待成功
	SD_DisSelect();
	return 1;//等待失败
}
//等待卡准备好
//返回值:0,准备好了;其他,错误代码
u8 SD_WaitReady(void)
{
	u32 t=0;
	do
	{
		if(SD_SPI_ReadWriteByte(0XFF)==0XFF)return 0;//OK
		t++;		  	
	}while(t<0XFFFF);//等待 
	return 1;
}
//等待SD卡回应
//Response:要得到的回应值
//返回值:0,成功得到了该回应值
//    其他,得到回应值失败
u8 SD_GetResponse(u8 Response)
{
	u16 Count=0xFFFF;//等待次数	   						  
	while ((SD_SPI_ReadWriteByte(0XFF)!=Response)&&Count)Count--;//等待得到准确的回应  	  
	if (Count==0)return MSD_RESPONSE_FAILURE;//得到回应失败   
	else return MSD_RESPONSE_NO_ERROR;//正确回应
}
//从sd卡读取一个数据包的内容
//buf:数据缓存区
//len:要读取的数据长度.
//返回值:0,成功;其他,失败;	
u8 SD_RecvData(u8*buf,u16 len)
{
	if(SD_GetResponse(0xFE))return 1;//等待SD卡发回数据起始令牌0xFE
    while(len--)//开始接收数据
    {
        *buf=SD_SPI_ReadWriteByte(0xFF);
        buf++;
    }
    //下面是2个伪CRC（dummy CRC）
    SD_SPI_ReadWriteByte(0xFF);
    SD_SPI_ReadWriteByte(0xFF);									  					    
    return 0;//读取成功
}
//向sd卡写入一个数据包的内容 512字节
//buf:数据缓存区
//cmd:指令
//返回值:0,成功;其他,失败;	
u8 SD_SendBlock(u8*buf,u8 cmd)
{
	u16 t;		  	  
	if(SD_WaitReady())
		return 1;				//等待准备失效
	SD_SPI_ReadWriteByte(cmd);
	if(cmd!=0XFD)				//不是结束指令
	{
		for(t=0;t<512;t++)
			SD_SPI_ReadWriteByte(buf[t]);//提高速度,减少函数传参时间
		
	    SD_SPI_ReadWriteByte(0xFF);//忽略crc
	    SD_SPI_ReadWriteByte(0xFF);
		t=SD_SPI_ReadWriteByte(0xFF);//接收响应
		if((t&0x1F)!=0x05)return 2;//响应错误									  					    
	}						 									  					    
    return 0;//写入成功
}

//向SD卡发送一个命令
//输入: u8 cmd   命令 
//      u32 arg  命令参数
//      u8 crc   crc校验值	   
//返回值:SD卡返回的响应															  
u8 SD_SendCmd(u8 cmd, u32 arg, u8 crc)
{
    u8 r1;	
	u8 Retry=0; 
	SD_DisSelect();//取消上次片选
	if(SD_Select())return 0XFF;//片选失效 
	//发送
    SD_SPI_ReadWriteByte(cmd | 0x40);//分别写入命令
    SD_SPI_ReadWriteByte(arg >> 24);
    SD_SPI_ReadWriteByte(arg >> 16);
    SD_SPI_ReadWriteByte(arg >> 8);
    SD_SPI_ReadWriteByte(arg);	  
    SD_SPI_ReadWriteByte(crc); 
	if(cmd==CMD12)SD_SPI_ReadWriteByte(0xff);//Skip a stuff byte when stop reading
    //等待响应，或超时退出
	Retry=0X1F;
	do
	{
		r1=SD_SPI_ReadWriteByte(0xFF);
//	}while((r1&0X80) && Retry--);	 
	}while((r1==0xff) && Retry--);
	//返回状态值
    return r1;
}		    																			  
//获取SD卡的CID信息，包括制造商信息
//输入: u8 *cid_data(存放CID的内存，至少16Byte）	  
//返回值:0：NO_ERR
//		 1：错误		

u8 SD_GetCID(u8 *cid_data)
{
    u8 r1;	   
    //发CMD10命令，读CID
    r1=SD_SendCmd(CMD10,0,0x01);
    if(r1==0x00)
	{
		r1=SD_RecvData(cid_data,16);//接收16个字节的数据	 
    }
	SD_DisSelect();//取消片选
	if(r1)return 1;
	else return 0;
}																				  
//获取SD卡的CSD信息，包括容量和速度信息
//输入:u8 *cid_data(存放CID的内存，至少16Byte）	    
//返回值:0：NO_ERR
//		 1：错误														   
u8 SD_GetCSD(u8 *csd_data)
{
    u8 r1;	 
    r1=SD_SendCmd(CMD9,0,0x01);//发CMD9命令，读CSD
    if(r1==0)
	{
    	r1=SD_RecvData(csd_data, 16);//接收16个字节的数据 
    }
	SD_DisSelect();//取消片选
	if(r1)return 1;
	else return 0;
}  
//获取SD卡的总扇区数（扇区数）   
//返回值:0： 取容量出错 
//       其他:SD卡的容量(扇区数/512字节)
//每扇区的字节数必为512，因为如果不是512，则初始化不能通过.														  
u32 SD_GetSectorCount(void)
{
    u8 csd[16];
    u32 Capacity;  
    u8 n;
	u16 csize;  					    
	//取CSD信息，如果期间出错，返回0
    if(SD_GetCSD(csd)!=0) return 0;	    
    //如果为SDHC卡，按照下面方式计算
    if((csd[0]&0xC0)==0x40)	 //V2.00的卡
    {	
		csize = csd[9] + ((u16)csd[8] << 8) + 1;
		Capacity = (u32)csize << 10;//得到扇区数	 		   
    }else//V1.XX的卡
    {	
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
		Capacity= (u32)csize << (n - 9);//得到扇区数   
    }
    return Capacity;
}
//初始化SD卡
u8 SD_Initialize(void)
{
    u8 r1;      // 存放SD卡的返回值
    u16 retry;  // 用来进行超时计数
    u8 buf[4];  
	u16 i;

	spi_dma_test();
 	SD_SPI_SpeedLow();	//设置到低速模式 
	
 	for(i=0;i<10;i++)SD_SPI_ReadWriteByte(0XFF);//发送最少74个脉冲
	retry=20;
	do
	{
		r1=SD_SendCmd(CMD0,0,0x95);//进入IDLE状态
	}while((r1!=0X01) && retry--);
 	SD_Type=0;//默认无卡
	if(r1==0X01)
	{
		if(SD_SendCmd(CMD8,0x1AA,0x87)==1)//SD V2.0
		{
			for(i=0;i<4;i++)buf[i]=SD_SPI_ReadWriteByte(0XFF);	//Get trailing return value of R7 resp
			if(buf[2]==0X01&&buf[3]==0XAA)//卡是否支持2.7~3.6V
			{
				retry=0XFFFE;
				do
				{
					SD_SendCmd(CMD55,0,0X01);	//发送CMD55
					r1=SD_SendCmd(CMD41,0x40000000,0X01);//发送CMD41
				}while(r1&&retry--);
				if(retry&&SD_SendCmd(CMD58,0,0X01)==0)//鉴别SD2.0卡版本开始
				{
					for(i=0;i<4;i++)buf[i]=SD_SPI_ReadWriteByte(0XFF);//得到OCR值
					if(buf[0]&0x40)SD_Type=SD_TYPE_V2HC;    //检查CCS
					else SD_Type=SD_TYPE_V2;   
				}
			}
		}else//SD V1.x/ MMC	V3
		{
			SD_SendCmd(CMD55,0,0X01);		//发送CMD55
			r1=SD_SendCmd(CMD41,0,0X01);	//发送CMD41
			if(r1<=1)
			{
				SD_Type=SD_TYPE_V1;
				retry=0XFFFE;
				do //等待退出IDLE模式
				{
					SD_SendCmd(CMD55,0,0X01);	//发送CMD55
					r1=SD_SendCmd(CMD41,0,0X01);//发送CMD41
				}while(r1&&retry--);
			}else//MMC卡不支持CMD55+CMD41识别
			{
				SD_Type=SD_TYPE_MMC;//MMC V3
				retry=0XFFFE;
				do //等待退出IDLE模式
				{											    
					r1=SD_SendCmd(CMD1,0,0X01);//发送CMD1
				}while(r1&&retry--);  
			}
			if(retry==0||SD_SendCmd(CMD16,512,0X01)!=0)
			{
				SD_Type=SD_TYPE_ERR;//错误的卡
			}
		}
	}
	SD_DisSelect();//取消片选
	SD_SPI_SpeedHigh();//高速
	if(SD_Type)
	{
		Statlog_Par.SDState=1;  //正常
		return 0;
	}
	
	else if(r1)
	{
		Statlog_Par.SDState=2;      //错误
		return r1; 	   
	}
	else
	{
		Statlog_Par.SDState=2;      //错误
		return 0xaa;//其他错误
	}
	
}
//读SD卡
//buf:数据缓存区
//sector:扇区
//cnt:扇区数
//返回值:0,ok;其他,失败.
u8 SD_ReadDisk(u8*buf,u32 sector,u8 cnt)
{
	u8 r1;
	if(SD_Type!=SD_TYPE_V2HC)sector <<= 9;//转换为字节地址
	if(cnt==1)
	{
		r1=SD_SendCmd(CMD17,sector,0X01);//读命令
		if(r1==0)//指令发送成功
		{
			r1=SD_RecvData(buf,512);//接收512个字节	   
		}
	}else
	{
		r1=SD_SendCmd(CMD18,sector,0X01);//连续读命令
		do
		{
			r1=SD_RecvData(buf,512);//接收512个字节	 
			buf+=512;  
		}while(--cnt && r1==0); 	
		SD_SendCmd(CMD12,0,0X01);	//发送停止命令
	}   
	SD_DisSelect();//取消片选
	return r1;//
}
//写SD卡
//buf:数据缓存区
//sector:起始扇区
//cnt:扇区数
//返回值:0,ok;其他,失败.
u8 SD_WriteDisk(u8*buf,u32 sector,u8 cnt)
{
	u8 r1;
	if(SD_Type!=SD_TYPE_V2HC)
		sector *= 512;//转换为字节地址
	if(cnt==1)
	{
		r1=SD_SendCmd(CMD24,sector,0X01);	//读命令
		if(r1==0)//指令发送成功
		{
			r1=SD_SendBlock(buf,0xFE);		//写512个字节	   
		}
	}
	else
	{
		if(SD_Type!=SD_TYPE_MMC)
		{
			SD_SendCmd(CMD55,0,0X01);	
			SD_SendCmd(CMD23,cnt,0X01);//发送指令	
		}
 		r1=SD_SendCmd(CMD25,sector,0X01);//连续读命令
		if(r1==0)
		{
			do
			{
				r1=SD_SendBlock(buf,0xFC);//接收512个字节	 
				buf+=512;  
			}while(--cnt && r1==0);
			r1=SD_SendBlock(0,0xFD);//接收512个字节 
		}
	}   
	SD_DisSelect();//取消片选
	return r1;//
}	

/*
SD卡写入
*/
void SD_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    
	  
	xSemaphoreTake(SD_SemSR,SD_xBlockTime);  //申请互斥信号量//帧头    
 	secpos=WriteAddr/512;				 //页地址  				512字节为一页
	secoff=WriteAddr%512;				//在页内的偏移
	secremain=512-secoff;				//页剩余空间大小   
 	if(NumByteToWrite<=secremain)		//写入字节 小于 页剩余空间大小
		secremain=NumByteToWrite;		//secremain=写入字节
	while(1)
	{
		SEGGER_RTT_SetTerminal(2); 
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_BLUE"secpos:%d\tsecoff:%d\tsecremain:%d\r\n",secpos,secoff,secremain);
		SEGGER_RTT_SetTerminal(0); 
		
		memset(SD_BUF,0,sizeof(SD_BUF));	
		SD_ReadDisk(SD_BUF,secpos,1);		//读取写入页
		SEGGER_RTT_SetTerminal(2); 
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_BLUE"readDisk:%s\r\n",SD_BUF);
		SEGGER_RTT_SetTerminal(0); 
		for(i=0;i<secremain;i++)	   		//写入数据加在已有数据后
		{
			SD_BUF[i+secoff]=pBuffer[i];	  
		}
		SEGGER_RTT_SetTerminal(2); 
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_BLUE"writeDisk:%s\r\n",SD_BUF);
		SEGGER_RTT_SetTerminal(0); 
		SD_WriteDisk(SD_BUF,secpos,1);		//将数据写入页

					   
		if(NumByteToWrite==secremain)
			break;	//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	pBuffer+=secremain;  				//指针偏移
			WriteAddr+=secremain;				//写地址偏移	   
		   	NumByteToWrite-=secremain;			//字节数递减
			if(NumByteToWrite>512)
				secremain=512;//下一个扇区还是写不完
			else 
				secremain=NumByteToWrite;		//下一个扇区可以写完了
		}	 
	};	
	xSemaphoreGive(SD_SemSR);       //帧尾
}

void SD_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{
	u32 secpos;
	u16 secoff;
	u16 secremain;	
	u16 i,j; 
	xSemaphoreTake(SD_SemSR,SD_xBlockTime);  //申请互斥信号量//帧头 
	secpos=ReadAddr/512;//扇区地址  
	secoff=ReadAddr%512;//在扇区内的偏移
	secremain=512-secoff;//扇区剩余空间大小 
	//如果剩余空间大于需要读取的长度，则剩余空间等于读取的长度
	if(NumByteToRead<=secremain)secremain=NumByteToRead;//不大于512个字节
	while(1)
	{
		SD_ReadDisk(SD_BUF,secpos,1);
		for(i=0;i<secremain;i++)	   		//复制
		{
			pBuffer[i]=SD_BUF[i+secoff];	  
		}
		if(NumByteToRead==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	pBuffer+=secremain;  				//指针偏移
			ReadAddr+=secremain;				//写地址偏移	   
		   	NumByteToRead-=secremain;			//字节数递减
			if(NumByteToRead>512)secremain=512;//下一个扇区还是写不完
			else secremain=NumByteToRead;		//下一个扇区可以写完了
		}	
	}
	xSemaphoreGive(SD_SemSR);       //帧尾
} 

//————————————————
//版权声明：本文为CSDN博主「BIN-XYB」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
//原文链接：https://blog.csdn.net/qq_30095023/article/details/131591790
void SD_test(void)
{
	u8 SD_BUF[512]={1};	
	u8 SD_BUF1[512]={2};
	u8 SD_BUF2[8];
	u8 SD_CID[16];
	u8 SD_CSD[16];
	u8 res=0;
	u16 i=0;
	u32 sector_number;
	
	for(i=0;i<512;i++)
	{
		SD_BUF[i]=i;
	}
	res=SD_GetCID(SD_CID);
	res=SD_GetCSD(SD_CSD);
	SD_WriteDisk(SD_BUF,0,1);//写入整个扇区  
	SD_ReadDisk(SD_BUF1,0,1);
	memset(SD_BUF2+8,0x33,8);
	SD_Write(SD_BUF2,0,sizeof(SD_BUF2));//写入整个扇区  
	SD_ReadDisk(SD_BUF1,0,1);
	sector_number=SD_GetSectorCount();
	sector_number=sector_number;
}

