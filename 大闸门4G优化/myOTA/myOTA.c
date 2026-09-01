#include "myOTA.h"
#include "string.h"
#include "md5.h"
#include "stdio.h"
NET_USERINFO net_user_info={"154787","O1Uvgm+SArsdiiyLiiVTucZXRxVIxJPXU7mA+m0lp4SANwEf2khZ5AwOPg/WmrLiP4m29jJDnoP3a2Fh5f8Ucg=="}; 
load_a load_A;
UpdateSTRUCT Update_Par;
MYOTA_INFO myota_info;

MD5_CTX md5_ctx;											//MD5相关变量
char md5_result[40];
unsigned char md5_t[16];

char md5_t1[4] = {0, 0, 0, 0};
void LOAD_A_Clear(void)
{
	nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
	nvic_irq_disable(TIMER1_IRQn);
	 timer_interrupt_disable(TIMER1, TIMER_INT_UP);
	 timer_deinit(TIMER1);
		rcu_periph_clock_disable(RCU_TIMER1);
		usart_deinit(UART7);
		gpio_deinit(GPIOE);
		gpio_deinit(GPIOB);
		gpio_deinit(GPIOG);
	rcu_periph_clock_disable(RCU_GPIOB);
	rcu_periph_clock_disable(RCU_GPIOG);
}

/*
		GD32_EraseFlash(225,1);
*/

void GD32_Read_Version(void)					//没有调用
{
	u16 myota_info_size=0;
	u16 rnum=0;
	u32 strlent=0;
	myota_info_size=sizeof(myota_info);
	if(myota_info_size%4==0)
	{
		rnum=myota_info_size/4;
	}
	else
	{
		rnum=myota_info_size/4+1;
	}
	GD32_ReadFlash_Word(MYOTA_INFO_ADDR,(uint32_t *)&myota_info,rnum);

	//如果字符串没有写入则重新赋值版本号
	if((u8)myota_info.nowVersion[0]==0xff)
	{
		strcpy(myota_info.nowVersion,"t1.0.0");
	}
	strlent=strlen(myota_info.nowVersion);
	//如果字符串过长则重新赋值版本号
	if(strlent>=(VERSION_LEN-1)||strlent==0)
	{
		strcpy(myota_info.nowVersion,"t1.0.0");
	}
}
void GD32_Write_Version(void)    				//没有调用
{
	u16 myota_info_size=0;
	u16 wnum=0;
	myota_info_size=sizeof(myota_info);
	if(myota_info_size%4==0)
	{
		wnum=myota_info_size/4;
	}
	else
	{
		wnum=myota_info_size/4+1;
	}
	//myota_info
	GD32_EraseFlash(225,1);
	GD32_WriteFlash_Word(MYOTA_INFO_ADDR,(uint32_t *)&myota_info,wnum);
}

void JumpToApp(void)
{
	LOAD_A(MYOTA_APP1_ADDR);
}
//GD32_EraseFlash(19,100);
/*
如果当前使用app1，则擦除app2
*/	
void Perpare_Erase(void)						//没有调用
{

	GD32_EraseFlash(125,100);
	

}
//GD32_WriteFlash_Word((0x0800C800+downStart),(uint32_t *)GD32_wrdata,tempu16); //GD32_wrdata
/*
将下载的内容写入flash
如果当前使用app1，则写入到app2
*/
void Download_WriteFlash(uint32_t downStart,uint32_t wnum)
{
	GD32_WriteFlash_Word((MYOTA_APP2_ADDR+downStart),(uint32_t *)GD32_wrdata,wnum); //GD32_wrdata
}


void VersionFallback(void)
{
	
}
void Set_NVIC_Offset(void)
{

	nvic_vector_table_set(MYOTA_APP1_ADDR,0);//设置偏移量
	
}
__asm void MSR_SP(uint32_t addr)
{
	MSR MSP,r0
	BX  r14   //
}
void LOAD_A(uint32_t addr)
{
	//0x2003FFFF
	//0x20011EB8
	//如果跳转地址争取，则跳转
	if((*(uint32_t *)addr>=0x20000000)&&(*(uint32_t *)addr<=0x2003FFFF))
	{
		LOAD_A_Clear();
		MSR_SP(*(uint32_t *)addr);              //设置sp
		load_A=(load_a)*(uint32_t *)(addr+4);   //设置pc
		
		load_A();
	}
	else //如果跳转地址错误，则版本回退，并复位
	{
		VersionFallback();
		__set_FAULTMASK(1); // 关闭所有中端
		NVIC_SystemReset(); // 复位
	}
}

void md5_test(void)
{
	u16 i;
	MD5_Init(&md5_ctx);
	for(i=0;i<1024;i++)
	{
		GD32_wrdata[i]='8';
	}
	MD5_Update(&md5_ctx, GD32_wrdata, 1024);
	memset(md5_result, 0, sizeof(md5_result));
	MD5_Final(&md5_ctx, md5_t);
	for(i = 0; i < 16; i++)
	{
		if(md5_t[i] <= 0x0f)
			sprintf(md5_t1, "0%x", md5_t[i]);
		else
			sprintf(md5_t1, "%x", md5_t[i]);
		
		strcat(md5_result, md5_t1);
	}
}