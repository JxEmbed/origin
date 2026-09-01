#include "AnglePulse.h"
#include "math.h"
#include "TMC5160A.h"
#include "StallGuard.h"
u32 forw_pulse=0;
u32 back_pulse=0;

u32 pulse_step=0;

/*
双向霍尔
根据判断霍尔的两个输出引脚   得到电机转动的方向-正转或者反转
*/
void AnglePulse_init(void)
{
	//上限位
	rcu_periph_clock_enable(RCU_GPIOD);
	gpio_init(GPIOD, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
	//下限位
//	rcu_periph_clock_enable(RCU_GPIOD);
	gpio_init(GPIOD, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
	//上限位中断初始化
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOD, GPIO_PIN_SOURCE_6);//connect key     EXTI line to key GPIO pin
	exti_init(EXTI_6, EXTI_INTERRUPT, EXTI_TRIG_FALLING);//下降沿中断
	exti_interrupt_flag_clear(EXTI_6);//清中断标志
	
	//下限位中断初始化
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOD, GPIO_PIN_SOURCE_7);//connect key     EXTI line to key GPIO pin
	exti_init(EXTI_7, EXTI_INTERRUPT, EXTI_TRIG_FALLING);//下降沿中断
	exti_interrupt_flag_clear(EXTI_7);//清中断标志
	
	
	nvic_irq_enable(EXTI5_9_IRQn, 2U, 0U);//enable and set key EXTI interrupt to the lowest priority 
	
}
/**/
s32 AngleP_Step;
u32 AngleP_Speed;
void Calc_Speed(void)
{
	//每秒霍尔的次数 AngleP_Speed		就是	 转/秒
	AngleP_Speed=abs(AngleP_Step);		//abs整数的绝对值
	AngleP_Step=0;
	
	if(dev_SavePar.StallGuard_Switch==1)
	{
		Judge_Stall();            //进行堵转检测
	}
}

void Calc_Opening(void)
{
	//开度=现在运行步数/总步数(下限位到上限位)
	nosave_par.opening=(float)Mess_Par.run_step/(float)Mess_Par.sum_step;
	
	if(nosave_par.opening<0)
	{
		nosave_par.opening=0;
	}
	else if(nosave_par.opening>1)
	{
		nosave_par.opening=1;
	}
	else if(isnan(nosave_par.opening))		//isnan判断是否是一个数据
	{
		nosave_par.opening=0;
	}
	else if(isinf(nosave_par.opening))		//isinf判断是否是无穷大
	{
		nosave_par.opening=0;
	}
	//将开度换算成米=开度*进水高度(闸门在上限位时,闸门底到河床的距离)
	nosave_par.openingm=nosave_par.opening*dev_SavePar.water_inletHeight;
	
	//确保电机运行到指定位置后停止	
	Cmp_Opening();			
}


//校验开度是否达到设定值			预防电机转动过程中中断损坏
//	如果工作在水位模式   而电机上升过程中达到设定开度值,会停止，控制结束
void Cmp_Opening(void)
{
	if(Control_Par.control_run==1)  //如果打开控制
	{
		if(Control_Par.setState==GATE_DOWN)
		{
			if(nosave_par.openingm<=Control_Par.set_openingm)		//控制状态打开--->下降状态--->真实开度<=期望开度--->电机停止、更新状态
			{
				TMC5160A_STOP();
				TMC5160A_EN=1;
				nosave_par.save_Mess_Par_flag=1;		
				Control_Par.setState=GATE_STOP;
				Control_Par.control_run=0;
			}
		}
		else if(Control_Par.setState==GATE_UP)
		{
			if(nosave_par.openingm>=Control_Par.set_openingm)
			{
				TMC5160A_STOP();
				TMC5160A_EN=1;
				nosave_par.save_Mess_Par_flag=1;		//存储当前步数
				Control_Par.setState=GATE_STOP;
				Control_Par.control_run=0;
			}
		}
	}
}


