/**
* qrcode.h -- 接收二维码读头数据
* 
* 作者: chenshugao
* 创建时间: 2014-5-28
* 最后修改时间: 2014-5-28
*/

#ifndef _QRCODE_H
#define _QRCODE_H

extern void *QrCodeTask(void *arg);
typedef struct {
    unsigned short addr;               //设备地址
    unsigned char runstate[4];  //状态字
} ctrlcabinet_t;  //当前库
extern ctrlcabinet_t  ctrlstate[4];

extern int ctrlnum;


#endif /*_QRCODE_H*/

