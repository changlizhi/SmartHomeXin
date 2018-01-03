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
#include "uplink_pkt.h"
#include "uplink_dl.h"
#include "include/lib/datachg.h"
#include "svrcomm/faal.h"
#include "svrcomm.h"


unsigned short gPktLen = 0;//完整帧长度 

static struct uplink_timer_t {
    int cnt;  //当前值
    int max;//超时间隔
    int flag; //生效标记
} UplinkTimer[UPLINKITF_NUM];

static struct uplink_fsm_t {
    unsigned char *pbuf;
    unsigned short cnt;
    unsigned short maxlen;
    unsigned char stat;
} UplinkFsm[UPLINKITF_NUM];


/**
* @brief 将数据包格式化为上行通信格式
* @param itf 接口编号
* @param pkt 数据包指针
* @return 成功返回数据包长度, 失败返回-1
*/
static int MakePacketG(unsigned char itf, uplink_pkt_tG *pkt)//只加校验和，别的不处理。
{
    unsigned char strDataLen[2] ;
    unsigned short crc = 0;
    strDataLen[0] = pkt->len[0];
    strDataLen[1] = pkt->len[1];

    //int dataLen = *(unsigned short*)strDataLen; //去data[]的实际占用长度
    unsigned short dataLen = makepkt_short(strDataLen);
    int nLen = 10 +dataLen+ 2 +1;//整帧长度= 帧头+数据体长度+CRC+结束符
    crc = GetCRC16(pkt,nLen - 3);

    pkt->data[dataLen] = crc>>8;
    pkt->data[dataLen+1] = crc;
    pkt->data[dataLen+2] = 0x16;
    PrintHexLog(0,&pkt->head,nLen);
    return (nLen);
}

/**
* @brief 设置接收定时器
* @param itf 接口编号
* @param max 超时时间(100ms)
*/
static void SetUpTimer(unsigned char itf, int max)
{
    UplinkTimer[itf].cnt = 0;
    UplinkTimer[itf].max = max;
    UplinkTimer[itf].flag = 1;
}

/**
* @brief 停止定时器
*/
static void StopUpTimer(unsigned char itf)
{
    UplinkTimer[itf].flag = 0;
}

/**
* @brief 清除接收状态
* @param itf 接口编号
*/
void UplinkClearState(unsigned char itf)
{
    UplinkFsm[itf].pbuf = UPLINK_RCVBUF(itf);
    UplinkFsm[itf].stat = 0;
    UplinkFsm[itf].cnt = 0;
    UplinkFsm[itf].maxlen = 0;

    StopUpTimer(itf);
}

/**
* @brief 接收定时器处理
*/
static void UpTimerProc(unsigned char itf)
{
    if(!UplinkTimer[itf].flag) {
        UplinkFsm[itf].pbuf = UPLINK_RCVBUF(itf);
        UplinkFsm[itf].stat = 0;
        UplinkFsm[itf].cnt = 0;
        UplinkFsm[itf].maxlen = 0;
        return;
    }

    UplinkTimer[itf].cnt++;
    if(UplinkTimer[itf].cnt >= UplinkTimer[itf].max) {
        UplinkClearState(itf);
    }

    return;
}


/*
*@brief 校验和检查
*@param P 需要校验的数据的首地址
*@param Len 需要校验的长度
*/
char  CalcCheckSumB(char* P, unsigned short Len)
{
    char ret=0;
    unsigned short nInx = 0;
    for ( nInx=0; nInx<Len; nInx++)
    {
        ret += P[nInx];
    }
    return ret;
}

/*
brief:检查报文中的终端地址是否是本机地址
para:pkt-报文指针
return:0-非本机地址1-本机地址
*/
static inline int IsHostAddr(uplink_pkt_tG *pkt)
{
    PrintLog(0, "本机地址:");

    return 1;
}

/*
*@brief 校验帧
*@param strStream 帧首地址
*@param Packlen  帧长度
*@返回1表示正确，返回0 表示错误。
*/
int CheckStreamOK(unsigned char* strStream, int PackLen)
{
    uplink_pkt_tG    *pkt = (uplink_pkt_tG    *)strStream;
//    unsigned char   check = 0;
//    unsigned char   check2 = 0;
    unsigned short crc;
    unsigned short crc2;
    if ( (strStream[0]) != UPLINK_HEAD ) return 0;
    if ( (strStream[6]) != UPLINK_HEAD ) return 0;
    if ( strStream[PackLen-1]  != UPLINK_TAIL ) return 0;

    crc = GetCRC16((unsigned char *)strStream,PackLen - 3);
    crc2    = strStream[PackLen-3]<<8;
    crc2    |= strStream[PackLen-2];
    if(crc != crc2) return 0;
    return 1;
#if 0
    char nCheckSum = CalcCheckSumB((char *)strStream,PackLen - 2);
    check2 = nCheckSum&0xff;
    check = strStream[PackLen-2];
    PrintLog(0, "%02x,%02x,%d,%02x,%02x\n",nCheckSum&0xff,strStream[PackLen-2],PackLen,check,check2);

    if (check2 != check) return 0;

    PrintLog(0, "rcv pkt!\n");
    /*by ydl modify 2010-08-24先检查报文是否发给本机*/
    if (IsHostAddr(pkt))
    {
        PrintLog(0, "报文是发给本机的!\n");
        return 1;
    }
    else
    {
        PrintLog(0, "报文不是发给本机的!\n");
        return 0;
    }
#endif

}
/**
* @brief 发送一个通信帧
* @param itf 接口编号
* @param pkt 通信帧指针
* @return 成功返回0, 否则失败
*/
int UplinkSendPktG(unsigned char itf, uplink_pkt_tG *pkt)
{
      /*此函数只在发送之前给集中器号赋值*/
    int len;
    int i=0;
    for(i=0;i<4;i++)
    {
        pkt->addr[i] = ParaTermG.deviceid[i*2]<<4 | ParaTermG.deviceid[i*2+1];
    }
    len = MakePacketG(itf, pkt);

    // if(UPLINK_NEEDPRINT(itf))
         faal_printpkt((faalpkt_t *)pkt, "SEND:");
     if((*UplinkInterface[itf].rawsend)((unsigned char *)pkt, len))
     {
        return 1;
    }
       return 0;
}

/**
* @brief 发送一个通信帧
* @param itf 接口编号
* @param pkt 通信帧指针
* @return 成功返回0, 否则失败
*/
int UplinkSendPktGAll(unsigned char itf, uplink_pkt_tG *pkt)
{
      /*此函数只在发送之前给集中器号赋值*/
    int len,nItf;
    int i=0;
    for(i=0;i<4;i++)
    {
        pkt->addr[i] = ParaTermG.deviceid[i*2]<<4 | ParaTermG.deviceid[i*2+1];
    }

       len = MakePacketG(itf, pkt);

    for(nItf = 0;nItf<UPLINKITF_NUM;nItf++)
    {
      if(UPLINK_NEEDPRINT(itf))faal_printpkt((faalpkt_t *)pkt, "SEND:");
       if((*UplinkInterface[nItf].rawsend)((unsigned char *)pkt, len))
       {
         continue;
        }
    }
       return 0;
}

/**
* @brief 接收数据帧(100ms运行一次)
* @param itf 接口编号
* @return 返回0  表示接收到一个正确的帧, 返回1表示没有
*/
int UplinkRecvPkt(unsigned char itf)
{
    struct uplink_fsm_t *fsm = &UplinkFsm[itf];
    const uplinkitf_t *pitf = &UplinkInterface[itf];

    unsigned char *pktG;
    unsigned char uc;
    UpTimerProc(itf);
    unsigned char strDataLen[2];
//    unsigned short dataLen;
    while( !(*pitf->getchar)(&uc) )
    {
        /*test:仅用于调试*/
        #if 0
        if (itf == UPLINKITF_SERIAL)
        {
            PrintLog(0, "ser:%02x\r\n", uc);
        }
        #endif
        PrintLog(0, "fsm->stat:%d,%x \n",fsm->stat, uc);
        //if(uc == 0x16) PrintLog(0, "\n");
        switch(fsm->stat) {
        case 0:
            if(UPLINK_HEAD == uc)
            {
                //找到帧头
                UplinkClearState(itf);
                *(fsm->pbuf)++ = uc;
                fsm->cnt++;
                fsm->stat = 1;
                fsm->maxlen = 6;//同一帧中两0x68之间相差6个字节
                SetUpTimer(itf, pitf->timeout);
            }
            break;

        case 1:
            *(fsm->pbuf)++ = uc;
            fsm->cnt++;
            if(fsm->cnt >= fsm->maxlen)
            {
                   fsm->stat = 2;//接到两个0x68
                   fsm->maxlen = 10;
            }
            break;
                    
        case 2://接收
            if(UPLINK_HEAD != uc)
            {
                UplinkClearState(itf);
                break;
            }
            *(fsm->pbuf)++ = uc;
                    fsm->cnt++;
            fsm->stat = 3;
            break;
        case 3:
            *(fsm->pbuf)++ = uc;
                     fsm->cnt++;

               if(fsm->cnt >= fsm->maxlen)
               {
                   fsm->stat = 4;//接到控制码和数据长度后，进入第三阶段分析

                  strDataLen[0] = *(fsm->pbuf-2);
                 strDataLen[1] = *(fsm->pbuf-1);
                //dataLen = *(unsigned short*)strDataLen;
                //DebugPrint(LOGTYPE_UPLINK, "fsm->stat: %d , strDataLen[0]:%0x, strDataLen[1]:%0x, dataLen=%d,make_short(strDataLen)=%d\n",fsm->stat,strDataLen[0],strDataLen[1],dataLen,make_short(strDataLen));
                  unsigned short dataLen = makepkt_short(strDataLen);
                            fsm->maxlen = dataLen+10+2;//下一环节需要获取的长度
            }
            break;

        case 4://接受DATA的第一字节
            *(fsm->pbuf)++ = uc;
            fsm->cnt++;
            //DebugPrint(LOGTYPE_UPLINK, "fsm->stat: %d , %0x, %d,%\n",fsm->stat,uc,fsm->maxlen);
            if(fsm->cnt >= fsm->maxlen)
                fsm->stat = 5;
            break;

        case 5:
            DebugPrint(LOGTYPE_UPLINK, "itf : %d,%x\n",itf,uc);
            if(UPLINK_TAIL != uc) {
                UplinkClearState(itf);
                break;
            }
            *(fsm->pbuf)++ = uc;
                    fsm->cnt++;
            StopUpTimer(itf);

               pktG = (unsigned char *)UPLINK_RCVBUF(itf);//转为帧

            faal_printpkt((faalpkt_t *)pktG, "RECV:");
               if(CheckStreamOK(( unsigned char*)pktG,(int)fsm->cnt))//正确
               {
                   //if(UPLINK_NEEDPRINT(itf))
                    faal_printpkt((faalpkt_t *)pktG, "RECV:");

                     UplinkClearState(itf);
                return 0;
               }
            else
                 {
                 DebugPrint(LOGTYPE_UPLINK, "CheckStreamOK error\n");
                  UplinkClearState(itf);

                  }
            break;

        default:
            UplinkClearState(itf);
            break;

    }
        }
       
    return 1;
}


/**
* @brief 获取超时时间和重发次数
* @param ptime 超时时间指针(100ms)
* @param retry 重发次数指针
*/
static void inline GetTimeout(int *ptime, int *retry)
{
    int i;
    i = (int)ParaUniG.snd_timeout;
    *ptime = i*10;
    *retry = (int)ParaUniG.snd_retry&0xff;
}

/**
* @brief 判断是否为不需要处理的报文，例如心跳、登录
* @pkt 通信帧指针
* @return 是回应报文返回1, 否则返回0
*/
 int IsEchoPkt(uplink_pkt_tG *pkt,unsigned char sndcmd)
{

    sndcmd &= FAALMASK_CMD;

    if(FAALCMD_ECHO_LOGON== sndcmd)
    {
        if(FAALCMD_ECHO_LOGON == pkt->cmd) return 1;
        else return 0;
    }
    else if(FAALCMD_ECHO_HBTEST == sndcmd)
    {
        if(FAALCMD_ECHO_HBTEST == pkt->cmd) return 1;
        else return 0;
    }
    else if(FAALCMD_LOGONOUT == sndcmd)
    {
        if(FAALCMD_ECHO_HBTEST == pkt->cmd) return 1;
        else return 0;
    }

    return 0;

}

/**
* @brief 发送报文并等待回应
* @param itf 接口编号
* @param pkt 通信帧指针
* @return 成功返回0,否则返回错误编码
*/
static int UplinkSendWait(unsigned char itf, uplink_pkt_tG *psnd)
{
    int i, j;
    int times, retry;

    uplinkitf_t *pitf = (uplinkitf_t*)&UplinkInterface[itf];
    uplink_pkt_tG  *prcv = (uplink_pkt_tG*)UPLINK_RCVBUF(itf);

    GetTimeout(&times, &retry);//只发一次,并且如果发送成功，15秒内主站不回确认，则判断为超时

    for(i=0; i<retry; i++)
    {
        if(!(*pitf->linestat)()) return UPRTN_FAIL;

        if(UplinkSendPktG(itf, psnd))
        {
               DebugPrint(0, "UplinkSendPktG error\n");
               return UPRTN_FAIL;
        }

        for(j=0; j<times; j++)
        {
            if(!UplinkRecvPkt(itf))
            {
                DebugPrint(LOGTYPE_UPLINK, "recvpak\n");
                if(!IsEchoPkt(prcv,psnd->cmd))
                {
                    return UPRTN_OKRCV;
                }
                else
                {
                    return UPRTN_OK;
                }
            }

            Sleep(10);
        }
    }

    DebugPrint(LOGTYPE_UPLINK, "recvpak timeout!\n");
    return UPRTN_TIMEOUT;
}

/**
* @brief 主动发送数据
* @param itf 接口编号
* @param flag 发送标志, 0-不等待响应, 1-等待响应
* @return 成功返回0,否则返回错误编码
*/
int UplinkActiveSend(unsigned char itf, unsigned char flag, uplink_pkt_tG *psnd)
{     
    static unsigned char fseq = 1;
    if(!flag)
         return UplinkSendPktG(itf, psnd);
     else
         return UplinkSendWait(itf, psnd);
}

/**
* @brief 登陆到服务器
* @param itf 接口编号
* @return 成功返回0,否则失败
*/
int UplinkLogon(unsigned char itf)
{
      int rtn,i;
      unsigned char *puc;
      uplink_pkt_tG* pkt = (uplink_pkt_tG *)UPLINK_SNDBUF(itf);
      pkt->ver = 0x01;
      pkt->cmd = FAALCMD_LOGON;
      FAAL_SETLEN(pkt, 0x4);
      puc = pkt->data;
      for(i=0; i<4; i++) puc[i] = ParaTermG.com_pwd[i];

      pkt->head = 0x68;
      pkt->dep = 0x68;
      pkt->cmd = 0xa1;

      FAAL_SETLEN(pkt,4);
      rtn = UplinkActiveSend( itf,1, pkt);
      if(rtn){
          PrintLog(0,"logon fail!\r\n");
        return 1;
      }
      else{
              PrintLog(0,"logon ok!\r\n");
            return 0;
      }
}

/**
* @brief 登陆退出服务器
* @param itf 接口编号
* @return 成功返回0,否则失败
*/
int UplinkLogonOut(unsigned char itf)
{
      int rtn,i;
      unsigned char *puc;
      uplink_pkt_tG* pkt = (uplink_pkt_tG *)UPLINK_SNDBUF(itf);
      pkt->ver = 0x01;
      pkt->head = 0x68;
      pkt->dep = 0x68;
      pkt->cmd = 0xa2;

      FAAL_SETLEN(pkt,0);
      rtn = UplinkActiveSend( itf,1, pkt);
      if(rtn){
          PrintLog(0,"logon out fail!\r\n");
        return 1;
      }
      else{
              PrintLog(0,"logon out ok!\r\n");
            return 0;
      }
}

/**
* @brief 链路检测,发送心跳测试
* @param itf 接口编号
* @return 成功返回0,否则返回错误编码
*/
int UplinkLinkTest(unsigned char itf)
{

    uplink_pkt_tG *pkt = (uplink_pkt_tG *)UPLINK_SNDBUF(itf);
    pkt->head = 0x68;
    pkt->ver = 0x01;
    pkt->dep= 0x68;
    pkt->cmd = 0xA4;//控制码
    FAAL_SETLEN(pkt,0);
    int rtn = UplinkActiveSend(itf, 1, pkt);//心跳检测需要主站回应答帧
    if(UPRTN_FAIL == rtn || UPRTN_TIMEOUT== rtn) {
        PrintLog(0, "link test fail\r\n");
    }
    else if(UPRTN_OK == rtn) {
        PrintLog(0, "link test ok.\r\n");
    }

    return(rtn);
}
/**
* @brief 设备检查软件版本号
* @param itf 接口编号
* @return 成功返回0,否则返回错误编码
*/

int UplinkCheckVer(unsigned char itf)
{

    uplink_pkt_tG *pkt = (uplink_pkt_tG *)UPLINK_SNDBUF(itf);
    pkt->head = 0x68;
    pkt->ver = 0x01;
    pkt->dep= 0x68;
    pkt->cmd = 0x84;//控制码
    FAAL_SETLEN(pkt,0);
    int rtn = UplinkActiveSend(itf, 0, pkt);
    if(UPRTN_FAIL == rtn || UPRTN_TIMEOUT== rtn) {
            PrintLog(0, "link test fail\r\n");
    }
    else if(UPRTN_OK == rtn) {
            PrintLog(0, "link test ok.\r\n");
    }

    return(rtn);
}

/**
* @brief 发送设备校验报文
* @param itf 接口编号
* @return 成功返回0,否则返回错误编码
*/

int UplinkDeviceCheck(unsigned char itf)
{
    unsigned char *puc;

    int i;
    int filestat = 0;
    uplink_pkt_tG *pkt = (uplink_pkt_tG *)UPLINK_SNDBUF(itf);
    pkt->head = 0x68;
    pkt->ver = 0x01;
    pkt->dep= 0x68;
    pkt->cmd = 0x81;//控制码
    FAAL_SETLEN(pkt, 0x1);
    puc = pkt->data;
    filestat = getFileDays();    //判断音频文化状态
    // 0X08代表音频正在治疗中，0X0F为音频时效已到，治疗结束。
    if(filestat == 0)    //音频文件未过期
        puc[0] = 0x08;
    else //音频文件过期
    {
        puc[0] = 0x0F;
    }

    int rtn = UplinkActiveSend(itf, 0, pkt);
    if(UPRTN_FAIL == rtn || UPRTN_TIMEOUT== rtn) {
            PrintLog(0, "link test fail\r\n");
    }
    else if(UPRTN_OK == rtn) {
            PrintLog(0, "link test ok.\r\n");
    }

    return(rtn);
}



