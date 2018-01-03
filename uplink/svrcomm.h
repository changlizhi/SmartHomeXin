/**
* svrcomm.h -- 服务器通信
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-19
* 最后修改时间: 2009-5-19
*/

#ifndef _SVRCOMM_H
#define _SVRCOMM_H

void SvrCommPeekEvent(unsigned long waitmask, unsigned long *pevent);
int SvrNoteProc(unsigned char itf);
//int AlarmProc(unsigned char itf);

void SvrMessageProc(unsigned char itf);

#define LINESTAT_OFF        0
#define LINESTAT_ON        1
extern int SvrCommLineState;
extern unsigned char UpMetStateFlag[8];
extern unsigned char SvrCommInterface;

#define SOFTCHANGE 0x33
//void SetSoftChange(void);

//void SvrSendFileInfo(unsigned char itf);

#endif /*_SVRCOMM_H*/

