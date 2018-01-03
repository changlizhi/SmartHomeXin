/**
* uplink_pkt.h -- 上行通信帧结构
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-18
* 最后修改时间: 2009-5-18
*/

#ifndef _UPLINK_PKT_H
#define _UPLINK_PKT_H

#define LEN_UPLINKDL            8
#define LEN_UPLINKHEAD       10
#define MINLEN_UPLINKPKT    (LEN_UPLINKDL+LEN_UPLINKHEAD)

#define UPLINK_HEAD        0x68
#define UPLINK_TAIL        0x16


#define UPLINKAPP_LEN_G(pkt)    (MAKE_SHORT(pkt->len))


/*广电版帧格式定义*/
typedef struct {
    unsigned char head;  //=68H
    unsigned char ver;
    unsigned char addr[4];
    unsigned char dep;  //=68H
    unsigned char cmd;  //控制码
    unsigned char len[2];  //数据长度
    unsigned char data[1022];  //数据域
} uplink_pkt_tG;

#define OFFSET_UPDATA_G      12  //广标头11个字节。



#define UPAFN_TRANFILE        0x30    //文件传输

#endif /*_UPLINK_PKT_H*/

