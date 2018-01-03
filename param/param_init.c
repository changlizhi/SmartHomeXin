/**
* operation.c -- 参数模块初始化
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-8
* 最后修改时间: 2009-5-8
*/

#include <stdio.h>

#include "include/debug.h"
#include "include/param/term.h"
#include "include/param/sceneuse.h"
#include "include/sys/schedule.h"
#include "operation_inner.h"
#include "include/environment.h"
extern int ParamSaveInit(void);
/**
* @brief 参数模块初始化
* @return 0成功, 否则失败
*/
DECLARE_INIT_FUNC(ParamInit);
int ParamInit(void)
{
    PrintLog(0,"load param ...\n");
    PrintLog(0,"  LoadParaUni ...\n");
    LoadParaUni();//读自定义参数
    PrintLog(0,"  LoadParaTerm ...\n");
    LoadParaTerm(); //读文件中的终端 配置参数信息
//    LoadParaDataUse();    //读取情景列表
//    LoadParaTimerTask();//读取定时任务

    Sleep(500);
    if(ParamSaveInit()) return 1;
    SET_INIT_FLAG(ParamInit);
    return 0;
}


