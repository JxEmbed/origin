/*
创建时间：2019-07-24
作者：
应用环境：
	1.芯片型号：STM32F030K6T6
	2.主频：48M
功能：采用时间片轮询的方法实现任务调度
使用基本定时器TIM6，定时时间为1ms
定时时间计算公式((1+TIM_Prescaler )*(1+TIM_Period )）/48M=((1+4799)*(1+9)）/48M=1ms=0.001s
                                                              48000/48000000=1ms
修改时间：2019-07-26
修改内容：因为STM32F030K6T6没有TIM6,遂将TIM6修改为TIM3

修改时间：2020-09-11
定时时间计算公式((1+TIM_Prescaler )*(1+TIM_Period )）/72M=((1+7999)*(1+8)）/72M=1ms=0.001s
                                                                     8000*9/72000000=72000/72000000
																																		  Tout= ((arr+1)*(psc+1))/Tclk；
修改时间：2020-12-07
应用环境：
	1.芯片型号：STM32F103VCT6
	2.主频：72M
	3.TIM3频率：36M
	修改时间：2020年12月22日18:41:46
修改内容：
	将TIM3改为TIM2
*/
#include "Time_slice_polling.h"
#include "stdio.h"

#include "hmi_driver.h"


uint8_t TIM2_Update=0;

//u32 times=0;
//时基=10ms
//void TimeSlicePolling_Sys_Init(u8 SYSCLK)
//{
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStructe;//定义结构体
//	NVIC_InitTypeDef  NVIC_InitStructe;//定义结构体
//	
//	//1.使能定时器时钟
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
//	
//	//2.初始化定时器，配置ARR,PSC
//	TIM_DeInit(TIM2);
//	TIM_TimeBaseInitStructe.TIM_Period=SYSCLK-1; 
//	TIM_TimeBaseInitStructe.TIM_Prescaler=1000-1;   //预分频数
//	TIM_TimeBaseInitStructe.TIM_ClockDivision=TIM_CKD_DIV1;
//	TIM_TimeBaseInitStructe.TIM_CounterMode=TIM_CounterMode_Up;
//	      
//	
//	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructe);
//	//3.开启定时器中断，配置NVIC
//	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//清除更新中断标志位
//	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
//	
//	
//	NVIC_InitStructe.NVIC_IRQChannel=TIM2_IRQn;   //指定要启用或禁用的IRQ通道。
//	NVIC_InitStructe.NVIC_IRQChannelPreemptionPriority=2;   //指定IRQ通道的抢占优先级
//	NVIC_InitStructe.NVIC_IRQChannelSubPriority=2;    // 指定指定IRQ通道的子优先级级别
//	NVIC_InitStructe.NVIC_IRQChannelCmd=ENABLE;
//	NVIC_Init(&NVIC_InitStructe);
//	
//	//4.使能定时器
//	TIM_Cmd(TIM2,ENABLE);
//}




/************************************************************************************** 
* FunctionName   : TaskProcess() 
* Description    : 任务处理 
* EntryParameter : None 
* ReturnValue    : None 
**************************************************************************************/  

/*  */
//void TIM2_IRQHandler(void)
//{
//static	u16 timea=0;
//	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
//	{
//		timea++;
//		if(timea>=1000)
//		{
//			times++;
//			timea=0;

//		}
//		
//		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//清除更新中断标志位
//		TIM2_Update=1;
////		TaskRemarks();//任务标志处理
//	}
//}








