/**
* plccomm.h -- 载波通信接口头文件
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-4-24
* 最后修改时间: 2009-4-24
*/

#ifndef _PLCOMM_H
#define _PLCOMM_H

//目的配置
typedef struct {
    unsigned short metid;  // 从1开始, 0无效
    unsigned char portcfg;// 1-RS232, 2-以太网
    unsigned char proto;
} plc_dest_t;


#define PLCOMM_BUF_LEN        272
unsigned char *GetPlCommBuffer(void);

void MakePlcDest(unsigned short metid, plc_dest_t *dest);

#define PLCHKTIME_POLL        1   //轮询方式较表
#define PLCHKTIME_BROCAST    2   //广播方式较表

//返回错误码
#define PLCERR_INVALID        -1
#define PLCERR_TIMEOUT        -2

int PlcRead(const plc_dest_t *dest, unsigned long itemid, unsigned char *buf, int len);
int PlcCtrlMet(const plc_dest_t *dest, unsigned long itemid,unsigned char *buf,int len);
void GetModulesName(char *pbuff,int nLen);
void SetPlcModeuleNo(const int type);
int GetPlcModeuleNo(void);

#ifndef DEFINE_PLCOMM
//extern const int PlcTimeChecking;
extern int PlcMetRegistStart;
extern unsigned char PlXcBuffer[256];

extern int PlcSamplingMetid ;
#endif

#endif /*_PLCOMM_H*/

