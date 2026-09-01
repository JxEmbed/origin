#include "gdflash.h"

u8 GD32_wrdata[1024]={0};


/*
start 起始地址
num   擦除数量
*/

void GD32_EraseFlash(uint32_t start,uint16_t num)
{
	uint16_t i;
	fmc_unlock();  //解锁所有扇区
	for(i=0;i<num;i++)
	{
		fmc_page_erase((0x08000000+start*2*1024)+(1024*2*i));
	}
	fmc_lock();
}

void GD32_WriteFlash_Word(uint32_t saddr,uint32_t *wdata,uint32_t wnum)
{
	uint32_t i;
	fmc_unlock();  //解锁所有扇区
	for(i=0;i<wnum;i++)
	{
		fmc_word_program(saddr+i*4,*(wdata+i));
	}
	fmc_lock();
}
void GD32_ReadFlash_Word(uint32_t saddr,uint32_t *rdata,uint32_t rnum)
{
	uint32_t i;
	for(i=0;i<rnum;i++)
	{
		rdata[i]=*(uint32_t *)(saddr+i*4);
	}
	
}

void gd32flash_test(void)
{
	u8 wrdata[100];
	u8 rdata[100];
	union gd32_union_test gd32_union_dat;
	gd32_union_dat.d_u8[0]=1;
	gd32_union_dat.d_u8[1]=2;
	gd32_union_dat.d_u8[2]=3;
	gd32_union_dat.d_u8[3]=4;
	uint32_t i=0;
	GD32_EraseFlash(19,100);
	for(i=0;i<99;i++)
	{
		wrdata[i]=i;
	}
	GD32_WriteFlash_Word(0x0800C800,(uint32_t *)wrdata,25);
	GD32_ReadFlash_Word(0x0800C800,(uint32_t *)rdata,25);
}



