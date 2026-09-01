
#include "TMC5160A.h"
#include "math.h"
#include "TMC5160_Register.h"
#include "TMC5160_Constants.h"
#include "TMC5160_Fields.h"
#include "AnglePulse.h"
#include "StallGuard.h"
//#include "TMC5160.h"
__TMC5160 TMC5160_Registers;
uint32_t TCOOLTHRS_Value;
uint32_t CHOPCONF_Value;
uint32_t DRV_STATUS_Value;
uint32_t RAMPSTAT_Value;
uint8_t TMC5160_isRun=0;
/**
REFR_DIR   PE10
REFL_STEP  PE12
DRV_ENN    PA10
*/
void TMC5160A_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOE);
	gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
	gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
	
	rcu_periph_clock_enable(RCU_GPIOA);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
	
	TMC5160A_DIR=1;
	TMC5160A_PUL=1;
	TMC5160A_EN=1;
	
//	TMC5160A_test1();
//	TMC5160A_SPI_init();
}
void TMC5160A_DIAGinit(void)
{
	//诊断输出信号 DIAG0。
	rcu_periph_clock_enable(RCU_GPIOF);
	
    rcu_periph_clock_enable(RCU_AF);

    /* configure button pin as input */
    gpio_init(GPIOF, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_14);

	/* enable and set key EXTI interrupt to the lowest priority */
	nvic_irq_enable(EXTI10_15_IRQn, 2U, 0U);

	/* connect key EXTI line to key GPIO pin */
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOF, GPIO_PIN_SOURCE_14);

	/* configure key EXTI line */
	exti_init(EXTI_14, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
	exti_interrupt_flag_clear(EXTI_14);
    
}
uint64_t testBA=0;
void TMC5160_SPIMode_init(void)
{
	TMC5160A_init();
	TMC5160A_SPI_init();
	TMC5160A_DIAGinit();
}
void TMC5160_init(void)
{
    DISENABLE_CONTROL();        //不使能电机
    
    
    
//获取版本号	
    volatile uint8_t MRES;  //用于细分选择
	volatile uint8_t version;
	TMC5160A_1_Read(0x04);
	version = TMC5160A_1_Read(0x04)>>24;
	if(version!=0x30)//获取版本号
	{
//		while(1);//版本号获取失败，请检查通讯，vccio供电
		
	}
//获取版本号	

	

//****这里参数看实际情况修改
	//配置GCONF寄存器
    //选择TMC5160的斩波模式
	TMC5160_Registers.GCONF.en_pwm_mode            = Stealthchop;       //可选 Stealthchop：通常翻译成静音模式，选择这斩波模式，电机中低速转动时噪声通常很低，不过有转速上限，在300RPM左右。
                                                                        //或者选 Spreadcycle：通常翻译成高速模式，电机可以低速跑，也可以高转速跑，例如能跑到3000RPM。电机转动时通常有种电流声。
    
    //选择 DIAG0引脚 的输出模式
    TMC5160_Registers.GCONF.diag0_int_pushpull     = ON;                //OFF：集电极开路输出(需要外部上拉，触发时输出低电平)
                                                                        // ON：推挽输出(可自己输出高低电平，触发时输出高电平)
    //选择 DIAG1引脚 的输出模式                                   
    TMC5160_Registers.GCONF.diag1_poscomp_pushpull = ON;                //OFF：集电极开路输出(需要外部上拉，触发时输出低电平)
                                                                        // ON：推挽输出(可自己输出高低电平，触发时输出高电平)
	//配置TMC5160_IHOLD_IRUN寄存器
	TMC5160_Registers.IHOLD_IRUN.IRUN              = 31;                //调整电机运行电流，数值范围0...31。调整这里可改变电机转动时的扭矩。
	TMC5160_Registers.IHOLD_IRUN.IHOLD             = 15;			        		//调整电机待机电流，数值范围0...31。调整这里可改变电机待机时的扭矩，可设置为IRUN的一半。
    
    //配置TMC5160_GLOBAL_SCALER寄存器
    TMC5160_Registers.GLOBAL_SCALER.Value          = 0;                //可等比例缩放电机运行电流和待机电流。数值范围0...255
	//配置细分
    MRES  = 0;			                                                //设置细分。数值范围0...8。调整这里可改变电机微步步距角
                                                                            //MRES = 0，是256细分。例如1.8度步距角的电机转一圈则需要51200个脉冲数。
                                                                            //MRES = 1，是128细分。例如1.8度步距角的电机转一圈则需要25600个脉冲数。
                                                                            //MRES = 2，是 64细分。例如1.8度步距角的电机转一圈则需要12800个脉冲数。	
                                                                            //MRES = 3，是 32细分；			//MRES = 4，是 16细分；			//MRES = 5，是  8细分。
                                                                            //MRES = 6，是  4细分；			//MRES = 7，是  2细分；			//MRES = 8，是  0细分，电机整步运行。
	
	TMC5160A_1_Write(TMC5160_GCONF,		    TMC5160_Registers.GCONF.Value);
	
    //配置电机电流
	TMC5160A_1_Write(TMC5160_IHOLD_IRUN,    0x70000  | TMC5160_Registers.IHOLD_IRUN.Value);
    TMC5160A_1_Write(TMC5160_GLOBAL_SCALER, TMC5160_Registers.GLOBAL_SCALER.Value);
																				
	TMC5160A_1_Write(TMC5160_CHOPCONF,		0x410153 | MRES << TMC5160_MRES_SHIFT);
	
			
	//配置限位开关（参考开关）
	TMC5160A_1_Write(STOP_L_R, OFF);			//不使能左右限位开关
    
#if 0

    tmc5160_writeInt(TMC5160_TPWMTHRS,  54);            //设置这寄存器，可让5160芯片根据速度，在运动过程中自动切换斩波模式。要先把“stealthchop”赋值给“TMC5160_Registers.GCONF.en_pwm_mode”。
                                                        //不过运动过程中切换斩波模式有副作用，不管是自动切换还是手动切换，切换那个点电机会抖动，会丢步。
                                                        //所以不建议运动过程中自动切换，不管是手动切换还是自动切换。
                                                        //寄存器配置，具体对应多少RPM，可看表格“斜坡计算公式”。
#else

    TMC5160A_1_Write(TMC5160_TPWMTHRS,  0);              
#endif
    
    
    
										
	//下面把一些运动参数清0，防止电机初始化时动作
	TMC5160A_1_Write(TMC5160_VMAX, 		0);				//电机最大速度寄存器清0
	
	TMC5160A_1_Write(TMC5160_XTARGET, 	0);				//电机目标位置 寄存器清0，
	
	TMC5160A_1_Write(TMC5160_XACTUAL, 	0);				//电机实际位置 寄存器清0，
	
	TMC5160A_1_Write(TMC5160_XENC, 		0);				//编码器位置计数寄存器清0	
	
	ENABLE_CONTROL();                                   //使能电机
    
	delay_ms(100);		//初始化完务必延时100毫秒，让内部完成初始化
}

/*
设定电流
*/
void TMC5160_SetCurrent(void)
{
	//配置TMC5160_IHOLD_IRUN寄存器
	TMC5160_Registers.IHOLD_IRUN.IRUN              = dev_SavePar.runCurrent;                //调整电机运行电流，数值范围0...31。调整这里可改变电机转动时的扭矩。
	TMC5160_Registers.IHOLD_IRUN.IHOLD             = 0;			        		//调整电机待机电流，数值范围0...31。调整这里可改变电机待机时的扭矩，可设置为IRUN的一半。
	//配置电机电流
	TMC5160A_1_Write(TMC5160_IHOLD_IRUN,0x70000  | TMC5160_Registers.IHOLD_IRUN.Value);
//	tmc5160_writeInt(TMC5160_IHOLD_IRUN,    0x70000  | TMC5160_Registers.IHOLD_IRUN.Value);
}
/*
设定正常运行一半的速度
*/
void TMC5160_SetDU()
{
	uint32_t tempu32;
	TMC5160A_1_Write(TMC5160_IHOLD_IRUN,0x70000  | TMC5160_Registers.IHOLD_IRUN.Value);
	tempu32=(float)Mess_Par.run_speed/60/12000000*pow(2,24)*51200/2;
	TMC5160A_1_Write(0x27,tempu32);				// 1.8度步距角，256细分，12MHz时钟频率的话，这转速太快了，电机扭矩都是会降低
}
void TMC5160_ReadCurrent(void)
{
	//GLOBALSCALER
//	TMC5160_Registers.DRV_STATUS=TMC5160A_1_Read(TMC5160_RAMPSTAT);
}
/*

*/
u8 setStallGuardFlag=0; //确保堵转只设置一次
void TMC5160_ReadTSTEP_Value(void)
{
	uint32_t tempu32;
	xSemaphoreTake(TMC5160A_SPI_SemSR,TMC5160A_SPI_xBlockTime);  //申请互斥信号量
	//两个步进输入信号之间的实际测量时间     DcStep模式下，TSTEP不是电机的平均速度，而是每个微步的速度   不对应实际电机速度
	TMC5160_Registers.TSTEP=TMC5160A_1_Read(TMC5160_TSTEP);
	//TCOOLTHRS≥TSTEP  使能堵转停止，如果被配置   使能堵转输出信号(DIAG0/1)(如果配置
	TMC5160A_1_Write(TMC5160_TCOOLTHRS,140);	 //
	TCOOLTHRS_Value=TMC5160A_1_Read(TMC5160_TCOOLTHRS);
	//斩波器配置
	CHOPCONF_Value=TMC5160A_1_Read(0x6C);
	//DRV_STATUS–STALLGUARD2值和驱动器错误标志
	DRV_STATUS_Value=TMC5160A_1_Read(0x6F);
	//
	RAMPSTAT_Value=TMC5160A_1_Read(TMC5160_RAMPSTAT);
	//
	TMC5160A_1_Write(TMC5160_GCONF,1);
	TMC5160_Registers.GCONF.Value=TMC5160A_1_Read(TMC5160_GCONF);
	//
	TMC5160_Registers.DRV_STATUS.Value=TMC5160A_1_Read(TMC5160_DRVSTATUS);
	TMC5160_Registers.RAMP_STAT.Value=TMC5160A_1_Read(TMC5160_RAMPSTAT);
//	TMC5160_Registers.RAMP_STAT.event_stop_sg=1;
//	TMC5160A_1_Write(TMC5160_RAMPSTAT,TMC5160_Registers.RAMP_STAT.Value);
//	if(TMC5160_Registers.RAMP_STAT.velocity_reached==1&&setStallGuardFlag==0&&TMC5160_isRun&&TMC5160_Registers.RAMPMODE==2)
//	{
//		setStallGuardFlag=1;
////		TMC5160_SetTCOOLTHRS();   //进行堵转设置
//		
//		
//	}
	if(Control_Par.setState==GATE_STOP)
	{
		
	}
	//到达目的速度
	else if(TMC5160_Registers.RAMP_STAT.velocity_reached==1)
	{
		
	}
	xSemaphoreGive(TMC5160A_SPI_SemSR);
}
/*
进行堵转设置
*/
void TMC5160_SetTCOOLTHRS(void)
{
	TMC5160_Registers.SW_MODE.Value=TMC5160A_1_Read(TMC5160_SWMODE);
	TMC5160_Registers.SW_MODE.sg_stop=1;
	TMC5160A_1_Write(TMC5160_SWMODE,TMC5160_Registers.SW_MODE.Value);               //打开堵转停止功能
	TMC5160A_1_Write(TMC5160_TCOOLTHRS,dev_SavePar.TCOOLTHRS);	 //
//	TMC5160A_1_Write(TMC5160_COOLCONF,0);	
//	TMC5160_Registers.COOLCONF.Value=TMC5160A_1_Read(TMC5160_COOLCONF);
	TMC5160_Registers.COOLCONF.Value=TMC5160A_1_Read(TMC5160_COOLCONF);
//	TMC5160_Registers.COOLCONF.sfilt=1;
	TMC5160_Registers.COOLCONF.SGT=dev_SavePar.SGT;
	TMC5160A_1_Write(TMC5160_COOLCONF,TMC5160_Registers.COOLCONF.Value);
}
void TMC5160_SetSGT(void)
{
	TMC5160_Registers.COOLCONF.Value=TMC5160A_1_Read(TMC5160_COOLCONF);
	TMC5160_Registers.COOLCONF.SGT=dev_SavePar.SGT;
	TMC5160A_1_Write(TMC5160_COOLCONF,TMC5160_Registers.COOLCONF.Value);
}
void TMC5160_SetAMAX(void)
{
	TMC5160A_1_Write(TMC5160_AMAX,dev_SavePar.AMAX);
}
//void TMC5160_TEST(void)
//{
//	TMC5160_Registers.COOLCONF.Value=TMC5160A_1_Read(TMC5160_COOLCONF);
//	TMC5160_Registers.COOLCONF.SGT=dev_SavePar.SGT;
//	TMC5160A_1_Write(TMC5160_COOLCONF,TMC5160_Registers.COOLCONF.Value);
//	TMC5160_Registers.COOLCONF.Value=TMC5160A_1_Read(TMC5160_COOLCONF);
//}

//位置模式的6点斜坡运动轨迹配置。速度寄存器和真实物理量之间的换算，可以看表格《斜坡计算公式》。
void TMC5160_MovePosition_SixPoint(int32_t position, uint32_t velocityMax)
{
	tmc5160_writeInt(TMC5160_RAMPMODE, 		0);				//位置模式

	//这里的参数可调整。也可以后面随时调整
	tmc5160_writeInt(TMC5160_VSTART, 0);			//电机启动速度寄存器。             数值范围 0...262143。
	tmc5160_writeInt(TMC5160_A1, 2000);             //第一段加速度寄存器。             数值范围 0...65535。
    tmc5160_writeInt(TMC5160_D1, 2000);             //V1和 VSTOP之间的减速度。         数值范围 1...65535。
	tmc5160_writeInt(TMC5160_V1, 70000);            //第一段加速度阈值速度。           数值范围 0...1048575。
	tmc5160_writeInt(TMC5160_AMAX, 10000);          //V1 与 VMAX 之间的第二个加速度。  数值范围 0...65535。
	tmc5160_writeInt(TMC5160_DMAX, 10000);          //VMAX和 V1之间的减速度。          数值范围 0...65535。
	tmc5160_writeInt(TMC5160_VSTOP, 10);				//VSTOP:停止速度 一般>=10
	tmc5160_writeInt(TMC5160_TZEROWAIT, 10);
	//上面的参数可调整。也可以后面随时调整
	
	
	tmc5160_writeInt(TMC5160_VMAX, velocityMax);
	tmc5160_writeInt(TMC5160_XTARGET, position);
	
}

//T型斜坡运动轨迹配置。配置难度比6点斜坡简单些，只需考虑一段加速度和一段减速度。速度寄存器和真实物理量之间的换算，可以看表格《斜坡计算公式》。
void TMC5160_MovePosition_T(int32_t position, uint32_t velocityMax)
{
	tmc5160_writeInt(TMC5160_RAMPMODE, 		0);				//位置模式

   
    //要是用T型加减速，下面的参数不修改
	tmc5160_writeInt(TMC5160_A1, 0);			//不启用
	tmc5160_writeInt(TMC5160_V1, 0);			//不启用
	tmc5160_writeInt(TMC5160_D1, 1);			//不启用
	//要是用T型加减速，上面面的参数不修改
    
	
	//这里的参数可调整
	tmc5160_writeInt(TMC5160_VSTART, 0);				//启动速度可以为0
	tmc5160_writeInt(TMC5160_AMAX, 10000);
	tmc5160_writeInt(TMC5160_DMAX, 10000);
	tmc5160_writeInt(TMC5160_VSTOP, 10);
	tmc5160_writeInt(TMC5160_TZEROWAIT, 10);
	//上面的参数可调整。也可以后面随时调整
	
	tmc5160_writeInt(TMC5160_VMAX, velocityMax);
	tmc5160_writeInt(TMC5160_XTARGET, position);
}


//位置模式下控制电机相对移动。只能在配置完加减速后使用
void TMC5160_moveBy(int32_t position)
{
	// determine actual position and add numbers of ticks to move
	position += tmc5160_readInt(TMC5160_XACTUAL);
	//在位置模式下，修改内容将启动一个运动
	tmc5160_writeInt(TMC5160_XTARGET,	position);				//设置电机目标位置
}
/*
清除堵转
*/
void TMC5160_ClearStallGuard(void)
{
	TMC5160_Registers.SW_MODE.Value=TMC5160A_1_Read(TMC5160_SWMODE);
	TMC5160_Registers.SW_MODE.sg_stop=0;
	TMC5160A_1_Write(TMC5160_SWMODE,TMC5160_Registers.SW_MODE.Value);               //打开堵转停止功能
//	TMC5160A_1_Write(TMC5160_TCOOLTHRS,dev_SavePar.TCOOLTHRS);	 //
//	TMC5160A_1_Write(TMC5160_COOLCONF,0);	
//	TMC5160_Registers.COOLCONF.Value=TMC5160A_1_Read(TMC5160_COOLCONF);
//	TMC5160_Registers.COOLCONF.Value=TMC5160A_1_Read(TMC5160_COOLCONF);
//	TMC5160_Registers.COOLCONF.SGT=dev_SavePar.SGT;
//	TMC5160A_1_Write(TMC5160_COOLCONF,TMC5160_Registers.COOLCONF.Value);
//	TMC5160_Registers.COOLCONF.Value=TMC5160A_1_Read(TMC5160_COOLCONF);
	setStallGuardFlag=0;
}
/*
	位置模式向上时是否堵转?
*/
void TMC5160_StallGuardProcess(void)
{
	//上升途中堵转 ---> 停止电机
	if(StallGuard_Par.StallGuard_occur==1 && Control_Par.setState==GATE_UP)		
	{
		Control_Par.setState=GATE_STOP;
		StallGuard_Par.stallGuard_count=0;
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_WHITE"TMC5160_StallGuardProcess:UP Control_Par.setState=%hhu\r\n",Control_Par.setState);
		StallGuard_Par.StallGuard_flag=1;		//StallGuard_flag  播放动画、电机停止、控制关闭
		TMC5160A_STOP();
		Control_Par.control_run=0;				//再次点击启动后重新开启控制
	}
	//判断是否发生堵转,发生堵转则转而向上运行一段距离
	/*
	如果发生了堵转，并且当前阶段是0,并且当前是下降，并且发生次数小于3次
	*/
	if(StallGuard_Par.StallGuard_occur==1&&StallGuard_Par.StallGuardStage==0&&Control_Par.setState==GATE_DOWN&&StallGuard_Par.StallGuardCount<3)
	{
		//堵转恢复参数
		StallGuard_Par.stallGuard_count=0;
		StallGuard_Par.StallGuard_occur=0;   		
		
		StallGuard_Par.StallGuardCount++;			//向下堵转处理次数
		StallGuard_Par.StallGuardStage=1;			//向下堵转处理阶段
		
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_WHITE"TMC5160_StallGuardProcess:%u Start\r\n",StallGuard_Par.StallGuardCount);
		delay_ms(500);
		//配置控制模式为位置模式
		TMC5160_Registers.RAMPMODE=0;
		TMC5160A_1_Write(TMC5160_RAMPMODE,TMC5160_Registers.RAMPMODE);
		//StallGuard2停止事件  写入“1”以清除标志和中断条件
		TMC5160_Registers.RAMP_STAT.Value=TMC5160A_1_Read(TMC5160_RAMPSTAT);
		TMC5160_Registers.RAMP_STAT.event_stop_sg=1;
		TMC5160A_1_Write(TMC5160_RAMPSTAT,TMC5160_Registers.RAMP_STAT.Value);
		//打开堵转停止功能
		TMC5160_ClearStallGuard();
//		TMC5160_moveBy(300000);  //提示:最大可能位移为+ / - ( ( 2^31 ) - 1 ) --->+/-2147483648
		TMC5160_moveBy(512000);//51200
	}
	//超过一段距离之后则停止				
	else if(StallGuard_Par.StallGuardStage==1)
	{
		if(TMC5160_Registers.RAMP_STAT.position_reached==1)
		{
			SEGGER_RTT_printf(0,RTT_CTRL_TEXT_WHITE"TMC5160_StallGuardProcess:%u End\r\n",StallGuard_Par.StallGuardCount);
			delay_ms(500);
			TMC5160A_RUNDOWN();
			
			//向下堵转处理完成  继续向下运行			防止向上运行时发生堵转
			StallGuard_Par.StallGuardStage=0;
			StallGuard_Par.stallGuard_count=0;
			StallGuard_Par.StallGuard_occur=0;
			
			SEGGER_RTT_printf(0,RTT_CTRL_TEXT_WHITE"TMC5160_StallGuardProcess:StallGuard_Par.StallGuardCount=%hhu Continue\r\n",StallGuard_Par.StallGuardCount);
		}
	}
	//堵转处理3次后  发生第四次堵转  
	else if(StallGuard_Par.StallGuardCount==3&&StallGuard_Par.StallGuard_occur==1)
	{
		StallGuard_Par.stallGuard_count=0;
		SEGGER_RTT_printf(0,RTT_CTRL_TEXT_WHITE"TMC5160_StallGuardProcess:DOWN StallGuard_Par.StallGuardCount=%hhu\r\n",StallGuard_Par.StallGuardCount);
		StallGuard_Par.StallGuard_flag=1;		//StallGuard_flag  播放动画、电机停止、控制关闭
		Control_Par.setState=GATE_STOP;
		TMC5160A_STOP();
		Control_Par.control_run=0;
	}
}

void TMC5160A_SPI_init(void)
{
	    /* 初始化GPIO外设 */
	rcu_periph_clock_enable(RCU_GPIOD);
	//MISO
    gpio_init(GPIOD, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
	
	//MOSI
    gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
	//SCLK
	gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
	
	//CS
    gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
	
	
}
uint64_t TMC5160A_1_WriteReg(u8 addr,uint64_t data)
{
	return TMC5160A_1_WriteBuff(((uint64_t)addr<<32)|data|0x8000000000);
}
uint64_t TMC5160A_1_ReadReg (u8 addr,uint64_t data)
{
	return TMC5160A_1_WriteBuff((((uint64_t)addr<<32)|data)&0x7FFFFFFFFF);
}
//uint64_t testBA=0;





void TMC5160A_test(void)
{
		testBA=TMC5160A_1_Read(0X0A);
		testBA=TMC5160A_1_Read(0X0A);
}
void TMC5160A_RunSetting(void)
{
	u32 tempu32;
	TMC5160A_EN=0;
	/*
		斩波器和驱动配置
	CHOPCONF: TOFF=3, HSTRT=4, HEND=1, TBL=2, CHM=0 (spreadCycle)
	C-12 
	256 微步插值|TPFD
	0000 0000| 0000 1101 |0000 0000| 1100 0011
	
	*/
//	TMC5160_ClearStallGuard();
	TMC5160_Registers.RAMP_STAT.Value=TMC5160A_1_Read(TMC5160_RAMPSTAT);
	TMC5160_Registers.RAMP_STAT.event_stop_sg=1;
	TMC5160A_1_Write(TMC5160_RAMPSTAT,TMC5160_Registers.RAMP_STAT.Value);
	TMC5160A_1_Write(0x6C,0xC00D00C3);
	/*
	GLOBALSCALER
	0 :满量程(或写 256 )
	1…31 :无效
	32…255 : 最大电流的 32 / 256…255 / 256
	*/
	TMC5160A_1_Write(TMC5160_GLOBAL_SCALER,0x00000000);
	testBA=TMC5160A_1_Read(TMC5160_GLOBAL_SCALER);
	/*
	驱动电流控制           
	4..0 IHOLD  静止状态下电机电流 (0=1/32…31=32/32)
	12..8 IRUN  电机运行电流(0=1/32…31=32/32)
	19..16 IHOLDDELAY 设置电机检测到静止状态( stst = 1 )及 TPOWERDOWN
					  之后电流开始下降过程的时钟周期数
	0000 0000 |0000 0110|0001 1111|000 01010
	0000 0000 |0000 0110|000|11111|000|01010
	
	0.050 4.7 6.6
	6/    =x/
	6.6    32
	          00000000  0001 1111 0000 0000
	*/
	//testBA=TMC5160A_1_WriteBuff(0x9000061F1F);
	//TMC5160A_1_Write(0x10,0x00061D0A);
	/*
	1f 31   6A
	0F 15   3A
	最大1D
	*/
	
	
//	TMC5160A_1_Write(0x10,0x00061F00);
	TMC5160_SetCurrent();
	/*
		1001 0001
	    0001 0001-11
	TPOWERDOWN 是电机静止标志（stst）有效到电机开始降电流
	之间的延时时间。时间范围约为 0 至 4 秒
	*/
	TMC5160A_1_Write(TMC5160_TPOWERDOWN,0x0000000A);
	
	
	
	/*
		GCONF – 全局配置标志
		使能 StealthChop 电压 PWM 模式 (取决于速度阈值)。
		00000000|00000000|00000000|00000100
	*/
//	testBA=TMC5160A_1_WriteBuff(0x8000000004);
	TMC5160A_1_Write(TMC5160_GCONF,0x00000000);
	/*
	
		电压 PWM 模式斩波器配置
		0xC40C0C11  
		        允许不同的静止模式  pwm_autograd  pwm_autoscale PWM 频率 PWM_GRAD PWM_OFS
		1100 0100   0000                 1             1           00    00001100 00010001
	*/
	TMC5160A_1_Write(TMC5160_PWMCONF,0xC40C0C11);
	/*
	两个步进输入信号之间的实际测量时间
	*/
//	TMC5160A_1_Write(0x12,0x000ff2F4);			// 只读
	/*
		1001 0011
		0001 0011-13
		StealthChop 电压 PWM模式的上限速度。																														
	*/

//	TMC5160A_1_Write(0x13,3000);
	
	/*
	|0000 0100 |0000 0000
	*/
//	testBA=TMC5160A_1_Read(0x34);
//	TMC5160A_1_Write(0x34,0x00000400);
//	testBA=TMC5160A_1_Read(0x34);
	/*
	DRV_CONF
               0000 0000|0000 1100|0000 0000|0000 0000
	 0xa5000000
	*/
//	testBA=TMC5160A_1_Read(0X0A);
//	TMC5160A_1_Write(0x0A,0x000C0000);
//	testBA=TMC5160A_1_Read(0X0A);
//	testBA=TMC5160A_1_WriteBuff(0x93000ff104);
    /*
	VSTART
	*/
	TMC5160A_1_Write(TMC5160_VSTART,100);
	/*
		1010 0100
		0010 0100-24
		VSTART 和 V1 之间的加速度(无符号)
	*/
	TMC5160A_1_Write(TMC5160_A1,500);
	/*
		1010 0101
		0010 0101-25
		第一加速/减速阶段阈值速度(无符号)
	
	*/
	TMC5160A_1_Write(TMC5160_V1,0);
	/*
		AMAX
		1010 0110
		0010 0110-26
		V1 和 VMAX 之间的加速度(无符号)
	*/
//	TMC5160A_1_Write(0x26,0x00000044);
	TMC5160A_1_Write(TMC5160_AMAX,dev_SavePar.AMAX);					// 100的话，加速度太慢了
	/*
		VMAX
		1010 0111
		0010 0111-27
		运动斜坡目标速度(位置模式确保
		VMAX≥VSTART ) (无符号)
		0…(2^23)-512
		
		
	*/
	//TMC5160A_1_Write(0x27,0x0002FF40);
	//=B12/60/$C$7*(2^24)*$C$6
	tempu32=(float)Mess_Par.run_speed/60/12000000*pow(2,24)*51200;
	TMC5160A_1_Write(TMC5160_VMAX,tempu32);				// 1.8度步距角，256细分，12MHz时钟频率的话，这转速太快了，电机扭矩都是会降低
	
//	TMC5160A_1_Write(0x27,238609);					// 这里大概200rpm
	tempu32=TMC5160A_1_Read(TMC5160_VMAX);
	tempu32=TMC5160A_1_Read(TMC5160_VMAX);
//	TMC5160A_1_Write(0x27,1193046);					// 这里大概2000rpm
	/*
		1010 1000
		0010 1000-28
		VMAX 和 V1 之间的减速度(无符号)
	*/
	TMC5160A_1_Write(TMC5160_DMAX,10000000);
	/*
		1010 1010
		0010 1010-2A
		V1 和 VSTOP 之间的减速度(无符号)
	*/
	TMC5160A_1_Write(TMC5160_D1,10000000);
	/*
		1010 1011
		0010 1011-2B
		电机停止速度(无符号)
		
	*/
	TMC5160A_1_Write(TMC5160_VSTOP,10000);
	return;
	
}
void TMC5160A_RunSetting2(float speed)
{
	u32 tempu32;
	TMC5160A_EN=0;
	/*
		斩波器和驱动配置
	CHOPCONF: TOFF=3, HSTRT=4, HEND=1, TBL=2, CHM=0 (spreadCycle)
	C-12 
	256 微步插值|TPFD
	0000 0000| 0000 1101 |0000 0000| 1100 0011*/
	TMC5160A_1_Write(0x6C,0xC00D00C3);
	/*
	GLOBALSCALER
	*/
	TMC5160A_1_Write(0x0B,0x00000000);
	/*
	驱动电流控制           
	4..0 IHOLD  静止状态下电机电流 (0=1/32…31=32/32)
	12..8 IRUN  电机运行电流(0=1/32…31=32/32)
	19..16 IHOLDDELAY 设置电机检测到静止状态( stst = 1 )及 TPOWERDOWN
					  之后电流开始下降过程的时钟周期数
	0000 0000 |0000 0110|0001 1111|000 01010
	0000 0000 |0000 0110|000|11111|000|01010
	
	0.050 4.7 6.6
	6/    =x/
	6.6    32
	          00000000  0001 1111 0000 0000
	*/
	//testBA=TMC5160A_1_WriteBuff(0x9000061F1F);
	//TMC5160A_1_Write(0x10,0x00061D0A);
	/*
	1f 31   6A
	0F 15   3A
	最大1D
	*/
	
	
//	TMC5160A_1_Write(0x10,0x00061F00);
	TMC5160_SetCurrent();
	
	/*
		1001 0001
	    0001 0001-11
	TPOWERDOWN 是电机静止标志（stst）有效到电机开始降电流
	之间的延时时间。时间范围约为 0 至 4 秒
	*/
	TMC5160A_1_Write(0x11,0x0000000A);
	
	
	
	/*
		GCONF – 全局配置标志
		使能 StealthChop 电压 PWM 模式 (取决于速度阈值)。
		00000000|00000000|00000000|00000100
	*/
//	testBA=TMC5160A_1_WriteBuff(0x8000000004);
	TMC5160A_1_Write(0x00,0x00000000);
	/*
	
		电压 PWM 模式斩波器配置
		0xC40C0C11  
		        允许不同的静止模式  pwm_autograd  pwm_autoscale PWM 频率 PWM_GRAD PWM_OFS
		1100 0100   0000                 1             1           00    00001100 00010001
	*/
	TMC5160A_1_Write(0x70,0xC40C0C11);
	/*
	两个步进输入信号之间的实际测量时间
	*/
//	TMC5160A_1_Write(0x12,0x000ff2F4);			// 只读
	/*
		1001 0011
		0001 0011-13
		StealthChop 电压 PWM模式的上限速度。																														
	*/

//	TMC5160A_1_Write(0x13,3000);
	
	/*
	|0000 0100 |0000 0000
	*/
//	testBA=TMC5160A_1_Read(0x34);
//	TMC5160A_1_Write(0x34,0x00000400);
//	testBA=TMC5160A_1_Read(0x34);
	/*
	DRV_CONF
               0000 0000|0000 1100|0000 0000|0000 0000
	 0xa5000000
	*/
//	testBA=TMC5160A_1_Read(0X0A);
//	TMC5160A_1_Write(0x0A,0x000C0000);
//	testBA=TMC5160A_1_Read(0X0A);
//	testBA=TMC5160A_1_WriteBuff(0x93000ff104);
	/*
		1010 0100
		0010 0100-24
		VSTART 和 V1 之间的加速度(无符号)
	*/
	TMC5160A_1_Write(0x24,0x000000E8);
	/*
		1010 0101
		0010 0101-25
		第一加速/减速阶段阈值速度(无符号)
	
	*/
	TMC5160A_1_Write(0x25,0x00000350);
	/*
		AMAX
		1010 0110
		0010 0110-26
		V1 和 VMAX 之间的加速度(无符号)
	*/
//	TMC5160A_1_Write(0x26,0x00000044);
	TMC5160A_1_Write(0x26,dev_SavePar.AMAX);					// 100的话，加速度太慢了
	/*
		VMAX
		1010 0111
		0010 0111-27
		运动斜坡目标速度(位置模式确保
		VMAX≥VSTART ) (无符号)
		0…(2^23)-512
		
		
	*/
	//TMC5160A_1_Write(0x27,0x0002FF40);
	//=B12/60/$C$7*(2^24)*$C$6
	tempu32=(float)speed/60/12000000*pow(2,24)*51200;
	TMC5160A_1_Write(0x27,tempu32);				// 1.8度步距角，256细分，12MHz时钟频率的话，这转速太快了，电机扭矩都是会降低
//	TMC5160A_1_Write(0x27,238609);					// 这里大概200rpm
	tempu32=TMC5160A_1_Read(0x27);
	tempu32=TMC5160A_1_Read(0x27);
//	TMC5160A_1_Write(0x27,1193046);					// 这里大概2000rpm
	/*
		1010 1000
		0010 1000-28
		VMAX 和 V1 之间的减速度(无符号)
	*/
	TMC5160A_1_Write(0x28,10000000);
	/*
		1010 1010
		0010 1010-2A
		V1 和 VSTOP 之间的减速度(无符号)
	*/
	TMC5160A_1_Write(0x2A,0x00001578);
	/*
		1010 1011
		0010 1011-2B
		电机停止速度(无符号)
		
	*/
	TMC5160A_1_Write(0x2B,0x0000000A);
	return;
	
}


/*
以下 SPI 数据示例支持步进和方向模式, 12mhz 时钟， 配置小于 30 rpm 运行 StealthChop 及大于 30
rpm 运行 spreadCycle 的斩波器参数。
SPI 发送: 0xEC000100C3; // CHOPCONF: TOFF=3, HSTRT=4, HEND=1, TBL=2, CHM=0 (spreadCycle)
SPI 发送: 0x9000061F0A; // IHOLD_IRUN: IHOLD=10, IRUN=31 (最大电流 ), IHOLDDELAY=6
SPI 发送: 0x910000000A; // TPOWERDOWN=10: 电机静止到电流减小之间的延时
SPI 发送: 0x8000000004; // EN_PWM_MODE=1 enables StealthChop (缺省 PWM_CONF 值)
SPI 发送: 0x93000001F4; // TPWM_THRS=500 对应切换速度 35000 = ca. 30RPM
以下SPI 数据示例使能并初始化运动控制器， 内部斜坡发生器控制电机运行一圈（51200微步） ， 最
后查询读取当前位置。
SPI 发送: 0xA4000003E8; // A1 = 1 000 第一阶段加速度
SPI 发送: 0xA50000C350; // V1 = 50 000 加速度阈值速度 V1
SPI 发送: 0xA6000001F4; // AMAX = 500 大于 V1 的加速度
SPI 发送: 0xA700030D40; // VMAX = 200 000
SPI 发送: 0xA8000002BC; // DMAX = 700 大于 V1 的减速度
SPI 发送: 0xAA00000578; // D1 = 1400 小于 V1 的减速度
SPI 发送: 0xAB0000000A; // VSTOP = 10 停止速度(接近于 0)
SPI 发送: 0xA000000000; // RAMPMODE = 0 (目标位置运动)
// 电机就绪!
SPI 发送: 0xADFFFF3800; // XTARGET = -51200 (向左运动一圈 (200*256 微步)
// 现在电机 1 开始旋转
*/
void TMC5160A_RUNUP(void)
{
	xSemaphoreTake(TMC5160A_SPI_SemSR,TMC5160A_SPI_xBlockTime);  //申请互斥信号量
	StallGuard_Par.StallGuard_flag=0;
	MOS_PIN=1;
	TMC5160A_EN=0;
	TMC5160_Registers.RAMPMODE=1;
	TMC5160_Registers.SW_MODE.Value=TMC5160A_1_Read(TMC5160_SWMODE);
	TMC5160_Registers.SW_MODE.sg_stop=0;
	TMC5160A_1_Write(TMC5160_SWMODE,TMC5160_Registers.SW_MODE.Value);               //打开堵转停止功能
	TMC5160A_1_Write(TMC5160_RAMPMODE,TMC5160_Registers.RAMPMODE);
	TMC5160A_RunSetting();
	/*
		1010 0000
		0010 0000
		RAMPMODE:
		0: 位置模式(使用所有 A、D 和 V 参数)
		1: 速度模式到正 VMAX (使用 AMAX 加速度)
		2: 速度模式至负 VMAX (使用 AMAX 加速度)
		3: 保持模式(除非发生停止事件，否则速度保
		持不变
	*/
	TMC5160_isRun=1;
	xSemaphoreGive(TMC5160A_SPI_SemSR);
}
void TMC5160A_RUN2(float speed)
{
	xSemaphoreTake(TMC5160A_SPI_SemSR,TMC5160A_SPI_xBlockTime);  //申请互斥信号量
	TMC5160A_EN=0;
	MOS_PIN=1;
	TMC5160A_RunSetting2(fabs(speed));
	/*
		1010 0000
		0010 0000
		RAMPMODE:
		0: 位置模式(使用所有 A、D 和 V 参数)
		1: 速度模式到正 VMAX (使用 AMAX 加速度)
		2: 速度模式至负 VMAX (使用 AMAX 加速度)
		3: 保持模式(除非发生停止事件，否则速度保
		持不变
	*/
	if(speed>0) //上升
	{
		Control_Par.setState=GATE_UP;
		TMC5160A_1_Write(0x20,0x00000001);
	}
	if(speed<0) //下降
	{
		Control_Par.setState=GATE_DOWN;
		TMC5160A_1_Write(0x20,0x00000002);
	}
	
	xSemaphoreGive(TMC5160A_SPI_SemSR);
}
void TMC5160A_ChangeSpeed(void)
{
	u32 tempu32;
	xSemaphoreTake(TMC5160A_SPI_SemSR,TMC5160A_SPI_xBlockTime);  //申请互斥信号量
	tempu32=(float)Mess_Par.run_speed/60/12000000*pow(2,24)*51200;
	TMC5160A_1_Write(0x27,tempu32);				// 1.8度步距角，256细分，12MHz时钟频率的话，这转速太快了，电机扭矩都是会降低
	xSemaphoreGive(TMC5160A_SPI_SemSR);
}
void TMC5160A_STOP(void)
{
	xSemaphoreTake(TMC5160A_SPI_SemSR,TMC5160A_SPI_xBlockTime);  //申请互斥信号量
//	TMC5160A_1_Write(0x6C,0x10410150);
//	TMC5160A_1_Write(0x27,0x00000000);
//	TMC5160A_1_Write(0x26,100000);					// 100的话，加速度太慢了
//	xSemaphoreGive(TMC5160A_SPI_SemSR);
	Flash_SaveMess_Par();
//	TMC5160A_EN=1;
	MOS_PIN=0;			
	TMC5160_stop();
	xSemaphoreGive(TMC5160A_SPI_SemSR);
	TMC5160A_EN=1;
	TMC5160_isRun=0;
}
// 减速停止电机，停止时间和AMAX寄存器有关
void TMC5160_stop(void)
{
	static uint32_t AMAX = 65535;				//这里可根据需要修改，也可后面随时修改
	TMC5160A_1_Write(TMC5160_AMAX, 	AMAX);	
	TMC5160_rotate(0);
}
void TMC5160_rotate(int32_t velocity)
{
    TMC5160A_1_Write(TMC5160_RAMPMODE, (velocity >= 0) ? TMC5160_MODE_VELPOS : TMC5160_MODE_VELNEG);
    
	TMC5160A_1_Write(TMC5160_VMAX, 		abs(velocity));
}

void TMC5160A_RUNDOWN(void)
{
	xSemaphoreTake(TMC5160A_SPI_SemSR,TMC5160A_SPI_xBlockTime);  //申请互斥信号量
	StallGuard_Par.StallGuard_flag=0;
	MOS_PIN=1;
	TMC5160A_EN=0;
	TMC5160_Registers.SW_MODE.Value=TMC5160A_1_Read(TMC5160_SWMODE);
	TMC5160_Registers.SW_MODE.sg_stop=0;
	TMC5160A_1_Write(TMC5160_SWMODE,TMC5160_Registers.SW_MODE.Value);               //打开堵转停止功能
	TMC5160_Registers.RAMPMODE=2; //速度模式至负 VMAX (使用 AMAX 加速度)
	TMC5160A_1_Write(TMC5160_RAMPMODE,TMC5160_Registers.RAMPMODE); 
	TMC5160A_RunSetting();
	/*
		1010 0000
		0010 0000
		RAMPMODE:
		0: 位置模式(使用所有 A、D 和 V 参数)
		1: 速度模式到正 VMAX (使用 AMAX 加速度)
		2: 速度模式至负 VMAX (使用 AMAX 加速度)
		3: 保持模式(除非发生停止事件，否则速度保
		持不变
	*/
	TMC5160_isRun=1;
	xSemaphoreGive(TMC5160A_SPI_SemSR);
}
/*
调试总步数
testStep=0 未开始
testStep=1 向上
testStep=2 向下
testStep=3 计数
*/

void TMC5160A_TestStep(void)
{
	nosave_par.test_step_count=0;
	Control_Par.control_run=0;
	nosave_par.test_step_state=1;
}
/*
nosave_par.test_step_state置1后闸门上升至上限位--->到上限位后nosave_par.test_step_state置3闸门下降
*/
void TMC5160A_TestStep2(void)
{
	if(nosave_par.test_step_flag!=1)
	{
		return;
	}
	if(nosave_par.test_step_state==1)		
	{
		if(MOTOR_LIMIT_UP==1) 	//上			//问题中断中已经判断并置位nosave_par.test_step_state=3
		{
			nosave_par.test_step_state=3;		//运行不到此处
			return;
		}
		else
		{
			nosave_par.test_step_state=2;
			Control_Par.setState=GATE_UP;
			TMC5160A_RUNUP();
		}
	}
	if(nosave_par.test_step_state==3)
	{
		nosave_par.test_step_state=4;
		Control_Par.setState=GATE_DOWN;
		TMC5160A_RUNDOWN();
	}
	if(nosave_par.test_step_state==5)					//测试结束
	{
//		Mess_Par.sum_step=nosave_par.test_step_count;
//		Mess_Par.run_step=0;
//		Flash_SaveMess_Par();
		nosave_par.test_step_flag=0;
		nosave_par.test_step_state=0;
	}
}
void TMC5160A_TestStep_Count(void)
{
	if(nosave_par.test_step_flag!=1)
	{
		return;
	}
	
	if(nosave_par.test_step_state==4)
	{
		nosave_par.test_step_count++;
	}
}
