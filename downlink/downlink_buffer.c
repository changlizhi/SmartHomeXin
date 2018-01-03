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
#include "downlink_dl.h"

#if 0
static int empty_rawsend(const unsigned char *buf, int len)
{
    return 0;
}

static int empty_getchar(unsigned char *buf)
{
    return 0;
}
#endif
static int linestat_empty(void)
{
    return 1;
}

extern int SerialGetChar(unsigned char *buf);
extern int SerialRawSend(const unsigned char *buf, int len);

extern int imet645_rawsend(const unsigned char *buf, int len);
extern int imet645_getchar(unsigned char *buf);


#define MAXLEN_DOWNSERIALITF_BUF    1152


static unsigned int g_serialitf_rcvbuf[MAXLEN_DOWNSERIALITF_BUF/4];
static unsigned int g_serialitf_sndbuf[MAXLEN_DOWNSERIALITF_BUF/4];


const downlinkitf_t DownlinkInterface[DOWNLINKITF_NUM] = {

    {(unsigned char *)g_serialitf_rcvbuf, (unsigned char *)g_serialitf_sndbuf,
        imet645_rawsend, imet645_getchar, linestat_empty, 20,
        1024, 1024, 512, 0},


};

