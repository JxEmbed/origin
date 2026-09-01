#ifndef __TDC1000CONF_H
#define __TDC1000CONF_H


/*
有关TDC1000的配置

*/
//0x42  0100 0010  0100 0000
#define TX_FREQ_DIV_8 (2<<5)
#define NUM_TX_1      (1)
#define NUM_TX_2      (2)
#define NUM_TX_3      (3)
#define NUM_TX_4      (4)
//01000001    reserve
#define RESERVE       (1<<6)
#define NUM_AVG_1     (0<<3)
#define NUM_RX_1      (1)
//02
#define TEMP_MODE_0   (0<<6)  //温度测量通道
#define TEMP_RTD_SEL_1000 (0<<5)
#define TEMP_CLK_DIV_8  (0<<4)
//03
#define BLANKING_EN         (1<<3)  //启用电源消隐
#define BLANKING_DIS        (0<<3)  //禁用电源消隐
#define ECHO_QUAL_THLD_35   (0<<0)   //35mv
#define ECHO_QUAL_THLD_50   (1<<0)   //50mv
#define ECHO_QUAL_THLD_75   (2<<0)   //75mv
#define ECHO_QUAL_THLD_125  (3<<0)   //125mv
#define ECHO_QUAL_THLD_220  (4<<0)   //220mv
#define ECHO_QUAL_THLD_410  (5<<0)   //410mv
#define ECHO_QUAL_THLD_775  (6<<0)   //775mv
#define ECHO_QUAL_THLD_1500 (7<<0)  //1500mv阈值
//04
#define RECEIVE_MODE_S  (0<<6)   //单回波
#define TRIG_EDGE_RISE (0<<5)    //上降沿
#define TRIG_EDGE_FALL (1<<5)    //下降沿
#define TX_PH_SHIFT_POS (0x1f)
//05寄存器 PGA_GAIN_0|PGA_CTRL_EN|LNA_CTRL_EN|LNA_FB|TIMING_REG
#define PGA_GAIN_0  (0<<5)    //0dB增益
#define PGA_GAIN_3  (1<<5)
#define PGA_GAIN_6  (2<<5)
#define PGA_GAIN_9  (3<<5)
#define PGA_GAIN_12 (4<<5)
#define PGA_GAIN_15 (5<<5)
#define PGA_GAIN_18 (6<<5)
#define PGA_GAIN_21 (7<<5)

#define PGA_CTRL_EN  (0<<4)
#define PGA_CTRL_DIS (1<<4)

#define LNA_CTRL_EN  (0<<3)
#define LNA_CTRL_DIS (1<<3)

#define LNA_FB       (0<<2)
#define TIMING_REG   (3<<0) 

//
#define ECHO_TIMEOUT_EN  (0<<2)  //启用回波超时（ 默认设置）
#define ECHO_TIMEOUT_DIS (1<<2)  //禁用超时

#endif