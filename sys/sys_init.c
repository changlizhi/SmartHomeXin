/**
* syscall_init.c -- 系统抽象模块初始化
* 
* 作者: zhuzhiqiang
* 创建时间: 2008-3-31
* 最后修改时间: 2009-5-6
*/

#include <stdio.h>

#include "include/debug.h"
extern int DriverInit(void);
extern int SysCallInit(void);
extern int DriverHighInit(void);
extern int StorageInit(void);

/**
* @brief 系统抽象模块初始化函数
* @return 返回0表示成功, 否则失败
*/
DECLARE_INIT_FUNC(SysInit);
int SysInit(void)
{

    if(SysCallInit()) return 1;
    if(DriverHighInit()) return 1;
    if(StorageInit()) return 1;

    SET_INIT_FLAG(SysInit);
    return 0;
}

