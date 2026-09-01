#ifndef __LOWPOWER_H
#define __LOWPOWER_H

#include "main.h"

void lowPower(void);
void LCD_LowPower_init(void);
void LowPower_Test(void);
void mcu_standby(void);
void mcu_deepsleep(void);
void RTC_AlarmConfig(void);

#define LCD_SHOWTIME (5*60)
#define ALARM_TIME_INTERVAL 5
#endif


