/*
 * TMC5160.c
 *
 *  Created on: 2024.06.12
 *      Author: LZY
 * 
 *  深圳市智联微电子有限公司-------------TMC一级代理商-------------
 *  可访问      https://www.chiplinkstech.com/
 */




#include "stdint.h"
#include "spi.h"
#include "TMC5160.h"
#include <stdlib.h>					//调用了abs()函数
#include <math.h>	                //调用了modf()函数



uint32_t register_value;            //一个临时变量
__TMC5160 TMC5160_Registers;


#define ENABLE_CONTROL()        HAL_GPIO_WritePin(DRV_ENN_GPIO_Port, DRV_ENN_Pin, GPIO_PIN_RESET)      //DRV_ENN引脚置低电平使能电机
#define DISENABLE_CONTROL()     HAL_GPIO_WritePin(DRV_ENN_GPIO_Port, DRV_ENN_Pin, GPIO_PIN_SET)        //DRV_ENN引脚置高电平不使能电机

#define CONTROL_MODE  0     ////选择 TMC5160 的工作模式
                            //0：纯SPI；
                            //1：SPI + S、D；
                            //2：TMC5160的独立模式


/**
  * @brief  往寄存器值写入参数，并更新获取到的SPI_STATUS状态字节
  * @param  address：寄存器地址
  * @param    value：写入的值
  * @return 返回SPI状态字，如果通信成功的话
  * @attention   有的寄存器仅读访问，也有寄存器可读可写，具体查TMC5160规格书确认
  *
  */
void tmc5160_writeInt(uint8_t address, int32_t value)
{
    uint8_t send_data[5] = {0,0,0,0,0};
	uint8_t datagram1[5] = {0,0,0,0,0};
	
	send_data[0] = address|0x80;
	send_data[1] = (value >> 24) & 0xff;
	send_data[2] = (value >> 16) & 0xff;
	send_data[3] = (value >>  8) & 0xff;
	send_data[4] =  value  & 0xff;
	
	HAL_GPIO_WritePin(SPI_CSN_GPIO_Port,SPI_CSN_Pin,GPIO_PIN_RESET);    
    HAL_SPI_TransmitReceive(&hspi1, send_data, datagram1, 5,100);		
	HAL_GPIO_WritePin(SPI_CSN_GPIO_Port,SPI_CSN_Pin,GPIO_PIN_SET);  	//SPI_CS片选拉高
    
//    TMC5160.SPI_STATUS.Value = datagram1[0];
}


/**
  * @brief  获取寄存器值并返回，并更新获取到的SPI_STATUS状态字节
  * @param  address：寄存器地址
  * @return 返回获取到的寄存器的值
  * @attention   有的寄存器仅写访问，也有寄存器可读可写，具体查TMC5160规格书确认
  *
  */
int32_t tmc5160_readInt(uint8_t address)
{
    uint8_t send_data[5]    = {0,0,0,0,0};
	uint8_t receive_data[5] = {0,0,0,0,0};
    
    send_data[0] = address;
    
    //SPI读，需要读两次。是因为每次SPI通信，返回的是前一次读或写访问返回的数据
	HAL_GPIO_WritePin(SPI_CSN_GPIO_Port,SPI_CSN_Pin,GPIO_PIN_RESET);    
    HAL_SPI_TransmitReceive(&hspi1, send_data, receive_data, 5,100);		
	HAL_GPIO_WritePin(SPI_CSN_GPIO_Port,SPI_CSN_Pin,GPIO_PIN_SET);  	//SPI_CS片选拉高
	HAL_GPIO_WritePin(SPI_CSN_GPIO_Port,SPI_CSN_Pin,GPIO_PIN_RESET);    
    HAL_SPI_TransmitReceive(&hspi1, send_data, receive_data, 5,100);
    
//    TMC5160.SPI_STATUS.Value = datagram1[0];
	
	HAL_GPIO_WritePin(SPI_CSN_GPIO_Port,SPI_CSN_Pin,GPIO_PIN_SET);  	//SPI_CS片选拉高

	return ((uint32_t)receive_data[1] << 24) | ((uint32_t)receive_data[2] << 16) | (receive_data[3] << 8) | receive_data[4];
}


void TMC5160_init(void)
{
    DISENABLE_CONTROL();        //不使能电机
    
    
    switch(CONTROL_MODE)        //选择 TMC5160 的工作模式
	{
		case 0://纯SPI。SPI接口既能配置电机的电流、细分能参数，也能控制电机的转速、方向、位移。这例程优先选择模式
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);	    //SPI_MODE
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);    	//SD_MODE
			break;
		case 1://SPI+SD。SPI配置电机的电流、细分能参数，而电机的转速、方向，和位移通过脉冲、方向引脚控制。那还可以使用本例程的“TMC5160_init”函数，然后发脉冲、方向信号控制电机转动。
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);	    //SPI_MODE		
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);    	//SD_MODE
            break;
        case 2://SD。独立模式。电机的电流、细分参数通常5160的 CFG0...6 引脚配置,而电机的转速、方向，和位移通过脉冲、方向引脚控制。本例程没支持这工作模式
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);  	//SPI_MODE
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET); 	//SD_MODE			
			break;
	}
    HAL_Delay(50);
    
//获取版本号	
    volatile uint8_t MRES;  //用于细分选择
	volatile uint8_t version;
	tmc5160_readInt(0x04);
	version = tmc5160_readInt(0x04)>>24;
	if(version!=0x30)//获取版本号
	{
//		while(1);//版本号获取失败，请检查通讯，vccio供电
		
	}
//获取版本号	

	

//****这里参数看实际情况修改
	//配置GCONF寄存器
    //选择TMC5160的斩波模式
	TMC5160_Registers.GCONF.en_pwm_mode            = Stealthchop;       //可选 Stealthchop：通常翻译成静音模式，选择这斩波模式，电机中低速转动时噪声通常很低，不过有转速上限，在300RPM左右。
                                                                        //或者选 Spreadcycle：通常翻译成高速模式，电机可以低速跑，也可以高转速跑，例如能跑到3000RPM。电机转动时通常有种电流声。
    
    //选择 DIAG0引脚 的输出模式
    TMC5160_Registers.GCONF.diag0_int_pushpull     = ON;                //OFF：集电极开路输出(需要外部上拉，触发时输出低电平)
                                                                        // ON：推挽输出(可自己输出高低电平，触发时输出高电平)
    //选择 DIAG1引脚 的输出模式                                   
    TMC5160_Registers.GCONF.diag1_poscomp_pushpull = ON;                //OFF：集电极开路输出(需要外部上拉，触发时输出低电平)
                                                                        // ON：推挽输出(可自己输出高低电平，触发时输出高电平)
	//配置TMC5160_IHOLD_IRUN寄存器
	TMC5160_Registers.IHOLD_IRUN.IRUN              = 31;                //调整电机运行电流，数值范围0...31。调整这里可改变电机转动时的扭矩。
	TMC5160_Registers.IHOLD_IRUN.IHOLD             = 15;			        		//调整电机待机电流，数值范围0...31。调整这里可改变电机待机时的扭矩，可设置为IRUN的一半。
    
    //配置TMC5160_GLOBAL_SCALER寄存器
    TMC5160_Registers.GLOBAL_SCALER.Value          = 0;                //可等比例缩放电机运行电流和待机电流。数值范围0...255
	//配置细分
    MRES  = 0;			                                                //设置细分。数值范围0...8。调整这里可改变电机微步步距角
                                                                            //MRES = 0，是256细分。例如1.8度步距角的电机转一圈则需要51200个脉冲数。
                                                                            //MRES = 1，是128细分。例如1.8度步距角的电机转一圈则需要25600个脉冲数。
                                                                            //MRES = 2，是 64细分。例如1.8度步距角的电机转一圈则需要12800个脉冲数。	
                                                                            //MRES = 3，是 32细分；			//MRES = 4，是 16细分；			//MRES = 5，是  8细分。
                                                                            //MRES = 6，是  4细分；			//MRES = 7，是  2细分；			//MRES = 8，是  0细分，电机整步运行。
	
	tmc5160_writeInt(TMC5160_GCONF,		    TMC5160_Registers.GCONF.Value);
	
    //配置电机电流
	tmc5160_writeInt(TMC5160_IHOLD_IRUN,    0x70000  | TMC5160_Registers.IHOLD_IRUN.Value);
    tmc5160_writeInt(TMC5160_GLOBAL_SCALER, TMC5160_Registers.GLOBAL_SCALER.Value);
																				
	tmc5160_writeInt(TMC5160_CHOPCONF,		0x410153 | MRES << TMC5160_MRES_SHIFT);
	
			
	//配置限位开关（参考开关）
	TMC5160_Stop_switch_Enable(STOP_L_R, OFF);			//不使能左右限位开关
    
#if 0

    tmc5160_writeInt(TMC5160_TPWMTHRS,  54);            //设置这寄存器，可让5160芯片根据速度，在运动过程中自动切换斩波模式。要先把“stealthchop”赋值给“TMC5160_Registers.GCONF.en_pwm_mode”。
                                                        //不过运动过程中切换斩波模式有副作用，不管是自动切换还是手动切换，切换那个点电机会抖动，会丢步。
                                                        //所以不建议运动过程中自动切换，不管是手动切换还是自动切换。
                                                        //寄存器配置，具体对应多少RPM，可看表格“斜坡计算公式”。
#else

    tmc5160_writeInt(TMC5160_TPWMTHRS,  0);              
#endif
    
    
    
										
	//下面把一些运动参数清0，防止电机初始化时动作
	tmc5160_writeInt(TMC5160_VMAX, 		0);				//电机最大速度寄存器清0
	
	tmc5160_writeInt(TMC5160_XTARGET, 	0);				//电机目标位置 寄存器清0，
	
	tmc5160_writeInt(TMC5160_XACTUAL, 	0);				//电机实际位置 寄存器清0，
	
	tmc5160_writeInt(TMC5160_XENC, 		0);				//编码器位置计数寄存器清0	
	
	ENABLE_CONTROL();                                   //使能电机
    
	HAL_Delay(100);		//初始化完务必延时100毫秒，让内部完成初始化
}


void TMC5160_rotate(int32_t velocity)
{
    tmc5160_writeInt(TMC5160_RAMPMODE, (velocity >= 0) ? TMC5160_MODE_VELPOS : TMC5160_MODE_VELNEG);
    
	tmc5160_writeInt(TMC5160_VMAX, 		abs(velocity));
}


// 速度模式，电机右方向，以固定转速转动，只能用T型加减速，只有VMAX和AMAX寄存器有效
void TMC5160_right(uint32_t velocity)
{
    static uint32_t AMAX = 10000;				//这里可根据需要修改，也可后面随时修改
	tmc5160_writeInt(TMC5160_AMAX, 	AMAX);	
    
	TMC5160_rotate(velocity);
}

// 速度模式，电机左方向，以固定转速转动，只能用T型加减速，只有VMAX和AMAX寄存器有效
void TMC5160_left(uint32_t velocity)
{
    static uint32_t AMAX = 10000;				//这里可根据需要修改，也可后面随时修改
	tmc5160_writeInt(TMC5160_AMAX, 	AMAX);	
    
	TMC5160_rotate(-velocity);
}

// 减速停止电机，停止时间和AMAX寄存器有关
void TMC5160_stop(void)
{
	TMC5160_rotate(0);
}


//位置模式的6点斜坡运动轨迹配置。速度寄存器和真实物理量之间的换算，可以看表格《斜坡计算公式》。
void TMC5160_MovePosition_SixPoint(int32_t position, uint32_t velocityMax)
{
	tmc5160_writeInt(TMC5160_RAMPMODE, 		0);				//位置模式

	//这里的参数可调整。也可以后面随时调整
	tmc5160_writeInt(TMC5160_VSTART, 0);			//电机启动速度寄存器。             数值范围 0...262143。
	tmc5160_writeInt(TMC5160_A1, 2000);             //第一段加速度寄存器。             数值范围 0...65535。
    tmc5160_writeInt(TMC5160_D1, 2000);             //V1和 VSTOP之间的减速度。         数值范围 1...65535。
	tmc5160_writeInt(TMC5160_V1, 70000);            //第一段加速度阈值速度。           数值范围 0...1048575。
	tmc5160_writeInt(TMC5160_AMAX, 10000);          //V1 与 VMAX 之间的第二个加速度。  数值范围 0...65535。
	tmc5160_writeInt(TMC5160_DMAX, 10000);          //VMAX和 V1之间的减速度。          数值范围 0...65535。
	tmc5160_writeInt(TMC5160_VSTOP, 10);				//VSTOP:停止速度 一般>=10
	tmc5160_writeInt(TMC5160_TZEROWAIT, 10);
	//上面的参数可调整。也可以后面随时调整
	
	
	tmc5160_writeInt(TMC5160_VMAX, velocityMax);
	tmc5160_writeInt(TMC5160_XTARGET, position);
	
}

//T型斜坡运动轨迹配置。配置难度比6点斜坡简单些，只需考虑一段加速度和一段减速度。速度寄存器和真实物理量之间的换算，可以看表格《斜坡计算公式》。
void TMC5160_MovePosition_T(int32_t position, uint32_t velocityMax)
{
	tmc5160_writeInt(TMC5160_RAMPMODE, 		0);				//位置模式

   
    //要是用T型加减速，下面的参数不修改
	tmc5160_writeInt(TMC5160_A1, 0);			//不启用
	tmc5160_writeInt(TMC5160_V1, 0);			//不启用
	tmc5160_writeInt(TMC5160_D1, 1);			//不启用
	//要是用T型加减速，上面面的参数不修改
    
	
	//这里的参数可调整
	tmc5160_writeInt(TMC5160_VSTART, 0);				//启动速度可以为0
	tmc5160_writeInt(TMC5160_AMAX, 10000);
	tmc5160_writeInt(TMC5160_DMAX, 10000);
	tmc5160_writeInt(TMC5160_VSTOP, 10);
	tmc5160_writeInt(TMC5160_TZEROWAIT, 10);
	//上面的参数可调整。也可以后面随时调整
	
	tmc5160_writeInt(TMC5160_VMAX, velocityMax);
	tmc5160_writeInt(TMC5160_XTARGET, position);
}


//位置模式下控制电机相对移动。只能在配置完加减速后使用
void TMC5160_moveBy(int32_t position)
{
	// determine actual position and add numbers of ticks to move
	position += tmc5160_readInt(TMC5160_XACTUAL);

	tmc5160_writeInt(TMC5160_XTARGET,	position);				//设置电机目标位置
}



//位置模式下控制电机移动到目标位置（绝对坐标）。只能在配置完加减速后使用
//如果目标位置寄存器，大于实际位置寄存器，电机正转
//如果目标位置寄存器，小于实际位置寄存器，电机反转
//寄存器值可正可负
void TMC5160_moveTo(int32_t position)
{
	tmc5160_writeInt(TMC5160_XTARGET,	position);				//设置电机目标位置
}


/**
  * @brief  配置编码器常数。配置完后就可以从寄存器 TMC5160_X_ENC 获取编码器位置数。
  * @param  mode： 可选择二进制模式还是十进制模式。如果编码器分辨率是二进制的，例如1024分辨率，那这里选择二进制模式，否则用十进制模式。
  *            @arg binary_mode：二进制模式
  *            @arg decimal_mode：十进制模式
  *
  * @param  Count：输入电机圈一圈需要的脉冲数。Count = (360°÷ 步距角) × 细分
  *
  * @param  Encoder_resolution：  输入增量式编码器的分辨率，程序会按照4倍频计算编码器常数。
  *                     Encoder_resolution是有符号的，以匹配电机旋转方向。如果编码器步数计数方向反了，也可交换编码器接口的A和B信号线。
  */
void TMC5160_Encoder_init(Encoder_MODE mode, uint32_t Count, int32_t Encoder_resolution)
{
	static double Count_1, Count_2;
	static int32_t Count_3,Count_4;
	
	Count_2 = modf( ( double )( Count ) / (Encoder_resolution * 4), &Count_1 );			//Count_1为整数部分，//Count_2是小数部分
    
    if(mode == decimal_mode)
    {   
        
        //把TMC5160_ENCMODE寄存器中的 enc_sel_decimal 位修改成10进制模式。其他位保持不变
        TMC5160_FIELD_WRITE(TMC5160_ENCMODE, TMC5160_ENC_SEL_DECIMAL_MASK, TMC5160_ENC_SEL_DECIMAL_SHIFT, decimal_mode);
        
        //配置编码器常亮寄存器
        if(Encoder_resolution >= 0)
        {
            Count_3 = (uint32_t)Count_1 << 16 | ( (uint32_t)(Count_2 * 10000) );
            
        }
        else
        {
            Count_3 = 0xFFFF + Count_1;
            Count_4 = 10000 + (int32_t)(Count_2 * 10000);
            Count_3 = Count_3 << 16 | Count_4;
        }
    }
    else if(mode == binary_mode)
    {
        
        //把TMC5160_ENCMODE寄存器中的 enc_sel_decimal 位修改成2进制模式。其他位保持不变
        TMC5160_FIELD_WRITE(TMC5160_ENCMODE, TMC5160_ENC_SEL_DECIMAL_MASK, TMC5160_ENC_SEL_DECIMAL_SHIFT, binary_mode);
        
        //配置编码器常亮寄存器
        if(Encoder_resolution >= 0)
        {
            Count_3 = (uint32_t)Count_1 << 16 | ((uint32_t)Count_2 * 65536);
        }
        else
        {
            Count_3 = 0xFFFF + Count_1;
            Count_4 = 65536 + (int32_t)(Count_2 * 65536);
            Count_3 = Count_3 << 16 | Count_4;
        }
    }
    
    tmc5160_writeInt(TMC5160_ENC_CONST, Count_3);
}


/**
  * @brief  使能或禁止限位限位开关（默认限位开关的高电平触发）
  * @note   限位开关触发后，默认立刻停止电机。如果考虑惯性，要减速停止，把0x34寄存器的bit11置1。分方向，要是电机朝左方向转，左限位触发会停止，右限位触发不会停止。
  * @param  Stop_switch：选择限位开关
  *            @arg STOP_R：右限位开关
  *            @arg STOP_L：左限位开关
  *            @arg STOP_L_R：左右限位开关
  * @param  Switch_State：使能或禁止限位开关
  *            @arg Stop_switch_OFF: 不使能限位开关（默认）
  *            @arg Stop_switch_ON:  使能限位开关
  */
void TMC5160_Stop_switch_Enable(uint8_t Stop_switch, uint8_t Switch_State)
{
	register_value = tmc5160_readInt(TMC5160_SWMODE);
    
    //限位开关触发后，默认电机立刻停止，如果要降低惯性影响，要减速停止，可把0x34寄存器的bit11置1。或者说下面这行取消注释
    //register_value = register_value | TMC5160_EN_SOFTSTOP_MASK;
	
	if(Switch_State == ON)
	{
		tmc5160_writeInt(TMC5160_SWMODE, register_value | Stop_switch);
	}
	else
	{
		tmc5160_writeInt(TMC5160_SWMODE, register_value & (~Stop_switch));
	}
}


/**
  * @brief  设置限位开关是高电平触发，还是低电平触发
  * @param  Stop_switch：选择限位开关
  *            @arg STOP_R：右限位开关
  *            @arg STOP_L：左限位开关
  *            @arg STOP_L_R：左右限位开关
  * @param  polarity：选择停止开关（限位开关）的触发极性
  *            @arg High_Level: 设置高电平触发（默认）
  *            @arg Low_Level:  设置低电平触发
  */
void TMC5160_Stop_switch_polarity(uint8_t Stop_switch, uint8_t polarity)
{
	
	register_value = tmc5160_readInt(TMC5160_SWMODE);
	
	if(polarity == Low_Level)
	{
		tmc5160_writeInt(TMC5160_SWMODE, register_value | (Stop_switch << 2));
	}
	else
	{
		tmc5160_writeInt(TMC5160_SWMODE, register_value & (~Stop_switch << 2));
	}
}


//位置模式下检查是否运动到位
uint8_t GetAixsPositionRached(void)
{
    register_value = tmc5160_readInt(TMC5160_RAMPSTAT);
    if( register_value & TMC5160_RS_POSREACHED )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


//检查是否到达最大速度
uint8_t GetAixsVelocityRached(void)
{
    register_value = tmc5160_readInt(TMC5160_RAMPSTAT);
    if( register_value & TMC5160_RS_VELREACHED )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


//获得电机实际位置（有符号）
int32_t TMC5160_Get_Position(void)
{
    return tmc5160_readInt(TMC5160_XACTUAL);
}


//获得内部斜坡发生器的实际电机速度（有符号）
//符号与运动方向相匹配,负号表示降低XACTUAL的运动。
int32_t TMC5160_Get_velocity(void)
{
    return tmc5160_readInt(TMC5160_VACTUAL);
}



//把电机位置数清0。例如当回到位置零点时
// 如果VMAX寄存器不是0，直接在当前位置。把 TMC5160_XACTUAL 寄存器清0，5160会控制电机走动。所以需要用下面方式在当前位置把 TMC5160_XACTUAL 寄存器清0。
void TMC5160_Zero_Position(void)
{
	register_value = tmc5160_readInt(TMC5160_VMAX);
	
	tmc5160_writeInt(TMC5160_VMAX, 0);
    
//    while( !( (tmc5160_readInt(TMC5160_RAMPSTAT) & 0x400) == 0x400 ) );		//等实际速度为0, 等 TMC5160_RAMPSTAT 寄存器的bit10置1
	while( !(tmc5160_readInt(TMC5160_RAMPSTAT) & 0x400) ) 		            //等实际速度为0, 等 TMC5160_RAMPSTAT 寄存器的bit10置1

    tmc5160_writeInt(TMC5160_XENC, 		0);				//编码器位置计数寄存器清0			                                
    tmc5160_writeInt(TMC5160_XACTUAL, 	0);				//电机实际位置寄存器清0
    tmc5160_writeInt(TMC5160_XTARGET, 	0);				//电机目标位置寄存器清0

	tmc5160_writeInt(TMC5160_VMAX, register_value);				//恢复VMAX寄存器的值
}


//TMC5160的紧急停止功能
//关于TMC5160的停止，如果只是通过配置寄存器，只能实现减速停止，也就是说不会马上停下来。
//如果要电机立刻停止，一定要用引脚配合，一个是通过停止开关，另一个是通过TMC5160的 ENCA_DCIN_ CFG5 引脚。
//这函数就是配置TMC5160的紧急停止功能（Emergency Stop）。这功能要在0x00寄存器开启，而且控制TMC5160的 ENCA_DCIN_ CFG5 引脚才能停止电机。
//控制引脚和编码器输入引脚复用，所以这功能和编码器接口之间只能选一个。
//建议：尽量让电机减速停止，而不是紧急停止。
/**
  * @param  state：选择这功能开启和关闭
  *            @arg OFF：关闭这功能
  *            @arg  ON：开启这功能
  *
  */
void TMC5160_Emergency_Stop(uint8_t state)
{
    //GCONF(0x00)寄存器的bit15开启和关闭。
    TMC5160_FIELD_WRITE(TMC5160_GCONF, TMC5160_STOP_ENABLE_MASK, TMC5160_STOP_ENABLE_SHIFT, state);
    
    //然后可控制TMC5160的 ENCA_DCIN_ CFG5 引脚置高电平，电机会紧急停止
    //例如用MCU的PB14控制 ENCA_DCIN_ CFG5 引脚。可在需要的地方控制。
    //HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); 
}





