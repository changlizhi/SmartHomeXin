/**
* alarm.c -- 事件告警接口
* 
* 作者: yangzhilong
* 创建时间: 2009-10-28
* 最后修改时间: 2009-10-28
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/basetype.h"
#include "include/environment.h"
#include "include/debug.h"
#include "include/sys/syslock.h"
#include "include/sys/timeal.h"
#include "include/sys/bin.h"
#include "include/lib/bcd.h"
#include "include/monitor/runstate.h"
#include "include/monitor/alarm.h"
#include "include/param/term.h"
#include "include/uplink/svrnote.h"
#include "include/debug/shellcmd.h"


#define ALARM_SAVEPATH        DATA_PATH
#define ALARM_MAGIC        0x9631f810

#define ALMNUM_PERFILE        256
#define ALARM_BASEMASK        ((unsigned char)~(ALMNUM_PERFILE-1))

static int LockIdAlarm = -1;
alarm_buf_t AlarmCache[MAX_ALARM];  //告警缓存区



/**
* @brief 获得事件储存文件名
* @param offset 偏移值
* @filename 返回文件名
*/
void AlarmFileName(unsigned short offset, char *filename)
{
    sprintf(filename, ALARM_SAVEPATH "i%d.alm", offset/ALMNUM_PERFILE);
    DebugPrint(0, "alam offset is : %d\n", offset);
    DebugPrint(0, "alam filename is: %s\n", filename);
}
alarm_buf_t * GetCurrentAlarm()
{
    alarm_buf_t *pbase = AlarmCache;
    int              idxbase = 0;
    idxbase = RunStateG.alarm.cur & ALARM_BASEMASK;
    return &pbase[idxbase];

}
/**
* @brief 保存事件
* @param pbuf 储存事件缓存区指针
*/
void UpdateAlarm(alarm_buf_t *pbuf)
{
    char            filename[200] = {0};
    alarm_buf_t     *pbase = AlarmCache;
    int              idxbase = 0;

    LockSysLock(LockIdAlarm);
    sysclock_t pclock;
    SysClockReadCurrent(&pclock);
    if(pclock.year !=17)
        return;
    AlarmFileName(RunStateG.alarm.cur, filename);
    memcpy( pbase+RunStateG.alarm.cur, pbuf, sizeof(alarm_buf_t) );
    pbuf->endtime = UTimeReadCurrent();
    idxbase = RunStateG.alarm.cur & ALARM_BASEMASK;
    if(SaveBinFile(filename, ALARM_MAGIC, (unsigned char *)&pbase[idxbase], sizeof(alarm_buf_t)*ALMNUM_PERFILE)==0)
    {
        DebugPrint(0 ,"idxbase is: %d start time is: %s current time is %s  save successed\n",idxbase,UTimeFormat(pbuf->starttime),UTimeFormat(pbuf->endtime));
    }
    else
    {
        DebugPrint(0 ,"save failed\n");
    }

    UnlockSysLock(LockIdAlarm);
}
/**
* @brief 保存事件
* @param pbuf 储存事件缓存区指针
*/
void SaveAlarm(alarm_buf_t *pbuf)
{
    char            filename[200] = {0};
    alarm_buf_t *pbase = AlarmCache;
    int              idxbase = 0;

    LockSysLock(LockIdAlarm);
    sysclock_t pclock;
    SysClockReadCurrent(&pclock);
    if(pclock.year !=17)
        return;
    AlarmFileName(RunStateG.alarm.cur, filename);
    pbuf->endtime = UTimeReadCurrent();
    memcpy( pbase+RunStateG.alarm.cur, pbuf, sizeof(alarm_buf_t) );

    idxbase = RunStateG.alarm.cur & ALARM_BASEMASK;
    if(SaveBinFile(filename, ALARM_MAGIC, (unsigned char *)&pbase[idxbase], sizeof(alarm_buf_t)*ALMNUM_PERFILE)==0)
    {
        DebugPrint(0 ,"idxbase is: %d start time is: %s current time is %s  save successed\n",idxbase,UTimeFormat(pbuf->starttime),UTimeFormat(pbuf->endtime));
    }
    else
    {
        DebugPrint(0 ,"save failed\n");
    }

    RunStateG.alarm.cur++;
    if(RunStateG.alarm.cur>=MAX_ALARM)
    {
        RunStateG.alarm.cur = 0;
    }
    if(RunStateG.alarm.cur == RunStateG.alarm.head)
    {
        RunStateG.alarm.head++;
        if( RunStateG.alarm.head >= MAX_ALARM )
        {
            RunStateG.alarm.head = 0;
        }
    }
    RunStateG.alarm.playstate = 0;
    SaveRunState();
    UnlockSysLock(LockIdAlarm);
}


/**
* @brief 清除事件计数
* @param idx 事件分类索引
*/
void ClearAlarm()
{
    runstate_tG *pstat = RunStateModifyG();

    pstat->alarm.head = 0;
    pstat->alarm.cur = 0;
    pstat->alarm.snd = 0;
}
/**
* @查询当前告警事件
*/
void QueAlarm()
{
    runstate_tG *pstat = RunStateModifyG();
    PrintLog(0,"当前告警head=%d",pstat->alarm.head );
    PrintLog(0," cur=%d",pstat->alarm.cur);
    PrintLog(0," snd=%d\r\n",pstat->alarm.snd );


}

/**
* @brief 生成事件告警
* @param pbuf 储存事件缓存区指针
*/
void MakeAlarmG( alarm_buf_t *pbuf)
{
    DebugPrint(0,"MakeAlarmG: \n");
    //PrintHexLog(0, (unsigned char *)pbuf, int len)
    if(NULL == pbuf)
    {
        ErrorLog("Invalid pointer!\n");
        return ;
    }
    sysclock_t pclock;
    SysClockReadCurrent(&pclock);
    if(pclock.year !=17)
        return;
    DebugPrint(0, "cur(%d) == head(%d)playstate(%d)\n",RunStateG.alarm.cur,RunStateG.alarm.head,RunStateG.alarm.playstate);
    if(RunStateG.alarm.playstate == 1) //上一次断电后没按停止键，
    {
        RunStateG.alarm.cur++;
        if(RunStateG.alarm.cur>=MAX_ALARM)
        {
            RunStateG.alarm.cur = 0;
        }
        if(RunStateG.alarm.cur == RunStateG.alarm.head)
        {
            RunStateG.alarm.head++;
            if( RunStateG.alarm.head >= MAX_ALARM )
            {
                RunStateG.alarm.head = 0;
            }
        }
    }
    if( RunStateG.alarm.head >= MAX_ALARM ||RunStateG.alarm.cur >= MAX_ALARM )
    {
        RunStateG.alarm.head = RunStateG.alarm.cur = 0;
    }
    RunStateG.alarm.playstate = 1;
    pbuf->starttime = UTimeReadCurrent();
    pbuf->endtime = UTimeReadCurrent();
    DebugPrint(0, "cur(%d) == head(%d)\n",RunStateG.alarm.cur,RunStateG.alarm.head);
    SaveRunState();

}



/**
* @brief 事件接口初始化
* @return 成功0, 否则失败
*/
DECLARE_INIT_FUNC(AlarmInit);
int AlarmInit(void)
{
    int offset = 0;
    char filename[200] = {0};
    alarm_buf_t *pbuf;

    memset(AlarmCache, 0, sizeof(AlarmCache));

    pbuf = AlarmCache;
    DebugPrint(0, "  load\n");
    for(offset=0; offset<MAX_ALARM; offset+=ALMNUM_PERFILE,pbuf+=ALMNUM_PERFILE)
    {
        AlarmFileName(offset, filename);

        DebugPrint(0, "i%d", offset/ALMNUM_PERFILE);
        if(ReadBinFile(filename, ALARM_MAGIC, (unsigned char *)pbuf, sizeof(alarm_buf_t)*ALMNUM_PERFILE) > 0)
        {
            DebugPrint(0, "AlarmInit ok, ");
        }
        else
        {
            DebugPrint(0, "AlarmInit fail, ");
        }
    }
    DebugPrint(0, "    load all\n");

    LockIdAlarm = RegisterSysLock();

    SET_INIT_FLAG(AlarmInit);

    return 0;
}

/**
* @brief 将事件变量转换成上行通信格式
* @param palm 事件变量指针
* @param pbuf 输出缓存区指针
* @return 存入的字节数
*/
static int FormatAlarm(const alarm_buf_t *palm, char *pdst)
{

    smallcpy(pdst, &palm->starttime, sizeof(palm->starttime));   //播放起始时间
    DebugPrint(0, "palm->starttime(%d) == palm->endtime(%d)\n",palm->starttime,palm->endtime);
    DebugPrint(0, "FormatAlarm sizeof(palm->endtime)(%d)\n",sizeof(palm->endtime));
    smallcpy(&pdst[4], &palm->endtime, sizeof(palm->endtime));   //播放起始时间
   
    return 10;
}


/**
* @brief 主动发送告警
* @param sendbuffer 缓存区链表指针
* @return 0-成功, 1-失败
*/
int ActiveSendAlarm(unsigned char *sendbuffer)
{
    int sendlen = 0, retlen = 0;
    char* p = NULL;

    //非法条件判断
    if(NULL == sendbuffer)
    {
        ErrorLog("Invalid pointer!\n");
        return -1;
    }

    if(RunStateG.alarm.cur == RunStateG.alarm.head)
    {
        DebugPrint(0, "cur(%d) == head(%d)\n",RunStateG.alarm.cur,RunStateG.alarm.head);
        return -1;
    }
    else if(RunStateG.alarm.cur > RunStateG.alarm.head)
    {
        if(RunStateG.alarm.snd > RunStateG.alarm.cur || RunStateG.alarm.snd < RunStateG.alarm.head)
        {
            RunStateG.alarm.snd = RunStateG.alarm.head;
        }
    }
    else
    {
        if(RunStateG.alarm.snd > RunStateG.alarm.cur && RunStateG.alarm.snd < RunStateG.alarm.head)
        {
            RunStateG.alarm.snd = RunStateG.alarm.head;
        }
    }


    if( RunStateG.alarm.snd>=MAX_ALARM)
    {
        RunStateG.alarm.snd = 0;
    }
    if( RunStateG.alarm.snd == RunStateG.alarm.cur )
    {
        DebugPrint(0, "snd == cur\n");
        return -1;
    }

    retlen = FormatAlarm(&AlarmCache[RunStateG.alarm.snd++], sendbuffer);
    SaveRunState();
    DebugPrint(0, "FormatAlarm retlen(%d)\n",retlen);
    return 0;
}




int shell_alarmtest(int argc, char *argv[])
{
    int i, num;
    utime_t utime;
    alarm_buf_t alarmbuf;

    if(2 != argc) {
        PrintLog(0, "usage: almtest num\n");
        return 1;
    }

    num = atoi(argv[1]);
    if(num <= 0 || num > 1000) {
        PrintLog(0, "invalid len\n");
        return 1;
    }

    utime=UTimeReadCurrent();
    memset(&alarmbuf, 0, sizeof(alarmbuf));



    PrintLog(0, "make %d alarm ok\n", num);

    return 0;
}
DECLARE_SHELL_CMD("almtest", shell_alarmtest, "告警测试");