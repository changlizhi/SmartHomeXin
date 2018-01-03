/**
* ethsvr.c -- 以太网通信(服务器模式)
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-19
* 最后修改时间: 2009-5-19
*/

#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/errno.h>

#include "include/basetype.h"
#include "include/debug.h"
#include "include/sys/schedule.h"
#include "include/sys/task.h"
#include "include/param/term.h"
#include "include/param/unique.h"
#include "uplink_pkt.h"
#include "uplink_dl.h"
#include "svrcomm.h"
#include "include/uplink/svrnote.h"
static int SockEthSvr = -1;
static struct sockaddr AddrEthSvr;
#define CLOSE_SOCKET(sock)   { \
    if((sock) >= 0) { \
        close(sock); \
        sock = -1; \
    }}
/**
* @brief 以太网通信网络初始化
* @return 成功0, 否则失败
*/
static int EthSvrNetInit(void)//crt开启UDP服务 
{
    struct sockaddr_in addr;
    int ctlflag;

    SockEthSvr = socket(AF_INET, SOCK_DGRAM, 0);
    //SOCK_DGRAM
   //    SockEthSvr = socket(AF_INET, SOCK_STREAM, 0);
    if(SockEthSvr < 0) {
        PrintLog(0,"create socket error.\r\n");
        return 1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(ParaUniG.termip.portlisten);
    //addr.sin_port = htons(8888);
    //addr.sin_port = htons(9999);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(SockEthSvr, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(SockEthSvr);
        SockEthSvr = -1;
        return 1;
    }


    ctlflag = fcntl(SockEthSvr, F_GETFL);
    ctlflag |= O_NONBLOCK;
    fcntl(SockEthSvr, F_SETFL, ctlflag);

    return 0;
}

static int EtherDisconnect(void)
{
    CLOSE_SOCKET(SockEthSvr);
    return 0;
}
/**
* @brief 以太网通信任务
*/
static void *EthSvrTask(void *arg)
{
    unsigned long ev;
    UplinkClearState(UPLINKITF_ETHMTN);
    EthSvrNetInit();//开启UDP服务
    while(1) {
        /*if(ParaTermG.reg.autoflag == 0) //网关未注册
        {
            Sleep(100);
            continue;
        }*/
        while(!UplinkRecvPkt(UPLINKITF_ETHMTN)) {

            SvrMessageProc(UPLINKITF_ETHMTN);
        }

        Sleep(10);
        if(exitflag)
        {
            EtherDisconnect();
            break;
        }
    }
}

/**
* @brief 以太网通信初始化
* @return 成功0, 否则失败
*/
DECLARE_INIT_FUNC(EthSvrInit);
int EthSvrInit(void)
{
    SysCreateTask(EthSvrTask, NULL);

    SET_INIT_FLAG(EthSvrInit);
    return 0;
}

static unsigned char EthSvrRcvBuffer[2048];
static int EthSvrRcvLen = 0;
static int EthSvrRcvHead = 0;

/**
* @brief 从以太网通信接口读取一个字节
* @param buf 返回字符指针
* @return 成功0, 否则失败
*/
int EthSvrGetChar(unsigned char *buf)
{
    unsigned int addrlen;

    if(SockEthSvr < 0) return 1;

    if(EthSvrRcvLen <= 0) {
        addrlen = sizeof(AddrEthSvr);
        EthSvrRcvLen = recvfrom(SockEthSvr, EthSvrRcvBuffer, 2048, 0, &AddrEthSvr, &addrlen);
        if(EthSvrRcvLen <= 0) return 1;
        else EthSvrRcvHead = 0;
    }

    *buf = EthSvrRcvBuffer[EthSvrRcvHead++];
    EthSvrRcvLen--;
    return 0;
}

/**
* @brief 向以太网通信接口发送数据
* @param buf 发送缓存区指针
* @param len 缓存区长度
* @return 成功0, 否则失败
*/
int EthSvrRawSend(const unsigned char *buf, int len)
{
    if(SockEthSvr < 0) return 1;
    struct sockaddr_in sin;
    memcpy(&sin,&AddrEthSvr, sizeof(sin));
    char IP[32] = {0};
    sprintf(IP, inet_ntoa(sin.sin_addr));
    DebugPrint(0,"IP:%s,Port:%d  \n",IP,sin.sin_port);
    sin.sin_port = 9999;
    //sendto(SockEthSvr, buf, len, 0, &AddrEthSvr, sizeof(AddrEthSvr));
    sin.sin_port = 9999;
    sendto(SockEthSvr, buf, len, 0, &AddrEthSvr, sizeof(AddrEthSvr));
    return 0;
}

