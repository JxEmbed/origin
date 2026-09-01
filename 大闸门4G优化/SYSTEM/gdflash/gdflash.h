#ifndef _GDFLASH_H
#define _GDFLASH_H
#include "main.h"
/*
0x0800C800
0x32000
*/
#define MYOTA_APP1_ADDR 0x0800C800
#define MYOTA_APP2_ADDR 0x0803E800
#define MYOTA_INFO_ADDR 0x08070800





void GD32_EraseFlash(uint32_t start,uint16_t num);
void GD32_WriteFlash_Word(uint32_t saddr,uint32_t *wdata,uint32_t wnum);
void GD32_ReadFlash_Word(uint32_t saddr,uint32_t *rdata,uint32_t rnum);

void gd32flash_test(void);
extern  u8 GD32_wrdata[1024];

union gd32_union_test
{
	u8 d_u8[4];
  u32 d_u32;
};
#endif
