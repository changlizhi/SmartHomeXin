/**
* uplink_dl.h -- 上行通信数据链路层
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-18
* 最后修改时间: 2009-5-18
*/

#ifndef _DOWNLINK_DL_H
#define _DOWNLINK_DL_H


//上行通信信道编号

#define DOWNLINKITF_IMET645            0   //串口

#define DOWNLINKITF_NUM            1

#define DOWNLINKATTR_NOECHO    0x01    //主动发送不需响应(如短信信道)


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
} downlinkitf_t;

extern const downlinkitf_t DownlinkInterface[DOWNLINKITF_NUM];
#define DOWNLINK_RCVBUF(itf)        (DownlinkInterface[itf].rcvbuf)
#define DOWLINK_SNDBUF(itf)            (DownlinkInterface[itf].sndbuf)
#define DOWLINK_RCVMAX(itf)            (DownlinkInterface[itf].rcvmax)
#define DOWLINK_TIMEOUT(itf)        (DownlinkInterface[itf].timeout)
#define DOWLINK_SNDMAX(itf)            (DownlinkInterface[itf].sndmax)
#define DOWLINK_SNDNOR(itf)            (DownlinkInterface[itf].sndnor)
#define DOWLINK_ATTR(itf)            (DownlinkInterface[itf].attr)

#define DWON_FAAL_RCVBUF(itf)           (DownlinkInterface[itf].rcvbuf)
#define DWON_FAAL_SNDBUF(itf)        (DownlinkInterface[itf].sndbuf)
#define DWON_FAAL_RCVMAX(itf)           (DownlinkInterface[itf].rcvmax)
#define DWON_FAAL_TIMEOUT(itf)        (DownlinkInterface[itf].timeout)
#define DWON_FAAL_SNDMAX(itf)        (DownlinkInterface[itf].sndmax)
#define DWON_FAAL_SNDNOR(itf)        (DownlinkInterface[itf].sndnor)
#define DWON_FAAL_ATTR(itf)            (DownlinkInterface[itf].attr)

void DownlinkClearState(unsigned char itf);
int DownlinkRecvPkt(unsigned char itf);


//char  CalcCheckSumB(char* P, unsigned short Len);
#endif /*_UPLINK_DL_H*/

