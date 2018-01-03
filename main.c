/**
* main.c -- 应用软件主程序入口
* 
* 作者: csg
* 创建时间: 2012-5-4
* 最后修改时间: 2012-5-4
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "include/basetype.h"
#include "include/debug.h"
#include "include/startarg.h"
#include "include/sys/init.h"
#include "include/version.h"
#include "include/sys/task.h"
#include "include/sys/schedule.h"
#include "include/sys/timeal.h"
#include "include/sys/timer.h"
#include "include/sys/gpio.h"
#include "include/param/term.h"
#ifdef OPEN_DEBUGPRINT
static const char VersionDebugInfo[] = "debug";
#else
#ifdef OPEN_ASSERTLOG
static const char VersionDebugInfo[] = "test";
#else
static const char VersionDebugInfo[] = "release";
#endif
#endif

int   exitflag = 0;
/**
* @brief 启动函数
*/
static void startup(void)
{

    if(SysInit())
    {
        return;
    }

    if(DebugInit())
    {
        return;
    }
    if(ParamInit())
    {
        return;
    }
    MonitorInit();


    SvrCommInit();
    CheckInitFlag();
    {
        sysclock_t clock;

        if(SysClockRead(&clock)) PrintLog(0,"read system clock failed.\n");
        else PrintLog(0,"current time = %s\n", SysClockFormat(&clock));
    }

    SysWaitTaskEnd();

}
/**
* 启动参数:
* '-b': 后台运行
* '-s': 启动命令行界面
* '-d': 不喂狗, 调试用
* '-rXXX': tty使用raw模式, XXX为串口波特率
* '-pXXX': tty使用raw模式,并且启动串口接收任务, XXX为串口波特率
*          -b, -s, -r , -p 不能同时有效
*/

static void ParseArgs(int argc, char *argv[])
{
    int i;

    for(i=1; i<argc; i++) {
        if('-' != argv[i][0] || 0 == argv[i][1]) continue;

        SetStartArg(argv[i]+1);
    }
}



/**
* @brief 主程序入口
* @param argc 参数数目
* @param argv 参数列表
* @return 0表示成功, 否则失败
*/
int main(int argc, char *argv[])
{
    printf("\n\n");
    printf("\033[1;32m" "深圳XX公司\n");
    printf("Relase Date: 20%02d-%d-%d %d:%d %s \033[0m\n\n",
        _mk_year, _mk_month, _mk_day, _mk_hour, _mk_minute, VersionDebugInfo);

    ParseArgs(argc, argv);
    if(MakeDeviceCode() !=0)//计算产生设备地址.
    {
        printf("应用程序非法,启动失败\n\n");
        return 0;
    }

    if(!GetStartArg('b', NULL, 0))//后台运行
    {
        if(fork() == 0)
        {
            startup();
        }
    }
    else //非后台运行
    {
        startup();
    }
    return 0;
}
