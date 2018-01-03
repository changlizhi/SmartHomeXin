/**
* uplink_buffer.c -- 上行通信缓存区定义
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-18
* 最后修改时间: 2009-5-18
*/

#include <stdio.h>

#include "include/basetype.h"
#include "include/debug.h"
#include "uplink_pkt.h"
#include "uplink_dl.h"

static int empty_rawsend(const unsigned char *buf, int len)
{
    return 0;
}

static int empty_getchar(unsigned char *buf)
{
    return 0;
}

static int linestat_empty(void)
{
    return 1;
}

extern int SerialGetChar(unsigned char *buf);
extern int SerialRawSend(const unsigned char *buf, int len);

extern int EthSvrGetChar(unsigned char *buf);
extern int EthSvrRawSend(const unsigned char *buf, int len);

extern int EtherGetChar(unsigned char *buf);
extern int EtherRawSend(const unsigned char *buf, int len);



extern int TcpSvrGetChar(unsigned char *buf);
extern int TcpSvrRawSend(const unsigned char *buf, int len);

extern int LocalSvrGetChar(unsigned char *buf);
extern int LocalSvrRawSend(const unsigned char *buf, int len);


#define MAXLEN_SERIALITF_BUF    1152
#define MAXLEN_ETHSVRITF_BUF    1152
#define MAXLEN_GPRSITF_BUF        1152
#define MAXLEN_TCPSVRITF_BUF    1152

static unsigned int g_gprsitf_rcvbuf[MAXLEN_GPRSITF_BUF/4];
static unsigned int g_gprsitf_sndbuf[MAXLEN_GPRSITF_BUF/4];
static unsigned int g_serialitf_rcvbuf[MAXLEN_SERIALITF_BUF/4];
static unsigned int g_serialitf_sndbuf[MAXLEN_SERIALITF_BUF/4];
static unsigned int g_ethsvritf_rcvbuf[MAXLEN_ETHSVRITF_BUF/4];
static unsigned int g_ethsvritf_sndbuf[MAXLEN_ETHSVRITF_BUF/4];
static unsigned int g_tcpsvritf_rcvbuf[MAXLEN_TCPSVRITF_BUF/4];
static unsigned int g_tcpsvritf_sndbuf[MAXLEN_TCPSVRITF_BUF/4];
static unsigned int g_localsvritf_rcvbuf[MAXLEN_TCPSVRITF_BUF/4];
static unsigned int g_localsvritf_sndbuf[MAXLEN_TCPSVRITF_BUF/4];
const uplinkitf_t UplinkInterface[UPLINKITF_NUM] = {

    {(unsigned char *)g_serialitf_rcvbuf, (unsigned char *)g_serialitf_sndbuf,
        empty_rawsend, empty_getchar, linestat_empty, 5,
        1024, 1024, 512, 0},

    {(unsigned char *)g_ethsvritf_rcvbuf, (unsigned char *)g_ethsvritf_sndbuf,
        EthSvrRawSend, EthSvrGetChar, linestat_empty, 5,
        1024, 1024, 1024, 0},

    {(unsigned char *)g_gprsitf_rcvbuf, (unsigned char *)g_gprsitf_sndbuf,
        EtherRawSend, EtherGetChar, linestat_empty, 6,
        1024, 1024, 1024, 0},
    {(unsigned char *)g_tcpsvritf_rcvbuf, (unsigned char *)g_tcpsvritf_sndbuf,
        TcpSvrRawSend, TcpSvrGetChar, linestat_empty, 6,
        1024, 1024, 1024, 0},
    {(unsigned char *)g_localsvritf_rcvbuf, (unsigned char *)g_localsvritf_sndbuf,
        LocalSvrRawSend, LocalSvrGetChar, linestat_empty, 6,
        1024, 1024, 1024, 0},

};

