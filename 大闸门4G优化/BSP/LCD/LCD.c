#include "LCD.h"
#include "cmd_queue.h"
void LCD_All_init(void)
{
	LCD_Power_init();
	LCD_RS232_ENinit();
	LCD_RS232_init(921600);
	LCD_DMA_Rx_Config();
}

void LCD_Power_init(void)
{
	//按键检测
	rcu_periph_clock_enable(RCU_GPIOG);
	gpio_init(GPIOG,GPIO_MODE_IPU,GPIO_OSPEED_50MHZ,GPIO_PIN_0);
	//屏幕电源使能
	rcu_periph_clock_enable(RCU_GPIOG);
	gpio_init(GPIOG,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_1);
	LCD_POWER_EN=0;
		//下限位中断初始化
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOG, GPIO_PIN_SOURCE_0);//connect key     EXTI line to key GPIO pin
	exti_init(EXTI_0, EXTI_INTERRUPT, EXTI_TRIG_FALLING);//下降沿中断
	exti_interrupt_flag_clear(EXTI_0);//清中断标志
	nvic_irq_enable(EXTI0_IRQn, 2U, 0U);//enable and set key EXTI interrupt to the lowest priority 
}

void LCD_RS232_ENinit(void)
{
	 /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOE);

    /* connect I2C_SCL_PIN to I2C_SCL */
    /* connect I2C_SDA_PIN to I2C_SDA */
    gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
	
	gpio_bit_set(GPIOE, GPIO_PIN_9);
	gpio_bit_reset(GPIOE, GPIO_PIN_9);
}
void LCD_RS232_init(uint32_t baudval)
{
    /* 初始化GPIO外设 */
    rcu_periph_clock_enable(RCU_GPIOE);
    /* TX管脚，PA9，复用推挽输出，速度50MHz */
    gpio_init(GPIOE, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    /* RX管脚，PA10，下拉输入，速度50MHz */
    gpio_init(GPIOE, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, GPIO_PIN_7);

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
	
	nvic_irq_enable(UART6_IRQn,0U, 0U);
//	usart_interrupt_enable(UART6, USART_INT_RBNE);  /* 参数：读数据缓冲区非空中断和过载错误中断 */
	usart_interrupt_enable(UART6, USART_INT_IDLE);  /* 参数：读数据缓冲区非空中断和过载错误中断 */
	usart_enable(UART6);  // 使能串口
}
void EXTI0_IRQHandler(void)
{
	if(RESET!=exti_interrupt_flag_get(EXTI_0))
	{
		exti_interrupt_flag_clear(EXTI_0);
		if(PGin(0)==0)
		{
			LCD_POWER_EN=1;
			nosave_par.LCD_timecount=LCD_SHOWTIME;
		}
	}
}
void UART6_IRQHandler(void)
{
	uint8_t rxdata;
	u16 new_head;
	if(RESET != usart_interrupt_flag_get(UART6, USART_INT_FLAG_RBNE)) /* receive */
	{ 
		/* receive data */
		rxdata = usart_data_receive(UART6); /* 接收 */
		queue_push(rxdata);
	}
	if(RESET!=usart_interrupt_flag_get(UART6,USART_INT_FLAG_IDLE))
	{
		rxdata = usart_data_receive(UART6); /* 接收 */
		usart_interrupt_flag_clear(UART6,USART_INT_FLAG_IDLE);
		/* 更新头指针（注意缓冲区大小掩码，防止溢出） */
		new_head = QUEUE_MAX_SIZE -   dma_transfer_number_get(DMA1,DMA_CH2);
		if(que._head<que._tail&&new_head>=que._tail)
		{
			return;
		}
		else if (que._head>new_head&&new_head>=que._tail)
		{
			return;
		}
		else
		{
			que._head = new_head;  
		}
		
	}
	if(RESET != usart_interrupt_flag_get(UART6, USART_INT_FLAG_RBNE_ORERR)) /* receive */
	{
		usart_interrupt_flag_clear(UART6, USART_INT_FLAG_RBNE_ORERR);
	}

}
/*
串口接收配置
*/
void LCD_DMA_Rx_Config(void)
{
	dma_parameter_struct dma_struct;
	rcu_periph_clock_enable(RCU_DMA1);//使能时钟
	dma_deinit(DMA1,DMA_CH2);
	dma_struct_para_init(&dma_struct);
	dma_struct.direction = DMA_PERIPHERAL_TO_MEMORY;		//传输方向 外设到内存
	dma_struct.memory_addr = (uint32_t)(&que._data);		//源地址
	dma_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	dma_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
	dma_struct.number = QUEUE_MAX_SIZE;
	dma_struct.periph_addr = (uint32_t)(&USART_DATA(UART6));//USART1+4
	dma_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;  //外设递增关闭
	dma_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
	dma_struct.priority = DMA_PRIORITY_HIGH;
 
	dma_init(DMA1,DMA_CH2,&dma_struct);
	
	dma_circulation_disable(DMA1, DMA_CH2);               //循环模式
	dma_memory_to_memory_disable(DMA1, DMA_CH2);          //内存到内存关闭
	dma_circulation_enable(DMA1, DMA_CH2);//启用循环接收数据
	//使能传输
	usart_dma_receive_config(UART6,USART_RECEIVE_DMA_ENABLE);
	dma_channel_enable(DMA1, DMA_CH2);
	usart_enable(UART6);  // 使能串口
}
/*
初始化DMA1  ---USART6
*/
void LCD_DMA_init(u32 len)
{
	u32 * welcome; //地址
	dma_parameter_struct dma_init_struct;
	
	nvic_irq_enable(DMA1_Channel4_IRQn, 0, 0);
		
	rcu_periph_clock_enable(RCU_DMA1);//使能时钟
	/* initialize DMA channel3 */
	dma_deinit(DMA1, DMA_CH4);
	dma_struct_para_init(&dma_init_struct);

	dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL; //传输方向 内存到外设
	dma_init_struct.memory_addr = (uint32_t)lcd_txbuff;   //传输内容
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE; //内存增加使能
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;  //内存宽度
//	dma_init_struct.number = ARRAYNUM(welcome);
	dma_init_struct.number = len;                          //发送长度
	dma_init_struct.periph_addr = (uint32_t)(&USART_DATA(UART6));//地址
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;//外设递增模式
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;//DMA发送宽度
	dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;   //信道优先级
	dma_init(DMA1, DMA_CH4, &dma_init_struct);            

	/* configure DMA mode */
	dma_circulation_disable(DMA1, DMA_CH4);              //循环模式关闭
	dma_memory_to_memory_disable(DMA1, DMA_CH4);         //内存到内存关闭

	/* USART DMA enable for transmission */
	usart_dma_transmit_config(UART6, USART_TRANSMIT_DMA_ENABLE);//使能DMA串口发送
	
	/* enable DMA transfer complete interrupt */
	dma_interrupt_enable(DMA1, DMA_CH4, DMA_INT_FTF); //打开发送完成中断

	/* enable DMA channel3 */
	dma_channel_enable(DMA1, DMA_CH4);
}

void UART6_SendDMAConfig(unsigned char *buf,unsigned short int len)
{
	dma_parameter_struct dma_struct;
	rcu_periph_clock_enable(RCU_DMA1);//使能时钟
	dma_deinit(DMA1,DMA_CH4);
	dma_struct_para_init(&dma_struct);
	dma_struct.direction = DMA_MEMORY_TO_PERIPHERAL;//传输方向 内存到外设
	dma_struct.memory_addr = (uint32_t)(buf);//源地址
	dma_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	dma_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
	dma_struct.number = len;
	dma_struct.periph_addr = (uint32_t)UART6+4;//USART1+4
	dma_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
	dma_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
	dma_struct.priority = DMA_PRIORITY_HIGH;
 
	dma_init(DMA0,DMA_CH6,&dma_struct);
	
	dma_circulation_disable(DMA0, DMA_CH6);
	dma_memory_to_memory_disable(DMA0, DMA_CH6);
	dma_circulation_disable(DMA0, DMA_CH6);//禁用循环接收数据
	//使能传输
	usart_dma_transmit_config(USART1,USART_TRANSMIT_DMA_ENABLE);
	dma_channel_enable(DMA0, DMA_CH6);
}