#ifndef __TIMS_H
#define __TIMS_H

#include "main.h"

void timer1_config(void);
void timer2_config(u16 prescaler,u32 period);
void timer3_config(void);

extern u32 time2_count;
extern u32 time3_count;
#endif

