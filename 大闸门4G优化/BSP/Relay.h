#ifndef __RELAY_H
#define __RELAY_H

#include "main.h"

void DoublePole_init(void);
void SinglePole_init(void);

void MOS_init(void);
void MOS_test(void);

//低电平断开    高电平导通  VCCFAN通5V
#define MOS_PIN PGout(5)			
#endif

