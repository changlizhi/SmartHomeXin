/**
* datause.c -- 支持数据配置参数
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-8
* 最后修改时间: 2009-5-8
*/

#include <stdio.h>
#include <string.h>

#define DEFINE_PARASCENEUSE

#include "param_config.h"
#include "include/debug.h"
#include "include/param/sceneuse.h"
#include "include/param/operation.h"

#include "operation_inner.h"

#include "save/param.h"
#include "include/sys/bin.h"

para_sceneuse_t ParaSceneUse;


/**
* @brief 载入缺省支持数据参数
*/
static void LoadDefParaDataUse(void)
{
    memset(&ParaSceneUse, 0, sizeof(ParaSceneUse));
    int i,j;
     //添加默认为全关的情景
//     ParaSceneUse.scenecls[0].num = 0xFF; //全关
//     ParaSceneUse.scenecls[1].num = 0xFF;//全开
     ParaSceneUse.flag = 0x003F;
     ParaSceneUse.scenecls[0].id = 1;
     ParaSceneUse.scenecls[0].num = 5;
     memcpy(ParaSceneUse.scenecls[0].scene_name,"All Open",10);


     ParaSceneUse.scenecls[0].num = j;

     ParaSceneUse.scenecls[1].id = 2;
    // ParaSceneUse.scenecls[1].num = 5;
     memcpy(ParaSceneUse.scenecls[1].scene_name,"All Close",10);
     ParaSceneUse.scenecls[1].num = j;


     ParaSceneUse.scenecls[2].id = 3;
     ParaSceneUse.scenecls[2].num = 0;
     memcpy(ParaSceneUse.scenecls[2].scene_name,"Scene3",7);
     ParaSceneUse.scenecls[3].id = 4;
     ParaSceneUse.scenecls[3].num = 0;
     memcpy(ParaSceneUse.scenecls[3].scene_name,"Scene4",7);
     ParaSceneUse.scenecls[4].id = 5;
     ParaSceneUse.scenecls[4].num = 0;
     memcpy(ParaSceneUse.scenecls[4].scene_name,"Scene5",7);
     ParaSceneUse.scenecls[5].id = 6;
     ParaSceneUse.scenecls[5].num = 0;
     memcpy(ParaSceneUse.scenecls[5].scene_name,"Scene6",7);

}

#define SCENEUSE_MAGIC    0x9572acd0

/**
* @brief 从文件中载入支持数据参数
* @return 0成功, 否则失败
*/
int LoadParaDataUse(void)
{
    LoadDefParaDataUse();

    PrintLog(0, "  load param datause...(size=%d)", sizeof(ParaSceneUse)+12);

    if(ReadBinFile(PARAM_SAVE_PATH "/sceneuse.bin", SCENEUSE_MAGIC, (unsigned char *)&ParaSceneUse, sizeof(ParaSceneUse)) < 0) {
        PrintLog(0, "sceneuse.bin no main, ");
        if(ReadBinFile(PARAM_BAK_PATH "/sceneuse.bin", SCENEUSE_MAGIC, (unsigned char *)&ParaSceneUse, sizeof(ParaSceneUse)) < 0) {
            PrintLog(0, "sceneuse.bin no bak\n");
            SetSaveParamFlag(SAVFLAG_SCENE);
            return 1;
    }
    }

    PrintLog(0, "sceneuse.bin ok\n");
    SetSaveParamFlag(SAVFLAG_SCENE);
    return 0;
}


/**
* @brief 保存支持数据参数
* @return 0成功, 否则失败
*/
int SaveParaSceneUse(void)
{
    SaveBinFile(PARAM_SAVE_PATH "/sceneuse.bin", SCENEUSE_MAGIC, (unsigned char *)&ParaSceneUse, sizeof(ParaSceneUse));
    SaveBinFile(PARAM_BAK_PATH "/sceneuse.bin", SCENEUSE_MAGIC, (unsigned char *)&ParaSceneUse, sizeof(ParaSceneUse));

    return 0;
}


cfg_timertaskcls_data_t ParaTimerTask[MAX_TIMERTASK];

/**
* @brief 载入缺省支持的定时任务
*/
static void LoadDefParaTimerTask(void)
{
    memset(ParaTimerTask, 0, sizeof(ParaTimerTask));

    ParaTimerTask[0].id = 1;
    ParaTimerTask[0].num = 1;
    ParaTimerTask[0].bStart = 1;
    ParaTimerTask[0].hour = 0;
    ParaTimerTask[0].min = 0;
    ParaTimerTask[0].weekflag = 0xff;
    memcpy(ParaTimerTask[0].task_name,"All Open",10);
    ParaTimerTask[0].met[0].attr = 2;    //1//1表示开关设备,2表示情景
    ParaTimerTask[0].met[0].metid = 1;

    ParaTimerTask[0].id = 2;
    ParaTimerTask[0].num = 1;
    ParaTimerTask[0].bStart = 1;
    ParaTimerTask[0].hour = 12;
    ParaTimerTask[0].min = 0;
    ParaTimerTask[0].weekflag = 0xff;
    memcpy(ParaTimerTask[0].task_name,"All Close",sizeof("All Close"));
    ParaTimerTask[0].met[0].attr = 2;    //1//1表示开关设备,2表示情景
    ParaTimerTask[0].met[0].metid = 2;
}

#define TIMERTASK_MAGIC    0x9572Bcd5

/**
* @brief 从文件中载入支持数据参数
* @return 0成功, 否则失败
*/
int LoadParaTimerTask(void)
{
    LoadDefParaTimerTask();

    PrintLog(0, "  load param datause...(size=%d)", sizeof(ParaTimerTask)+12);

    if(ReadBinFile(PARAM_SAVE_PATH "/timertask.bin", SCENEUSE_MAGIC, (unsigned char *)ParaTimerTask, sizeof(ParaTimerTask)) < 0) {
        PrintLog(0, "timertask.bin no main, ");
        if(ReadBinFile(PARAM_BAK_PATH "/timertask.bin", SCENEUSE_MAGIC, (unsigned char *)ParaTimerTask, sizeof(ParaTimerTask)) < 0) {
            PrintLog(0, "timertask.bin no bak\n");
            SetSaveParamFlag(SAVFLAG_TIMERTASK);
            return 1;
    }
    }

    PrintLog(0, "timertask.bin ok\n");
    SetSaveParamFlag(SAVFLAG_SCENE);
    return 0;
}


/**
* @brief 保存支持数据参数
* @return 0成功, 否则失败
*/
int SaveParaTimerTask(void)
{
    SaveBinFile(PARAM_SAVE_PATH "/timertask.bin", SCENEUSE_MAGIC, (unsigned char *)ParaTimerTask, sizeof(ParaTimerTask));
    SaveBinFile(PARAM_BAK_PATH "/timertask.bin", SCENEUSE_MAGIC, (unsigned char *)ParaTimerTask, sizeof(ParaTimerTask));

    return 0;
}
