#include "hmi_chart.h"
#include "hmi_driver.h"
#include "cmd_process.h"
#include "TMC5160A.h"
#include "TMC5160_Register.h"
#include "TMC5160_Constants.h"
#include "TMC5160_Fields.h"
#include "AnglePulse.h"
//#include "TMC5160.h"
void hmi_StallGuard2_update(void)
{
	uint8_t tempu8[2];
	uint16_t tempu16;
	uint8_t i=0;
//	GraphChannelDataClear(current_screen_id,24,0);
//	GraphChannelDataClear(current_screen_id,24,1);
	if(TMC5160_Registers.TSTEP>2813)
	{
		tempu16=0xffff;
	}
	else
	{
		tempu16=(float)TMC5160_Registers.TSTEP/1407*65535;
	}
	tempu8[0]=(tempu16>>8);
	tempu8[1]=(tempu16&0xff);
	GraphChannelDataAdd(current_screen_id,24,0,tempu8,2); 
	tempu16=(float)TMC5160_Registers.DRV_STATUS.SG_RESULT/1024*65535;
	tempu8[0]=(tempu16>>8);
	tempu8[1]=(tempu16&0xff);
	GraphChannelDataAdd(current_screen_id,24,1,tempu8,2); 
	tempu16=(float)AngleP_Speed/1024*65535;
	tempu8[0]=(tempu16>>8);
	tempu8[1]=(tempu16&0xff);
	GraphChannelDataAdd(current_screen_id,24,2,tempu8,2); 
}


