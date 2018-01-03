/**
* driver_init.c -- 驱动初始化函数
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-4-24
* 最后修改时间: 2009-5-6
*/

#include <stdio.h>
#include "include/debug.h"

extern int GpioInit(void);

/**
* @brief 驱动接口子模块初始化函数(高级部分)
* @return 0成功, 否则失败
*/
DECLARE_INIT_FUNC(DriverHighInit);
int DriverHighInit(void)
{
    if(GpioInit()) return 1;
    SET_INIT_FLAG(DriverHighInit);
    return 0;
}
