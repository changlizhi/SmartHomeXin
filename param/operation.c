/**
* operation.c -- 参数操作
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-7
* 最后修改时间: 2009-5-8
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "param_config.h"
#include "include/debug.h"
#include "include/sys/syslock.h"
#include "include/param/capconf.h"
#include "include/param/operation.h"
#include "include/param/unique.h"
#include "include/param/sceneuse.h"
#include "include/debug/shellcmd.h"
#include "save/param.h"
#include "operation_inner.h"

static int ParamSaveSysLock = -1;
/**
* @brief 初始化参数储存
* @return 0成功, 否则失败
*/
DECLARE_INIT_FUNC(ParamSaveInit);
int ParamSaveInit(void)
{
    PrintLog(0,"  param save init..\n");

    ParamSaveSysLock = RegisterSysLock();
    if(ParamSaveSysLock < 0) {
        ErrorLog("register syslock fail\n");
        return 1;
    }

    SET_INIT_FLAG(ParamSaveInit);

    return 0;
}

static unsigned int SaveFlag = 0;

/**
* @brief 清除参数储存标志
*/
void ClearSaveParamFlag(void)
{
    SaveFlag = 0;
}

/**
* @brief 设置参数储存标志
* @param flag 标志位
*/
void SetSaveParamFlag(unsigned int flag)
{
    SaveFlag |= flag;
}


typedef int (*savefunc_t)(void);
static const savefunc_t FunctionSave[FILEINDEX_MAX] = {
    SaveParaTerm, SaveParaUni, SaveParaSceneUse,SaveParaTimerTask,
};

/**
* @brief 储存参数
*/
void SaveParam(void)
{
    int index;
    unsigned int mask;

    LockSysLock(ParamSaveSysLock);

    for(index=0,mask=1; index<FILEINDEX_MAX; index++,mask<<=1)
    {
        if(mask&SaveFlag)
        {
            DebugPrint(0, "save index:%d\n", index);
            (*FunctionSave[index])();
        }
    }

    UnlockSysLock(ParamSaveSysLock);

    SaveFlag = 0;
}


