#include "tims.h"
#include "math.h"
#include "LCD.h"
#include "AnglePulse.h"
#include "TMC5160A.h"
/*
1s判断一次，连续判断5个
*/
void timer1_config(void)
{
    /* ----------------------------------------------------------------------------
    TIMER1 Configuration:
	120 000 000       1000
	120 000 000/120/1000000=1000hz
	0.001s=1ms
	
	
	计算公式:
		f_out=f_clk/((psc+1)*(arr+1))
    ---------------------------------------------------------------------------- */
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER1);

    timer_deinit(TIMER1);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER1 configuration */
    timer_initpara.prescaler         = 119;						//预分频寄存器值
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;		//向上计数
    timer_initpara.period            = 999999;					//自动重装载寄存器值
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;		//给数字滤波器/死区单元 用的 二次分频
    timer_init(TIMER1, &timer_initpara);

    /* enable the TIMER interrupt */
    timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
    timer_interrupt_enable(TIMER1, TIMER_INT_UP);
	
	nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    nvic_irq_enable(TIMER1_IRQn, 1, 1);
	
	timer_enable(TIMER1);
}
u32 time3_freq;
void timer3_config(void)
{
    /* ----------------------------------------------------------------------------
    TIMER1 Configuration:
	120 000 000       1000
    TIMER1CLK = SystemCoreClock/12000 = 10KHz, the period is 1s(10000/10000 = 1s).
	12 000 000/12=1000 000    1000 000/10=100 000 
	120 000 000 /12=10 000 000
	10 000 000/10 000=1000
	120000000
	120000
	12 1000000
	120 1000000
    ---------------------------------------------------------------------------- */
//  time3_freq=rcu_clock_freq_get(CK_SYS);//120000000
//	time3_freq=rcu_clock_freq_get(CK_AHB);//120000000
//	time3_freq=rcu_clock_freq_get(CK_APB1);//60000000
//	time3_freq=rcu_clock_freq_get(CK_APB2);//120000000
	timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER3);

    timer_deinit(TIMER3);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER1 configuration */
    timer_initpara.prescaler         = 11999;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 9999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;	
    timer_init(TIMER3, &timer_initpara);

    /* enable the TIMER interrupt */
    timer_interrupt_flag_clear(TIMER3, TIMER_INT_FLAG_UP);
    timer_interrupt_enable(TIMER3, TIMER_INT_UP);

	nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    nvic_irq_enable(TIMER3_IRQn, 1, 1);
	
	timer_enable(TIMER3);
}
void timer2_config(u16 prescaler,u32 period)
{
    /* ----------------------------------------------------------------------------
    TIMER1 Configuration:
	120 000 000       1000
 

	120 000 000/12/2000=10 000 000/2000=5000hz
	1/5000s=0.0002s
    ---------------------------------------------------------------------------- */
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER2);

    timer_deinit(TIMER2);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = prescaler;  			
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = period;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER2, &timer_initpara);

    /* enable the TIMER interrupt */
    timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);
    timer_interrupt_enable(TIMER2, TIMER_INT_UP);
    
	nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    nvic_irq_enable(TIMER2_IRQn, 1, 1);
	
	timer_enable(TIMER2);
}

//1S定时器
void TIMER1_IRQHandler()
{
	if(SET == timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_UP)) 
	{
        /* clear update interrupt bit */
        timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
		
		Calc_Opening();
		
		if(_4GCat1_Usart.rxtimer>0)
		{
			_4GCat1_Usart.rxtimer--; 
			if(_4GCat1_Usart.rxtimer==0)
			{
				_4GCat1_Usart.rxlen=_4GCat1_Usart.rxpointer; 
				_4GCat1_Usart.rxflag=1;
				_4GCat1_Usart.rxpointer=0;
				xSemaphoreGive(_4GCat1_Usart.semRx);
			}
		} 
		
    }
}
u32 time2_count=0;


void TIMER2_IRQHandler(void)
{
	if(SET == timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_UP)) 
	{
		time2_count++;
		/* clear update interrupt bit */
		timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);
		
		Calc_Speed();			//计算速度检测堵转
	}
}


u32 time3_count=0;
/*
1秒计时
*/
void TIMER3_IRQHandler(void)
{
	if(SET == timer_interrupt_flag_get(TIMER3, TIMER_INT_FLAG_UP)) 
	{
		WatLev_HighCheck();		//水位安全检测
		gd32_wdgt_feed_dog();	
//		Calc_Speed();			//计算速度检测堵转
		timer_interrupt_flag_clear(TIMER3, TIMER_INT_FLAG_UP);
		time3_count++;
		
//测试RTC闹钟唤醒
		if(time3_count > 10)
		{
//			RTC_AlarmSet(5);
//			time3_count=0;
		}
/*************************************************************
开度为0  后水位、超声波 10min采集一次(串口屏可设置)		采集间隔
  不为0	 后水位、超声波 实时采集
*************************************************************/
	
	//测流槽为测流设备时才需自己读取后水位和测流		
	if( dev_SavePar.TypeUltra==0)
	{
		if(PDin(11)==1)
		{
			nosave_par.Read_AftLT++;
			if(nosave_par.Read_AftLT >= dev_SavePar.collect_interval)
			{
				nosave_par.Read_AftLT=0;
				nosave_par.Read_AftLF=1;
			}
		}
		else 
		{
			nosave_par.Read_AftLT=0;
			nosave_par.Read_AftLF=1;
		}
	}
	//读取外部设备
	else if(dev_SavePar.TypeUltra==1)
	{
		if(PDin(11)==1)
		{
			nosave_par.Read_ExterndevT=0;
		}
		else
		{
			nosave_par.Read_ExterndevT++;
		}
		if(nosave_par.Read_ExterndevT >= (dev_SavePar.data_interval-60))
		{
			nosave_par.Read_ExterndevF=1;
			nosave_par.Read_ExterndevT=0;
		}
	}

		
/*************************************************************
如果开度为0   则1小时存一次数据
如果开度不为0 则以设置的数据间隔存数据
*************************************************************/
		nosave_par.RunLog_ParSave_t++;
		if(nosave_par.RunLog_ParSave_t>=dev_SavePar.data_interval)
		{
			
			nosave_par.RunLog_ParSave_t=0;
			nosave_par.RunLog_ParSave_flag=1;
		}
		
//		nosave_par.data_t++;
//		if(nosave_par.opening>0&&nosave_par.data_t>=(dev_SavePar.data_interval))
//		{
//			nosave_par.data_t=0;
//			nosave_par.data_flag=1;
//		}
//		if(nosave_par.opening==0&&nosave_par.data_t>=(60*60))
//		{
//			nosave_par.data_t=0;
//			nosave_par.data_flag=1;
//		}
/*************************************************************
屏幕背光关闭倒计时
*************************************************************/
		if(nosave_par.LCD_timecount>0)
		{
			nosave_par.LCD_timecount--;
			LCD_POWER_EN=1;
		}
		else
		{
			nosave_par.current_identity=IDENTITY_NULL;
			LCD_POWER_EN=0;
			//LCD_POWER_EN=1;
		}
/*************************************************************
开度为0时  	 	 发送运行数据的间隔：6小时
  不为0时        发送运行数据的间隔：15分钟
*************************************************************/
	if(PDin(11)==1)
	{
		nosave_par.dataUpload_t++;
		if(nosave_par.dataUpload_t>dev_SavePar.send_interval) 
		{
			nosave_par.dataUpload_t=0;
			nosave_par.dataUpload_flag=1;
		}
	}
	else
	{
		nosave_par.dataUpload_t++;
		if(nosave_par.dataUpload_t>(15*60)) 	//15分钟发送一次
		{
			nosave_par.dataUpload_t=0;
			nosave_par.dataUpload_flag=1;
		}
	}
/*************************************************************
		检查版本更新的间隔：半小时
*************************************************************/
		nosave_par.checkup_t++;
		if(nosave_par.checkup_t>(5*60))   //1小时间隔
		{
			nosave_par.checkupflag=1;
			nosave_par.checkup_t=0;
		}
/*************************************************************
		发送设置参数的间隔：2小时
*************************************************************/
		nosave_par.dev_Save_t++;
		if(nosave_par.dev_Save_t>(2*60*60))   //1小时间隔
		{
			nosave_par.dev_Save_flag=1;
			nosave_par.dev_Save_t=0;
		}
/*************************************************************
		发送版本的间隔：1小时
*************************************************************/
		nosave_par.SendVer_t++;
		if(nosave_par.SendVer_t>(24*60*60))    //发送版本信息
		{
			nosave_par.SendVerflag=1;
			nosave_par.SendVer_t=0;
		}
/*************************************************************
		检查NQTT链接状态的间隔：10秒
*************************************************************/
		nosave_par.checkNet_t++;
		if(nosave_par.checkNet_t>60*4)  //10秒查一次
		{
			nosave_par.checkNetflag=1;
			nosave_par.checkNet_t=0;
		}
/*************************************************************
		发送权限信息：1小时加6秒
*************************************************************/		
		nosave_par.Author_Par_t++;
		if(nosave_par.Author_Par_t>(60*60+6))
		{
			nosave_par.Author_Par_flag=1;
			nosave_par.Author_Par_t=0;
		}
		
/*************************************************************
		每天0点置位 nosave_par.L76K_SendF =1  进行定位
*************************************************************/	
		if((g_rtc.hours==0) && (g_rtc.minutes==0) && (g_rtc.secs==0))
		{
			nosave_par.L76K_SendF=1;
		}

		
	}
}
