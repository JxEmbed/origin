/*
 * TMC5160_Constants.h
 *
 *  Created on: 2024.06.12
 *      Author: tmctest
 */

#ifndef TMC_IC_TMC5160_TMC5160_CONSTANTS_H_
#define TMC_IC_TMC5160_TMC5160_CONSTANTS_H_


#define TMC5160_REGISTER_COUNT   128
#define TMC5160_MOTORS           1
#define TMC5160_WRITE_BIT        0x80
#define TMC5160_ADDRESS_MASK     0x7F
#define TMC5160_MAX_VELOCITY     8388096


// ramp modes (Register TMC5160_RAMPMODE)
#define TMC5160_MODE_POSITION  0            //位置（定位）模式
#define TMC5160_MODE_VELPOS    1            //速度模式的正方向（电机将会以固定转速转动）
#define TMC5160_MODE_VELNEG    2            //速度模式的负方向（电机将会以固定转速转动）
#define TMC5160_MODE_HOLD      3            //保持模式

// limit switch mode bits (Register TMC5160_SWMODE)
#define TMC5160_SW_STOPL_ENABLE    0x0001
#define TMC5160_SW_STOPR_ENABLE    0x0002
#define TMC5160_SW_STOPL_POLARITY  0x0004
#define TMC5160_SW_STOPR_POLARITY  0x0008
#define TMC5160_SW_SWAP_LR         0x0010
#define TMC5160_SW_LATCH_L_ACT     0x0020
#define TMC5160_SW_LATCH_L_INACT   0x0040
#define TMC5160_SW_LATCH_R_ACT     0x0080
#define TMC5160_SW_LATCH_R_INACT   0x0100
#define TMC5160_SW_LATCH_ENC       0x0200
#define TMC5160_SW_SG_STOP         0x0400
#define TMC5160_SW_SOFTSTOP        0x0800

// Status bits (Register TMC5160_RAMPSTAT)
#define TMC5160_RS_STOPL          0x0001
#define TMC5160_RS_STOPR          0x0002
#define TMC5160_RS_LATCHL         0x0004
#define TMC5160_RS_LATCHR         0x0008
#define TMC5160_RS_EV_STOPL       0x0010
#define TMC5160_RS_EV_STOPR       0x0020
#define TMC5160_RS_EV_STOP_SG     0x0040
#define TMC5160_RS_EV_POSREACHED  0x0080
#define TMC5160_RS_VELREACHED     0x0100
#define TMC5160_RS_POSREACHED     0x0200
#define TMC5160_RS_VZERO          0x0400
#define TMC5160_RS_ZEROWAIT       0x0800
#define TMC5160_RS_SECONDMOVE     0x1000
#define TMC5160_RS_SG             0x2000

// Encoderbits (Register TMC5160_ENCMODE)
#define TMC5160_EM_DECIMAL     0x0400
#define TMC5160_EM_LATCH_XACT  0x0200
#define TMC5160_EM_CLR_XENC    0x0100
#define TMC5160_EM_NEG_EDGE    0x0080
#define TMC5160_EM_POS_EDGE    0x0040
#define TMC5160_EM_CLR_ONCE    0x0020
#define TMC5160_EM_CLR_CONT    0x0010
#define TMC5160_EM_IGNORE_AB   0x0008
#define TMC5160_EM_POL_N       0x0004
#define TMC5160_EM_POL_B       0x0002
#define TMC5160_EM_POL_A       0x0001



enum 
{
	Spreadcycle = 0,			//通常翻译成高速模式，也可以翻译成非静音模式
	Stealthchop,				//通常翻译成静音模式
};  //GCONF(0x00)寄存器的en_pwm_mode位(bit2)


typedef enum
{
	binary_mode = 0,		//编码器分辨率选择二进制模式
	decimal_mode,			//编码器分辨率选择十进制模式
}Encoder_MODE;  //ENCMODE(0x38)寄存器的enc_sel_decimal位(bit10)


typedef enum
{
	STOP_L = 1u,
	STOP_R,
	STOP_L_R,
} TMC5160_Stop_switch;

typedef enum
{
  OFF = 0u,
  ON  = 1u,
} Stop_switch_state;

typedef enum
{
  High_Level = 0u,			//停止（限位）开关高电平触发电机停止
  Low_Level,				//停止（限位）开关低电平触发电机停止
} Stop_switch_polarity;


#endif /* TMC_IC_TMC5160_TMC5160_CONSTANTS_H_ */

