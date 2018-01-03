/**
* uplink_dl.h -- 上行通信数据链路层
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-18
* 最后修改时间: 2009-5-18
*/

#ifndef _UPLINK_DL_H
#define _UPLINK_DL_H

#ifndef _FAAL_H
#include "svrcomm/faal.h"
#endif
#ifndef _FAAL_DL_H
#include "svrcomm/faal_dl.h"
#endif


//上行通信信道编号

#define UPLINKITF_SERIAL        0   //串口
#define UPLINKITF_ETHMTN        1   //以太网服务器
#define UPLINKITF_ETHER            2  //以太网
#define UPLINKITF_TCPSVR              3
#define UPLINKITF_LOCALSVR         4

#define UPLINKITF_NUM            5

#define UPLINKATTR_NOECHO    0x01    //主动发送不需响应(如短信信道)

#define UPLINK_NEEDPRINT(itf)   (((itf) >=UPLINKITF_ETHMTN) && (LOGITF_SVRCOMM != GetLogInterface()))

typedef struct {
    unsigned char *rcvbuf;  //接收缓存
    unsigned char *sndbuf;    //发送缓存
    int (*rawsend)(const unsigned char *buf, int len);    //发送函数
    int (*getchar)(unsigned char *buf);    //接收函数
    int (*linestat)(void);
    int timeout;   //接收超时

    int rcvmax;    //接收数据区最大长度
    int sndmax;    //发送数据区最大长度
    int sndnor;     //发送数据一般长度上限

    unsigned int attr;   //通道属性
} uplinkitf_t;

extern const uplinkitf_t UplinkInterface[UPLINKITF_NUM];
#define UPLINK_RCVBUF(itf)        (UplinkInterface[itf].rcvbuf)
#define UPLINK_SNDBUF(itf)        (UplinkInterface[itf].sndbuf)
#define UPLINK_RCVMAX(itf)        (UplinkInterface[itf].rcvmax)
#define UPLINK_TIMEOUT(itf)        (UplinkInterface[itf].timeout)
#define UPLINK_SNDMAX(itf)        (UplinkInterface[itf].sndmax)
#define UPLINK_SNDNOR(itf)        (UplinkInterface[itf].sndnor)
#define UPLINK_ATTR(itf)            (UplinkInterface[itf].attr)

#define FAAL_RCVBUF(itf)   (UplinkInterface[itf].rcvbuf)
#define FAAL_SNDBUF(itf)    (UplinkInterface[itf].sndbuf)
#define FAAL_RCVMAX(itf)   (UplinkInterface[itf].rcvmax)
#define FAAL_TIMEOUT(itf)    (UplinkInterface[itf].timeout)
#define FAAL_SNDMAX(itf)    (UplinkInterface[itf].sndmax)
#define FAAL_SNDNOR(itf)    (UplinkInterface[itf].sndnor)
#define FAAL_ATTR(itf)    (UplinkInterface[itf].attr)
//#define FAAL_TID(itf)    (g_faalitf[itf].tid)

void UplinkClearState(unsigned char itf);
int UplinkRecvPkt(unsigned char itf);
int UplinkSendPktG(unsigned char itf, uplink_pkt_tG *pkt); //广电
int UplinkSendPktGAll(unsigned char itf, uplink_pkt_tG *pkt); //广播


#define UPRTN_OK            0          //发送成功
#define UPRTN_FAIL        1            //发送失败
#define UPRTN_OKRCV        2            //发送成功, 收到命令包
#define UPRTN_FAILRCV      3            //发送之前收到命令包
#define UPRTN_TIMEOUT     4           //发送超时

//flag = 0, not wait echo, 1-wait echo
int UplinkActiveSend(unsigned char itf, unsigned char flag, uplink_pkt_tG *psnd);

int UplinkLogon(unsigned char itf);
int UplinkLinkTest(unsigned char itf);
int UplinkCheckVer(unsigned char itf);
int UplinkDeviceCheck(unsigned char itf);
int UplinkLogonOut(unsigned char itf);

int IsEchoPkt(uplink_pkt_tG *pkt,unsigned char sndcmd);


char  CalcCheckSumB(char* P, unsigned short Len);
#endif /*_UPLINK_DL_H*/

