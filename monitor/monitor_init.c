/**
* mdb.c -- 监测模块初始化
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-15
* 最后修改时间: 2009-5-15
*/

#include <stdio.h>

#include "include/debug.h"

extern int MonitorTaskInit(void);

/**
* @brief 监测模块初始化
* @return 成功0, 否则失败
*/
DECLARE_INIT_FUNC(MonitorInit);
int MonitorInit(void)
{
    PrintLog(0,"monitor init...\n");
    MonitorTaskInit();

    SET_INIT_FLAG(MonitorInit);
    return 0;
}
