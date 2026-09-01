#ifndef __TMC5160A_H
#define __TMC5160A_H

#include "main.h"
#include "TMC5160_Register.h"
/**
REFR_DIR   PE10
REFL_STEP  PE12
DRV_ENN    PA10
*/
#define TMC5160A_REFR_DIR      PEout(10)
#define TMC5160A_REFL_STEP     PEout(12)
#define TMC5160A_DRV_ENN       PAout(10)
#define TMC5160A_DIR TMC5160A_REFR_DIR
#define TMC5160A_PUL TMC5160A_REFL_STEP
#define TMC5160A_EN  TMC5160A_DRV_ENN

#define TMC5160A_SPI_MISO  PDin(14)
#define TMC5160A_SPI_MOSI PDout(12)
#define TMC5160A_SPI_SCLK PDout(10)
#define TMC5160A_SPI_CS   PDout(8)

//诊断信号输入
#define TMC5160A_DIAG0   PFin(14)
#define ENABLE_CONTROL()        TMC5160A_EN=0     //DRV_ENN引脚置低电平使能电机
#define DISENABLE_CONTROL()     TMC5160A_EN=1        //DRV_ENN引脚置高电平不使能电机

#define tmc5160_writeInt TMC5160A_1_Write
#define tmc5160_readInt  TMC5160A_1_Read
void TMC5160A_init(void);
void TMC5160A_SPI_init(void);
void motor_A_test(void);
void TMC5160_SPIMode_init(void);
uint64_t TMC5160A_1_WriteReg(u8 addr,uint64_t data);
uint64_t TMC5160A_1_ReadReg (u8 addr,uint64_t data);
void motor_A_test1(void);
void TMC5160A_test1(void);
void TMC5160A_RUNUP(void);
void TMC5160A_RUNDOWN(void);
void TMC5160A_STOP(void);
void TMC5160A_test(void);
void TMC5160A_TestStep(void);
void TMC5160A_TestStep2(void);
void TMC5160A_TestStep_Count(void);
void TMC5160A_ChangeSpeed(void);
void TMC5160A_RunSetting2(float speed);
void TMC5160A_RUN2(float speed);
void TMC5160_rotate(int32_t velocity);
void TMC5160_stop(void);
void TMC5160_init(void);
void TMC5160_SetCurrent(void);
void TMC5160_ReadTSTEP_Value(void);
void TMC5160_SetTCOOLTHRS(void);
void TMC5160A_DIAGinit(void);    //诊断信号输入初始化
void TMC5160_ClearStallGuard(void);
void TMC5160_TEST(void);
void TMC5160_SetSGT(void);
void TMC5160_SetAMAX(void);
void TMC5160_StallGuardProcess(void);
void TMC5160_moveBy(int32_t position);

typedef enum
{
	GATE_STOP=0,
	GATE_UP,
	GATE_DOWN,
}gate_control_enum;


extern u8 testStep;
extern u32 testStepCount;
extern __TMC5160 TMC5160_Registers;
#endif

