/**
* uplink_dl.c -- 上行通信数据链路层
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-18
* 最后修改时间: 2009-5-18
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/basetype.h"
#include "include/debug.h"
#include "include/lib/datachg.h"
#include "include/sys/schedule.h"
#include "include/lib/align.h"
#include "include/param/term.h"
#include "include/param/unique.h"
#include "downlink_dl.h"
#include "downlink645.h"
#include "include/lib/datachg.h"


//unsigned short gPktLen = 0;//完整帧长度 

static struct downlink_timer_t {
    int cnt;  //当前值
    int max;//超时间隔
    int flag; //生效标记
} DownlinkTimer[DOWNLINKITF_NUM];

static struct downlink_fsm_t {
    unsigned char *pbuf;
    unsigned short cnt;
    unsigned short maxlen;
    unsigned char stat;
} DownlinkFsm[DOWNLINKITF_NUM];


/**
* @brief 设置接收定时器
* @param itf 接口编号
* @param max 超时时间(100ms)
*/
static void SetDownTimer(unsigned char itf, int max)
{
    DownlinkTimer[itf].cnt = 0;
    DownlinkTimer[itf].max = max;
    DownlinkTimer[itf].flag = 1;
}

/**
* @brief 停止定时器
*/
static void StopDownTimer(unsigned char itf)
{
    DownlinkTimer[itf].flag = 0;
}

/**
* @brief 清除接收状态
* @param itf 接口编号
*/
void DownlinkClearState(unsigned char itf)
{
    DownlinkFsm[itf].pbuf = DOWNLINK_RCVBUF(itf);
    DownlinkFsm[itf].stat = 0;
    DownlinkFsm[itf].cnt = 0;
    DownlinkFsm[itf].maxlen = 0;

    SetDownTimer(itf,5);
}

/**
* @brief 接收定时器处理
*/
static void DownTimerProc(unsigned char itf)
{
    if(!DownlinkTimer[itf].flag) {
        DownlinkFsm[itf].pbuf = DOWNLINK_RCVBUF(itf);
        DownlinkFsm[itf].stat = 0;
        DownlinkFsm[itf].cnt = 0;
        DownlinkFsm[itf].maxlen = 0;
        return;
    }

    DownlinkTimer[itf].cnt++;
    if(DownlinkTimer[itf].cnt >= DownlinkTimer[itf].max) {
        DownlinkClearState(itf);
    }

    return;
}

/*
* @brief 判断645报文合法性
* @return 0合法1 非法报文
*/
int faal645_checkpkt(unsigned char itf,downlink_pktmodbus_t *pkt)
{
    unsigned short i, len;
    unsigned char *puc;
    unsigned char chk;
    unsigned short crc;
    unsigned short crc2;
    crc     = GetCRC16((unsigned char *)pkt,6);

    if(pkt->data[0] == 0xF0 || pkt->data[0] == 0xF1)
    {
         crc2     = pkt->data[4]<<8;
         crc2   |= pkt->data[5];

    }
    else
    {
           crc2     = pkt->data[5]<<8;
        crc2 |= pkt->data[4];
    }


    if(crc != crc2) return 0;

    return 0;
}

int DownlinkRecvPkt645(unsigned char itf1)
{
    int itf = itf1;
    struct downlink_fsm_t *fsm = &DownlinkFsm[itf];
    const downlinkitf_t *pitf = &DownlinkInterface[itf];
    downlink_pktmodbus_t *pkt;
    unsigned char uc;

    DownTimerProc(itf);


    while(!(*pitf->getchar)(&uc)) {
        PrintLog(0, "[stat%d]uc: %02x \n", fsm->stat, uc);
        switch(fsm->stat) {
        case 0:
            DownlinkClearState(itf);
            *(fsm->pbuf)++ = uc;
            fsm->stat = 1;
            fsm->maxlen = 7;
            fsm->cnt = 0;
            SetDownTimer(itf, pitf->timeout);
            break;
        case 1:
            *(fsm->pbuf)++ = uc;
            fsm->cnt++;
            if(fsm->cnt >= fsm->maxlen)
            {
                StopDownTimer(itf);
                DownlinkClearState(itf);
                pkt = (downlink_pktmodbus_t*)DOWNLINK_RCVBUF(itf);
            /*此处需要添加校验的函数*/
            int rtn = faal645_checkpkt(itf,pkt);
            if(!rtn)
            {
                return 0;
            }
            PrintLog(0, "recv frame checkpkt:%d \n",rtn);
            }
            break;
        default:
            DownlinkClearState(itf);
            break;
        }
    }

    return 1;
}

