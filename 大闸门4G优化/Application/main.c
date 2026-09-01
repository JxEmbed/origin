/*!
    \file    main.c
    \brief   led spark with systick, USART print and key example

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

#include "main.h"
#include "systick.h"
//#include "gd32f20x_eval.h"

#include <stdio.h>
#include "string.h"
#include "TMC5160A.h"
#include "gd32f20x_rcu_add.h"
QueueHandle_t RS485_semRx;  //定义互斥信号量
//QueueHandle_t _4GCat1_semRx;  //定义互斥信号量
QueueHandle_t LCD_semTx;   //定义接受
QueueHandle_t SD_SemSR;    //SD卡写入读取信号量 
QueueHandle_t Flash_SemSR; //flash写入读取信号量
QueueHandle_t SDFF_SemSR;    //SD卡写入读取信号量 
QueueHandle_t DS3231_SemSR;  //DS3231信号量
QueueHandle_t TMC5160A_SPI_SemSR;  //DS3231信号量
/*
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

//解除调试端口复用
void JtagOff(void)
{
	// 关闭 SWJ
	rcu_periph_clock_enable(RCU_AF);
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP,ENABLE);
}

void init_all(void)
{
	/* configure systick */
	delay_init();
	JtagOff();
}
int main(void)
{
	Set_NVIC_Offset();			//远程升级 设置中断向量表偏移地址

	init_all();
	
	RCC_GetClocksFreq(&rccClock);
	W25QXX_Init();
//	strt=sizeof(M_STRUCT);
//	strt1=sizeof(W_STR);
	SEGGER_RTT_Init();
	SEGGER_RTT_SetTerminal(0); 
	SEGGER_RTT_printf(0,RTT_CTRL_TEXT_GREEN"App Run!\r\n");


//	wdgt_init();
	temp_rtc.years=23;
	temp_rtc.months=10;
	temp_rtc.days=20;
	
	
	RS485_semRx = xSemaphoreCreateBinary(); //创建互斥信号量
//_4GCat1_semRx = xSemaphoreCreateBinary(); //创建互斥信号量
	LCD_semTx   = xSemaphoreCreateBinary(); //创建互斥信号量
	SD_SemSR    = xSemaphoreCreateBinary(); //创建互斥信号量
	Flash_SemSR = xSemaphoreCreateBinary(); //创建互斥信号量
	_4GCat1_Usart.semRx=xSemaphoreCreateBinary(); //创建互斥信号量
	_4GCat1_Usart.semTx=xSemaphoreCreateBinary(); //创建互斥信号量
	SDFF_SemSR=xSemaphoreCreateMutex();         //创建互斥信号量
	xSemaphoreGive(SDFF_SemSR);       //帧尾
	DS3231_SemSR=xSemaphoreCreateMutex();       //创建互斥信号量
	xSemaphoreGive(_4GCat1_Usart.semTx);       //帧尾
	xSemaphoreGive(DS3231_SemSR);       //帧尾
	//TMC5160A
	TMC5160A_SPI_SemSR=xSemaphoreCreateMutex();         //创建互斥信号量
	xSemaphoreGive(TMC5160A_SPI_SemSR);       //帧尾
	
//	nvic_configuration();
//	rtc_configuration_test();
//	while(1);
	
	//xSemaphoreCreateMutex
	xTaskCreate( StartTask, "StartTask",2000, NULL,1, &xHandleTask0);
	 /* 创建任务 */
//	AppTaskCreate();
  /* 启动任务调度，开始执行任务 */
	vTaskStartScheduler();
	while(1);
}


