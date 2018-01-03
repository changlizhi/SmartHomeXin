/**
* uplink645.h -- 645通信
* 
* 作者: chengrongtao
* 创建时间: 2010-5-28
* 最后修改时间: 2010-5-28
*/

#ifndef _DWONLINK_downlink645_H
#define _DWONLINK_downlink645_H
#define DOWNLINK_HEAD        0x68
#define DOWNLINK_TAIL        0x16
#define HEAD645_LEN        10
#define TAIL645_LEN        2


#define ITEM645_LEN        2
#define PWD645_LEN        3

#define DATA645_LEN(len)    (len-ITEM645_LEN-PWD645_LEN)
typedef struct {
    unsigned char addr;
    unsigned char cmd;
    unsigned char data[4];
}downlink_pktmodbus_t;//645报文格式

extern void *UplinkModBusTask(void *arg);

#endif /*_UPLINK_uplink645_H*/

