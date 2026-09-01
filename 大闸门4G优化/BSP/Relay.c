#include "Relay.h"

void DoublePole_init(void)
{
	 rcu_periph_clock_enable(RCU_GPIOA);
    /* TX管脚，PA9，复用推挽输出，速度50MHz */
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
	rcu_periph_clock_enable(RCU_GPIOG);
    /* TX管脚，PA9，复用推挽输出，速度50MHz */
    gpio_init(GPIOG, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
	rcu_periph_clock_enable(RCU_GPIOD);
    /* TX管脚，PA9，复用推挽输出，速度50MHz */
    gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
	gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
	rcu_periph_clock_enable(RCU_GPIOE);
    /* TX管脚，PA9，复用推挽输出，速度50MHz */
    gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
	rcu_periph_clock_enable(RCU_GPIOF);
    /* TX管脚，PA9，复用推挽输出，速度50MHz */
    gpio_init(GPIOF, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
	gpio_bit_set(GPIOA, GPIO_PIN_8);
	gpio_bit_set(GPIOG, GPIO_PIN_4);
}
void SinglePole_init(void)
{
	 rcu_periph_clock_enable(RCU_GPIOD);
    /* TX管脚，PA9，复用推挽输出，速度50MHz */
   gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
}

void MOS_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOG);
    /* TX管脚，PA9，复用推挽输出，速度50MHz */
	gpio_init(GPIOG, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
	MOS_PIN=0;
}

void MOS_test(void)
{
	while(1)
	{
		MOS_PIN=!MOS_PIN;
		delay_ms(500);
	}
}



















