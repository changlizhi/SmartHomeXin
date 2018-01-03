
/**
* datause.h -- 支持数据配置参数头文件
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-8
* 最后修改时间: 2009-5-8
*/

#ifndef _PARAM_SCENEUSE_H
#define _PARAM_SCENEUSE_H

//情景配置设置 (在终端支持的数据配置内)
#define MAX_SCENECLASS    16      //最大情景数
#define MAX_METNUM      32      //一个情景最大支持的表数

typedef struct __attribute__((packed)){
//typedef struct {
    unsigned char  attr;              //对象属性
    unsigned short metid;            //对象ID
    unsigned char  state[8];          //对象状态
} cfg_datafns_t;

typedef struct __attribute__((packed)){
//typedef struct {
    unsigned char id;                //情景编号(可设定编号等于所存入的数据下标号+1)
    unsigned char num;                //控制的设备个数
    char              scene_name[16];        //情景名称
    cfg_datafns_t met[MAX_METNUM];
} cfg_scenecls_data_t;

typedef struct __attribute__((packed)){
//typedef struct {
    unsigned short  flag;    //情景数掩码0xFFFF
    cfg_scenecls_data_t scenecls[MAX_SCENECLASS];  //情景
} para_sceneuse_t;

#ifndef DEFINE_PARASCENEUSE
extern  para_sceneuse_t ParaSceneUse;
#define ParaSceneUseCls(sceneclass)    (ParaSceneUse.scenecls[sceneclass])
#endif

extern const cfg_scenecls_data_t  AllCloseCls;    //全关
extern const cfg_scenecls_data_t  AllOpenCls; //全开

int SaveParaSceneUse(void);
int SaveParaTimerTask(void);
#define MAX_TIMERTASK    10      //最大定时任务数
#define MAX_ACTION      32      //单个任务最大动作组数

typedef struct {
    unsigned char     id;                        //任务编号(可设定编号等于所存入的数据下标号+1)
    unsigned char     num;                    //控制的动作组数
    char                  task_name[16];            //任务名称
    char                hour;                    //小时
    char                min;                    //分钟
    char                weekflag;                //星期标志
    char                bStart;                    //是否启用
    cfg_datafns_t         met[MAX_ACTION];
} cfg_timertaskcls_data_t;

extern  cfg_timertaskcls_data_t ParaTimerTask[MAX_TIMERTASK];

#endif /*_PARAM_SCENEUSE_H*/

