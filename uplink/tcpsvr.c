/**
* ethsvr.c -- 以太网通信(服务器模式)
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-19
* 最后修改时间: 2009-5-19
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <sys/ioctl.h>


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
#define CLOSE_SOCKET(sock)   { \
    if((sock) >= 0) { \
        close(sock); \
        sock = -1; \
    }}
static int SockTcpSvr = -1;
static int cSock         = -1; //客户端socket
static int cSockConnectNum = 0;
//static struct sockaddr AddrTcpSvr;

static int EtherDisconnect(void)
{
    CLOSE_SOCKET(SockTcpSvr);
    return 0;
}
/**
* @brief 以太网通信网络初始化
* @return 成功0, 否则失败
*/
static int TcpSvrNetInit(void)
{
      int nResult=0;
    struct sockaddr_in addr;
//    int ctlflag;

    SockTcpSvr = socket(AF_INET, SOCK_STREAM, 0);

    if(SockTcpSvr < 0) {
        PrintLog(0,"create socket error.\r\n");
        return 1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9997);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(SockTcpSvr, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        PrintLog(0,"bind error(%d).\r\n", errno);
        close(SockTcpSvr);
        SockTcpSvr = -1;
        return 1;
    }

    PrintLog(0,"ether server listen at port %d...\n",9997);

    nResult = listen(SockTcpSvr, 5);//SOMAXCONN);
        if ( -1 == nResult)
        {
            close(SockTcpSvr);
        }

    return 0;
}

static int WaitClientConnect()
{
  struct sockaddr_in rec_addr;
  socklen_t rec_len = sizeof(rec_addr);
  if(cSock>=0)
      return cSock;
  cSock = accept(SockTcpSvr,  (struct sockaddr *)&rec_addr,&rec_len);
  if(cSockConnectNum)
  {
      close(cSock);
    cSockConnectNum = 0;
  }
  else 
  {
      cSockConnectNum = 1;
  }
  return cSock;
}
/**
* @brief 以太网通信任务
*/
static void *TcpSvrTask(void *arg)
{
    unsigned long ev;
    UplinkClearState(UPLINKITF_TCPSVR);
    TcpSvrNetInit();//开启Tcp 服务

    while(1) {

        /*if(ParaTermG.reg.autoflag == 0) //网关未注册
        {
            Sleep(100);
            continue;
        }*/
        if(WaitClientConnect() >= 0)
         {
                SvrCommPeekEvent(SVREV_NOTE, &ev);

            if(ev&SVREV_NOTE) {
                SvrNoteProc(UPLINKITF_TCPSVR);
            }

            while(!UplinkRecvPkt(UPLINKITF_TCPSVR)) {

                SvrMessageProc(UPLINKITF_TCPSVR);
            }
        }
        if(exitflag)
        {
            EtherDisconnect();
            break;
        }
        Sleep(10);
    }
}

/**
* @brief 以太网通信初始化
* @return 成功0, 否则失败
*/
DECLARE_INIT_FUNC(TcpSvrInit);
int TcpSvrInit(void)
{
    SysCreateTask(TcpSvrTask, NULL);

    SET_INIT_FLAG(TcpSvrInit);
    return 0;
}

static unsigned char TcpSvrRcvBuffer[2048];
static int TcpSvrRcvLen = 0;
static int TcpSvrRcvHead = 0;

/**
* @brief 从以太网通信接口读取一个字节
* @param buf 返回字符指针
* @return 成功0, 否则失败
*/
int TcpSvrGetChar(unsigned char *buf)
{
    if(SockTcpSvr < 0 ||cSock < 0 ) return 1;

    if(TcpSvrRcvLen <= 0)
    {
        //ether_recvlen = recv(sock_ether, ether_recvbuf, 2048, 0);
        TcpSvrRcvLen = recv(cSock, TcpSvrRcvBuffer, 2048, MSG_DONTWAIT);
        if(((TcpSvrRcvLen < 0) && (errno != EWOULDBLOCK)) ||
                (TcpSvrRcvLen == 0)) {
            PrintLog(LOGTYPE_SHORT, "connection corrupted(%d,%d).\n", TcpSvrRcvLen, errno);
            CLOSE_SOCKET(cSock);
            cSockConnectNum = 0;
            return 1;
        }
        else if(TcpSvrRcvLen < 0) {
            return 1;
        }
        else {
            TcpSvrRcvHead = 0;
        }
    }

    *buf = TcpSvrRcvBuffer[TcpSvrRcvHead++];
    TcpSvrRcvLen--;
    return 0;
}

/**
* @brief 向以太网通信接口发送数据
* @param buf 发送缓存区指针
* @param len 缓存区长度
* @return 成功0, 否则失败
*/
int TcpSvrRawSend(const unsigned char *buf, int len)
{
    if(SockTcpSvr < 0 ||cSock < 0 ) return 1;

    if(send(cSock, buf, len, MSG_NOSIGNAL) < 0) {
        DebugPrint(1, "send to client fail\r\n");
        goto mark_failend;
    }
    PrintHexLog(0, buf, len);
    return 0;

#if 0
    //wait until send buffer empty
    for(i=0; i<100; i++) {
        osh_sleep(10);

        if(ioctl(sock_ether, SIOCOUTQ, &buflen)) goto mark_failend;

        //debug_print(1, "buflen = %d\r\n", buflen);
        if(0 == buflen) return 0;
    }
#endif

mark_failend:
    PrintLog(LOGTYPE_SHORT, "connection corrupted.\r\n");
    PrintLog(0,"\n connect courrupted. \n");
    CLOSE_SOCKET(cSock);
    return 1;
}

