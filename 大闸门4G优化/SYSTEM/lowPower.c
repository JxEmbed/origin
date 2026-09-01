#include "lowPower.h"

void lowPower(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_GPIOD);
	rcu_periph_clock_enable(RCU_GPIOE);
	rcu_periph_clock_enable(RCU_GPIOF);
	rcu_periph_clock_enable(RCU_GPIOG);
	gpio_init(GPIOA,GPIO_MODE_AIN,GPIO_OSPEED_50MHZ,GPIO_PIN_ALL);
	gpio_init(GPIOB,GPIO_MODE_AIN,GPIO_OSPEED_50MHZ,GPIO_PIN_ALL);
	gpio_init(GPIOC,GPIO_MODE_AIN,GPIO_OSPEED_50MHZ,GPIO_PIN_ALL);
	gpio_init(GPIOD,GPIO_MODE_AIN,GPIO_OSPEED_50MHZ,GPIO_PIN_ALL);
	gpio_init(GPIOE,GPIO_MODE_AIN,GPIO_OSPEED_50MHZ,GPIO_PIN_ALL);
	gpio_init(GPIOF,GPIO_MODE_AIN,GPIO_OSPEED_50MHZ,GPIO_PIN_ALL);
	gpio_init(GPIOG,GPIO_MODE_AIN,GPIO_OSPEED_50MHZ,GPIO_PIN_ALL);
	rcu_periph_clock_enable(RCU_PMU);
	pmu_wakeup_pin_enable();
	pmu_to_standbymode();
	
}

void LCD_LowPower_init(void)
{
	//按键检测
	rcu_periph_clock_enable(RCU_GPIOD);
	gpio_init(GPIOD,GPIO_MODE_IPU,GPIO_OSPEED_50MHZ,GPIO_PIN_7);
	
	rcu_periph_clock_enable(RCU_GPIOE);
	gpio_init(GPIOE,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_12);
	
	rcu_periph_clock_enable(RCU_GPIOD);
	gpio_init(GPIOD,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_8);
	PEout(12)=1;
	
		//下限位中断初始化
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOD, GPIO_PIN_SOURCE_7);//connect key     EXTI line to key GPIO pin
	exti_init(EXTI_7, EXTI_INTERRUPT, EXTI_TRIG_FALLING);//下降沿中断
	exti_interrupt_flag_clear(EXTI_7);//清中断标志
	nvic_irq_enable(EXTI5_9_IRQn, 2U, 0U);//enable and set key EXTI interrupt to the lowest priority 
}

void LowPower_Test(void)
{
	Power_30V_DIS;
	gpio_bit_reset(GPIOE,GPIO_PIN_5);   //关闭5V升压
	gpio_bit_reset(GPIOE,GPIO_PIN_6);
	PCout(5)=0;        //关闭485电源
	PCout(4)=1;        //关闭485芯片电源
	gpio_bit_set(GPIOE, GPIO_PIN_9); //关闭232芯片电源
	
	PGout(6)=1;
	PGout(8)=1;
	PGout(7)=1;
	
	mcu_deepsleep();
}
void mcu_standby(void)
{
	pmu_to_standbymode();
}
void mcu_deepsleep(void)
{
	rcu_system_clock_source_config(RCU_CKSYSSRC_IRC8M);
	pmu_to_deepsleepmode(PMU_LDO_NORMAL, WFI_CMD);
}

void RTC_AlarmConfig(void)
{
    /* enable PMU and BKPI clocks */
    rcu_periph_clock_enable(RCU_BKPI);
    rcu_periph_clock_enable(RCU_PMU);
    /* allow access to backup domain */
    pmu_backup_write_enable();
    /* reset backup domain */
    bkp_deinit();

    /* enable IRC40K */
    rcu_osci_on(RCU_IRC40K);
    /* wait till IRC40K is ready */
    rcu_osci_stab_wait(RCU_IRC40K);
    /* select RCU_IRC40K as RTC clock source */
    rcu_rtc_clock_config(RCU_RTCSRC_IRC40K);
    /* enable RTC Clock */
    rcu_periph_clock_enable(RCU_RTC);

    /* wait for RTC registers synchronization */
    rtc_register_sync_wait();
    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();
    /* enable the RTC alarm interrupt */
    rtc_interrupt_enable(RTC_INT_ALARM);
    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();
    /* set RTC prescaler: set RTC period to 1s */
    rtc_prescaler_set(40000);
    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();
//    rtc_counter_set(0U);
//    /* wait until last write operation on RTC registers has finished */
//    rtc_lwoff_wait();
//    rtc_alarm_config(ALARM_TIME_INTERVAL);
//    /* wait until last write operation on RTC registers has finished */
//    rtc_lwoff_wait();
	
	//配置NVIC
	nvic_irq_enable(RTC_Alarm_IRQn, 3U, 0U);
	
    /* EXTI configuration */
    exti_deinit();
    exti_init(EXTI_17, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    rtc_flag_clear(RTC_FLAG_ALARM);
    exti_interrupt_flag_clear(EXTI_17);
    exti_interrupt_enable(EXTI_17);
}

/*
	RTC 是 20 位向上计数器（RLR 只有低 20 位有效）  0xF FFFF = 1 048 575
	时钟配置为1hz	
	最长定时=1 秒 +1 
	1 048 575 秒 ≈ 12 天 3 小时 16 分钟 15 秒
*/
void RTC_AlarmSet(uint32_t alarm)
{
	/* update RTC alarm time */
	rtc_register_sync_wait();
	/* wait until last write operation on RTC registers has finished */
	rtc_lwoff_wait();
	rtc_counter_set(0U);
	/* wait until last write operation on RTC registers has finished */
	rtc_lwoff_wait();
	rtc_alarm_config(alarm);
	/* wait until last write operation on RTC registers has finished */
	rtc_lwoff_wait();
}

