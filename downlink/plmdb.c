/**
* plmdb.c -- 载波表数据
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-22
* 最后修改时间: 2009-5-22
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEFINE_PLMDB

#include "include/basetype.h"
#include "include/environment.h"
#include "include/debug.h"
#include "include/sys/timeal.h"
#include "include/sys/timer.h"

#include "include/sys/bin.h"
#include "include/sys/cycsave.h"
#include "include/sys/mutex.h"
#include "include/sys/syslock.h"
#include "include/lib/dbtime.h"
#include "include/lib/bcd.h"
#include "plmdb.h"
#include "plcomm.h"
#include "include/debug/shellcmd.h"

#define PLMDB_MAGIC        0xfedc9708
#define PLMDB_SAVEPATH        DATA_PATH

extern char HextoInt(char Hex_in);
static int PlMdbLockId = -1;
extern unsigned char datachange[4][MAX_METER];//抄读数据转换如果抄9090抄不别来,就抄9010,抄完二次后失败,转换
mdbcur_t  MdbCurData[MAX_METER+1];

void LockPlMdb(void)
{
    LockSysLock(PlMdbLockId);
}
void UnlockPlMdb(void)
{
    UnlockSysLock(PlMdbLockId);
}
#define LOCK_PLMDB        LockSysLock(PlMdbLockId)
#define UNLOCK_PLMDB    UnlockSysLock(PlMdbLockId)


static const char *PlFileName[] = {
    "daypl", "monpl", "dayimp","Lday","Lmon","Limp",
};

ReadMetErr_Alarm ReadMetErrAlarm[MAX_METER];


/**
* @brief 抄表失败告警
* @return 成功0, 否则失败
*/
DECLARE_INIT_FUNC(ReadMetErrAlarmInit);
int ReadMetErrAlarmInit(void)
{
    if(ReadBinFile(READMETERR_SAVE_PATH "rdmeter.db", READMETERR_MAGIC, (unsigned char *)ReadMetErrAlarm, sizeof(ReadMetErrAlarm)) > 0)
    {
        DebugPrint(0, "ReadMetErrAlarmInit init ok\n");
    }
    else
    {
        DebugPrint(0, "ReadMetErrAlarmInit init fail\n");
        memset(&ReadMetErrAlarm, 0, sizeof(ReadMetErrAlarm));
    }

    SET_INIT_FLAG(ReadMetErrAlarmInit);

    return 0;
}


/**
* @brief 获取数据库的储存文件名
* @param dbid 数据库ID
* @param clock 储存数据库文件时的时钟
* @param filename 返回的文件名
* @return 成功返回0, 否则失败
*/
static int PlMdbFileName(char *filename, unsigned char dbid, dbtime_t dbtime)
{
    AssertLogReturn(dbid > PLMDB_LIMP, 1, "invalid dbid(%d)\n", dbid);

    if('m' == PlFileName[dbid][0]) {
        sprintf(filename, PLMDB_SAVEPATH "%s@%02d%02d.db",
                        PlFileName[dbid], dbtime.s.year, dbtime.s.month);
    }
    else if('L' == PlFileName[dbid][0]){
        sprintf(filename, PLMDB_SAVEPATH "%s",PlFileName[dbid]);

    }
    else {
        sprintf(filename, PLMDB_SAVEPATH "%s@%02d%02d%02d.db",
                        PlFileName[dbid], dbtime.s.year,dbtime.s.month, dbtime.s.day);
    }

    return 0;
}



//DECLARE_CYCLE_SAVE(SavePlMdb, 0);

/**
* @brief 载波表数据初始化
*/
DECLARE_INIT_FUNC(PlMdbInit);
int PlMdbInit(void)
{
    sysclock_t clock;
    dbtime_t dbtime;
    int rtn;
    int i;
    memset( MdbCurData, PLDATA_EMPTY, sizeof(MdbCurData) );
    for(i =1;i<255;i++)
    {
        MdbCurData[i].addr = i/4+1 ;
        if(i<240)
        {
            int index = i%4;
            index = (index-1)*2;
            MdbCurData[i].runstate[index] = i;
        }
    }
    PlMdbLockId = RegisterSysLock();

    SET_INIT_FLAG(PlMdbInit);
    return 0;
}

/**
* @brief 时钟转换为读取时间格式
* @param clock 时钟
* @param flag 0-转换为日冻结时间, 1-转换为月冻结时间
* @return 读取时间
*/
static inline unsigned short ClockToReadTime(const sysclock_t *clock, int flag)
{
    unsigned short us;

    us = (unsigned short)clock->hour * 60 + (unsigned short)clock->minute;
    if(flag) {
        unsigned short us2 = clock->day;

        if(us2 == 0 || us2 > 31) us2 = 1;
        us += (us2-1)*1440;
    }

    return us;
}

/**
* @brief 时钟转换为读取时间格式
* @param fileclock 文件时间
* @param readtime 读取时间
* @param flag 0-转换为日冻结时间, 1-转换为月冻结时间
* @param clock 返回时钟
*/
static inline void ReadTimeToClock(const sysclock_t *fileclock, unsigned short readtime, int flag, sysclock_t *clock)
{
    clock->second = 0;
    clock->minute = readtime%60;
    readtime /= 60;
    clock->month = fileclock->month;
    clock->year = fileclock->year;

    if(flag) {
        clock->hour = readtime%24;
        clock->day = readtime/24 + 1;
    }
    else {
        clock->hour = readtime;
        clock->day = fileclock->day;
    }
}





/*多功能表告警数据文件*/
/*
 * 返回0成功
*/

#define DATA_PATH        "/data/"
#define MDB_MAGIC        0xfedc9708


