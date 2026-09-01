#include "Hall_Flow.h"



void Hall_Flow_init(void)
{
	//配置PA1外部中断
	rcu_periph_clock_enable(RCU_GPIOB);//GPIOA时钟使能
	gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_5);//PA1配置成浮空输入
	gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOB, GPIO_PIN_SOURCE_5);//connect key     EXTI line to key GPIO pin
	exti_init(EXTI_5, EXTI_INTERRUPT, EXTI_TRIG_FALLING);//下降沿中断
	exti_interrupt_flag_clear(EXTI_5);//清中断标志
	nvic_irq_enable(EXTI5_9_IRQn, 2U, 0U);//enable and set key EXTI interrupt to the lowest priority 

}

