#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H
#include "main.h"

void Bluetooth_Init(void);
void Bt_SendStr(char * str);
void Bt_RxProcess(void);
void Bt_SendWatLev(void);
#define Bluetooth_PowerEN   PDout(3)
#define Bt_Wakeup           PDout(5)
#endif


