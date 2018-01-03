/**
* unique.h -- 自定义参数
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-18
* 最后修改时间: 2009-5-18
*/
#ifndef _PARA_UNIQUE_H
#define _PARA_UNIQUE_H

#include "commport.h"

typedef struct __attribute__((packed)){
//typedef struct {
    unsigned char autogetip;//是否自动获取IP地址
    unsigned char ipterm[4];//IP地址
    unsigned char maskterm[4];//掩码
    unsigned char ipgatew[4];//网关
    unsigned char ipproxy[4];//代理
    unsigned char mac[6];     //mac地址。
    unsigned short portproxy;//代理端口
    unsigned char proxy_type;//代理类型
    unsigned char proxy_connect;//连接方式
    char username[32];//用户名称
    char pwd[32];//密码
    char mobile1[12];        //短信接收号码1
    char mobile2[12];        //短信接收号码2
    char mobile3[12];
    char email[32];    //接收告警的邮箱地址
    unsigned short portlisten;//UDP帧听端口
} cfg_termip_tG;

//typedef struct __attribute__((packed)){
typedef struct {
    char username[32];//用户名称
    char pwd[32];//密码
    unsigned char isopensms;    //开启短信报警功能0为关闭,1为开启
    char smsmobile1[12];        //短信接收号码1
    char smsmobile2[12];        //短信接收号码2
    char smsmobile3[12];        //短信接收号码3
    unsigned char isopenemail;    //开启邮件报警服务功能0为关闭,1为开启
    char recvemail[32];        //接收告警的邮箱地址
    char emailsender[32];        //163邮箱地址
    char emailpwd[32];        //163邮箱密码
} cfg_userinfo_tG;
//级联参数设置
typedef struct{
    int cascaMode; //级联模式0   从模式 1 主模式
    cfg_para_485port_t para_485port;
    char cascade_addr[4];    //作为从终端时主终端地址
}cfg_cascade_tG;

//多功能表参数
typedef struct{
    cfg_para_485port_t para_485port;
    char usrname[6];//通信用户名
    char passwd[6];//通信密码
}cfg_MetConfig_tG;
typedef struct {
    unsigned short start_time;  //hour*60+minute
    unsigned short end_time;
}period_time_t;

typedef struct {
    period_time_t period_time[8];
}cfg_termperiod_t;

typedef struct {
    unsigned char read_period[12];      //允许抄表时段(缺省为0)
    unsigned char cenmet_fenum;        //总表费率数(0~4，缺省为0)
    unsigned char siphmet_fenum;         //单相表费率数(0~4，缺省为0)
    unsigned char muphmet_fenum;     //简易多功能表费率数(0~4，缺省为0)
    unsigned char watermet_fenum;     //水表费率数(0~4，缺省为0)
    unsigned char gasmet_fenum;         //气表费率数(0~4，缺省为0)
    unsigned char daytomonth;          //0-无效；1-把每月1号日冻结当为月冻结数据；2-把月冻结数据当为当天日冻结数据，缺省为0
    unsigned char chktimeday[4];          //D0~D30分别表示每月1号~31号，置1表示较时，0表示不较时，缺省为04 00 00 00，即每月3号较时
    unsigned char keepalive_cyc;          //心跳周期(分钟), 缺省为15分钟
    unsigned char snd_timeout;          //发送超时(秒), 缺省为30秒
    unsigned char snd_retry;              //重发次数，缺省为2
    unsigned char proto_cenmet;          //总表通讯规约
    unsigned char clientmode;              //0-永久在线, 1-被动激活, 2-时段在线
    unsigned char timedown;                 //无通信自动断线时间 minute

    unsigned char addr_mac[6];  //终端MAC地址

    cfg_termip_tG      termip;               //集中器ip地址和端口
    cfg_userinfo_tG     userinfo;                //用户信息
    cfg_cascade_tG cascade_term;         //级联终端地址
    cfg_MetConfig_tG metConfig;        //多功能表通讯参数
    cfg_para_485port_t para_485bus;        //485总线

    unsigned char linewasterhour;

    char xccode[8];//晓程编码
    char drcode[8];//东软编码
    char dxcode[8];//鼎信编码
    char rskcode[8];//瑞斯康编码
    char lkcode[8];//朗金编码
    char kncode[8];//科能编码

    unsigned char autojudge;//是否自动判断路由电力载波猫厂家1:自动识别0:手动设置
    unsigned char autoStudy;//自动学习标记  1 自动学习 0 手动学习 默认为手动学习

    char manuno[14];                 //终端外部生产编号
    char manuno_inner[12];          //终端内部生产编号
    unsigned char keepalive_sndretry;/*发送次数*/
    unsigned char keepalive_dialtime;/*重间隔时间*/
    unsigned short itemid1;/*用于电表抄写数据项(0x9010,0x9090)*/
    unsigned short itemid2;
    //上行串口设置
    unsigned char serial_databits;//调试串口数据位
    unsigned char serial_stopbits;//调试串口停止位
    char serial_checkbit;//调试串口校验位
    unsigned int serial_baud;//调试串波特率
    unsigned char poll_time;//级联轮询
    unsigned char serialtoird;//红外与维护口互用:2为调试串口,0为红外,1为维护串口
    unsigned char downlink;//下行通信方式--组网方式
    unsigned char metmon_dev;//485抄表时间
    unsigned char itemnum;//载波自动抄表的数据项
    unsigned char checktimeday;//校时那一天
    unsigned char checktimehour;//校时小时
    unsigned char pflag; //0:无效 1:有效
    unsigned char prdcount;//时段数
    cfg_termperiod_t period;    //抄表时段added by yzl.2010.6.24
    unsigned char cassflag; //对总表和配电终端时行选择(线损率计算:0总表,1配电终端)
    unsigned char checktime;//校时超时时间

} para_uni_tG;

#ifndef DEFINE_PARAUNI
extern para_uni_tG ParaUniG;
#endif

int SaveParaUni(void);

#endif 

