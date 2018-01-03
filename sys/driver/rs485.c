/**
* rs485.c -- RS485驱动接口
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-4-23
* 最后修改时间: 2009-5-9
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
//#include <sqlite3.h>
#include "include/debug.h"
#include "include/debug/statistic.h"
#include "include/debug/shellcmd.h"
#include "include/sys/schedule.h"
#include "include/param/commport.h"
#include "include/sys/uart.h"
#include "include/sys/rs485.h"
#include "include/sys/gpio.h"
#include "include/sys/mutex.h"

#include "include/environment.h"
/*by ydl modify 2011-05-05*/
static sys_mutex_t Rs485Mutex[RS485_PORTNUM];


DECLARE_STATISTIC(STATISGROUP_SYS, rs4850_sendbytes, 0);
DECLARE_STATISTIC(STATISGROUP_SYS, rs4850_recvbytes, 0);
DECLARE_STATISTIC(STATISGROUP_SYS, rs4851_sendbytes, 0);
DECLARE_STATISTIC(STATISGROUP_SYS, rs4851_recvbytes, 0);
DECLARE_STATISTIC(STATISGROUP_SYS, rs4852_sendbytes, 0);
DECLARE_STATISTIC(STATISGROUP_SYS, rs4852_recvbytes, 0);

/**
* @brief 初始化RS485端口
* @return 0成功, 否则失败
*/
DECLARE_INIT_FUNC(Rs485Init);
/*by ydl modify 2011-05-04*/

int Rs485Init(void)
{
    int i = 0;

    for(i=0; i<RS485_PORTNUM; i++) //设置485模式
    {
        if(UartOpen(i)) return 0;
        UartSet(i, 9600, 8, 1, 'N');
    }
    SET_INIT_FLAG(Rs485Init);

    return 0;
}

/**
* @brief 锁住Rs485端口
* @param port 端口号 0~2
*/

void Rs485Lock(unsigned int port)
{
    AssertLogReturnVoid(port>=RS485_PORTNUM, "invalid port(%d)\n", port);
    SysLockMutex(&Rs485Mutex[port]);
}



void Rs485Unlock(unsigned int port)
{
    AssertLogReturnVoid(port>=RS485_PORTNUM, "invalid port(%d)\n", port);
    SysUnlockMutex(&Rs485Mutex[port]);
}




void Rs485Set(unsigned int port, int baud, int databits, int stopbits, char parity)
{
    AssertLogReturnVoid(port>=RS485_PORTNUM, "invalid port(%d)\n", port);

    UartSet(port, baud, databits, stopbits, parity);
}


void rs485_setbaud(unsigned int port, unsigned int baud, unsigned char frame)
{
    int databits;
    int stopbits;
    char parity;
    databits=(frame & RS485_BITMASK)+5;
    switch(frame & RS485_STOPMASK)
    {
        case RS485_STOP1 :
            stopbits=1;
            break;
        case RS485_STOP2 :
            stopbits=2;
            break;
        default:stopbits=1;break;
    }
    switch(frame&RS485_PARITYMASK)
    {
        case RS485_NOPARITY:
            parity='N';
            break;
        case RS485_ODDPARITY:
            parity='O';
            break;
        case RS485_EVENPARITY:
            parity='E';
            break;
        default:parity='N';
    }

    Rs485Set(port, baud,databits,stopbits,parity);

}



/*by ydl modify 2011-05-05*/
int Rs485Send(unsigned int port, const unsigned char *buf, int len)
{
    AssertLogReturn(port>=RS485_PORTNUM, 1, "invalid port(%d)\n", port);
    AssertLog(len<=0, "invalid len(%d)\n", len);
    DebugPrint(0,"Send Data:");
    PrintHexLog(0, buf, len);
    UartSend(port, buf, len);

    if(0 == port) ADD_STATISTIC(rs4850_sendbytes, len);
    else if(1 == port) ADD_STATISTIC(rs4851_sendbytes, len);
    else if(2 == port) ADD_STATISTIC(rs4852_sendbytes, len);
    return 0;
}


int Rs485Recv(unsigned int port, unsigned char *buf, int len)
{
    int rtn;

    AssertLogReturn(port>=RS485_PORTNUM, -1, "invalid port(%d)\n", port);
    AssertLog(len<=0, "invalid len(%d)\n", len);

    rtn = UartRecv(port, buf, len);
    ///DebugPrint(0,"Rs485Recv Data:");
    //PrintHexLog(0, buf, len);
    if(0 == port) ADD_STATISTIC(rs4850_recvbytes, rtn);
    else if(1 == port) ADD_STATISTIC(rs4851_recvbytes, rtn);
    else if(2 == port) ADD_STATISTIC(rs4852_recvbytes, rtn);

    return rtn;
}

static int Send232Data(int argc, char *argv[])
{

    int port;
    //unsigned char  buf[10]={0x68,0x68,0x68};
    //unsigned char  buf[16]={0x68,0x82,0x00, 0x39,0x80,0xA5,0x00,0x68,0x04,0x04,0x20,0xC0,0x01,0x00,0x99,0x16};
    unsigned char  buf[14]={0xFE,0xFE,0xFE,0xFE,0x68,0xFF,0xFF,0x02,0x02,0x01,0x00,0x6B,0x16};
    unsigned char  recvbuf[100];
    port = atoi(argv[1]);
    Rs485Send(port,buf,13);
    DebugPrint(0,"发送串口测试数据Port = %d\n",port);
    PrintHexLog(0, buf, 16);
    int i=0;
    while(i<10)
    {
        int rtn = Rs485Recv(port, recvbuf, 30);
        if(rtn>0)
            PrintHexLog(0, recvbuf, rtn);
        i++;
        Sleep(10);

    }
    return -1;

}
DECLARE_SHELL_CMD("rs232", Send232Data, "发送串口测试数据");


