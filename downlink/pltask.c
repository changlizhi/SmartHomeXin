/**
* pltask.c -- 载波表任务
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-29
* 最后修改时间: 2009-5-29
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/basetype.h"
#include "include/environment.h"
#include "include/debug.h"
#include "include/sys/timeal.h"
#include "include/sys/timer.h"
#include "include/sys/event.h"
#include "include/sys/task.h"
#include "include/sys/syslock.h"
#include "include/sys/schedule.h"
#include "include/sys/diskspace.h"
#include "include/sys/gpio.h"
#include "include/sys/uart.h"
#include "include/lib/dbtime.h"
#include "include/lib/bcd.h"
#include "include/param/term.h"
#include "include/param/commport.h"
#include "include/param/operation.h"
#include "include/param/unique.h"
#include "include/debug/shellcmd.h"
#include "plmdb.h"
#include "plcomm.h"
#include "include/debug/shellcmd.h"
#include "include/uplink/svrnote.h"
#include "save/param.h"
/*by ydl add 2011-04-08*/
#include "include/sys/rs485.h"
#include "include/param/sceneuse.h"
#include "include/monitor/alarm.h"
#include "downlink/qrcode.h"
typedef struct{
    unsigned char proto;//设备协议
    unsigned short cmd_on;//开启 设备命令标识
    unsigned short cmd_off;//关闭设备命令标识
}met_onoff_t;

const static met_onoff_t met_onoff[] = 
{
    {1, 0, 0},//智能开关
    {2, 0x001B, 0x001A},//无线表
    {0, 0 , 0},
};

#define SCENE_STATE_LEN 2

int PlCheckTimebyHost_Day = 0;       // 标志主站校时后自动校时标志。
//static unsigned short PlCurMetid = 0; //当前表号


int plcreading = 0;
unsigned short TempHumsensor1 = 0;
unsigned short TempHumsensor2 = 0;

int TempHumsensorTimerId1 = -1;
int TempHumsensorTimerId2 = -1;



int ctrlnum = 0;
ctrlcabinet_t  ctrlstate[4];




extern int PlcModuleNo;
extern int FlagImpSaved;
extern unsigned char SceneOnId;//开启当前情景
extern unsigned char SceneOffId;//关闭当前情景
extern int MdbSaveMetAlarm(void);//需要加锁操作


/*************日冻结数据项处理*******************************/
#define ERR_COM_TIME    2
//#define ITEMDNUM   2
unsigned char datachange[4][MAX_METER];//抄读数据转换如果抄9090抄不别来,就抄9010,抄完二次后失败,转换
unsigned short readitemid[ITEMDNUM][3]={{0,0x9010,4},{0,0xc020,1}};//{是否抄离(1抄读),数据项,长度}
/********************************************************************/
unsigned short mreaditemid[ITEMDNUM][3]={{0,0x9040,4},{0,0xc020,1}};//{是否抄离(1抄读),数据项,长度}

/*************当前表数据及状态项处理********************/
unsigned short socketreaditemid[ITEMDNUM][3]={{0,0x9040,14},{0,0x8ea0,2}};//{是否抄离(1抄读),数据项,长度}
/********************************************************************/
unsigned char MusicPause  = 0;//音乐暂停标志
unsigned char MusicPauseTmeout = 0;//音乐暂停标志

//amixer cget    controls

static int musicstart = 0;
char  musicname[100] ={0};


//开启
//id :情景编号0为默认设置
static void SceneOnTask(unsigned char sceneid)
{
    unsigned short metid;
    unsigned int i;
    unsigned char id,proto;
    static int musicid = 0;
    plc_dest_t dest;
    id = sceneid;
    if(id != 0)
        {
        //背景音乐
        system("killall -9 madplay");
        char   music[100] = {0};
        musicid++;
        if(musicid>3)
          musicid = 1;
        sprintf(musicname,"madplay /work/back%d.mp3 &",musicid);
        musicstart = 1;
    }
    return;
}
static void *SceneTask(void *arg)
{
    Sleep(50);
    PrintLog(0, "In SceneTask....\n");
    while(1)
    {
        if(SceneOnId){//有情景任务开启
            PrintLog(0, "开启情景任务!\n");
            SceneOnTask(SceneOnId);
            SceneOnId = 0;

        }
        if(SceneOffId){//情景任务关闭(恢复到默认设置)
            SceneOnTask(0);
            SceneOffId = 0;
        }
        Sleep(50);
        if(exitflag)
            break;
    }
    return;
}
static void *MusicTask(void *arg)
{

    while(1)
    {
        if(musicstart)
        {
            musicstart = 0;
            system(musicname);
        }
        if(MusicPause && MusicPauseTmeout>0)
        {
            MusicPauseTmeout--;
            if(MusicPauseTmeout == 0)
            {
                setvolume(ParaTermG.music_volume);
                MusicPause = 0;
            }
        }
        Sleep(50);
        if(exitflag)
            break;
    }
    return;
}

static void RTimeUpdateTemperature1(unsigned long arg, utime_t utime)
{
    PrintLog(0, "start task RTimeUpdateDev  ...\n");
    TempHumsensor1 = 0xF0;
    return;
}

static void RTimeUpdateTemperature2(unsigned long arg, utime_t utime)
{
    PrintLog(0, "start task RTimeUpdateDev  ...\n");
    TempHumsensor2 = 0xF1;
    return;
}

static void *PlcTask(void *arg)
{
    rtimer_conf_t conf1;
    rtimer_conf_t conf2;
    unsigned short i;
    unsigned char rbuf[8];
    memset((unsigned char*)&conf1,0,sizeof(rtimer_conf_t));
    memset((unsigned char*)&conf2,0,sizeof(rtimer_conf_t));
    conf1.curtime = UTimeReadCurrent();
    conf1.bonce = 0;
    conf1.tdev = 5;
    conf1.tmod = UTIMEDEV_MINUTE;//每过两分钟更新一次数据


    conf2.curtime = UTimeReadCurrent();
    conf2.bonce = 0;
    conf2.tdev = 6;
    conf2.tmod = UTIMEDEV_MINUTE;//每过两分钟更新一次数据
    if(TempHumsensorTimerId1 >= 0) SysStopRTimer(TempHumsensorTimerId1);
    TempHumsensorTimerId1 = SysAddRTimer(&conf1, RTimeUpdateTemperature1, 0);

    if(TempHumsensorTimerId2 >= 0) SysStopRTimer(TempHumsensorTimerId2);
    TempHumsensorTimerId2 = SysAddRTimer(&conf2, RTimeUpdateTemperature2, 0);
    while(1){
        if(plcreading)
        {
            Sleep(10);
            continue;
        }

        if(TempHumsensor1 == 0xF0)
        {
            plc_dest_t dest;
            MakePlcDest(TempHumsensor1, &dest);
            if(PlcRead(&dest, 0x0003, rbuf, sizeof(rbuf)) >= 0) //数据标识待定.....
                TempHumsensor1 = 0;
        }

        if(TempHumsensor2 == 0xF1)
        {
            plc_dest_t dest;
            MakePlcDest(TempHumsensor2, &dest);
            if(PlcRead(&dest, 0x0003, rbuf, sizeof(rbuf)) >= 0)//数据标识待定.....
               TempHumsensor2 = 0;
        }

        if(ctrlnum>0)
        {
            for(i=0;i<ctrlnum;i++)
            {
                plc_dest_t dest;
                Sleep(50);
                MakePlcDest(ctrlstate[i].addr, &dest);
                if(PlcCtrlMet(&dest, 0x0005, ctrlstate[i].runstate, 4) >= 0)//数据标识待定.....
                   Sleep(10);
            }
            ctrlnum = 0;
        }
        Sleep(10);
        if(exitflag)
            break;
    }
    return 0;
}

extern int PlcCommInit(void);
extern int PlMdbInit(void);
extern void PIActiveUpdateInit(void);
extern int IMet645();
extern int QrCode();

/**
* @brief 载波模块初始化函数
*/
DECLARE_INIT_FUNC(PlcInit);
int PlcInit(void)
{
    PrintLog(0,"plc init...\n");
    PlcCommInit();
    PlMdbInit();
    IMet645();
    QrCode();

    SysCreateTask(PlcTask, NULL);
    SysCreateTask(SceneTask, NULL);
    SysCreateTask(MusicTask, NULL);

    SET_INIT_FLAG(PlcInit);
//    PrintLog(0,"plc init end\n");
    return 0;
}

/**
* @brief 载波较时定时器
* @param arg 定时器参数
* @param utime 当前时间
*/
static void RTimerTimerTask(unsigned long arg, utime_t utime)
{
    SceneOnId = arg;
}
static int TimerIdTask= -1;
//定时任务测试
int shell_TimerTask(int argc, char *argv[])
{

    int sceneid = 0;
    int hour = 0;
    int min = 0;
    int sec = 0;

    if(4 != argc && 5 !=argc) {
        PrintLog(0, "usage: timertask num\n");
        return 1;
    }

    sceneid = atoi(argv[1]);
    hour         = atoi(argv[2]);
    min         = atoi(argv[3]);
    if(5== argc)
        sec         = atoi(argv[4]);

    rtimer_conf_t conf;


    if(TimerIdTask >= 0) SysStopRTimer(TimerIdTask);

    conf.curtime = UTimeReadCurrent();
    conf.bonce = 0;
    conf.tdev = 1;
    conf.tmod = UTIMEDEV_DAY;  // 1 day

    SysClockReadCurrent(&conf.basetime);
    conf.basetime.hour = hour;
    conf.basetime.minute = min;
    conf.basetime.second = sec;

    TimerIdTask = SysAddRTimer(&conf, RTimerTimerTask, sceneid);

    return 0;
}
DECLARE_SHELL_CMD("timertask", shell_TimerTask, "定时任务测试");
