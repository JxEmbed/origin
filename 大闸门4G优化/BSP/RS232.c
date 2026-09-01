#include "RS232.h"
#define TXBUFFER_LEN 100
uint8_t txbuffer[TXBUFFER_LEN];
#define USART0_DATA_ADDRESS      ((uint32_t)&USART_DATA(USART0))
void USART1_Config(uint32_t baudval)
{
    /* 初始化GPIO外设 */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* TX管脚，PA9，复用推挽输出，速度50MHz */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    /* RX管脚，PA10，，速度50MHz */
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_3);

    /* 初始化USART外设 */
    rcu_periph_clock_enable(RCU_USART1);  // 使能串口0时钟
		usart_deinit(USART1);
    usart_baudrate_set(USART1, baudval);  // 波特率115200
    usart_parity_config(USART1, USART_PM_NONE);  // 无校检
    usart_word_length_set(USART1, USART_WL_8BIT);  // 8位数据位
    usart_stop_bit_set(USART1, USART_STB_1BIT);  // 1位停止位
	  usart_hardware_flow_rts_config(USART1, USART_RTS_DISABLE); /* 禁用rts */
    
    usart_hardware_flow_cts_config(USART1, USART_CTS_DISABLE); /* 无硬件数据流控制 */

    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);  // 使能串口发送
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);  // 使能串口接收
    usart_enable(USART1);  // 使能串口
		
		nvic_irq_enable(USART1_IRQn,0U, 0U);
		usart_interrupt_enable(USART1, USART_INT_RBNE);  /* 参数：读数据缓冲区非空中断和过载错误中断 */
		usart_interrupt_enable(USART1, USART_INT_IDLE);  /* 参数：读数据缓冲区非空中断和过载错误中断 */
}

