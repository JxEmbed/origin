/*
* TMC5160_Register.h
*
*  Created on: 2024.06.12
*
*/

#ifndef TMC5160_REGISTER_H
#define TMC5160_REGISTER_H


#include "stdint.h"


// ===== TMC5160 register set =====

#define TMC5160_GCONF          0x00
#define TMC5160_GSTAT          0x01
#define TMC5160_IFCNT          0x02
#define TMC5160_SLAVECONF      0x03
#define TMC5160_INP_OUT        0x04
#define TMC5160_X_COMPARE      0x05
#define TMC5160_OTP_PROG       0x06
#define TMC5160_OTP_READ       0x07
#define TMC5160_FACTORY_CONF   0x08
#define TMC5160_SHORT_CONF     0x09
#define TMC5160_DRV_CONF       0x0A
#define TMC5160_GLOBAL_SCALER  0x0B
#define TMC5160_OFFSET_READ    0x0C
#define TMC5160_IHOLD_IRUN     0x10
#define TMC5160_TPOWERDOWN     0x11
#define TMC5160_TSTEP          0x12
#define TMC5160_TPWMTHRS       0x13
#define TMC5160_TCOOLTHRS      0x14
#define TMC5160_THIGH          0x15

#define TMC5160_RAMPMODE       0x20             //5160的运动模式，有速度模式和位置模式可选
#define TMC5160_XACTUAL        0x21             //电机实际位置
#define TMC5160_VACTUAL        0x22             //电机实际速度寄存器。    只读。           数值范围 -8388608...8388607
#define TMC5160_VSTART         0x23             //电机启动速度寄存器。    可读可写。       数值范围 0...262143
#define TMC5160_A1             0x24             //第一段加速度寄存器。    只写。           数值范围 0...65535
#define TMC5160_V1             0x25             //第一段加速度阈值速度。  只写。           数值范围 0...1048575
#define TMC5160_AMAX           0x26             //V1 与 VMAX 之间的第二个加速度。  只写。  数值范围 0...65535
#define TMC5160_VMAX           0x27             //最大速度。             只写。           数值范围 0...8388096
#define TMC5160_DMAX           0x28             //VMAX和 V1之间的减速度。 只写。          数值范围 0...65535
#define TMC5160_D1             0x2A             //V1和 VSTOP之间的减速度。只写。          数值范围 1...65535
#define TMC5160_VSTOP          0x2B             //电机停止速度。          只写。          数值范围 1...262143。建议数值至少大于10
#define TMC5160_TZEROWAIT      0x2C             //
#define TMC5160_XTARGET        0x2D             //位置模式下的目标位置。

#define TMC5160_VDCMIN         0x33
#define TMC5160_SWMODE         0x34
#define TMC5160_RAMPSTAT       0x35              //斜坡和参考开关状态寄存器
#define TMC5160_XLATCH         0x36
#define TMC5160_ENCMODE        0x38
#define TMC5160_XENC           0x39
#define TMC5160_ENC_CONST      0x3A
#define TMC5160_ENC_STATUS     0x3B
#define TMC5160_ENC_LATCH      0x3C
#define TMC5160_ENC_DEVIATION  0x3D

#define TMC5160_MSLUT0         0x60
#define TMC5160_MSLUT1         0x61
#define TMC5160_MSLUT2         0x62
#define TMC5160_MSLUT3         0x63
#define TMC5160_MSLUT4         0x64
#define TMC5160_MSLUT5         0x65
#define TMC5160_MSLUT6         0x66
#define TMC5160_MSLUT7         0x67
#define TMC5160_MSLUTSEL       0x68
#define TMC5160_MSLUTSTART     0x69
#define TMC5160_MSCNT          0x6A
#define TMC5160_MSCURACT       0x6B
#define TMC5160_CHOPCONF       0x6C
#define TMC5160_COOLCONF       0x6D
#define TMC5160_DCCTRL         0x6E
#define TMC5160_DRVSTATUS      0x6F
#define TMC5160_PWMCONF        0x70
#define TMC5160_PWMSCALE       0x71
#define TMC5160_PWM_AUTO       0x72
#define TMC5160_LOST_STEPS     0x73



#pragma anon_unions
typedef union // 用来描述 GCONF(0x00)的数据字
{
	uint32_t Value;
	struct //
	{
		// 低位在前，高位在后
		uint8_t recalibrate                 : 1;                                    
		uint8_t faststandstill              : 1;                                                              
		uint8_t en_pwm_mode                 : 1;           // 0：使用Spreadcycle斩波模式，通常翻译高速斩波模式 
                                                           // 1：使用Stealthchop斩波模式，通常翻译成静音斩波模式
        uint8_t multistep_filt              : 1;                             
		uint8_t shaft                       : 1;    
		uint8_t diag0_error 	            : 1;    
		uint8_t diag0_otpw 	                : 1;    
		uint8_t diag0_stall    		        : 1;           // 1：在电机堵转时启用 DIAG0（在使用此功能之前设置 TCOOLTHRS，只有SD_MODE=1时有效） 
		uint8_t diag1_stall    		        : 1;           // 1：在电机堵转时启用 DIAG1（在使用此功能之前设置 TCOOLTHRS，只有SD_MODE=1时有效）
        uint8_t diag1_index                 : 1;    
        uint8_t diag1_onstate               : 1;    
        uint8_t diag1_steps_skipped         : 1;    
        uint8_t diag0_int_pushpull          : 1;           // 设置 DIAG0 引脚的输出模式。0：集电极开路输出(需要外部上拉，触发时输出低电平)
                                                           // 1：推挽输出(可自己输出高低电平，触发时输出高电平)
        uint8_t diag1_poscomp_pushpull      : 1;           // 设置 DIAG1 引脚的输出模式。0：集电极开路输出(需要外部上拉，触发时输出低电平)
                                                           // 1：推挽输出(可自己输出高低电平，触发时输出高电平)
        uint8_t small_hysteresis            : 1;                                   
        uint8_t stop_enable                 : 1;           // 0：不使用 ENCA_DCIN引脚 控制电机紧急停止
                                                           // 1:使能紧急停止功能：当 ENCA_DCIN引脚 为高电平时电机紧急停止，
        uint8_t direct_mode                 : 1;                            
        uint8_t test_mode                   : 1;
	};
}__GCONF;


#pragma anon_unions
typedef union // 用来描述 GLOBAL_SCALER(0x0B) 的数据字
{
	uint32_t Value;
	struct
	{
		// 低位在前，高位在后
		uint8_t GLOBAL_SCALER          : 8;    //可改变电机电流。取值范围0...255
	};
}__GLOBAL_SCALER;



typedef union // 用来描述 IHOLD_IRUN(0x10) 的数据字
{
	uint32_t Value;
	struct
	{
		// 低位在前，高位在后
		uint8_t IHOLD               : 5;    // 调整电机运行电流，数值范围0...31。调整这里可改变电机转动时的扭矩。
        uint8_t reserved_1          : 3;    // 保留位
		uint8_t IRUN                : 5;    // 调整电机待机电流，数值范围0...31。调整这里可改变电机待机时的扭矩，可设置为IRUN的一半。
        uint8_t reserved_2          : 3;    // 保留位
		uint8_t IHOLDDELAY          : 4;    // 调整运行电流到待机电流的延时，数值范围0...15。如果设为0，是瞬间断电。平滑过渡可避免断电时电机抖动。
        uint8_t reserved_3          : 4;    // 保留位
	};
}__IHOLD_IRUN;




typedef union // 用来描述 CHOPCONF(0x6C) 的数据字
{
	uint32_t Value;
	struct
	{
		// 低位在前，高位在后
		uint32_t TOFF                : 4;    //关闭时间设置控制慢衰减阶段的持续时间 NCLK= 24 + 32*TOFF。如果值置0，电机不锁轴                       
        uint32_t HSTRT               : 3;
        uint32_t HEND                : 4;
        uint32_t TFD_3               : 1;
        uint32_t disfdcc             : 1;
        uint32_t reserved_1          : 1;
        uint32_t chm                 : 1;
        uint32_t TBL                 : 2;
        uint32_t reserved_2          : 1;
        uint32_t vhighfs             : 1;
        uint32_t vhighchm            : 1;
        uint32_t TPFD                : 4;
        uint32_t MRES                : 4;    //设置细分
        uint32_t intpol              : 1;    //如果这 bit 置1，设置其他细分时的电机转动效果，和设置256细分的电机转动效果一样
        uint32_t dedge               : 1;    //0：脉冲信号的上升沿触发电机转动.         1，脉冲信号的双边沿触发电机转动，这样可降低脉冲频率要求。
        uint32_t diss2g              : 1;    //0：开启接地短路保护。   1：禁用短路至 GND 保护。
        uint32_t diss2vs             : 1;    //0：开启短路至 VS 保护   1：禁用短路至 VS 保护。
	};
}__CHOPCONF;


typedef union // 用来描述 COOLCONF(0x6D) 的数据字
{
	uint32_t Value;
	struct
	{
        uint32_t semin               : 4;   // 如果 SG_RESULT 的结果低于 SEMIN * 32，则电机电流会 增加以减小电机负载角。如果值为 0，关闭 CoolStep 功能。
        uint32_t reserved_1          : 1;   // 保留位，保持0  
        uint32_t seup                : 2;   // 电流增量步长。%00 … %11: 1、2、4、8
        uint32_t reserved_2          : 1;   // 保留位，保持0   
        uint32_t semax               : 4;   // 如果 SG_RESULT 结果等于或高于(SEMIN+SEMAX+1) × 32，则电机电流变小以节能。
        uint32_t reserved_3          : 1;   // 保留位，保持0   
        uint32_t sedn                : 2;   // 电流下降速度。%00: 每32个 StallGuard4 值减1；%01: 每8个 StallGuard4 值减1；%10: 每 2个 StallGuard4 值减1；%11: 每1个 StallGuard4 值减1。
        uint32_t seimin              : 1;   // 控制的最小电流。 0: 1/2 电流设置(IRUN)；1: 1/4 电流设置(IRUN)
        int32_t  SGT                 : 7;   // StallGuard2 灵敏度设置，-64-64 该值控制堵转输出的StallGuard2电平  较高的值会降低StallGuard2 的敏感度，并需要更多扭矩来指示堵转
        uint32_t reserved_4          : 1;   // 保留位，保持0 
        uint32_t sfilt               : 1;   //  1: 过滤
        uint32_t reserved_5          : 1;   // 保留位，保持0 
	};
}__COOLCONF;  //智能电流控制 COOLSTEP 和 STALLGUARD2


typedef union // 用来描述 PWMCONF(0x70) 的数据字
{
	uint32_t Value;
	struct
	{
		// 低位在前，高位在后
		uint32_t PWM_OFS             : 8;  
        uint32_t PWM_GRAD            : 8;   
        uint32_t pwm_freq            : 2;          
        uint32_t pwm_autoscale       : 1;
        uint32_t pwm_autograd        : 1;   
        uint32_t freewheel           : 2; 
        uint32_t reserved_1          : 1;   //保留位，保持0
        uint32_t reserved_2          : 1;   //保留位，保持0
        uint32_t PWM_REG             : 4;
        uint32_t PWM_LIM             : 4;
    };
}__PWMCONF;


//SPI只要通信成功，都有数据返回，包括返回SPI_STATUS状态字节
#pragma anon_unions
typedef union // 用来描述SPI_STATUS的数据字
{
	uint32_t Value;
	struct // 用来描述控制寄存器的数据字
	{
		// 低位在前，高位在后
		uint8_t reset_flag   		        : 1;    // GSTAT[0]–1:发生复位的信号（通过读取 GSTAT 清除）
		uint8_t driver_error     	        : 1;    // GSTAT[1]–1:信号驱动程序 1 驱动程序错误（通过读取 GSTAT 清除）
		uint8_t sg2                         : 1;    // DRV_STATUS[24] - 1：StallGuard 信号
		uint8_t standstill                  : 1;    // DRV_STATUS[31] - 1：电机静止信号
		uint8_t velocity_reached 	        : 1;    // RAMP_STAT[8]   - 1：达到目标速度的信号（仅限内部加减速算法）
		uint8_t position_reached 	        : 1;    // RAMP_STAT[9]   - 1：达到目标位置信号（仅限内部加减速算法）
		uint8_t status_stop_l               : 1;    // RAMP_STAT[0]   - 1：左侧停止开关状态信号（仅限内部加减速算法）
		uint8_t status_stop_r    	        : 1;    // RAMP_STAT[1]   - 1：右侧停止开关状态信号（仅限内部加减速算法）
	};
}__SPI_STATUS;

typedef union
{
	uint32_t Value;
	struct
	{
		//低位在前，高位在后
		uint8_t status_stop_l          :1;    // 保留位
		uint8_t status_stop_r          :1;     //
		uint8_t status_latch_l         :1;
		uint8_t status_latch_r         :1;
		uint8_t event_stop_l           :1;
		uint8_t event_stop_r           :1;
		uint8_t event_stop_sg          :1;
		uint8_t event_pos_reached      :1;
		uint8_t velocity_reached       :1;
		uint8_t position_reached       :1;
		uint8_t vzero                  :1;
		uint8_t t_zerowait_active      :1;
		uint8_t second_move            :1;
		uint8_t status_sg              :1;
	};
}__RAMP_STAT;
typedef union
{
	uint32_t Value;
	struct
	{
		/*
		SG_RESULT
		较高的值意味着较低的机械负载。值0表示最高负载  
		堵转检测将SG_RESULT 与 0 进行比较，以便检测堵转。SG_RESULT通过将其与可编程上限和下限进行比较
		*/
		uint32_t SG_RESULT             :10;     
		uint8_t  reserved_1            :2;
		uint8_t  s2vsa                 :1;
		uint8_t  s2vsb                 :1;
		uint8_t  stealth               :1;
		uint8_t  fsactive              :1;
		uint16_t CSACTUAL              :5;
		uint8_t  reserved_2            :3;
		uint8_t  stallGuard            :1;
		uint8_t  ot                    :1;
		uint8_t  otpw                  :1;
		uint8_t  s2ga                  :1;
		uint8_t  s2gb                  :1;
		uint8_t  ola                   :1;
		uint8_t  olb                   :1;
		uint8_t  stst                  :1;
		
	};
}__DRV_STATUS;
typedef union
{
	uint32_t Value;
	struct
	{
		uint8_t  stop_l_enable         :1;
		uint8_t  stop_r_enable         :1;
		uint8_t  pol_stop_l            :1;
		uint8_t  pol_stop_r            :1;
		uint8_t  swap_lr               :1;
		uint8_t  latch_l_active        :1;
		uint8_t  latch_l_inactive      :1;
		uint8_t  latch_r_active        :1;
		uint8_t  latch_r_inactive      :1;
		uint8_t  en_latch_encoder      :1;
		uint8_t  sg_stop               :1;
		uint8_t  en_softstop           :1;
		
	};
}__SW_MODE;
typedef struct
{
	uint32_t        TSTEP;
    __GCONF         GCONF;                  //全局配置标志
    __GLOBAL_SCALER GLOBAL_SCALER;          //电机电流的全局缩放        
    __IHOLD_IRUN    IHOLD_IRUN;             //驱动电流控制
    __CHOPCONF      CHOPCONF;               //斩波器和驱动配置
    __COOLCONF      COOLCONF;               //CoolStep 智能电流控制寄存器和 stallGuard 2 配置 只写
    __PWMCONF       PWMCONF;                //电压 PWM 模式斩波器配置
	__SPI_STATUS	SPI_STATUS;             
	__RAMP_STAT     RAMP_STAT;              //斜坡状态和开关事件状态
	__DRV_STATUS    DRV_STATUS;             //StallGuard2 值和驱动错误标志
	__SW_MODE       SW_MODE;                //开关模式配置
	uint32_t        RAMPMODE;               //运行模式
}__TMC5160;

extern __TMC5160 TMC5160_Registers;



#endif /* TMC5160_REGISTER_H */
