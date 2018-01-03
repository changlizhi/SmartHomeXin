/**
* uplink645.c -- 645通信
* 
* 作者: chengrongtao
* 创建时间: 2010-5-28
* 最后修改时间: 2010-5-28
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/debug.h"
#include "include/lib/bcd.h"
#include "include/sys/schedule.h"
#include "include/sys/task.h"
#include "include/sys/rs485.h"
#include "include/sys/gpio.h"
#include "save/param.h"
#include "include/param/commport.h"
#include "include/param/term.h"
#include "include/debug.h"
#include "downlink_dl.h"
#include "downlink645.h"
#include "include/sys/uart.h"
#include "include/debug/shellcmd.h"
#include "include/lib/align.h"
#include "plmdb.h"
#include "uplink/svrcomm.h"
#include "include/uplink/svrnote.h"
#include "include/monitor/alarm.h"
#include "downlink/plcomm.h"
extern int PlcMetReading;    // 1为 正在进行点抄
int    UpdateSensorId = 0;

#define RS485I    1
extern void MakeAlarmG(const alarm_buf_t *pbuf);

//rs485III的基本读写函数
int imet645_getchar(unsigned char *buf)
{
    if(Rs485Recv(RS485I, buf, 1)>0)return 0;
    else return 1;
}
int imet645_rawsend(const unsigned char *buf, int len)
{
    return(Rs485Send(RS485I, buf, len));
}

extern int SaveParaMeter(void);
extern int SaveParaUni(void);
extern int DownlinkRecvPkt645(unsigned char itf1);





typedef int (*operation645_fn)(unsigned char * addr,unsigned char *data,int len);
typedef struct 
{
    unsigned short itemid;
    unsigned char itemlen;
    operation645_fn pfunc;
    unsigned short offset;
} paraitem645_t;

extern void SaveParam(void);
extern void SetSaveParamFlag(unsigned int flag);


/**
* @brief 设置参数命令处理
* @param itf 接口编号
*/
static void ModBusLocksState(unsigned char itf)
{
    DebugPrint(0,"ModBusLocksState\n");
    downlink_pktmodbus_t *prcv = (downlink_pktmodbus_t *)DOWNLINK_RCVBUF(itf);
    unsigned char* pdata = prcv->data;
    int i = 0, rtn = 1;
    int len = 0;
    unsigned short imet  = 0;

    imet =     prcv->addr;
    pdata = prcv->data;

    MdbCurData[imet].addr = prcv->addr;
    MdbCurData[imet].runstate[1] =  prcv->data[0];
    MdbCurData[imet].runstate[3] =    prcv->data[1];
    MdbCurData[imet].runstate[5] =  prcv->data[2];
    MdbCurData[imet].runstate[7] =    prcv->data[3];

    UpdateSensorId = imet;
    SvrCommNote(SVRNOTEID_UPSENSOR);
}
/**
* @brief 设置参数命令处理
* @param itf 接口编号
*/
static void ModBusTempHum(unsigned char itf)
{
    DebugPrint(0,"ModBusTempHum\n");
    downlink_pktmodbus_t *prcv = (downlink_pktmodbus_t *)DOWNLINK_RCVBUF(itf);
    unsigned char* pdata = prcv->data;
    int i = 0, rtn = 1;
    int len = 0;
    unsigned short imet  = 0;

    imet =     prcv->addr;
    pdata = prcv->data;
    PrintHexLog(0,pdata,4);
    if(imet == 0x00F0)
    {
        MdbCurData[imet].addr = prcv->addr;
        memcpy(MdbCurData[imet].runstate,pdata,4);
    }
    else if(imet == 0x00F1)
    {
        memcpy(MdbCurData[imet-1].runstate[4],pdata,4);
    }
    UpdateSensorId = 240;
    SvrCommNote(SVRNOTEID_UPSENSOR);
}

struct srvmsgmodbus_func_t {
    unsigned char cmd;
    void (*pf)(unsigned char itf);
};
static const struct srvmsgmodbus_func_t srvmsgmodbus_func[] = 
{
    {0x83, ModBusTempHum},      //表返回数据
    {0x85, ModBusLocksState},      //表返回数据
    {0, NULL},
};


/**
* @brief 服务器命令处理
* @param itf 接口编号
*/
void SvrMessageProcModBus(unsigned char itf)
{
    downlink_pktmodbus_t *prcv = (downlink_pktmodbus_t *)DOWNLINK_RCVBUF(itf);
    const struct srvmsgmodbus_func_t *plist = srvmsgmodbus_func;

    while(0 != plist->cmd)
    {
        if(plist->cmd == prcv->cmd) break;
        plist++;
    }
    if(0 == plist->cmd) return;

    (*plist->pf)(itf);
    return;
}


/**
* @brief 645任务--以集中器为表
*/
void *UplinkModBusTask(void *arg)
{
    DownlinkClearState(DOWNLINKITF_IMET645);

    while(1)
    {
        if(!PlcMetReading )    //没有进行点抄时
        {

            if(!DownlinkRecvPkt645(DOWNLINKITF_IMET645))
            {
                DebugPrint(0,"搜索设备....");
                SvrMessageProcModBus(DOWNLINKITF_IMET645);
            }
        }
        if(exitflag)
            break;
        Sleep(10);
    }
}

/**
* @brief 通过645协议读取imet数据
* @return 成功0, 否则失败
*/
DECLARE_INIT_FUNC(IMet645);
int IMet645(void)
{

    SysCreateTask(UplinkModBusTask, NULL);
    SET_INIT_FLAG(IMet645);
    return 0;
}

