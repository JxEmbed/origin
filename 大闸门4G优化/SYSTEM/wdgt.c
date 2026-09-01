#include "wdgt.h"
/*
独立看门狗计算

设置预分频 40K/64=0.625khz 一个周期是 1.6ms
					40K/128=312.5hz 一个周期是3.2ms
参数1：重载值 参数2：预分频值 


10s进入

//————————————————
//版权声明：本文为CSDN博主「小吴同学啊」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
//原文链接：https://blog.csdn.net/weixin_43647919/article/details/128903502

*/

void gd32_wdgt_init(void)//
{
	//开启时钟源（IRC40K)，并等待其稳定以供看门狗使用
	rcu_osci_on(RCU_IRC40K); //开启IRC40k 默认就是 IRC40k的时钟
	while(SUCCESS != rcu_osci_stab_wait(RCU_IRC40K)){}	//等待IRC40k稳定下来
	
	//使能寄存器 写功能
    fwdgt_write_enable();
    //设置预分频 40K/64=0.625k 一个周期是 1.6ms
    //设置初值
    fwdgt_config(3125, FWDGT_PSC_DIV128); //2400*1.6ms=3.84S
    //喂狗
    fwdgt_counter_reload();
	fwdgt_write_disable();
    //使能独立看门狗
    fwdgt_enable();
}
void gd32_wdgt_feed_dog(void)
{
	fwdgt_write_enable();
	fwdgt_counter_reload();
	fwdgt_write_disable();
}