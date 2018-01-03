#ifndef _PARA_TERM_H
#define _PARA_TERM_H

//主站通信接口
//#define SVRADDR_SMS    1    //短信
#define SVRADDR_GPRS    2   //GPRS/CDMA
#define SVRADDR_DTMF    3    //DTMF
#define SVRADDR_ETHERNET    4   //以太网
#define SVRADDR_IR    5    //红外
#define SVRADDR_RS232    6   //串口
#define SVRADDR_CSD    7    //GSM CSD
#define SVRADDR_RADIO    8    //无线
#define SVRADDR_SMSWAKE    9    //短信唤醒
//#define SVRADDR_METCHK        10   //485校表
#if 0
typedef union {
    struct {
        unsigned char chn;    //通道类型
        unsigned char uc[15];
    } mix;

    struct __attribute__((packed)){
        unsigned char chn;   //通道类型
        unsigned char reserv;
        unsigned char reserv2;
        unsigned int ip;   //IP
        unsigned short port;   //端口号
    } net;

    struct {
        unsigned char chn;
        unsigned char phone[8];  //电话号码
    } sms;

    #define channel    mix.chn
} svraddr_t;
#endif

int paraterm_svraddr(unsigned char flag, itemop_t *op);
int paraterm_gateaddr(unsigned char flag, itemop_t *op);
int paraterm_apn(unsigned char flag, itemop_t *op);
int paraterm_usrname(unsigned char flag, itemop_t *op);
int paraterm_time(unsigned char flag, itemop_t *op);
int paraterm_realtime(unsigned char flag, itemop_t *op);

int paraterm_port_chkbit(unsigned char flag, itemop_t *op);
int paraterm_port_baud(unsigned char flag, itemop_t *op);
int paraterm_port_func(unsigned char flag, itemop_t *op);
int paraterm_port_databit(unsigned char flag, itemop_t *op);
int paraterm_port_stopbit(unsigned char flag, itemop_t *op);

int paraterm_softver(unsigned char flag, itemop_t *op);
int paraterm_hardware(unsigned char flag, itemop_t *op);

int paraterm_allmetotal(unsigned char flag, itemop_t *op);
int paraterm_normetotal(unsigned char flag, itemop_t *op);
int paraterm_simmetotal(unsigned char flag, itemop_t *op);
int paraterm_impmetotal(unsigned char flag, itemop_t *op);

int paraterm_linewase(unsigned char flag, itemop_t *op);
int paraterm_upreaddev(unsigned char flag, itemop_t *op);

int paraterm_upimpdata(unsigned char flag, itemop_t *op);
int paraterm_updaydata(unsigned char flag, itemop_t *op);
int paraterm_cascadeaddr(unsigned char flag, itemop_t *op);
int paraterm_runstate(unsigned char flag, itemop_t *op);
int paraterm_cameralist(unsigned char flag, itemop_t *op);
int paraterm_camera(unsigned char flag, itemop_t *op);
int paraterm_scenelist(unsigned char flag, itemop_t *op);
int paraterm_scene(unsigned char flag, itemop_t *op);
int paraterm_timertasklist(unsigned char flag, itemop_t *op);
int paraterm_timertask(unsigned char flag, itemop_t *op);

int paraterm_monlastread(unsigned char flag, itemop_t *op);
int paraterm_monlastup(unsigned char flag, itemop_t *op);

int paraterm_peibian_addr(unsigned char flag, itemop_t *op);
int paraterm_reset(unsigned char flag, itemop_t *op);
int paraterm_sampling(unsigned char flag, itemop_t *op);

int paramet_optmet(unsigned char flag, itemop_t *op);
int paramet_batchoptmet(unsigned char flag, itemop_t *op);
#endif

