/*!
    \file    main.h
    \brief   the header file of main

    \version 2023-06-30, V2.5.0, firmware for GD32F20x
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#ifndef __MAIN_H
#define __MAIN_H

#include "i2c.h"
#include "at24cxx.h"
#include "Bluetooth.h"
#include "type.h"
#include "RTC.h"
#include "DS3231.h"
#include "RS232.h"
#include "task1.h"
#include "RS485.h"
#include "queue.h"
#include "semphr.h"
#include "tool.h"
#include "Relay.h"
#include "Hall_Flow.h"
#include "sys.h"
#include "4GCat1.h"
#include "lowPower.h"
#include "delay.h"
#include "onenet.h"
#include "L76K.h"
#include "ultrasonic.h"
//#include "Lora.h"
#include "motor.h"
#include "data.h"
#include "spi.h"
#include "w25qxx.h"
#include "tims.h"
#include "power.h"
#include "mmc_sd.h"
#include "wdgt.h"
#include "SEGGER_RTT.h"
#include "myOTA.h"
#include "gdflash.h"
#include "other_bsp.h"

/* led spark function */
void led_spark(void);

extern QueueHandle_t RS485_semRx;  //定义互斥信号量
extern QueueHandle_t _4GCat1_semRx;  //定义互斥信号量
extern QueueHandle_t LCD_semTx;   //定义接受
extern QueueHandle_t SD_SemSR;    //SD卡写入读取信号量
extern QueueHandle_t Flash_SemSR; //flash写入读取信号量
extern QueueHandle_t SDFF_SemSR;    //SD卡写入读取信号量 
extern QueueHandle_t DS3231_SemSR;  //DS3231信号量
extern QueueHandle_t TMC5160A_SPI_SemSR;  //DS3231信号量

#define DS3231_xBlockTime 1000
#define TMC5160A_SPI_xBlockTime 5000
typedef struct{
	DateTimeDef dateTime;  //8
	float water[8];//40    32
}M_STRUCT;
typedef struct{
	DateTimeDef starttime;
	
	float f1;
	float f2;
	float f3;
	float f4;
	float f5;
	float f6;
	float f7;
	float f8;
	float f9;
	float f10;
}W_STR;

#endif /* __MAIN_H */





