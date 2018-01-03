
/**
* term.h -- 终端参数头文件
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-6
* 最后修改时间: 2009-5-6
*/

#ifndef _PARAM_TERM_H
#define _PARAM_TERM_H

#include "capconf.h"
#include "../version.h"

typedef struct {
    //unsigned char uplink;               //上行通信接口
    unsigned char unuse[3];

    //unsigned char addr_mac[6];      //终端MAC地址

    //char manuno[14];                 //终端外部生产编号
    //char manuno_inner[12];          //终端内部生产编号
} para_manuf_t;

#ifndef DEFINE_PARAMANUF
para_manuf_t ParaManuf;
#endif


#define SVRADDR_TYPE_GPRS            2  // GPRS/CDMA
#define SVRADDR_TYPE_PSTN            3  // PSTN
#define SVRADDR_TYPE_ETHER        4  // 以太网
#define SVRADDR_TYPE_RS232        6  // RS232/485
#define SVRADDR_TYPE_CSD            7  // CSD
#define SVRADDR_TYPE_RADIO        8  // 无线
#define SVRADDR_TYPE_SMSWAKE        9  // 短信唤醒
#define SVRADDR_TYPE_ADSL            20  // ADSL

#define MAX_CAMERA_NUM            4
#define MAX_CAMERA_CHANEL_NUM    2
//密码等级
#define PASSWD_HIGH        2
#define PASSWD_LOW                1
#define PASSWD_READONLY       0
#define PASSWD_ERROR          -1

#define AUTLEVEL_HIGH            2
#define AUTLEVEL_LOW             1
#define AUTLEVEL_OTHER          0
#define AUTLEVEL_ERROR         -1

extern int PasswdLevel;
extern int Autlevel;

typedef union {
    struct {
        unsigned char chn;  //通道类型
        unsigned char uc[8];
    } mix;

    struct //__attribute__((packed)){
    {
        unsigned char chn;  //通道类型
        unsigned char unused; //高字节填充
        unsigned char unused2;  //填充字符
              unsigned char ip[4];  //IP
        unsigned short port;  //端口号
    } net;

    struct {
        unsigned char chn;  //通道类型
        unsigned char phone[8];
    } pstn;
    #define channel mix.chn
} cfg_svraddr_tG;




typedef struct __attribute__((packed)){
//typedef struct{
    unsigned char     index;            //索引号
    unsigned char        nchanel;        //通道数
    unsigned char        fac_type;        //厂商代码,1为海康威视,2为亚安
    unsigned short     net_port;        //网络端口号
    char             net_addr[32];    //网络地址
    char                username[16];    //登录用户名
    char                password[16];    //登录密码
    char                videoname[16];
}Camera;

//typedef struct __attribute__((packed)){
typedef struct{
    unsigned char     autoflag;                //注册标志1为已注册,0为未注册
    char                register_code[13];        //注册码
}RegisterInfo;

typedef struct{
    unsigned short maxtolMeter;//8811
    unsigned short maxplcmet;  //8812
    unsigned short maxcenmet; //8813
    unsigned char  maximpmet; //8814
}MaxMeter_tG;

// 端口1~4 4路RS485,
#define RS485_PORTNUMPORT    3   //最大485端口数目
#define PORTCFG_NUM    7//UPLINKITF_NUM//8   //最大端口参数数目
typedef struct __attribute__((packed)){
//typedef struct{
    unsigned char valid;    //有效标志
    unsigned char baud;   //波特率,
    unsigned char parity;
    unsigned char databits;
    unsigned char stopbits;
    unsigned char func;        //端口功能//0,抄表1级联2被抄
} portcfg_t;

typedef struct {
    cfg_svraddr_tG svraddr;  //8010 主站通讯地址
    cfg_svraddr_tG svraddr_1;  //8011 备用主站通讯地址1
    cfg_svraddr_tG svraddr_2;  //8012 备用主站通讯地址1
    unsigned char sms_addr[16];  //8013 短信中心号码
    cfg_svraddr_tG gate_addr;  //8014 默认网关地址
    char apn[16];  //8015 APN
    char  MusicState;            //音频文件状态 0--有效，治疗中。1--音频已经过期。2--音频不存在。3--
    unsigned char deviceid[8];
    unsigned short ledscreenPort; //LED显示屏端口
    char strledscreenaddr[16];      //LED显示屏地址
    char strmusicpwd[16];
    unsigned short music_volume;  //音量大小
    unsigned char  first_start;  //首次启动
    unsigned char  Musicmonth;
    unsigned char login_update_system;
    unsigned char login_down_musice;
    char cdma_usr[32];  //8019CDMA登陆用户名//存放的字符串可以直接使用，例cmnet
    char cdma_pwd[32];  //801ACDMA登陆用户名//同上

    unsigned char nor_pwd[3];   //8020 普通密码(只读权限)
    unsigned char com_pwd[4];  //8021 设置密码 (权限低)
    unsigned char adm_pwd[3];  //8022 管理员密码   (  权限高)

    unsigned char search_device;  //8810 振铃次数
    unsigned char task_starthour;  //8815 定时任务起始时间
    unsigned char task_startdev;  //8816 定时任务执行间隔
                                  //(00H:30分钟 01H:60分钟(默认) 02H:120分钟
                                  //03H:240分钟 04H:360分钟 05H:720分钟)
    unsigned char plc_route_type;  //8817 载波中继方式(00:自动中继 其他:固定中继)
    unsigned char gate_linewaste[2];  //8818 线损率报警阈值(0.1%)
    unsigned char hour_upimpdata;  //8819 重点户数据上传时间(时0~23, 为FF是表示不主动上报，默认为FF)
    unsigned char hour_updaydata;  //881A 日冻结数据上传时间(时0~23, 为FF是表示不主动上报，默认为FF)
    unsigned int   alarm_flag;  //881B 告警使能控制字
                               //D7=1允许线损超阈值/负值报警
                               //D6=1允许失压报警
                               //D5=1允许失流报警
                               //D4=1允许电流反向告警
                               //D3=1允许电表编程告警
                               //D2=1允许抄表失败告警
                               //D1=1允许电表时钟异常告警
                               //D0=1允许停电事件告警
                               //其它预留

    unsigned char cascade_addr[16];  //881D 级联从终端地址列表(4个, 某地址等于FFFFFFFF时表示不存在)
    unsigned char cascade_flag;  //881E 集中器/配变终端级联标志(只使用D2,D1,D0位,其他位置0;
                                 //D2=1，表示允许级联; D1=1，表示允许级联对; D0=1，表示该终端为主终端)
    unsigned char day_read_mondata;  //8820 月末数据抄收开始时间(日, 1~28)
    unsigned char hour_read_mondata;  //8820 月末数据抄收开始时间(时, 0~23)

    unsigned char day_up_mondata;  //8821 月末数据上传开始时间(日, 1~28)
    unsigned char hour_up_mondata;  //8821 月末数据上传开始时间(时, 0~23)

    unsigned char peibian_addr[4];    //8822:台区对应配变地址在自定义参数设置
    unsigned char device_code[16];
    MaxMeter_tG  stmaxmeter; //各种类型表数。
    unsigned char ct[2]; //台变对应配变CT
    RegisterInfo    reg;


    Camera       camerainfo[MAX_CAMERA_NUM];

    portcfg_t port[PORTCFG_NUM];   //87xx, 端口配置
}para_term_tG; //广电协议

#ifndef DEFINE_PARATERM
extern para_term_tG ParaTermG;
#endif

//int CheckDataLenFromDio(unsigned short dio);
int  AlterTermPara(unsigned short dio, char *pData, int nLen, unsigned short nTn);
int  TermPara8010(char *pstr,cfg_svraddr_tG* pcf);
int  BuffRev(char* pstr,  int nLen);
int  ReadTermPara(unsigned short  dio, char *res);
//int LoadParaManuf(void);
int SaveParaTerm(void);
int  MakeDeviceCode();
void  GetDeviceCode(unsigned char *deviceid,int *autoflag);
//int SaveParaManuf(void);

//注：软件版本分为CCXX-AIT-FFF-NNNNN 格式，CC 表示厂商代码（BCD 码），XX 表示不同类型的软件，
//A 主版本，I 次版本，T 小版本，FFF 附加信息，NNNNNN 用行政区码表示地方定制信息。
inline static void maketermversion(unsigned char* buf, int buflen)
{
/*
    if(buflen<8)  return ;

    buf[0] = 0x01;
    buf[1] = VERSION_PROJECT;
    buf[2] = ((VERSION_MAJOR<<4)&0xf0) | (VERSION_MINOR&0x0f);
    buf[3] = 0;
    buf[4] = 0;
    buf[5] = 0x80;
    buf[6] = 0x41;
    buf[7] = 0;
    */
}
#endif

