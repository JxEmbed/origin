#ifndef __SYS_H
#define __SYS_H	
#include "gd32f20x.h"

//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
#define output_offset 0x0c
#define input_offset  0x08
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA+output_offset) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB+output_offset) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC+output_offset) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD+output_offset) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE+output_offset) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF+output_offset) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG+output_offset) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA+input_offset) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB+input_offset) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC+input_offset) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD+input_offset) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE+input_offset) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF+input_offset) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG+input_offset) //0x40011E08 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

#endif
