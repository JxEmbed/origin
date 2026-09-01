#ifndef __TIME_SLICE_POLLING_H
#define __TIME_SLICE_POLLING_H
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "misc.h"
#include "main.h"

extern uint8_t TIM2_Update;

void TimeSlicePolling_Sys_Init(u8 SYSCLK);


	// 任务结构  
typedef struct _TASK_COMPONENTS  
{
	uint8_t Run;                 // 程序运行标记：0-不运行，1运行  
	uint16_t Timer;              // 计时器  
	uint16_t ItvTime;              // 任务运行间隔时间  
	void (*TaskHook)(void);    // 要运行的任务函数  
} TASK_COMPONENTS;       // 任务定义  

#define TASKS_MAX 4

void TaskRemarks(void);
void TaskProcess(void);
void task1(void);
void task2(void);
void task3(void);
void task4(void);
void task5(void);
void task6(void);
#endif




