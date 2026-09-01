#ifndef _RTC_H_
#define _RTC_H_


#include "stdint.h"
#include "type.h"
#include "gd32f20x_rtc.h"
#define STM32F103

#define USE_EXT_RCC 0

//占用7个字节 实际占用8字节(4字节对齐)
typedef struct {
    uint16_t years;
    uint8_t months;
    uint8_t days;
	uint8_t weeks;
    uint8_t hours;
    uint8_t minutes;
    uint8_t secs;
} DateTimeDef;
extern DateTimeDef g_rtc;
typedef struct {
    uint8_t hours;
    uint8_t minutes;
    uint8_t secs;
} TimeDef;

typedef DateTimeDef S_RTC;
/* RTC 时钟初始化 */
void HardwareRTC_Init(void);
/* 设置DateTime */
void RTC_SetDateTime(DateTimeDef datetime);
/* 获取DateTime */
DateTimeDef RTC_GetDateTime(void);
/* 用字符串设置时间 */
void RTC_SetDateTimeFromString(uint8_t* pbuffer);
/* 获取星期几 */
const char* RTC_DateTimeToWeekDay(DateTimeDef datetime);
/* 计算时间间隔 */
DateTimeDef RTC_DateTimeSpan(DateTimeDef dateTime1,DateTimeDef dateTime2);
/* DateTime 天数加减 */
DateTimeDef RTC_DateTimeAddDays(DateTimeDef dateTime,int16_t days);

void RTC_AWU_SET(void);
float RTC_Minute(DateTimeDef dateTime1,DateTimeDef dateTime2);
////每月天数表
//extern const uint8_t monthDaysTable[];

#ifndef STM32F103
//使用外部低速时钟作为RTC时钟(精度由高到低)
#define USE_LSE_AS_RTCCLK
//#define USE_HSE_AS_RTCCLK
//#define USE_LSI_AS_RTCCLK
#else

//使用外部低速时钟作为RTC时钟(精度由高到低)
//#define USE_LSE_AS_RTCCLK
//#define USE_HSE_AS_RTCCLK
#define USE_LSI_AS_RTCCLK
#endif
uint32_t DateTime_ToCounts(DateTimeDef datetime);
void RTC_SET_ALARM(u32 sec);
void DateTimeCpy(DateTimeDef dateTime1,DateTimeDef dateTime2);
void rtc_configuration(void);
void clock_init(void);
extern DateTimeDef lcd_rtc;
extern  DateTimeDef g_rtc;
extern	DateTimeDef temp_rtc;
extern DateTimeDef _4G_rtc;
extern DateTimeDef lora_rtc;
#include "main.h"
#endif

