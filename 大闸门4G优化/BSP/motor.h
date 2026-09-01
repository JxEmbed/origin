#ifndef __MOTOR_H
#define __MOTOR_H

#include "main.h"


void motor_init(void);
uint32_t TMC5160A_1_Read(unsigned char addr);
void TMC5160A_1_Write(unsigned char addr,uint32_t data);
uint64_t TMC5160A_1_WriteBuff(uint64_t data);

void motor_dirver(void);
void motor_control(void);
void motor_control_zhang(void);
void motor_AlmPend_init(void);
void Motor_Zhang_Init(void);
void Hall_init(void);
void motor_limit(void);
void motor_AutoControl(void);
void motor_control1(void);
/***************************/
#define MOTOR_LIMIT_UP      PDin(13)
#define MOTOR_LIMIT_DOWN    PDin(11)

/***************************/
//#define MOTOR_SPI_MISO  PFin(4)
//#define MOTOR_SPI_MOSI PFout(3)
//#define MOTOR_SPI_SCLK PFout(2)
//#define MOTOR_SPI_CS   PFout(1)

#define Power_48V_EN   PDout(9)

#define MOTOR_DRV_ENN  PFout(0)

/***************************/

#define MOTO_DIR      PFout(5)
#define MOTO_PUL      PFout(6)
#define MOTO_EN       PFout(7)


/********************************/
#define MOTO_PEND_IN  PDin(9)
#define MOTO_ALM_IN   PDin(10)

/*********************************/
#define MOTO_FWD      PFout(2)
#define MOTO_REV      PFout(4)

#endif
