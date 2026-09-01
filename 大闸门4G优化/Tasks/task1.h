#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "RTC.h"
#include "timers.h"
/***************************************************************************
函数声明
***************************************************************************/
 void StartTask(void *pvParameters);
 void vTask1(void *pvParameters);
 void vTask2(void *pvParameters);
 void vTask3(void *pvParameters);
 void vTask4(void *pvParameters);
 void AppTaskCreate (void);
 void SystemTimer(void);
 void vTimerCallback(xTimerHandle pxTimer);
/***************************************************************************
变量声明
***************************************************************************/
 extern  TaskHandle_t xHandleTask0;
 extern  TaskHandle_t xHandleTask1;
 extern  TaskHandle_t xHandleTask2;
 extern  TaskHandle_t xHandleTask3;
 extern  TaskHandle_t xHandleTask4;
 
extern u32 lora_send;
extern u32 lora_rece;
 




