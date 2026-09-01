#include "motor.h"
#include "math.h"
#include "TMC5160A.h"
#include "math.h"
#include "StallGuard.h"



static uint8_t SPI_Software_SendData(uint8_t ByteSend)
{
	uint8_t i, ByteReceive = 0x00;
	
	for (i = 0; i < 8; i ++)
	{
		TMC5160A_SPI_SCLK=1;
		TMC5160A_SPI_SCLK=0;
		TMC5160A_SPI_MOSI= ((ByteSend & (0x80 >> i))>0);
		
//		SPI_Delay(0xff);
		if (TMC5160A_SPI_MISO == 1)
		{
			ByteReceive |= (0x80 >> i);
		}
		TMC5160A_SPI_SCLK=1;
//		SPI_Delay(0xff);
	}
	
	return ByteReceive;
}
uint64_t testB=0;
uint64_t Rxdat=0;
uint32_t TMC5160A_1_Read(unsigned char addr)
{
	uint32_t result=0;
  unsigned char inst;                                             
  inst = 0x7F & addr;   //0111 1111
	TMC5160A_SPI_CS=0;
	delay_us(20);
	testB=SPI_Software_SendData(inst);
	testB<<=8;
	testB|=SPI_Software_SendData(0x00);
	testB<<=8;
	testB|=SPI_Software_SendData(0x00);
	testB<<=8;
	testB|=SPI_Software_SendData(0x00);
	testB<<=8;
	testB|=SPI_Software_SendData(0x00); 
	delay_us(2);
	TMC5160A_SPI_CS=1; 
	delay_us(2);
//需要读取两次
	TMC5160A_SPI_CS=0;
	delay_us(20);
	testB=SPI_Software_SendData(inst);
	result|=SPI_Software_SendData(0);
	result<<=8;
	result|=SPI_Software_SendData(0);
	result<<=8;
	// 09        21        00            00
	//0000 1001 0010 0001 0000 0000  0000 0000
	result|=SPI_Software_SendData(0);  //0x09210000  
	result<<=8;
	//0010 0001
	//2100000000
	result|=SPI_Software_SendData(0);
	delay_us(2);
	TMC5160A_SPI_CS=1; 
	delay_us(2);
	return result;	
}

void TMC5160A_1_Write(unsigned char addr,uint32_t data)
{
	
   unsigned char inst;                                             
	inst =  addr|0x80;   //0111 1111
	TMC5160A_SPI_CS=0;
//	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_TXE);
	delay_us(20);
	testB=SPI_Software_SendData(inst);
	testB<<=8;
	testB|=SPI_Software_SendData((data>>24)&0xff);
	testB<<=8;
	testB|=SPI_Software_SendData((data>>16)&0xff);
	testB<<=8;
	testB|=SPI_Software_SendData((data>>8)&0xff);
	testB<<=8;
	//0011 1001
	//3900000000
	testB|=SPI_Software_SendData((data>>0)&0xff); 
//	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	
//	Rxdat=SPI_Software_SendData(inst);
//	Rxdat<<=8;
//	Rxdat|=SPI_Software_SendData((data>>24)&0xff);
//	Rxdat<<=8;
//	Rxdat|=SPI_Software_SendData((data>>16)&0xff);
//	Rxdat<<=8;
//	// 09        21        00            00
//	//0000 1001 0010 0001 0000 0000  0000 0000
//	Rxdat|=SPI_Software_SendData((data>>8)&0xff);  //0x09210000  
//	Rxdat<<=8;
//	//0010 0001
//	//2100000000
//	Rxdat|=SPI_Software_SendData((data>>0)&0xff);
//	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
//	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_TXE);
	delay_us(2);
	TMC5160A_SPI_CS=1; 
	delay_us(2);
	//return i;	
}

uint64_t TMC5160A_1_WriteBuff(uint64_t data)
{
	uint64_t res=0;
	TMC5160A_SPI_CS=0;
//	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_TXE);
	delay_us(2);
	//EC 00 01 00 C3
	res=SPI_Software_SendData((data>>32)&0xff);
	res<<=8;
	res|=SPI_Software_SendData((data>>24)&0xff);
	res<<=8;
	res|=SPI_Software_SendData((data>>16)&0xff);
	res<<=8;
	res|=SPI_Software_SendData((data>>8)&0xff);
	res<<=8;
	res|=SPI_Software_SendData(data&0xff);
	delay_us(2);
	TMC5160A_SPI_CS=1; 
	delay_us(2);
	return res;
}

void motor_limit(void)
{
	//上限位
	rcu_periph_clock_enable(RCU_GPIOD);
	gpio_init(GPIOD, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
	//下限位
//	rcu_periph_clock_enable(RCU_GPIOD);
	gpio_init(GPIOD, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
	//上限位中断初始化
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOD, GPIO_PIN_SOURCE_13);//connect key     EXTI line to key GPIO pin
	exti_init(EXTI_13, EXTI_INTERRUPT, EXTI_TRIG_RISING);//上升沿中断
	exti_interrupt_flag_clear(EXTI_13);//清中断标志
	
	//下限位中断初始化
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOD, GPIO_PIN_SOURCE_11);//connect key     EXTI line to key GPIO pin
	exti_init(EXTI_11, EXTI_INTERRUPT, EXTI_TRIG_RISING);//上升沿中断
	exti_interrupt_flag_clear(EXTI_11);//清中断标志
	nvic_irq_enable(EXTI10_15_IRQn, 2U, 0U);//enable and set key EXTI interrupt to the lowest priority 
	
	
}

void motor_AlmPend_init(void)
{
	//上限位
	rcu_periph_clock_enable(RCU_GPIOD);
	gpio_init(GPIOD, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
	//下限位
	rcu_periph_clock_enable(RCU_GPIOD);
	gpio_init(GPIOD, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
	//上限位中断初始化
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOD, GPIO_PIN_SOURCE_9);//connect key     EXTI line to key GPIO pin
	exti_init(EXTI_9, EXTI_INTERRUPT, EXTI_TRIG_FALLING);//下降沿中断
	exti_interrupt_flag_clear(EXTI_9);//清中断标志
	nvic_irq_enable(EXTI5_9_IRQn, 2U, 0U);//enable and set key EXTI interrupt to the lowest priority 
	//下限位中断初始化
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOD, GPIO_PIN_SOURCE_10);//connect key     EXTI line to key GPIO pin
	exti_init(EXTI_10, EXTI_INTERRUPT, EXTI_TRIG_FALLING);//下降沿中断
	exti_interrupt_flag_clear(EXTI_10);//清中断标志
	nvic_irq_enable(EXTI10_15_IRQn, 2U, 0U);//enable and set key EXTI interrupt to the lowest priority 
}
//void motor_dirver(void)
//{
////	rcu_periph_clock_enable(RCU_GPIOF);
////	gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
////	gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
////	gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
////	MOTO_EN=1;
//motor_limit();
//TMC5160A_init();
////	Hall_init();
////	motor_AlmPend_init();
//}
/*
霍尔传感器初始化
*/
void Hall_init(void)
{
	//上限位
	rcu_periph_clock_enable(RCU_GPIOD);
	gpio_init(GPIOD, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
	//下限位
//	rcu_periph_clock_enable(RCU_GPIOD);
	gpio_init(GPIOD, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
	//上限位中断初始化
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOD, GPIO_PIN_SOURCE_6);//connect key     EXTI line to key GPIO pin
	exti_init(EXTI_6, EXTI_INTERRUPT, EXTI_TRIG_RISING);//下降沿中断
	exti_interrupt_flag_clear(EXTI_6);//清中断标志
	
	//下限位中断初始化
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOD, GPIO_PIN_SOURCE_7);//connect key     EXTI line to key GPIO pin
	exti_init(EXTI_7, EXTI_INTERRUPT, EXTI_TRIG_RISING);//下降沿中断
	exti_interrupt_flag_clear(EXTI_7);//清中断标志
	nvic_irq_enable(EXTI5_9_IRQn, 2U, 0U);//enable and set key EXTI interrupt to the lowest priority 
	
}
void device_test(void)
{
	motor_dirver();
	gpio_bit_reset(GPIOF,GPIO_PIN_7);
	gpio_bit_set(GPIOF,GPIO_PIN_5);
	u16 time_count=10000;
	u8 dir=0;
	while(1)
	{
		gpio_bit_set(GPIOF,GPIO_PIN_6);
		delay_us(100);
		gpio_bit_reset(GPIOF,GPIO_PIN_6);
		delay_us(100);
		PFout(5)=dir;
		while(!PDin(10))
		{
			delay_ms(10);
		}
		if(PCin(13))
		{
			dir=0;
			delay_ms(10);
		}
		if(PEin(6))
		{
			dir=1;
			delay_ms(10);
		}
//		MOTOR_DRV_ENN=0;
//		testBA=TMC5160A_1_WriteBuff(0xEC000100C3);
//		testBA=TMC5160A_1_WriteBuff(0x9000061F0A);
//		testBA=TMC5160A_1_WriteBuff(0x910000000A);
//		testBA=TMC5160A_1_WriteBuff(0x8000000004);
//		
//		testBA=TMC5160A_1_WriteBuff(0x93000001F4);
//		testBA=TMC5160A_1_WriteBuff(0xA4000003E8);
//		testBA=TMC5160A_1_WriteBuff(0xA50000C350);
//		testBA=TMC5160A_1_WriteBuff(0xA6000001F4);
//		
//		testBA=TMC5160A_1_WriteBuff(0xA700030D40);
//		testBA=TMC5160A_1_WriteBuff(0xA8000002BC);
//		testBA=TMC5160A_1_WriteBuff(0xAA00000578);
//		testBA=TMC5160A_1_WriteBuff(0xAB0000000A);
//		
//		testBA=TMC5160A_1_WriteBuff(0xA000000000);
//		testBA=TMC5160A_1_WriteBuff(0xADFFFF3800);
//		testBA=TMC5160A_1_WriteBuff(0x2100000000);
//		delay_ms(1000);
		
	
	}
}


void motor_AutoControl(void)
{
	/*默认是导通状态
	没到限位是导通状态，低电平 0
	到了限位是断开状态，高电平 1
	*/
	//如果两个都是高电平，说明要么是限位坏了，要么是没接限位
	if(MOTOR_LIMIT_DOWN==1&&MOTOR_LIMIT_UP==1)
	{
		
	}
	if(Control_Par.control_run==1)  //如果打开控制
	{
		/*
		开度设定最小		且下限位未触发	则下降
						下限位触发		停止			限位中断置Control_Par.control_run=0时 运行不到此处
		*/
		if(fabs(Control_Par.set_openingm-0)<=0.001)
		{
			if(MOTOR_LIMIT_DOWN==0) //向下
			{
				Control_Par.setState=GATE_DOWN;
				TMC5160A_RUNDOWN();
			}
			else                  //停止      //1,电机已经在限位,还要往限位方向走	  2,预防电机转动过程中中断损坏
			{
				Control_Par.setState=GATE_STOP;
				Control_Par.control_run=0;
			}
		}
		/*
		设定开度为最大值		上限位未触发		则   上升
							上限位触发		则   停止			限位中断置Control_Par.control_run=0时 运行不到此处
		*/
		else if(fabs(Control_Par.set_openingm-dev_SavePar.water_inletHeight)<=0.001)
		{
			if(MOTOR_LIMIT_UP==0) //向上
			{
				Control_Par.setState=GATE_UP;
				TMC5160A_RUNUP();
			}
			else                  //停止     //预防电机转动过程中中断损坏	
			{
				Control_Par.setState=GATE_STOP;
				Control_Par.control_run=0;
			}
		}
		/*
		如果设定开度大于当前开度 且  上限位未触发     则向上提
		*/
		else if(Control_Par.set_openingm>nosave_par.openingm)
		{
			if(MOTOR_LIMIT_UP==0) //向上
			{
				Control_Par.setState=GATE_UP;
				TMC5160A_RUNUP();
			}
			else                  //停止   //预防电机转动过程中中断损坏
			{
				Control_Par.setState=GATE_STOP;
				Control_Par.control_run=0;
			}
		}
		/*
		如果设定开度小于当前开度	且  下限位未触发     则向下降
		*/
		else if(Control_Par.set_openingm<nosave_par.openingm)
		{
			if(MOTOR_LIMIT_DOWN==0) //向下
			{
				Control_Par.setState=GATE_DOWN;
				TMC5160A_RUNDOWN();
			}
			else                  //停止		//预防电机转动过程中中断损坏
			{
				Control_Par.setState=GATE_STOP;
				Control_Par.control_run=0;
			}
		}
	}
	else 		//停止
	{
//		Control_Par.setState=GATE_STOP;
		TMC5160A_STOP();				//Control_Par.control_run=0时 运行不到此处
	}

}


void motor_control1(void)
{	
	//后水位的水位超高 且 下限位未触发 且 堵转无法处理未发生	则	关闭闸门
	if(Statlog_Par.WatLev_Safe==2 && MOTOR_LIMIT_DOWN==0 && StallGuard_Par.StallGuard_flag == 0) //如果水位超高
	{
		Control_Par.setState=GATE_DOWN;
		TMC5160A_RUNDOWN();
		Control_Par.control_run=0;
		return;
	}
	//浮球的水位超高	 且 下限位未触发 且 堵转无法处理未发生	则	关闭闸门
	if(Statlog_Par.FloatBall_Safe==2 && MOTOR_LIMIT_DOWN==0 && StallGuard_Par.StallGuard_flag==0)  //如果水位到达预警
	{
		Control_Par.setState=GATE_DOWN;
		TMC5160A_RUNDOWN();
		Control_Par.control_run=0;		//关掉控制模式
		return;
	}
	/*
		防止闸门已经在限位,还要往限位方向走
	*/
	//如果当前向上运行，并且上限位接收到信号
	else if(Control_Par.setState==GATE_UP && MOTOR_LIMIT_UP==1)		
	{
		Control_Par.setState=GATE_STOP;
		TMC5160A_STOP();
	}
	//如果当前向下运行，并且下限位接收到信号
	else if(Control_Par.setState==GATE_DOWN && MOTOR_LIMIT_DOWN==1)
	{
		Control_Par.setState=GATE_STOP;
		TMC5160A_STOP();
	}
	
	if(Control_Par.control_run==1)  //如果打开控制
	{
		if(Control_Par.control_mode==0) 	//如果是开度控制
		{
			motor_AutoControl();
		}
		if(Control_Par.control_mode==1) 	//后水位控制
		{
			autC_LevControl2();
		}
		if(Control_Par.control_mode==2) 	//流量控制
		{
			autC_FloControl();
		}
		
		//堵转处理函数中已经停止电机  此处函数运行不到
		//发生堵转就停止  StallGuard_Par.StallGuard_flag置1处都将电机停止()
		if(StallGuard_Par.StallGuard_flag==1) 	
		{
			TMC5160A_STOP();
		}
	}
	/*
	为加快停止反应时间，
	停止逻辑写在按键停止中
	*/
	else
	{
		autC_reset();		//清空PID中历史偏差
	}
}

void Motor_Zhang_Init(void)
{
	rcu_periph_clock_enable(RCU_GPIOF);
	
  gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
	
	gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
	gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
	MOTO_FWD=0;
	MOTO_REV=0;
}
