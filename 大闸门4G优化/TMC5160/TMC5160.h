/*
 * TMC5160.h
 *
 *  Created on: 2024.06.12
 *      Author: LZY
 */
 
#include "stdint.h"
#include "TMC5160_Register.h"
#include "TMC5160_Constants.h"
#include "TMC5160_Fields.h"

#ifndef TMC_IC_TMC5160_H_
#define TMC_IC_TMC5160_H_


// ramp modes (Register TMC5160_RAMPMODE)
#define TMC5160_MODE_POSITION  0            //位置（定位）模式
#define TMC5160_MODE_VELPOS    1            //速度模式的正方向（电机将会以固定转速转动）
#define TMC5160_MODE_VELNEG    2            //速度模式的负方向（电机将会以固定转速转动）
#define TMC5160_MODE_HOLD      3            //保持模式



extern uint32_t register_value;            //一个临时变量


// Generic mask/shift macros
#define FIELD_GET(data, mask, shift) \
	(((data) & (mask)) >> (shift))
#define FIELD_SET(data, mask, shift, value) \
	(((data) & (~(mask))) | (((value) << (shift)) & (mask)))

// 获取某个寄存器的某一个参数
#define TMC5160_FIELD_READ(address, mask, shift) \
	FIELD_GET(tmc5160_readInt(address), mask, shift)
// 修改某个寄存器的某一个参数，其他位保持不变
#define TMC5160_FIELD_WRITE(address, mask, shift, value) \
	(tmc5160_writeInt(address, FIELD_SET(tmc5160_readInt(address), mask, shift, value)))




void TMC5160_init(void);
void TMC5160_MovePosition_SixPoint(int32_t position, uint32_t velocityMax);
void TMC5160_MovePosition_T(int32_t position, uint32_t velocityMax);
void TMC5160_moveBy(int32_t position);
void TMC5160_moveTo(int32_t position);
uint8_t GetAixsPositionRached(void);
uint8_t GetAixsVelocityRached(void);
int32_t TMC5160_Get_Position(void);
int32_t TMC5160_Get_velocity(void);
void TMC5160_Zero_Position(void);
void TMC5160_Encoder_init(Encoder_MODE mode, uint32_t Count, int32_t Encoder_resolution);
void TMC5160_Stop_switch_Enable(uint8_t Stop_switch, uint8_t Switch_State);
void TMC5160_Stop_switch_polarity(uint8_t Stop_switch, uint8_t polarity);
void TMC5160_Emergency_Stop(uint8_t state);


void TMC5160_rotate(int32_t velocity);
void TMC5160_right(uint32_t velocity);
void TMC5160_left(uint32_t velocity);
void TMC5160_stop(void);

void tmc5160_writeInt(uint8_t address, int32_t value);
int32_t tmc5160_readInt(uint8_t address);








#endif /* TMC_IC_TMC5160_H_ */
