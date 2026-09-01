#include "Dev_data.h"
#include "string.h"
DEV_STRUCT dev_SavePar;  //设备存储的信息


Author_STRUCT Author_Par; //权限管理信息
CONTROL_STRUCT Control_Par;  //控制参数
RUNMESS_STRUCT Mess_Par; //运行信息 



void Zhangbaoqu_DevPar(void)
{
	memset(&dev_SavePar,0,sizeof(dev_SavePar));
	strcpy(dev_SavePar.dev_name,"渠道智能测控分体板闸");
//strcpy(dev_SavePar.factory_time,"202309");
	strcpy(dev_SavePar.dev_code,"5279560816674");
	strcpy(dev_SavePar.software_version,"V1.1.3");
	strcpy(dev_SavePar.form_factor,"2.0m*2.2m");
	strcpy(dev_SavePar.motor_power,"750W");
	dev_SavePar.water_inletWidth=2.2f;
	dev_SavePar.water_inletHeight=1.2f;
	dev_SavePar.Ultra_Spacing=0.1f;
	dev_SavePar.base_heightDiff=0;
	dev_SavePar.Ultra_initialDist=0.1;
	dev_SavePar.Ultra_Num=9;
	dev_SavePar.FroHeight_Air=3.75;
	dev_SavePar.AftHeight_Air=0.99;
	dev_SavePar.Gradient=0.001;
	dev_SavePar.flow_min=0;
	dev_SavePar.flow_max=0.7;
	dev_SavePar.flow_deviation=5;
	dev_SavePar.Side_K=0.85;
	dev_SavePar.level_min=0;
	dev_SavePar.level_max=1;
	dev_SavePar.level_deviation=5;
	dev_SavePar.Correct_K=1.02;
	dev_SavePar.data_interval=10;
	dev_SavePar.send_interval=20;
	dev_SavePar.Channel_Angle=70;
	dev_SavePar.Roughness=0.012;
	Flash_SaveDevData();
}










