#ifndef __GPRSTASK_H
#define __GPRSTASK_H

#include "main.h"

struct RANDER_FLOW_DATA
{

	float air_hight;			
	float silt_deep;				
	float trapezoid_width_T;
	float trapezoid_width_B;
	float trapezoid_hight;
	float trapezoid_area;
	float average_speed;		//三台流速仪的平均流速
	float theta;
	float flow;
	float a;								//边坡系数固定值1.25
	float K;								//断面平均流速与表水平均流速比固定值取0.82
	float H1;								//水位计探头距渠堤距离
	float H2;								//泥位传感器距渠堤距离
	float h1;								//水位计读取的数据
	float h2;								//泥位计读取的数据
	float B;								//渠宽
	float A;								//断面面积
};
void SQ_DATA_Trans(void);

#endif

