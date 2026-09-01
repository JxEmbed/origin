#include "test.h"
#include "ff.h"
#include "string.h"
#include "stdio.h"
#include "hmi_driver.h"

DIR dp;	
FILINFO Fileinfo;
uint16_t count = 0;

void testshow(char * str);
void testSD(uint8_t *dir_path)
{
	u8 ret;
	ret = f_opendir(&dp, (char *)dir_path);
    if (ret == FR_NO_PATH)  // 没有该文件夹
    {
       // printf("没有 %s\n", dir_path);
     //   while(1);
    }                                               
    else if (ret != FR_OK)
    {
    //    printf("打开 %s 失败\n", dir_path);
    }
    else	// 打开成功
    {
        while(1)
        {
            ret = f_readdir(&dp, &Fileinfo);
            if(ret != FR_OK || Fileinfo.fname[0]==0 )
            {
                break;		// 读取失败或者读取完所有条目
            }
            else if (Fileinfo.fname[0] == '.')	// 隐藏文件
            {
                continue;
            }
            else if(Fileinfo.fattrib & AM_DIR)	// 是文件夹
            {
//                printf("Directory\n");
                continue;	// 文件夹不处理，继续读下一个
            }
            else
            {
				if (Fileinfo.fname[0] != 0)
				{
					snprintf(lcd_datu8,sizeof(lcd_datu8),"%s;;",Fileinfo.fname); //本年水量
					testshow(lcd_datu8);
					count++;
//                        printf("%s\n", Fileinfo.fname);	// 打印短文件名
				}
            }  
        }
	}
}



void testshow(char * str)
{
	
			
	Record_Add(16,9,(u8 *)str);
}