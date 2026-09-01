#ifndef __ANGLEPULSE_H
#define __ANGLEPULSE_H
#include "main.h"

void AnglePulse_init(void);
void Calc_Opening(void);
void Cmp_Opening(void);
void Calc_Speed(void);

extern u32 forw_pulse;
extern u32 back_pulse;
extern u32 pulse_step;

#define Stall_Threshold 10

extern s32 AngleP_Step;
extern u32 AngleP_Speed;
#endif


