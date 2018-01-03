/**
* syslock.c -- 系统复位函数
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-5
* 最后修改时间: 2009-5-15
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/reboot.h>
#include <sys/reboot.h>

#include "include/sys/reset.h"
#include "include/debug.h"
#include "include/sys/cycsave.h"
#include "include/sys/syslock.h"
#include "include/sys/gpio.h"
#include "include/sys/schedule.h"

extern void EnableFeedWatchdog(int flag);

void SysRestart(void)
{
    PrintLog(0,"sysrestart...\r\n");
    SysCycleSave(1);
    SysLockHalt();

    EnableFeedWatchdog(0);
    reboot(LINUX_REBOOT_CMD_RESTART);

    Sleep(500);
    exit(0);
}

void SysPowerDown(void)
{
    PrintLog(0,"sysPowerdown...\r\n");

    SysCycleSave(1);
    SysLockHalt();

    EnableFeedWatchdog(0);

    reboot(LINUX_REBOOT_CMD_RESTART);

    Sleep(500);
    exit(0);
}
