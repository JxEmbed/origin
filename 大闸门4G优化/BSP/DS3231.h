#ifndef __DS3231_H
#define __DS3231_H

#include "main.h"
#include "RTC.h"

#define DS3231_SOFTWARE_ENABLE 1
void DS3231_GetRTC(void);
void DS3231_SetRTC(DateTimeDef dateTime);
uint8_t IIC_DS3231_ByteWrite(uint8_t WriteAddr , uint8_t date);
uint8_t DS3231_setDate(uint8_t year,uint8_t mon,uint8_t day);
uint8_t DS3231_setTime(uint8_t hour , uint8_t min , uint8_t sec);
void DS3231_init(void);
void DS3231_SoftWare_init(void);

#define DS3231_SDA_W PCout(9)
#define DS3231_SDA_R PCin(9)

#define DS3231_SCL_W PAout(8)

#define IIC_SDA_H  DS3231_SDA_W=1
#define IIC_SDA_L  DS3231_SDA_W=0
#define IIC_SCL_H  DS3231_SCL_W=1
#define IIC_SCL_L  DS3231_SCL_W=0


#endif



