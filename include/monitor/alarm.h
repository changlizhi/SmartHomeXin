/**
* alarm.h -- 事件告警接口
* 
* 作者: yangzhilong
* 创建时间: 2009-10-26
* 最后修改时间: 2009-10-26
*/

#ifndef _ALARM_H
#define _ALARM_H

#define MAX_ALARM            1024

typedef struct {
    utime_t     starttime;       // 播放起始时间
    utime_t     endtime;           // 播放结束时间
    unsigned short palyedid;      //告警编码
} alarm_buf_t;
extern alarm_buf_t AlarmCache[MAX_ALARM];


#ifndef  TRUE
#define  TRUE   1
#endif

#ifndef  FALSE
#define  FALSE  0
#endif

#ifndef BOOL
#define BOOL  int
#endif


void ClearAlarm();
int ActiveSendAlarm(unsigned char *sendbuffer);


#endif

