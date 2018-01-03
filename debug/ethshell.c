/**
* ethshell.c -- 以太网命令行调试
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-26
* 最后修改时间: 2009-5-26
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/errno.h>

#include "include/basetype.h"
#include "include/debug.h"
#include "include/sys/schedule.h"
#include "include/sys/timeal.h"
#include "include/sys/timer.h"
#include "include/sys/task.h"
#include "shell.h"

static int SockEthShell = -1;
static struct sockaddr AddrEthShell;
static int LogonOk = 0;
static char LogonString[] = "logon 7865ae31";

/**
* @brief 以太网命令行网络初始化
* @return 成功0, 否则失败
*/
static int EthShellNetInit(void)
{
    struct sockaddr_in addr;

    SockEthShell = socket(AF_INET, SOCK_DGRAM, 0);
    if(SockEthShell < 0) {
        PrintLog(0,"create socket error.\r\n");
        return 1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5461);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //addr.sin_addr.s_addr = 0;

    if(bind(SockEthShell, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        PrintLog(0,"bind error(%d).\r\n", errno);
        close(SockEthShell);
        SockEthShell = -1;
        return 1;
    }
    PrintLog(0,"EthShellNetInit init OK\n");
    return 0;
}

static int TimerIdEthShell = -1;
static int CTimerEthShell(unsigned long arg)
{
    TimerIdEthShell = -1;

    SetLogType(0);
    SetLogInterface(0);

    LogonOk = 0;
    return 1;
}


static void ReadLine(char *buf, int maxlen)
{
    unsigned int addrlen = sizeof(AddrEthShell);
    int len;

    while(1) {
        len = recvfrom(SockEthShell, buf, maxlen-1, 0, &AddrEthShell, &addrlen);
        if(len > 0 && len < maxlen) {
            DebugPrint(0, "recv %d bytes\n", len);
            buf[len] = 0;
            return;
        }

        Sleep(10);
    }
}

#define SHELLARG_NUM    12
static char CmdLineArgBuf[SHELLARG_NUM][128];
static char *CmdLineArgV[SHELLARG_NUM];

/**
* @brief 以太网命令行任务
*/
static void *EthShellTask(void *arg)
{
    static char command[256];

    shell_func pfunc;
    int argc;

    while(1)
    {
        ReadLine(command, 256);

        DebugPrint(0, "read: %s\n", command);

        if(!LogonOk)
        {
            if(0 == strcmp(command, LogonString))
            {
                LogonOk = 1;
                SetLogInterface(1);

                /*20100817 by ydl:为调试方便将登录超时时间置为1小时*/
                if(TimerIdEthShell < 0) TimerIdEthShell = SysAddCTimer(60*30, CTimerEthShell, 0);
                else SysClearCTimer(TimerIdEthShell);
                //Sleep(5);
                PrintLog(0, "logon OK\n");
            }
        }
        else {
            argc = ShellParseArg(command, CmdLineArgV, SHELLARG_NUM);
            if(argc > 0)
            {
                pfunc = FindShellFunc(CmdLineArgV[0]);
                if(NULL != pfunc)
                {
                    SetLogInterface(1);

                    if(TimerIdEthShell < 0) TimerIdEthShell = SysAddCTimer(60, CTimerEthShell, 0);
                    else SysClearCTimer(TimerIdEthShell);

                    //Sleep(5);
                    (*pfunc)(argc, CmdLineArgV);
                    PrintLog(0, "\n");
                }
            }
        }
        if(exitflag)
            break;
    }

    return 0;
}

DECLARE_INIT_FUNC(EthShellInit);
int EthShellInit(void)
{
    int argc;
    PrintLog(0,"EthShellInit init...\n");
    for(argc=0; argc<SHELLARG_NUM; argc++) {
        CmdLineArgV[argc] = CmdLineArgBuf[argc];
    }

    if(EthShellNetInit()) return 1;
    SysCreateTask(EthShellTask, NULL);
    PrintLog(0,"EthShellInit init OK\n");
    SET_INIT_FLAG(EthShellInit);

    return 0;
}

static char EthShellPrintBuf[1536];
char *GetEthShellBuffer(void)
{
    return EthShellPrintBuf;
}

void EthShellPrint(const char *str)
{
    if(SockEthShell < 0) return;

    sendto(SockEthShell, str, strlen(str)+1, 0, &AddrEthShell, sizeof(AddrEthShell));
}

void EthShellQuit(void)
{
    if(TimerIdEthShell >= 0) {
        SysStopCTimer(TimerIdEthShell);
        TimerIdEthShell = -1;
    }

    LogonOk = 0;

    return;
}


