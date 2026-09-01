#include "rundata.h"
#include "math.h"

RUNLOG_STRUCT RunLog_Par;  //运行数据
RUNLOG_STRUCT RunLog_Par_Old;  //运行数据
NOSAVE_STRUCT nosave_par={0,1,1}; //不需要存储的数据
/*
处理运行数据
需要放到1秒执行一次的位置
*/
