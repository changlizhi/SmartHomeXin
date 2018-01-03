/**
* rs485met.c -- 485抄表通信接口
* 
* 作者: 
* 创建时间: 2009-11-30
* 最后修改时间: 
*/

#include <stdio.h>
#include <string.h>

#include "include/basetype.h"
#include "include/debug.h"
#include "include/lib/datachg.h"
#include "include/sys/schedule.h"
#include "include/sys/timeal.h"
#include "include/param/capconf.h"
#include "downlink/plcomm.h"
#include "include/sys/rs485.h"
#include "include/param/unique.h"
#include "rfmet.h"
#include "include/param/term.h"


static unsigned char Rs485BusBuffer[RS485BUS_BUF_LEN];

/**
* @brief 读数据
* @param dest 目的地址
* @param itemid 数据项标识
* @param buf 数据帧缓存区指针
* @param len 缓存区长度
* @return 成功返回实际数据长度, 失败返回-1
*/
int RFMetRead(const plc_dest_t *dest, unsigned long itemid, unsigned char *buf, int len)
{
    int rtn,portnum;
    unsigned short crc;
    unsigned char     port;
    unsigned short     metid;
    metid    =    dest->metid;
    unsigned char  modbusbuf[8] = {0};
    if(metid > MAX_METER) {
        return -1;
    }

    modbusbuf[0] = metid;
    modbusbuf[1] = itemid;
    modbusbuf[5] = 0x02;
    crc = GetCRC16(modbusbuf,6);
    if(metid == 240 || metid  == 241)
    {
        modbusbuf[6] = crc>>8;
        modbusbuf[7] = crc;
    }
    else
    {
        modbusbuf[6] = crc;
        modbusbuf[7] = crc>>8;
    }

    PrintHexLog(0,modbusbuf,8);
    portnum = dest->portcfg;
    port = portnum;

    Rs485Lock(port);
    Sleep(10);
    while(Rs485Recv(port, buf, 1) > 0);

    rtn = Rs485Send(port,modbusbuf,8);

    Rs485Unlock(port);

    return rtn;
}

/**
* @brief 表拉合闸控制
* @param dest 目的地址
* @param pconfig 写配置
* @return 成功返回0, 否则失败
*/
int RFMetCtrl(const plc_dest_t *dest,unsigned long itemid,unsigned char *buf,int len)
{
    int rtn,portnum;
    unsigned short crc;
    unsigned char     port;
    unsigned short     metid;
    metid    =    dest->metid;
    unsigned char  modbusbuf[8] = {0};
    if(metid > MAX_METER) {
        return -1;
    }

    modbusbuf[0] = metid;
    modbusbuf[1] = itemid;
    modbusbuf[2] = buf[0];
    modbusbuf[3] = buf[1];
    modbusbuf[4] = buf[2];
    modbusbuf[5] = buf[3];

    crc = GetCRC16(modbusbuf,6);

    if(metid == 240 || metid  == 241)
    {
        modbusbuf[6] = crc>>8;
        modbusbuf[7] = crc;
    }
    else
    {
        modbusbuf[6] = crc;
        modbusbuf[7] = crc>>8;
    }

    portnum = dest->portcfg;
    port = portnum;

    Rs485Lock(port);
    Sleep(10);
    while(Rs485Recv(port, buf, 1) > 0);

    rtn = Rs485Send(port,modbusbuf,8);

    Rs485Unlock(port);

    return rtn;


}

