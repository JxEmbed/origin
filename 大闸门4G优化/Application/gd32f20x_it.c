/*!
    \file    gd32f20x_it.c
    \brief   interrupt service routines

    \version 2023-06-30, V2.5.0, firmware for GD32F20x
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "gd32f20x_it.h"
#include "main.h"
#include "systick.h"
#include "cmd_queue.h"
#include "AnglePulse.h"
#include "TMC5160A.h"
#include "Lora.h"
#include "StallGuard.h"
#include "AnglePulse.h"
/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
//void SVC_Handler(void)
//{
//}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
//void PendSV_Handler(void)
//{
//}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
//RTC_Alarm中断
void RTC_Alarm_IRQHandler(void)
{
	if(RESET != rtc_flag_get(RTC_CTL_ALRMIF))
	{
        /* clear the RTC alarm and EXTI_17 interrupt flags */
        rtc_flag_clear(RTC_CTL_ALRMIF);
        exti_interrupt_flag_clear(EXTI_17);
   }
}	
void xPortSysTickHandler( void );
void SysTick_Handler(void)
{
//	delay_decrement();


	#if (INCLUDE_xTaskGetSchedulerState  == 1 )
		if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
		{
	#endif   
			xPortSysTickHandler();
	#if (INCLUDE_xTaskGetSchedulerState  == 1 )
		}
	#endif  
//————————————————
//版权声明：本文为CSDN博主「欢喜6666」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
//原文链接：https://blog.csdn.net/qq_37554315/article/details/131060820
}
void USART1_IRQHandler(void)
{
	uint8_t rxdata;
	if(RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE)) /* receive */
	{ 
	/* receive data */
		rxdata = usart_data_receive(USART1); /* 接收 */
		usart_interrupt_flag_clear(USART1,USART_INT_FLAG_RBNE); 
		if(RS485_RxState==1)
		{
			return;
		}
		if(RS485_RxPtr<RS485_MAX_SIZE)
		{
			RS485_RxBuff[RS485_RxPtr++]=rxdata;
		}
		
		
	}
	if(RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_IDLE))
	{
		rxdata = usart_data_receive(USART1); /* 接收 */
		usart_interrupt_flag_clear(USART1,USART_INT_FLAG_IDLE);
		RS485_RxState=1;
		xSemaphoreGive(RS485_semRx);
		//RS485_RxPtr=0;
	}
}

void UART7_IRQHandler(void)
{
	u8 res;u32 retry=0;     
	//串口接收中断
	 if(RESET != usart_interrupt_flag_get(UART7, USART_INT_FLAG_RBNE)) /* receive */
	{
		res =usart_data_receive(UART7); /* 接收 */
		usart_interrupt_flag_clear(UART7,USART_INT_FLAG_RBNE); 
		_4GCat1_Usart.rxtimer=20;//1ms 100 10
		if(_4GCat1_Usart.rxpointer<UART_BUFFER_LEN)
		{
			_4GCat1_Usart.rxbuffer[_4GCat1_Usart.rxpointer++]=res; 				//读取接收到的数据
		}			 
	}  	
	if(RESET != usart_interrupt_flag_get(UART7, USART_INT_FLAG_IDLE))
	{
		res = usart_data_receive(UART7); /* 接收 */
		usart_interrupt_flag_clear(UART7,USART_INT_FLAG_IDLE);
		
	}
	//串口发送中断
	if(RESET != usart_interrupt_flag_get(UART7, USART_INT_FLAG_TBE))
	{
		usart_interrupt_flag_clear(UART7 ,USART_INT_FLAG_TBE);  
		_4GCat1_Usart.txpointer++;//发送数组指针+1 
		
		//如果没有发送完成则继续发送下一个字节
		if(_4GCat1_Usart.txpointer<_4GCat1_Usart.txlen)
		{
			while(usart_flag_get(UART7, USART_FLAG_TC) == RESET)
			{
				retry++;
				delay_ms(1);
				if(retry>100)break;
			}	retry=0; 
			usart_data_transmit(UART7, _4GCat1_Usart.txbuffer[_4GCat1_Usart.txpointer]); /* transmit */
			while(usart_flag_get(UART7, USART_FLAG_TC) == RESET)
			{
				retry++;
				delay_ms(1);
				if(retry>100)break;
			}	retry=0; 
			
		}
		//如果已经发送完成
		else
		{
			usart_interrupt_disable(UART7, USART_INT_TBE);  /* 参数：读数据缓冲区非空中断和过载错误中断 */
			
//			retry=0;
//			while(usart_flag_get(UART7, USART_FLAG_TC) != SET)
//			{
//				retry++;
//				delay_us(10);
//				if(retry>30000) break;
//			}	retry=0; 	
			xSemaphoreGive(_4GCat1_Usart.semTx);		
		}  
	}
	
}
void USART5_IRQHandler(void)
{
	u8 res;
	//串口接收中断
	 if(RESET != usart_interrupt_flag_get(USART5, USART_INT_FLAG_RBNE)) /* receive */
	{
		res =usart_data_receive(USART5); /* 接收 */
		usart_interrupt_flag_clear(USART5,USART_INT_FLAG_RBNE); 
		if(L76K_RxBuffPtr<L76K_REBUFF_LEN)	
		{
			L76K_RxBuff[L76K_RxBuffPtr++]=res;
		}			
	}  	
	if(RESET != usart_interrupt_flag_get(USART5, USART_INT_FLAG_IDLE))
	{
		res = usart_data_receive(USART5); /* 接收 */
		usart_interrupt_flag_clear(USART5,USART_INT_FLAG_IDLE);
		L76K_RxFlag=1;
		L76K_RxBuffPtr=0;
	}
}
u8 testC=0;
void EXTI5_9_IRQHandler(void)
{
	if(RESET!=exti_interrupt_flag_get(EXTI_5))
	{
		exti_interrupt_flag_clear(EXTI_5);
		testC++;
	}
	if(RESET != exti_interrupt_flag_get(EXTI_6)) /* receive */
	{
		exti_interrupt_flag_clear(EXTI_6);
		if(PDin(6)==0)  //霍尔双向开关
		{
			forw_pulse++;			
			if(PDin(7)==0)
			{
				Mess_Par.run_step++;
				AngleP_Step++;
			}
		}
	}
	if(RESET!=exti_interrupt_flag_get(EXTI_7))
	{
		exti_interrupt_flag_clear(EXTI_7);
		if(PDin(7)==0)    //霍尔双向开关
		{
			back_pulse++;
			if(PDin(6)==0)
			{
				AngleP_Step--;				//计算速度
				if(Mess_Par.run_step>0)
				{
					Mess_Par.run_step--;
				}
				TMC5160A_TestStep_Count();
			}
		}
	}
	if(RESET!=exti_interrupt_flag_get(EXTI_8))
	{
		exti_interrupt_flag_clear(EXTI_8);
	}
	if(RESET!=exti_interrupt_flag_get(EXTI_9))
	{
		exti_interrupt_flag_clear(EXTI_9);
		if(FLOAT_BALL_IN==1)
		{
			
		}
	}
}

void EXTI10_15_IRQHandler(void)
{
	if(RESET != exti_interrupt_flag_get(EXTI_11)) /* receive */
	 {
		exti_interrupt_flag_clear(EXTI_11);
		if(PDin(11)==1&&Control_Par.setState==GATE_DOWN)   //下限位
		{
			if(Mess_Par.run_step!=0)			//已经触发下限位 但步数没有递减到0
			{
				Mess_Par.run_step=0;
			}
			if(nosave_par.test_step_flag==1&&nosave_par.test_step_state==4)
			{
				nosave_par.test_step_state=5;
			}
			TMC5160A_STOP();
			nosave_par.save_Mess_Par_flag=1;
			if(Control_Par.control_mode==0)		//如果是开度控制
			{
				Control_Par.control_run=0;
			}
			Control_Par.setState=GATE_STOP;
			
		}
	 }
	 if(RESET != exti_interrupt_flag_get(EXTI_13)) /* receive */
	 {
		exti_interrupt_flag_clear(EXTI_13);
		
		if(PDin(13)==1&&Control_Par.setState==GATE_UP)  //上限位
		{
			
			if(nosave_par.test_step_flag==1&&nosave_par.test_step_state==2)
			{
				nosave_par.test_step_state=3;
			}
			TMC5160A_STOP();
			nosave_par.save_Mess_Par_flag=1;
			if(Control_Par.control_mode==0)
			{
				Control_Par.control_run=0;
			}
			Control_Par.setState=GATE_STOP;
		}
	 }
	 if(RESET != exti_interrupt_flag_get(EXTI_10)) /* receive */
	 {
		exti_interrupt_flag_clear(EXTI_10);
		 //ALM信号
		if(PDin(10)==1)
		{

			
			
		}
	 }
	  
	  if(RESET != exti_interrupt_flag_get(EXTI_12)) /* receive */
	 {
		exti_interrupt_flag_clear(EXTI_12);
	 }
	 if(RESET != exti_interrupt_flag_get(EXTI_14)) /* receive */
	 {
		exti_interrupt_flag_clear(EXTI_14);
		 if(TMC5160A_DIAG0==0)
		 {
		
		 }
	 }
	 if(RESET != exti_interrupt_flag_get(EXTI_15)) /* receive */
	 {
		exti_interrupt_flag_clear(EXTI_15);
	 }
	  
	
}



void UART3_IRQHandler(void)
{
	u8 res;
	//串口接收中断
	 if(RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_RBNE)) /* receive */
	{
		res =usart_data_receive(UART3); /* 接收 */
		usart_interrupt_flag_clear(UART3,USART_INT_FLAG_RBNE); 
		if(Lora_RxBuffPtr<Lora_RxbuffLen)	
		{
			Lora_Rxbuff[Lora_RxBuffPtr++]=res;
		}
	}
	if(RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_IDLE))
	{
		res = usart_data_receive(UART3); /* 接收 */
		usart_interrupt_flag_clear(UART3,USART_INT_FLAG_IDLE);
		Lora_RxFlag=1;
		
	}
}
void DMA1_Channel4_IRQHandler(void)
{
	if(RESET!=dma_interrupt_flag_get(DMA1,DMA_CH4,DMA_INT_FLAG_FTF))
	{
		dma_interrupt_flag_clear(DMA1,DMA_CH4,DMA_INT_FLAG_FTF);
		xSemaphoreGive(LCD_semTx);
	}
}

//RI_4G  收到云端发来信息 
void EXTI2_IRQHandler(void)
{
	if(RESET!=exti_interrupt_flag_get(EXTI_2))
	{
		exti_interrupt_flag_clear(EXTI_2);
		nosave_par.URC_ComF=1;
	}
}
