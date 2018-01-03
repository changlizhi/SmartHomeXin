/**
* unique.c -- 自定义参数
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-18
* 最后修改时间: 2009-5-18
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEFINE_PARAUNI

#include "param_config.h"
#include "include/debug.h"
#include "include/param/unique.h"
#include "include/sys/xin.h"
#include "include/sys/sqlitedb.h"
#include "include/lib/datachg.h"
para_uni_tG ParaUniG;
extern  void StringToIp(const char *str, unsigned char *ip);

static void LoadDefParaUni(void)
{
    memset((unsigned char*)&ParaUniG,0,sizeof(ParaUniG));

    memset(ParaUniG.read_period,0xFF,sizeof(ParaUniG.read_period));
    ParaUniG.cenmet_fenum = 4;//默认为4费率
    ParaUniG.siphmet_fenum = 0;//默认为0费率
    ParaUniG.muphmet_fenum = 4;//默认为4费率
    //ParaUniG.watermet_fenum = 0;//默认为0费率
    //ParaUniG.gasmet_fenum = 0;
    ParaUniG.daytomonth = 0;
    memset(ParaUniG.chktimeday,0,sizeof(ParaUniG.chktimeday));
    ParaUniG.chktimeday[0] = 0x04;
    ParaUniG.keepalive_cyc = 1;//心跳周期为15分钟
    ParaUniG.snd_timeout = 30; //默认为30秒
    ParaUniG.snd_retry = 2;//重发次数默认为2次
    /*by ydl modify 2011-06-28:总表规约默认为97协议*/
    //ParaUniG.proto_cenmet = 0;//总表规约
    ParaUniG.proto_cenmet = 10;//总表规约
    /*end*/
    ParaUniG.clientmode = 0; //默认永久在线
    ParaUniG.timedown = 30; //默认30分钟

    ParaUniG.termip.ipterm[0] = 192;  //集中器默认IP为192.168.0.223
    ParaUniG.termip.ipterm[1] = 168;
    ParaUniG.termip.ipterm[2] = 62;
    ParaUniG.termip.ipterm[3] = 223;
    ParaUniG.termip.maskterm[0] = 255;
    ParaUniG.termip.maskterm[1] = 255;
    ParaUniG.termip.maskterm[2] = 255;
    ParaUniG.termip.maskterm[3] = 0;
    ParaUniG.termip.ipgatew[0] = 192;
    ParaUniG.termip.ipgatew[1] = 168;
    ParaUniG.termip.ipgatew[2] = 62;
    ParaUniG.termip.ipgatew[3] = 1;
          ParaUniG.termip.portlisten = 9999;//UPD 侦听端口
          strcpy(ParaUniG.termip.username,"admin");
    strcpy(ParaUniG.termip.pwd,"12345");
          ParaUniG.metConfig.para_485port.port = 2;
          ParaUniG.metConfig.para_485port.baud = 2400;
          ParaUniG.metConfig.para_485port.databits = 8;
    ParaUniG.metConfig.para_485port.stopbits = 1;
    ParaUniG.metConfig.para_485port.parity = 1;
    ParaUniG.metConfig.usrname[0] = 0;
    ParaUniG.metConfig.passwd[0] = 0;

    strcpy(ParaUniG.xccode,"xc"); //固定，不从文件中读取
    strcpy(ParaUniG.drcode,"se");
    strcpy(ParaUniG.dxcode,"ct");
    strcpy(ParaUniG.rskcode,"10");
    strcpy(ParaUniG.lkcode,"LK");
    strcpy(ParaUniG.kncode,"NT");


    /*by ydl add 2011-05-15*/
    ParaUniG.addr_mac[0] = 0x56;
    ParaUniG.addr_mac[1] = 0x34;
    ParaUniG.addr_mac[2] = 0x12;
    ParaUniG.addr_mac[3] = 0x48;
    ParaUniG.addr_mac[4] = 0x80;
    ParaUniG.addr_mac[5] = 0x00;
    /*end*/
    ParaUniG.keepalive_sndretry = 3;
    ParaUniG.keepalive_dialtime = 0;
    ParaUniG.itemid1 = 0x9010;
    ParaUniG.itemid2 = 0x9090;

    ParaUniG.serial_baud = 115200;
    ParaUniG.serial_databits = 8;
    ParaUniG.serial_stopbits = 1;
    ParaUniG.serial_checkbit = 'n';
    ParaUniG.poll_time = 5;
    ParaUniG.serialtoird = 0;
    ParaUniG.downlink = 2;
    ParaUniG.metmon_dev = 5;
    ParaUniG.itemnum = 2;
    ParaUniG.checktimeday = 1;
    ParaUniG.checktimehour = 1;
    ParaUniG.cassflag = 0;
    ParaUniG.checktime = 30;
}

/*
extern void SetPlcModeuleNo(const int type);

int LoadPlcModulesNo(void)
{
    SetPlcModeuleNo(ParaUniG.downlink);
    return 0;
}
*/
static void StringToPrdTime(char *str, int prd)
{
    int timestart, timeend, i;
    char *pstr;

    pstr = str;
    while(0 != *pstr) {
        if(':' == *pstr) {
            *pstr = 0;
            pstr++;
            break;
        }
        pstr++;
    }
    if(0 == *pstr) return;
    i = atoi(str);
    if(i < 0 || i > 23) return;
    timestart = i*60;
    str = pstr;

    while(0 != *pstr) {
        if('~' == *pstr) {
            *pstr = 0;
            pstr++;
            break;
        }
        pstr++;
    }
    if(0 == *pstr) return;
    i = atoi(str);
    if(i < 0 || i > 59) return;
    timestart += i;
    str = pstr;

    while(0 != *pstr) {
        if(':' == *pstr) {
            *pstr = 0;
            pstr++;
            break;
        }
        pstr++;
    }
    if(0 == *pstr) return;
    i = atoi(str);
    if(i < 0 || i > 23) return;
    timeend = i*60;
    str = pstr;

    i = atoi(str);
    if(i < 0 || i > 59) return;
    timeend += i;

    if(timeend > 1438) return;

    ParaUniG.period.period_time[prd].start_time = (unsigned short)timestart;
    ParaUniG.period.period_time[prd].end_time = (unsigned short)timeend;
}


int LoadParaUni(void)
{    
    XINREF pf;
    int i;
    DebugPrint(0, "  load param unique...");
    DebugPrint(0,"sizeof(uni)=%d\r\n",sizeof(para_uni_tG));
    LoadDefParaUni();
//    ReadFromTable("ParaUni", &ParaUniG.read_period[0], sizeof(para_uni_tG));
//    return 0;
    pf = XinOpen(UNIQUE_SAVE_PATH "/unique.xin", 'r');
    if(NULL == pf) {
        DebugPrint(0, "no file\n");
        //SaveParaUni();
        return 1;
    }
       char str[64];
    char name[16];
    DebugPrint(0, "ok\n");

    ParaUniG.autojudge = XinReadInt(pf, "autojudge", 0);
    ParaUniG.autoStudy = XinReadInt(pf, "autostudy", 0);

    ParaUniG.keepalive_cyc = XinReadInt( pf, "keepalivecyc", 15);//心跳周期为15分钟
    ParaUniG.snd_timeout = XinReadInt( pf, "sndtimeout", 30); //默认为30秒
    ParaUniG.snd_retry = XinReadInt( pf, "sndretry", 2);//重发次数默认为2次
    ParaUniG.proto_cenmet = XinReadInt( pf, "protocenmet", 0);//总表规约

    ParaUniG.termip.autogetip    = XinReadInt(pf, "autogetip", 1);
    ParaUniG.termip.portlisten = XinReadInt(pf, "portlisten", 9999);//UPD侦听端口

    if(XinReadString(pf, "ipterm", str, 24) > 0)
    {
        StringToIp(str, ParaUniG.termip.ipterm);//集中器IP
    }
    if(XinReadString(pf, "maskterm", str, 24) > 0)
    {
        StringToIp(str, ParaUniG.termip.maskterm); //掩码
    }
       if(XinReadString(pf, "ipgatew", str, 24) > 0)
    {
        StringToIp(str, ParaUniG.termip.ipgatew); //网关
    }
    if(XinReadString(pf, "username", str, 32) > 0)
    {
        strcpy(ParaUniG.termip.username,str);
    }
      if(XinReadString(pf, "pwd", str, 32) > 0)
    {
        strcpy(ParaUniG.termip.pwd,str);
    }

//加载用户信息
    if(XinReadString(pf, "username", str, 32) > 0)
    {
        strcpy(ParaUniG.userinfo.username,str);
    }
      if(XinReadString(pf, "pwd", str, 32) > 0)
    {
        strcpy(ParaUniG.userinfo.pwd,str);
    }
    ParaUniG.userinfo.isopensms    = XinReadInt(pf, "opensms", 0);
    if(XinReadString(pf, "mobile1", str, 12) > 0)
    {
        strcpy(ParaUniG.userinfo.smsmobile1,str);
    }
      if(XinReadString(pf, "mobile2", str, 12) > 0)
    {
        strcpy(ParaUniG.userinfo.smsmobile2,str);
    }
    if(XinReadString(pf, "mobile3", str, 12) > 0)
    {
        strcpy(ParaUniG.userinfo.smsmobile3,str);
    }

    ParaUniG.userinfo.isopenemail = XinReadInt(pf, "openemail", 0);
    if(XinReadString(pf, "recvemail", str, 32) > 0)
    {
        strcpy(ParaUniG.userinfo.recvemail,str);
    }
      if(XinReadString(pf, "emailsender", str, 32) > 0)
    {
        strcpy(ParaUniG.userinfo.emailsender,str);
    }
    if(XinReadString(pf, "emailpwd", str, 12) > 0)
    {
        strcpy(ParaUniG.userinfo.emailpwd,str);
    }
    //级联参数
    ParaUniG.cascade_term.cascaMode = XinReadInt(pf, "cascamode", 0);
    ParaUniG.cascade_term.para_485port.port = CASCADE_REALPORT;
       ParaUniG.cascade_term.para_485port.baud = XinReadInt(pf, "baud0", 1200); //默认为1200
    ParaUniG.cascade_term.para_485port.databits = XinReadInt(pf, "databits0", 8);//默认为8
    ParaUniG.cascade_term.para_485port.stopbits = XinReadInt(pf, "stopbits0", 1);//默认为1
    ParaUniG.cascade_term.para_485port.parity = XinReadInt(pf, "parity0", 1);//校验位
    ParaUniG.cascade_term.para_485port.type = XinReadInt(pf, "type0", 2);//级联

/*by ydl modify 2011-03-21*/
#if 0
    if(XinReadString(pf, "cascadeip", str, 24) > 0)//级联负控地址
    {
        StringToIp(str, ParaUniG.cascade_term.cascade_addr);
    }
#else
    XinReadHex(pf, "cascadeip", (unsigned char *)ParaUniG.cascade_term.cascade_addr, 4);
    /*by ydl add 2011-07-04*/
    unsigned char *paddr = (unsigned char *)ParaUniG.cascade_term.cascade_addr;
    int tmp;
    tmp = paddr[2];
    paddr[2] = paddr[3];
    paddr[3] = tmp;
    /*end*/
#endif
    //485抄表参数
    ParaUniG.metConfig.para_485port.port = CENMET_REALPORT;
       ParaUniG.metConfig.para_485port.baud = XinReadInt(pf, "baud1", 1200); //默认为1200
    ParaUniG.metConfig.para_485port.databits = XinReadInt(pf, "databits1", 8);//默认为8
    ParaUniG.metConfig.para_485port.stopbits = XinReadInt(pf, "stopbits1", 1);//默认为1
    ParaUniG.metConfig.para_485port.parity = XinReadInt(pf, "parity1", 1);//校验位
    ParaUniG.metConfig.para_485port.type = XinReadInt(pf, "type1", 0);//485抄表

    //485总线参数
    ParaUniG.para_485bus.port = RS485BUS_REALPORT;
       ParaUniG.para_485bus.baud = XinReadInt(pf, "baud2", 1200); //默认为1200
    ParaUniG.para_485bus.databits = XinReadInt(pf, "databits2", 8);//默认为8
    ParaUniG.para_485bus.stopbits = XinReadInt(pf, "stopbits2", 1);//默认为1
    ParaUniG.para_485bus.parity = XinReadInt(pf, "parity2", 1);//校验位
    ParaUniG.para_485bus.type = XinReadInt(pf, "type2", 1);//485总线

    ParaUniG.linewasterhour = XinReadInt(pf, "lwhour",23);//级联线损请求。

    /*by ydl add 2011-05-16*/
    unsigned char tmpbuf[6];
    if(XinReadHex(pf, "mac", tmpbuf, 6) == 6)
    {
        memcpy(ParaUniG.addr_mac, tmpbuf, 6);
    }

    XinReadString(pf, "manuno", ParaUniG.manuno, 14);
    XinReadString(pf, "manuin", ParaUniG.manuno_inner, 12);

    ParaUniG.keepalive_sndretry = XinReadInt(pf,"keepalive_retry",3);
    ParaUniG.keepalive_dialtime = XinReadInt(pf,"keepalive_time",30);
    if(XinReadHex(pf,"itemid1",tmpbuf,2)==2){
        tmpbuf[2] = tmpbuf[0];
        tmpbuf[0] = tmpbuf[1];
        tmpbuf[1] = tmpbuf[2];
        ParaUniG.itemid1 = make_short(tmpbuf);
    }
    else ParaUniG.itemid1 = 0x9010;
    if(XinReadHex(pf,"itemid2",tmpbuf,2)==2){
        tmpbuf[2] = tmpbuf[0];
        tmpbuf[0] = tmpbuf[1];
        tmpbuf[1] = tmpbuf[2];
        ParaUniG.itemid2 = make_short(tmpbuf);
    }
    else ParaUniG.itemid2 = 0x9090;

    ParaUniG.serial_baud = XinReadInt(pf, "serial_baud", 115200);
    ParaUniG.serial_databits = XinReadInt(pf, "serial_databits", 8);
    ParaUniG.serial_stopbits = XinReadInt(pf, "serial_stopbits", 1);
    {
        char tmpbuf[4];
        if(XinReadString(pf, "serial_checkbit", tmpbuf, 4) > 0) {
            ParaUniG.serial_checkbit = tmpbuf[0];
        }
        else ParaUniG.serial_checkbit = 'n';
    }
    ParaUniG.poll_time = XinReadInt(pf,"poll_time",5);
    ParaUniG.serialtoird = XinReadInt(pf,"seriatoird",0);
    ParaUniG.downlink = XinReadInt(pf,"downlink",2);
    ParaUniG.metmon_dev = XinReadInt(pf,"metdev",2);
    ParaUniG.itemnum = XinReadInt(pf,"itemnum",1);
    ParaUniG.itemnum = 2;
    ParaUniG.checktimeday = XinReadInt(pf,"checkday",1);
    ParaUniG.checktimehour = XinReadInt(pf,"checkhour",1);
    /*end*/

    ParaUniG.pflag = XinReadInt(pf, "periodflag", 0);
    ParaUniG.prdcount = XinReadInt(pf, "count", 0);
    if(ParaUniG.prdcount > 8) {
        ParaUniG.prdcount = 8;
    }

    for(i=0; i<ParaUniG.prdcount; i++) {
        sprintf(name, "prd%d", i+1);
        if(XinReadString(pf, name, str, 64) > 0) {
            StringToPrdTime(str, i);
        }
    }
    ParaUniG.cassflag = XinReadInt(pf,"cassflag",0);
    ParaUniG.checktime = XinReadInt(pf,"checktime",30);
    XinClose(pf);
    return 0;
}

extern inline void IpToString(const unsigned char *ip, char *str);
int SaveParaUni(void)
{
    XINREF pf;
    char str[48];

    DebugPrint(0, "Save ParaUni\n");
    //SaveToTable("ParaUni", &ParaUniG.read_period[0], sizeof(para_uni_tG));
    //return 0;
    pf = XinOpen(UNIQUE_SAVE_PATH "/unique.xin", 'w');
    if(NULL == pf) {
        DebugPrint(0, "no file\n");
        return 1;
    }

    XinWriteInt(pf, "autojudge", ParaUniG.autojudge, 0);
    XinWriteInt(pf,"autostudy",ParaUniG.autoStudy,0);
    XinWriteInt( pf, "keepalivecyc", ParaUniG.keepalive_cyc, 0 );
    XinWriteInt( pf, "sndtimeout", ParaUniG.snd_timeout, 0 );
    XinWriteInt( pf, "sndretry", ParaUniG.snd_retry, 0 );
    XinWriteInt( pf, "protocenmet", ParaUniG.proto_cenmet, 0 );

    XinWriteInt( pf, "portlisten", ParaUniG.termip.portlisten, 0);
    XinWriteInt(pf, "autogetip", ParaUniG.termip.autogetip, 0);

    IpToString(ParaUniG.termip.ipterm, str);
       XinWriteString(pf, "ipterm", str);
    IpToString(ParaUniG.termip.maskterm, str);
       XinWriteString(pf, "maskterm", str);
    IpToString(ParaUniG.termip.ipgatew, str);
       XinWriteString(pf, "ipgatew", str);

    XinWriteString(pf, "username", ParaUniG.termip.username);
    XinWriteString(pf, "pwd", ParaUniG.termip.pwd);

//保存用户参数
    XinWriteString(pf, "username", ParaUniG.userinfo.username);
    XinWriteString(pf, "pwd", ParaUniG.userinfo.pwd);

    XinWriteInt(pf, "opensms", ParaUniG.userinfo.isopensms, 0);
    XinWriteString(pf, "mobile1", ParaUniG.userinfo.smsmobile1);
    XinWriteString(pf, "mobile2", ParaUniG.userinfo.smsmobile2);
    XinWriteString(pf, "mobile3", ParaUniG.userinfo.smsmobile3);

    XinWriteInt(pf, "openemail", ParaUniG.userinfo.isopenemail, 0);
    XinWriteString(pf, "recvemail", ParaUniG.userinfo.recvemail);
    XinWriteString(pf, "emailsender", ParaUniG.userinfo.emailsender);
    XinWriteString(pf, "emailpwd", ParaUniG.userinfo.emailpwd);


    //级联参数
    XinWriteInt(pf, "cascamode", ParaUniG.cascade_term.cascaMode, 0);
    XinWriteInt(pf, "baud0", ParaUniG.cascade_term.para_485port.baud, 0);
    XinWriteInt(pf, "databits0", ParaUniG.cascade_term.para_485port.databits, 0);
    XinWriteInt(pf, "stopbits0", ParaUniG.cascade_term.para_485port.stopbits, 0);
    XinWriteInt(pf, "parity0", ParaUniG.cascade_term.para_485port.parity, 0);
    XinWriteInt(pf, "type0", ParaUniG.cascade_term.para_485port.type, 0);

    /*by ydl add 2011-07-04*/
    unsigned char tmpbuf[4];
    unsigned char *paddr = tmpbuf;
    int tmp;

    memcpy(tmpbuf, ParaUniG.cascade_term.cascade_addr, 4);
    tmp = paddr[2];
    paddr[2] = paddr[3];
    paddr[3] = tmp;
    XinWriteHex(pf, "cascadeip", tmpbuf, 4);
    /*end*/

    /*by ydl add 2011-03-21*/
    //XinWriteHex(pf, "cascadeip", ParaUniG.cascade_term.cascade_addr, 4);


    //485抄表参数
    XinWriteInt(pf, "baud1", ParaUniG.metConfig.para_485port.baud, 0);
    XinWriteInt(pf, "databits1", ParaUniG.metConfig.para_485port.databits, 0);
    XinWriteInt(pf, "stopbits1", ParaUniG.metConfig.para_485port.stopbits, 0);
    XinWriteInt(pf, "parity1", ParaUniG.metConfig.para_485port.parity, 0);
    XinWriteInt(pf, "type1", ParaUniG.metConfig.para_485port.type, 0);

    //485总线参数
    XinWriteInt(pf, "baud2", ParaUniG.para_485bus.baud, 0);
    XinWriteInt(pf, "databits2", ParaUniG.para_485bus.databits, 0);
    XinWriteInt(pf, "stopbits2", ParaUniG.para_485bus.stopbits, 0);
    XinWriteInt(pf, "parity2", ParaUniG.para_485bus.parity, 0);
    XinWriteInt(pf, "type2", ParaUniG.para_485bus.type, 0);

    /*by ydl add 2011-05-15*/
    //mac地址
    XinWriteHex(pf, "mac", ParaUniG.addr_mac, 6);

    if (XinWriteString(pf, "manuno", ParaUniG.manuno))
    {
        DebugPrint(0, "save manuno error\n");
    }
    if (XinWriteString(pf, "manuin", ParaUniG.manuno_inner))
    {
        DebugPrint(0, "save manuin error\n");
    }
    XinWriteInt(pf,"keepalive_retry",ParaUniG.keepalive_sndretry,0);
    XinWriteInt(pf,"keepalive_time",ParaUniG.keepalive_dialtime,0);
    {
        depart_short(ParaUniG.itemid1,tmpbuf);
        tmpbuf[2] = tmpbuf[1];
        tmpbuf[1] = tmpbuf[0];
        tmpbuf[0] = tmpbuf[2];
    }
    XinWriteHex(pf,"itemid1",tmpbuf,2);

    {
        depart_short(ParaUniG.itemid2,tmpbuf);
        tmpbuf[2] = tmpbuf[1];
        tmpbuf[1] = tmpbuf[0];
        tmpbuf[0] = tmpbuf[2];
    }
    XinWriteHex(pf,"itemid2",tmpbuf,2);

    XinWriteInt(pf,"serial_baud",ParaUniG.serial_baud,0);
    XinWriteInt(pf,"serial_databits",ParaUniG.serial_databits,0);
    XinWriteInt(pf,"serial_stopbits",ParaUniG.serial_stopbits,0);
    tmpbuf[0]=ParaUniG.serial_checkbit;
    tmpbuf[1] = 0;
    XinWriteString(pf,"serial_checkbit",(char *)tmpbuf);
    XinWriteInt(pf,"poll_time",ParaUniG.poll_time,0);
    XinWriteInt(pf,"seriatoird",ParaUniG.serialtoird,0);
    XinWriteInt(pf,"downlink",ParaUniG.downlink,0);
    XinWriteInt(pf,"metdev",ParaUniG.metmon_dev,0);
    XinWriteInt(pf,"itemnum",ParaUniG.itemnum,0);
    XinWriteInt(pf,"checkday",ParaUniG.checktimeday,0);
    XinWriteInt(pf,"checkhour",ParaUniG.checktimehour,0);
    XinWriteInt(pf,"cassflag",ParaUniG.cassflag,0);
    /*end*/

    XinClose(pf);
    return 0;
}

