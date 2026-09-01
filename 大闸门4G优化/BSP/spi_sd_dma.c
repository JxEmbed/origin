#include "spi_sd_dma.h"
#include "string.h"
#include "spi_sd.h"
/*
SD卡 使用SPI连接，DMA传输
*/


void gpio_config(void)
{
	rcu_periph_clock_enable(RCU_GPIOA); //  
	rcu_periph_clock_enable(RCU_AF);    //
	rcu_periph_clock_enable(RCU_SPI0);
	/* configure SPI0 GPIO: NSS/PA4, SCK/PA5, MISO/PA6, MOSI/PA7 */
//	gpio_pin_remap_config(GPIO_SPI0_REMAP, DISABLE);
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
	SD_CS=1;
}
	u8 spi0_send_array;
	u8 spi0_receive_array;

void dma_config(void)
{
	rcu_periph_clock_enable(RCU_DMA0);
	
	dma_parameter_struct dma_init_struct;
	//使能时钟
	rcu_periph_clock_enable(RCU_DMA0);
	
	dma_struct_para_init(&dma_init_struct);

	/* configure SPI0 transmit DMA: DMA0 DMA_CH2 */
	dma_deinit(DMA0, DMA_CH2);
	dma_init_struct.periph_addr  = (uint32_t)(&SPI_DATA(SPI0));		
	dma_init_struct.memory_addr  = (uint32_t)&spi0_send_array;
	dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;		//从spi0_send_array 搬到 SPI_DATA(SPI0)
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
	dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;			//高优先级
	dma_init_struct.number       = 1;								//一次传输多少个单元
	dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;		
	dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;		//内存地址是否自增
	dma_init(DMA0, DMA_CH2, &dma_init_struct);
	/* configure DMA mode */
	dma_circulation_disable(DMA0, DMA_CH2);							//关闭循环模式
	dma_memory_to_memory_disable(DMA0, DMA_CH2);					//关闭内存到内存

	/* configure SPI0 receive DMA: DMA0 DMA_CH1 */
	dma_deinit(DMA0, DMA_CH1);
	dma_init_struct.periph_addr  = (uint32_t)(&SPI_DATA(SPI0));
	dma_init_struct.memory_addr  = (uint32_t)&spi0_receive_array;
	dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;		//从SPI_DATA(SPI0) 搬到 spi0_receive_array
	dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
	dma_init(DMA0, DMA_CH1, &dma_init_struct);
	/* configure DMA mode */
	dma_circulation_disable(DMA0, DMA_CH1);
	dma_memory_to_memory_disable(DMA0, DMA_CH1);

	
}
u8 spi_dma_reset(u8 txbuf)
{
	u8 rxbuf;
	spi0_send_array=txbuf;
	dma_memory_address_config(DMA0,DMA_CH2,(u32)&txbuf);
	dma_memory_address_config(DMA0,DMA_CH1,(u32)&rxbuf);
	dma_transfer_number_config(DMA0,DMA_CH2,1);
	dma_transfer_number_config(DMA0,DMA_CH1,1);
	spi_dma_A();

rxbuf=spi0_receive_array;
	return rxbuf;
}
extern spi_parameter_struct spi_struct;
void spi_config(void)
{
	spi_parameter_struct spi_init_struct;
	/* deinitilize SPI and the parameters */
	spi_i2s_deinit(SPI0);
	spi_struct_para_init(&spi_struct);
	spi_struct.device_mode          = SPI_MASTER;
	spi_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX	;
	spi_struct.frame_size           = SPI_FRAMESIZE_8BIT;
	spi_struct.nss                  = SPI_NSS_SOFT;
	spi_struct.endian               = SPI_ENDIAN_MSB;
	spi_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
	spi_struct.prescale             = SPI_PSC_256;
	spi_init(SPI0, &spi_struct);
	
//	dma_channel_enable(DMA0, DMA_CH2);
//	spi_dma_enable(SPI0, SPI_DMA_TRANSMIT);

//	dma_channel_enable(DMA0, DMA_CH1);

//	spi_dma_enable(SPI0, SPI_DMA_RECEIVE);

//	spi_enable(SPI0);

}
void spi_dma_A(void)
{
	dma_channel_disable(DMA0, DMA_CH2);
	dma_flag_clear(DMA0, DMA_CH2, DMA_FLAG_FTF);
	dma_channel_enable(DMA0, DMA_CH2);
	spi_dma_enable(SPI0, SPI_DMA_TRANSMIT);

	dma_channel_enable(DMA0, DMA_CH1);

	spi_dma_enable(SPI0, SPI_DMA_RECEIVE);

	spi_enable(SPI0);

//	spi_nss_output_enable(SPI0);
	while(!dma_flag_get(DMA0, DMA_CH2, DMA_FLAG_FTF)){
	}
	dma_flag_clear(DMA0, DMA_CH2, DMA_FLAG_FTF);
	while(!dma_flag_get(DMA0, DMA_CH1, DMA_FLAG_FTF)){
    }
	dma_flag_clear(DMA0, DMA_CH1, DMA_FLAG_FTF);
	spi_disable(SPI0);
}
void SPI1TxDMAEnable(unsigned int mem_addr, unsigned int buf_size)
{
	//tx
	dma_channel_enable(DMA0, DMA_CH2);
//	while(DMA_GetCmdStatus(DMA1_CHANNEL3));
	dma_transfer_number_config(DMA0, DMA_CH2, buf_size);	//设置传输数据字节数
	dma_memory_address_config(DMA0, DMA_CH2, mem_addr);	//设置内存地址
	spi_dma_enable(SPI0, SPI_DMA_TRANSMIT);
	//rx
	dma_channel_enable(DMA0, DMA_CH1);
//	while(DMA_GetCmdStatus(DMA1_CHANNEL2));
	dma_transfer_number_config(DMA0, DMA_CH1, buf_size);
	dma_memory_address_config(DMA0, DMA_CH1, mem_addr);	//接收通道的内存地址和发送通道设置成一样的，之前的数据被覆盖前已经通过发送通道发出去了
	spi_dma_enable(SPI0, SPI_DMA_RECEIVE);
	//enable
	spi_enable(SPI0);
//	DMA1_CHANNEL2->CTLR |= DMA_CTLR_CHEN;	//DMA Enable
//	DMA1_CHANNEL3->CTLR |= DMA_CTLR_CHEN;	//DMA Enable
//	while(!DMA_GetIntBitState(DMA1_INT_TC2));
//	DMA_ClearIntBitState(DMA1_INT_TC2);
//	while(!DMA_GetIntBitState(DMA1_INT_TC3));
//	DMA_ClearIntBitState(DMA1_INT_TC3);
	while(!dma_flag_get(DMA0, DMA_CH2, DMA_FLAG_FTF)){
	}
	dma_flag_clear(DMA0, DMA_CH2, DMA_FLAG_FTF);
	while(!dma_flag_get(DMA0, DMA_CH1, DMA_FLAG_FTF)){
    }
	dma_flag_clear(DMA0, DMA_CH1, DMA_FLAG_FTF);
}
void spi_dma_test(void)
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
	dma_config();
	
	
//	/* enable peripheral clock */
//	/* configure GPIO */
//	gpio_config();
//	/* configure DMA */
//    dma_config();
//	/* configure SPI */
//	spi_config();

//	spi0_init();
//	spi_enable(SPI0);
}

#define TRANSFER_NUM                     0x400                  
u8 dma_spi0_init(uint8_t *tx_buf, uint8_t *rx_buf, uint16_t data_len)
{
	dma_parameter_struct dma_init_struct;   //DMA初始化结构体
	
	rcu_periph_clock_enable(RCU_DMA0);//使能时钟
	
	
	/******配置DMA发送**************************************************/
//	nvic_irq_enable(DMA0_Channel2_IRQn, 0, 0); //配置中断优先级
	dma_deinit(DMA0, DMA_CH2);              //重置DMA
	dma_struct_para_init(&dma_init_struct); //重置初始化结构体

	dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL; //传输方向：从内存到外设
	dma_init_struct.memory_addr = (uint32_t)tx_buf;       //内存地址
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;   //内存递增使能
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;  //传输数据宽度
	dma_init_struct.number = data_len;                     //传输长度
	dma_init_struct.periph_addr = (uint32_t)(&SPI_DATA(SPI0)); //外设地址
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;  //外设地址累加禁用
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;  //外设地址宽度
	dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;        //DMA传输优先级
	dma_init(DMA0, DMA_CH2, &dma_init_struct);
	/* DMA0 channel0 mode configuration */
	dma_circulation_disable(DMA0, DMA_CH2);       //关闭循环
	dma_memory_to_memory_disable(DMA0, DMA_CH2);  //内存到内存禁止
	/* DMA0 channel0 interrupt configuration */
//	dma_interrupt_enable(DMA0, DMA_CH2, DMA_INT_FTF); //中断使能
	/* enable DMA0 transfer */
	spi_dma_enable(SPI0,SPI_DMA_TRANSMIT);
	dma_channel_enable(DMA0, DMA_CH2);
		
/****SPI0接收 DMA初始化***************************************************/		
//	nvic_irq_enable(DMA0_Channel1_IRQn, 0, 0); //配置中断优先级
	dma_deinit(DMA0, DMA_CH1);                 //重置DMA配置
	dma_struct_para_init(&dma_init_struct);    //重置初始化结构体
	dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY; //传输方向：从内存到
	dma_init_struct.memory_addr = (uint32_t)rx_buf;
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;   //内存递增使能
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;  //内存宽度
	dma_init_struct.number = data_len;                     //数据长度
	dma_init_struct.periph_addr = (uint32_t)(&SPI_DATA(SPI0)); //外设地址
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;  //外设地址累加禁用
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;  //外设地址宽度
	dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;        //
	dma_init(DMA0, DMA_CH1, &dma_init_struct);
	/* DMA0 channel0 mode configuration */
	dma_circulation_disable(DMA0, DMA_CH1);       //关闭循环
	dma_memory_to_memory_disable(DMA0, DMA_CH1);  //内存到内存禁止
	/* DMA0 channel0 interrupt configuration */
//	dma_interrupt_enable(DMA0, DMA_CH1, DMA_INT_FTF); //中断使能
	/* enable DMA0 transfer */
	spi_dma_enable(SPI0,SPI_DMA_RECEIVE);
	dma_channel_enable(DMA0, DMA_CH1);
	
}
/*采用DMA发送接收SPI数据*/
u8 SD_SPI_DMA_ReadWriteByte(u8 ByteSend)
{
	u8 rx_buf;
	 // 1. 初始化DMA（每次传输前重新配置长度，避免残留）
    dma_spi0_init(&ByteSend, &rx_buf, 1);
		
    // 2. 拉低NSS，使能DMA通道
//    gpio_bit_reset(SPI_NSS_GPIO_PORT, SPI_NSS_GPIO_PIN);
	while(spi_i2s_flag_get(SPI0, SPI_FLAG_TBE) == RESET){ // 等待SPI发送缓冲器为空
	;
	}
    dma_channel_enable(DMA0, DMA_CH1);
    dma_channel_enable(DMA0, DMA_CH2);

    // 3. 等待DMA收发完成（TX和RX都完成才退出）
	while(spi_i2s_flag_get(SPI0, SPI_FLAG_TRANS) == SET){ // 等待通信结束
	;
	}

	while(spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE) == RESET){ // 等待SPI接收缓冲器非空
	;
	}
//    while(dma_flag_get(DMA0, DMA_CH1, DMA_FLAG_FTF) == RESET);
//    while(dma_flag_get(DMA0, DMA_CH2, DMA_FLAG_FTF) == RESET);

    dma_channel_disable(DMA0, DMA_CH1);
    dma_channel_disable(DMA0, DMA_CH2);
    dma_flag_clear(DMA0, DMA_CH1, DMA_FLAG_FTF);
    dma_flag_clear(DMA0, DMA_CH2, DMA_FLAG_FTF);
	return rx_buf;
}


