/**
* serial.c -- 串口上行通信
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-6-8
* 最后修改时间: 2009-6-8
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/basetype.h"
#include "include/debug.h"
#include "include/sys/task.h"
#include "include/sys/schedule.h"
#include "include/sys/uart.h"
#include "include/uplink/svrnote.h"
#include "uplink_pkt.h"
#include "uplink_dl.h"
#include "svrcomm.h"
#include "include/param/unique.h"
#define  QRCODE_UART     UART_QRCODE //    4

static int SerialStarted = 0;

/**
* @brief 串口通信任务(主动发送)
*/
void SerialActiveTask(void)
{
    unsigned long ev;

    if(!SerialStarted) {
        ErrorLog("Serial not start, return\n");
        return;
    }

    UplinkClearState(UPLINKITF_SERIAL);

    while(1) {
        SvrCommPeekEvent(SVREV_NOTE, &ev);

        if(ev&SVREV_NOTE) {
            SvrNoteProc(UPLINKITF_SERIAL);
        }

        if(!UplinkRecvPkt(UPLINKITF_SERIAL)) {
            SvrMessageProc(UPLINKITF_SERIAL);
        }

        Sleep(10);
    }

    return;
}

/**
* @brief 串口通信任务(不主动发送)
*/
static void *SerialPassiveTask(void *arg)
{
    char qrcodebuf[512]={0};
    int  recvlen = 0;
    memset(qrcodebuf, 0, 512);
    if(!SerialStarted) {
        ErrorLog("Serial not start, return\n");
        return 0;
    }
    UplinkClearState(UPLINKITF_SERIAL);

    Sleep(100);

    while(1) {

        recvlen = UartRecv(QRCODE_UART, qrcodebuf, 512);
        if(recvlen > 50)  //收到二维码命令
        {

            if( !strncmp(qrcodebuf, "GM", 2) ) //柜门
            {
                qrcodeValidity(qrcodebuf);
            }
        }
        memset(qrcodebuf, 0, 512);

        Sleep(10);
    }
    return 0;
}

/**
* @brief 启动串口通信任务
* @param mode 0不启动任务, 1启动任务
* @param baud 串口波特率
* @return 成功返回0, 失败返回1
*/
DECLARE_INIT_FUNC(UplinkSerialStart);
int UplinkSerialStart(void)
{
    if(UartOpen(QRCODE_UART)) {
        PrintLog(0,"can not open uart %d\n", QRCODE_UART);
        return 1;
    }
    UartSet(QRCODE_UART, 115200, 8, 1, 'n');
    SerialStarted = 1;
    if(1) {
        SysCreateTask(SerialPassiveTask, NULL);
    }

    SET_INIT_FLAG(UplinkSerialStart);
    return 0;
}

/**
* @brief 从串口通信接口读取一个字节
* @param buf 返回字符指针
* @return 成功0, 否则失败
*/
int SerialGetChar(unsigned char *buf)
{
    if(UartRecv(QRCODE_UART, buf, 1) > 0) return 0;
    else return 1;
}

/**
* @brief 向串口通信接口发送数据
* @param buf 发送缓存区指针
* @param len 缓存区长度
* @return 成功0, 否则失败
*/
int SerialRawSend(const unsigned char *buf, int len)
{
    return(UartSend(QRCODE_UART, buf, len));
}


