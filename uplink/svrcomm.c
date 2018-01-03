/**
* svrcomm.c -- 服务器通信
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-19
* 最后修改时间: 2009-5-19
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/basetype.h"
#include "include/debug.h"
#include "include/startarg.h"
#include "include/sys/event.h"
#include "include/sys/schedule.h"
#include "include/debug/shellcmd.h"
#include "include/sys/task.h"
#include "include/sys/timeal.h"
#include "include/sys/timer.h"
#include "include/lib/datachg.h"
#include "include/monitor/runstate.h"
#include "include/uplink/svrnote.h"
#include "include/param/unique.h"
#include "include/param/term.h"
#include "uplink_pkt.h"
#include "uplink_dl.h"
#include "keepalive.h"
#include "svrcomm.h"
#include "include/monitor/alarm.h"
#include "downlink/plcomm.h"
#include "downlink/plmdb.h"

extern unsigned char wifi_update_system_state;
extern unsigned char wifi_down_musice_state;

static unsigned int SvrCommNoteId = 0;
static sys_event_t SvrCommEvent;
unsigned char SvrCommInterface = UPLINKITF_ETHER;
int SvrCommLineState = 0;
unsigned char UpMetStateFlag[8]={0};    //电表状态变化标志D0位为１表示metid = 1的表状态有变化，为０表示无变化
extern int SendMessage(char *uuid,char *pwd,char *mobile,char *content);
//extern int SendAlarmMail(char * fromaddr,char *username,char*pwd,char *to,char *content,char *picdir);
extern int IntToBcd(int l, unsigned char *bcd, int maxlen);
/*
*@ 任务标志
*/
int svrcomm_havetask(void)
{
    if(SvrCommNoteId)return 1;
    else return 0;
}
/**
* @brief 接收上行通信事件
* @param waitmask 接收掩码
* @param pevent 返回事件掩码指针
*/
void SvrCommPeekEvent(unsigned long waitmask, unsigned long *pevent)
{
    SysWaitEvent(&SvrCommEvent, 0, waitmask, pevent);
}

/**
* @brief 生成上行通信事件
* @param id 事件编号
*/
void SvrCommNote(int id)
{
    AssertLogReturnVoid(id<SVRNOTEID_ALARM|| id > SVRNOTEID_UPMETINFO, "invalid note id(%d)\n", id);
    SvrCommNoteId |= id;
    SysSendEvent(&SvrCommEvent, SVREV_NOTE);
}

/**
* @brief 音频播放事件上报处理
* @param itf 接口编号
* @return 成功返回0,   中止返回1.
*/
int AlarmProc(unsigned char itf)
{
     uplink_pkt_tG *psnd;
    int   filestat =0;
    unsigned char sendbuff[256]={0};
    unsigned char alarmData[8]={0};
    int rtn1 = 0;
    unsigned short dataLen =  1;
    psnd = (uplink_pkt_tG *)sendbuff;
    FAAL_SETLEN(psnd, 8);
    psnd->cmd = 0x02;
    psnd->head = 0x68;
    psnd->dep = 0x68;

    while(1)
    {
        rtn1 = ActiveSendAlarm(alarmData);
        DebugPrint(0, "ActiveSendAlarm rtn1(%d)\n",rtn1);
        if(rtn1 == -1)
            break;
        else
        {
            memcpy(psnd->data,alarmData,8);
            rtn1 = UplinkActiveSend(2, 0, psnd);   //改为不等待。
        }
    }
    filestat = getFileDays();

    if(filestat == 0)//音频文件在有效时间内
    {
        //判断系统是当前未更新，而且未下载音频文件，刚将WIFI关闭，减少发热
        if(wifi_update_system_state == 0 && wifi_down_musice_state == 0)
          system("wifi up");
    }
    return 0;
}

#define FREEZETYPE_DAY          0x40
#define FREEZETYPE_MONTH     0x80


#undef FREEZETYPE_DAY
#undef FREEZETYPE_MONTH

/**
* @brief 温湿度数据上报
* @param itf 接口编号
* @return 成功返回0,   中止返回1.
*/
int SensorDataProc(unsigned char itf)
{
     uplink_pkt_tG *psnd;
    unsigned char sendbuff[256]={0};
    int rtn1,rtn2,rtn3;
    runstate_tG *pstat = RunStateModifyG();

    psnd = (uplink_pkt_tG *)sendbuff;

    if(UpdateSensorId == 240)
      psnd->cmd = 0x8B;
    else
     psnd->cmd = 0x88;

    psnd->head = 0x68;
    psnd->dep = 0x68;
    FAAL_SETLEN(psnd, 8);
    memcpy(psnd->data,MdbCurData[UpdateSensorId].runstate,MAX_RUNSTATE);
    rtn1 = UplinkActiveSend(2, 0, psnd);   //改为不等待。
    return 0;
}

/**
* @brief 上行通信事件处理
* @param itf 接口编号
* @return 完成返回0,未完成返回1
*/
int SvrNoteProc(unsigned char itf)
{
    int tid;
    unsigned int mask;

    //DebugPrint(LOGTYPE_SHORT, "actsend %08XH, %08XH, %d\n", SvrCommNoteId, SvrCommNoteIdTaskCls2, ParaMix.bactsend);
    DebugPrint(0, "处理上报事件%02x,itf=%d\n",SvrCommNoteId,itf);
    //告警
    if(SVRNOTEID_ALARM & SvrCommNoteId)
    {
        DebugPrint(0, "Start alarm\n");
        if(AlarmProc(itf))
        {
            DebugPrint(0, "AlarmProc not finished\n");
            goto mark_end;
        }
        mask  = SVRNOTEID_ALARM;
        SvrCommNoteId &= ~ mask;
    }

    if(SVRNOTEID_UPSENSOR & SvrCommNoteId)  //上传温湿度
    {
        mask  = SVRNOTEID_ALARM;
        SvrCommNoteId &= ~ SVRNOTEID_UPSENSOR;
        SensorDataProc(itf);

    }
    SvrCommNoteId &= ~SVREV_NOTE;
    DebugPrint(0, "处理上报事件%02x\n",SvrCommNoteId);
    PrintHexLog(0, UpMetStateFlag, 8);
mark_end:

    return 1;
}

/**
* @brief 上行通信方式判断
*/
static inline void LoadSvrCommItf(void)
{
    SvrCommInterface = ParaTermG.svraddr.net.chn;

       switch(SvrCommInterface)
       {
           case 0xff:
              SvrCommInterface = UPLINKITF_ETHER;
            break;
        case 0x02:
               SvrCommInterface = UPLINKITF_ETHER;
            break;
        case 0x03:
               SvrCommInterface = UPLINKITF_ETHER;
               break;
        case 0x04: //以太网
               SvrCommInterface = UPLINKITF_ETHER;
            break;
        case 0x05:
               SvrCommInterface = UPLINKITF_ETHER;
            break;
        default:
               SvrCommInterface = UPLINKITF_ETHER;
            break;
       }

    switch(SvrCommInterface)
    {
        case UPLINKITF_ETHER:
            break;
        default:
            SvrCommInterface = UPLINKITF_ETHER;
            break;
    }

}

static const char cons_svritf_sflag[] = {
    'U', 'N', 'G', 'E', 'S', 'I', 'R',
};

extern void EtherTask(void);

//static int SerialPassiveStarted = 0;

/**
* @brief 上行通信处理任务
*/
static void *SvrCommTask(void *arg)
{
    DebugPrint(0,"enter svrcommtask\n");

    ClearKeepAlive();

    Sleep(100);

    switch(SvrCommInterface) {
    case UPLINKITF_ETHER:
        EtherTask();
        break;
    default: break;
    }

    while(1) Sleep(1000);
}




extern int EthSvrInit(void);
extern int TcpSvrInit(void);
extern int LocalSvrInit();
//extern int SvrMessgeInit(void);

/**
* @brief 服务器通信初始化
*/
DECLARE_INIT_FUNC(SvrCommInit);

int SvrCommInit(void)
{
    SysInitEvent(&SvrCommEvent);

    LoadSvrCommItf();
    EthSvrInit();
    Sleep(50);

    SysCreateTask(SvrCommTask, NULL);
    SET_INIT_FLAG(SvrCommInit);

    return 0;
}

static int SendSms(int argc, char *argv[])
{

    int ret = SendMessage("94480","dcf42e23aa7b02e50c173a166250ef72","15012890652","智能报警设备有不明物体发生入侵，请确认！");
    if(ret == 100)
        DebugPrint(0,"发送短信成功!");
    else
        DebugPrint(0,"发送短信失败!ErrorCode=%d",ret);
    return -1;

}
DECLARE_SHELL_CMD("sendsms", SendSms, "发送报警测试短信");

