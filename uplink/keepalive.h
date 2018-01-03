/**
* keepalive.h -- 保持与服务器的连接
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-19
* 最后修改时间: 2009-5-19
*/

#ifndef _SVRCOMM_KEEPALIVE_H
#define _SVRCOMM_KEEPALIVE_H

#define KEEPALIVE_FLAG_LOGONFAIL    0
#define KEEPALIVE_FLAG_LOGONOK    1
#define KEEPALIVE_FLAG_TIMEOUT        2

int KeepAliveInPeriod(void);
void ClearKeepAlive(void);
int KeepAliveProc(void);
int RefreshKeepAlive(void);
void SetKeepAlive(unsigned char flag);

#endif /*_SVRCOMM_KEEPALIVE_H*/

