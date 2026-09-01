#include "RTC.h"

#include "string.h"
#include "stdlib.h"

DateTimeDef g_rtc;
DateTimeDef lcd_rtc;
DateTimeDef temp_rtc;
DateTimeDef _4G_rtc;
DateTimeDef lora_rtc;
/* 选择RTC时钟源*/
static void RTC_SelectClk(void);
/* 判断该年份是否为闰年 */
static uint8_t IsLeapYear(uint16_t years);
/* DateTime 转换为相应的 计数值 */
 uint32_t DateTime_ToCounts(DateTimeDef datetime);
/* 计数值 转换为 DateTime */
static DateTimeDef Counts_ToDateTime(uint32_t secCount);
//每月天数表
static const uint8_t monthDaysTable[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
//星期几表
static const char WeekDayTable[][3*2+1] = {"星期天","星期一","星期二","星期三","星期四","星期五","星期六"};


/**
  * @brief  RTC配置
  * @param  None
  * @retval None
  */
void rtc_configuration(void)
{
    /* enable PMU and BKPI clocks 使能电源时钟和备份区域时钟*/
    rcu_periph_clock_enable(RCU_BKPI);
    rcu_periph_clock_enable(RCU_PMU);

    /* allow access to BKP domain 允许访问BKP区域*/
    pmu_backup_write_enable();

    //复位备份区域，开启外部低速振荡器
    /* reset backup domain */
    bkp_deinit();

    /* enable LXTAL 使能外部低速晶振 32.768K */
    rcu_osci_on(RCU_LXTAL);
    /* wait till LXTAL is ready */
    rcu_osci_stab_wait(RCU_LXTAL);

    //选择 RTC 时钟，并使能
    /* select RCU_LXTAL as RTC clock source */
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);

    /* enable RTC Clock 使能RTC时钟	 */
    rcu_periph_clock_enable(RCU_RTC);

    rtc_configuration_mode_enter();

    /* wait for RTC registers synchronization */
    rtc_register_sync_wait();

    /* wait until last write operation on RTC registers has finished 等待写RTC寄存器完成*/
    rtc_lwoff_wait();

    /* enable the RTC second interrupt 使能RTC秒中断*/
    rtc_interrupt_enable(RTC_INT_SECOND);

    /* wait until last write operation on RTC registers has finished 等待写RTC寄存器完成*/
    rtc_lwoff_wait();

    /* set RTC prescaler: set RTC period to 1s 设置预分频*/
    rtc_prescaler_set(32767);

    /* wait until last write operation on RTC registers has finished 等待写RTC寄存器完成*/
    rtc_lwoff_wait();

    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    nvic_irq_enable(RTC_IRQn, 1, 0);
}
/**
  * @brief  RTC时钟初始化
  * @param  None
  * @retval None
  */
void clock_init(void)
{
    if(0xA5A5 != bkp_data_read(BKP_DATA_0))
    {
        //第一次运行  初始化设置
        rtc_configuration();//RTC初始化
        /* wait until last write operation on RTC registers has finished */
        rtc_lwoff_wait();
        /* change the current time */
        rtc_counter_set(0);
        /* wait until last write operation on RTC registers has finished */
        rtc_lwoff_wait();
        rtc_lwoff_wait();//等待写RTC寄存器完成	  
        rtc_lwoff_wait();//等待写RTC寄存器完成
        bkp_data_write(BKP_DATA_0, 0xA5A5);//写配置标志
    }
    else
    {
        /* check if the power on reset flag is set */
        if(rcu_flag_get(RCU_FLAG_PORRST) != RESET)
        {
            printf("\\r\\n\\n Power On Reset occurred....");
        }
        else if(rcu_flag_get(RCU_FLAG_SWRST) != RESET)
        {
            /* check if the pin reset flag is set */
            printf("\\r\\n\\n External Reset occurred....");
        }
        printf("\\r\\n No need to configure RTC....");
        rtc_register_sync_wait();//等待RTC寄存器同步
        rtc_interrupt_enable(RTC_INT_SECOND);//使能RTC秒中断
        rtc_lwoff_wait();//等待写RTC寄存器完成
    }
    rtc_configuration_mode_exit();//退出配置模式， 更新配置
    rcu_all_reset_flag_clear();//清除复位标志；
}

/* 判断该年份是否为闰年 */
uint8_t IsLeapYear(uint16_t years)
{
    return (years %4 == 0)?((years%100 == 0)?((years%400==0)?1:0):1):0;
}

DateTimeDef Counts_ToDateTime(uint32_t secCount)
{
	DateTimeDef datetime = {0};
    uint32_t tmp=0;
    uint16_t tmp1=0;
	tmp=secCount/86400;//得到天数
    //超过一天
    if(0!=tmp) {
        tmp1=1970;//从1970年开始
        while(tmp>=365) {
            //是闰年
            if(IsLeapYear(tmp1)) {
                if(tmp>=366)
                    tmp-=366;//减掉闰年的天数
                else
                    break;
            }else {
                tmp-=365;//平年
            }
            tmp1++;
        }
        datetime.years=tmp1;//得到年份
        tmp1=0;
        //超过一个月
        while(tmp>=28) {
            //当年是闰年且轮循到2月
            if(IsLeapYear(datetime.years)&&tmp1==1) {
                if(tmp>=29)
                    tmp-=29;
                else
                    break;
            } else {
                if(tmp>=monthDaysTable[tmp1+1])
                    tmp-=monthDaysTable[tmp1+1];
                else
                    break;
            }
            ++tmp1;
        }
        datetime.months=++tmp1;//得到月份，tmp1=0表示1月，所以要加1
        datetime.days=++tmp;//得到日期，因为这一天还没过完，所以tmp只到其前一天，但是显示的时候要显示正常日期
    }
    tmp=secCount%86400;//得到秒钟数

    datetime.hours=tmp/3600;//小时
    datetime.minutes=(tmp%3600)/60;//分钟
    datetime.secs=(tmp%3600)%60;//秒
	return datetime;
}

/* DateTime 转换为相应的 计数值 */
uint32_t DateTime_ToCounts(DateTimeDef datetime)
{
uint16_t temp_years;
    u32 timeCount = 0;
    u16 years;
    u8 month;
    temp_years=datetime.years;
    if(temp_years<100)
    {
        temp_years+=2000;
    }
    //年
    for(years = 1970; years <temp_years; ++years)
        timeCount +=(IsLeapYear(years))?31622400UL:31536000UL;
    //月
    for(month = 1; month <datetime.months; ++month) {
        timeCount += monthDaysTable[month]*86400UL;
        //闰年二月份增加一天
        timeCount +=(IsLeapYear(temp_years)&&(month==2))?86400UL:0;
    }
    //日
    timeCount += 86400UL*(datetime.days-1);
    //时
    timeCount += 3600UL*datetime.hours;
    //分
    timeCount += 60UL*datetime.minutes;
    //秒
    timeCount += datetime.secs;
    return timeCount;
}



/* 获取星期几 */
const char* RTC_DateTimeToWeekDay(DateTimeDef datetime)
{
	u8 day;
	
	if(datetime.months<3){
		datetime.months += 12;
		--datetime.years;
	}
	day = (datetime.days+1+2*datetime.months+3*(datetime.months+1)/5+
		datetime.years+(datetime.years>>2)-datetime.years/100+datetime.years/400) % 7;
	return WeekDayTable[day];
}

/* 计算时间间隔 */
DateTimeDef RTC_DateTimeSpan(DateTimeDef dateTime1,DateTimeDef dateTime2)
{
	uint32_t secCount1 = DateTime_ToCounts(dateTime1);
	uint32_t secCount2 = DateTime_ToCounts(dateTime2);
	
	DateTimeDef timespan = Counts_ToDateTime( /*(secCount2>secCount1)?*/(secCount2-secCount1)/*:(secCount1-secCount2)*/ );
	timespan.years -= 1970;
	
	return timespan;
}

/* DateTime 天数加减 */
DateTimeDef RTC_DateTimeAddDays(DateTimeDef dateTime,int16_t days)
{
	uint32_t dateTimeCount  = DateTime_ToCounts(dateTime);
	dateTimeCount += days*86400UL;
	return Counts_ToDateTime(dateTimeCount);
}
/* 计算时间间隔 */
float RTC_Minute(DateTimeDef dateTime1,DateTimeDef dateTime2)
{
	float result=0;
	uint32_t secCount1 = DateTime_ToCounts(dateTime1);
	uint32_t secCount2 = DateTime_ToCounts(dateTime2);
	
	result=(float)(secCount2-secCount1)/60;   //计算时
	
	return result;
}
void DateTimeCpy(DateTimeDef dateTime1,DateTimeDef dateTime2)
{
	dateTime1.years=dateTime2.years;
	dateTime1.months=dateTime2.months;
	dateTime1.days=dateTime2.days;
	dateTime1.hours=dateTime2.hours;
	dateTime1.minutes=dateTime2.minutes;
	dateTime1.secs=dateTime2.secs;
}