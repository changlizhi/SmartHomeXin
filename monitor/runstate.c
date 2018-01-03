/**
* RunState.c -- 运行状态
* 
* 作者: yangzhilong
* 创建时间: 2009-10-29
* 最后修改时间: 2009-10-29
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEFINE_RUNSTATE

#include "include/basetype.h"
#include "include/debug.h"
#include "include/sys/syslock.h"
#include "include/sys/flat.h"
#include "include/sys/cycsave.h"
#include "include/monitor/runstate.h"
#include "include/environment.h"
#include "include/debug/shellcmd.h"
#include "include/param/unique.h"
#include "include/sys/timeal.h"

#define RUNSTATE_MAGIC            0xfefc9701
#define RUNSTATE_SAVEPATH        PARAM_PATH
//#define FLATID_RUNSTATE        2

runstate_tG RunStateG;

/**
* @brief 获取运行状态修改指针
*/
runstate_tG* RunStateModifyG(void)
{
    return &RunStateG;
}

/**
* @brief 清除运行状态
*/
void ClearRunState(void)
{
    memset(&RunStateG, 0, sizeof(RunStateG));
    SaveRunState();
}

/**
* @brief 运行状态初始化
*/
DECLARE_INIT_FUNC(RunStateInit);
int RunStateInit(void)
{
    memset(&RunStateG, 0, sizeof(RunStateG));
    RunStateG.softchg.flag = 0;
    DebugPrint(0, "  load runstate(size=%d)...", sizeof(RunStateG));
    if(ReadBinFile(RUNSTATE_SAVEPATH "/runstate.db", RUNSTATE_MAGIC, (unsigned char *)&RunStateG, sizeof(RunStateG)) > 0)
    {
        DebugPrint(0, "ok\n");
    }
    else
    {
        DebugPrint(0, "fail\n");

    }
    SET_INIT_FLAG(RunStateInit);
    return 0;
}

/**
* @brief 保存运行状态
*/
void SaveRunState(void)
{
    int ret = 0;
    ret = SaveBinFile(RUNSTATE_SAVEPATH"/runstate.db", RUNSTATE_MAGIC, (unsigned char *)&RunStateG, sizeof(RunStateG));
    //ret = WriteFlatFile(FLATID_RUNSTATE, (unsigned char *)&RunStateG, sizeof(RunStateG));
    DebugPrint(0, "SaveRunState already write %d\n", ret);
}
DECLARE_CYCLE_SAVE(SaveRunState, 0);
