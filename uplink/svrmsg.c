/**
* msgproc.c -- 上行通信命令处理
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-18
* 最后修改时间: 2009-5-18
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/basetype.h"
#include "include/version.h"
#include "include/debug.h"
#include "include/sys/schedule.h"
#include "include/sys/reset.h"
#include "include/sys/mutex.h"
#include "include/sys/timeal.h"
#include "include/sys/timer.h"
#include "include/sys/bin.h"
#include "include/sys/syslock.h"
#include "include/sys/gpio.h"
#include "include/lib/bcd.h"
#include "include/lib/dbtime.h"
#include "include/lib/align.h"
#include "include/lib/datachg.h"
#include "save/param.h"
#include "include/param/term.h"
#include "include/param/capconf.h"
#include "include/param/unique.h"

#include "include/debug/svrcomm_shell.h"
#include "uplink_pkt.h"
#include "uplink_dl.h"
#include "svrcomm.h"
#include "downlink/plcomm.h"
#include "downlink/plmdb.h"

#include "include/param/sceneuse.h"

static unsigned char recv_othebuf[512];/*用于有第二个接收函数发送缓存*/

unsigned short SceneMask = 0;//某位为1表示该位代表的情景编号开启
                        //仅用于开启某个情景
unsigned char SceneOnId = 0;//开启当前情景
unsigned char SceneOffId = 0;//关闭当前情景

unsigned char DevMask[MAX_METER+1] = {0};//为1表示写设备状态成功
extern int plcreading;                                    //为0表示失败

unsigned char     LedStr[512]={0};
unsigned char       UpdateLedFlag = 0; //UpdateLedFlag为1表示需要更新数据到显示屏，为0表示不用更新数据

unsigned char     audiourl[512]={0};            //服务器下发的音频文件下载地址
unsigned char       UpdateAudiourlFlag = 0;    //理新音频文件标志，0表示未有音频更新任务，1表示有音频更新任务

unsigned char       DeviceCheckResultFlag = 0;    //服务器返回的设备验证结果，0X18代表音频继续播放，0X1F为音频治疗强行中止。


#define MET_ONOFF_NUM sizeof(met_onoff)/sizeof(met_onoff[0])

/*
// 写设备参数
*/
static int svr_login_terminal(int itf)
{
    faalpkt_t *precv = (faalpkt_t *)FAAL_RCVBUF(itf);
    faalpkt_t *psend = (faalpkt_t *)FAAL_SNDBUF(itf);
    unsigned char * pu = precv->data;

//    int rtn;

    psend->cmd = FAALCMD_ECHO_LOGIN;

    if(memcmp(pu+32,ParaUniG.termip.pwd,32)==0)
    {
        FAAL_SETLEN(psend, 1);
        psend->data[0] = FAALERR_OK;
        faal_sendpkt(itf, psend);
    }
    else
    {
        FAAL_SETLEN(psend, 1);
        psend->data[0] = FAALERR_PWERR;
        faal_sendpkt(itf, psend);
    }

    return 0;
}
static int svr_heart_beat(int itf)
{
    faalpkt_t *precv = (faalpkt_t *)FAAL_RCVBUF(itf);
    faalpkt_t *psend = (faalpkt_t *)FAAL_SNDBUF(itf);
    unsigned char * pu = precv->data;
    psend->cmd = FAALCMD_ECHO_HEARTBEAT;
    FAAL_SETLEN(psend, 0);
    faal_sendpkt(itf, psend);
    return 0;
}

extern void ClearSaveParamFlag(void);
extern void SaveParam(void);



//#ifdef DEBUGMODE
//#define MAX_PKTLEN 40//400
//#else
#define MAX_PKTLEN (UPLINK_SNDMAX(itf)-100)
//#endif


extern int UplinkRecvPkt(unsigned char itf);


#define MAX_FRAME 7



static sys_mutex_t QueryCacheMutex;

//主站下发音频文件处理函数，
static int svr_update_audio_url(int itf)
{
    faalpkt_t *precv = (faalpkt_t *)FAAL_RCVBUF(itf);
    unsigned short datalen = makepkt_short(precv->len);//数据长度
    memcpy(audiourl,precv->data,datalen);
    UpdateAudiourlFlag = 1;
    PrintLog(0,"audio usl is :%s",audiourl);

    return 1;

}

//主站对设备的校验回复命令处理函数
static int svr_device_check_result(int itf)
{
    faalpkt_t *precv = (faalpkt_t *)FAAL_RCVBUF(itf);
    unsigned short datalen = makepkt_short(precv->len);//数据长度
    DeviceCheckResultFlag = precv->data[0];
    if(DeviceCheckResultFlag == 0x18)
       PrintLog(0,"audio is  Continue Play :%02x\n",DeviceCheckResultFlag);
    else if(DeviceCheckResultFlag == 0x1F)
       PrintLog(0,"audio is  Stop Play :%02x\n",DeviceCheckResultFlag);
    return 0;

}



typedef int (*msgproc_pf)(int itf);
typedef struct {
    unsigned char cmd;
    msgproc_pf pf;
} svrmsg_func_t;

const svrmsg_func_t g_srvmsg_flist[] = {
    {FAALCMD_LOGIN, svr_login_terminal},
    {FAALCMD_HEARTBEAT,svr_heart_beat},
    {FAALCMD_UPDATEAUDIOSTR, svr_update_audio_url},
    {FAALCMD_DEVICECHECK, svr_device_check_result},

    {0xff, NULL},
};

extern void faal_printpkt(faalpkt_t *pkt, char *prompt);
void SvrMessageProc(unsigned char itf)
{ 
    faalpkt_t *precv = (faalpkt_t *)FAAL_RCVBUF(itf);
    faalpkt_t *psend = (faalpkt_t *)FAAL_SNDBUF(itf);
    int rtn;
    unsigned char cmd;
    const svrmsg_func_t *plist = g_srvmsg_flist;
    PrintLog(0,"SvrMessageProc:%02x\n",precv->head);
    PrintLog(0,"SvrMessageProcdep:%02x\n",precv->dep);
    PrintLog(0,"SvrMessageProc:%02x\n",precv->cmd);
    PrintHexLog(0,&precv->head,30);

    PrintLog(0,"SvrMessageProc:%02x",precv->cmd);

    psend->cmd = precv->cmd;

    rtn = (int)makepkt_short(precv->len);
    rtn &= 0xffff;
    rtn += LEN_FAAL_HEAD;
    if(rtn > FAAL_RCVMAX(itf)) return;

    cmd = (precv->cmd)&FAALMASK_CMD;
    while(0xff != plist->cmd)
    {
        PrintLog(0,"plist->cmd:%02x,cmd:%02x\n",plist->cmd,cmd);
        if(cmd == plist->cmd)
        {
            if(*plist->pf == NULL)
                return;
            rtn = (*plist->pf)(itf);
            PrintLog(0,"rtn:%d\n",rtn);
            if(rtn)
            {
                psend->len[0] = 0;
                psend->len[1] = 0;
                psend->cmd |= FAALMASK_DIR;
                faal_sendpkt(itf, psend);
            }

            return;
        }

        plist++;
    }

}
/**
* @brief 消息处理模块初始化
*/
DECLARE_INIT_FUNC(SvrMessgeInit);
int SvrMessgeInit(void)
{
    SysInitMutex(&QueryCacheMutex);

    SET_INIT_FLAG(SvrMessgeInit);
    return 0;
}
