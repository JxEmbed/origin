#include "other_bsp.h"
/*
赫斯曼浮球开关		一般为常闭   浮上去断开，落下来闭合	--->水位将浮球顶起来了  常闭 0--->开 1
PDin(9)
*/
void Float_Ball_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOG);
	//lora电源使能引脚
    gpio_init(GPIOG, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
	
	//中断配置
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOG, GPIO_PIN_SOURCE_9);//connect key     EXTI line to key GPIO pin
	exti_init(EXTI_9, EXTI_INTERRUPT, EXTI_TRIG_RISING);//上升沿中断
	exti_interrupt_flag_clear(EXTI_9);//清中断标志
	nvic_irq_enable(EXTI5_9_IRQn, 2U, 0U);//enable and set key EXTI interrupt to the lowest priority 
}
