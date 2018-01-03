/**
* term.h -- 终端参数
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-6
* 最后修改时间: 2009-5-6
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEFINE_PARATERM
#define DEFINE_PARAMANUF
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>


#include "include/basetype.h"
#include "param_config.h"
#include "include/debug.h"
#include "include/version.h"
#include "include/param/term.h"
#include "include/param/unique.h"
#include "include/sys/xin.h"
#include "include/sys/bin.h"
#include "include/sys/timeal.h"
#include "include/sys/syslock.h"
#include "include/sys/reset.h"
#include "include/sys/diskspace.h"
#include "include/lib/bcd.h"
#include "operation_inner.h"
#include "include/lib/align.h"
#include "include/lib/crc.h"
#include "include/lib/datachg.h"
#include "include/debug/shellcmd.h"

para_term_tG ParaTermG;
para_manuf_t ParaManuf;

const portcfg_t g_def_portcfg[PORTCFG_NUM]={
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
};

int PasswdLevel = -1;     //密码级别
int Autlevel = -1;             //权限级别
#define CAMERA_PARAM_MAGIC        0x872563bb
#define UUID_MAGIC                    0x9631fb14

int  getuciConfigvar(const char *varname, char *buffer)
{
    char str_tmp[256];
    FILE *fd=NULL;
    sprintf(str_tmp, "uci -c/opt/ft get ftconfig.@ftconfig[0].%s 2>&1",varname);
    fd = popen(str_tmp, "r");
    if(fd == NULL) return -1;
    memset(str_tmp, '\0', 100);
    fgets(str_tmp, 100, fd);
    pclose(fd);
    PrintLog(0,"str_tmp:%s\n",str_tmp);
    if((!strncmp(str_tmp,"uci:",strlen("uci:"))) || (strlen(str_tmp)<2) )
    {
        return -1;
    }
    else
    {
        str_tmp[strlen(str_tmp)-1]='\0';
        sprintf(buffer, "%s",str_tmp);
        return 0;

    }

}
int  setuciConfigvar(const char *varname, char *buffer)
{
    char str_tmp[256];
    FILE *fd=NULL;
    sprintf(str_tmp, "uci -c/opt/ft set ftconfig.@ftconfig[0].%s=%s",varname,buffer);
    fd = popen(str_tmp, "r");
    if(fd == NULL) return -1;
    memset(str_tmp, '\0', 100);
    fgets(str_tmp, 100, fd);
    pclose(fd);
    PrintLog(0,"str_tmp:%s\n",str_tmp);
    if((!strncmp(str_tmp,"uci:",strlen("uci:"))) || (strlen(str_tmp)<2) )
    {
        return -1;
    }
    else
    {
        str_tmp[strlen(str_tmp)-1]='\0';
        sprintf(buffer, "%s",str_tmp);
        return 0;

    }

}

int   MakeDeviceCode()
{
    memset( (unsigned char*)&ParaTermG,0,sizeof(ParaTermG) );
    char str_tmp[256];
    FILE *fd=NULL;
    sprintf(str_tmp, "uci -c/opt/ft get ftconfig.@ftconfig[0].sn 2>&1");
    fd = popen(str_tmp, "r");
    if(fd == NULL) return -1;
    memset(str_tmp, '\0', 100);
    fgets(str_tmp, 100, fd);
    pclose(fd);
    if((!strncmp(str_tmp,"uci:",strlen("uci:"))) || (strlen(str_tmp)<3) )
    {
        return 0;
    }
    else
    {
        str_tmp[strlen(str_tmp)-1]='\0';
        sprintf(ParaTermG.device_code,"%s",str_tmp);
        //memcpy(ParaTermG.device_code,str_tmp);
        return 0;
    }


}

void  TestDeviceCode()
{

    char     filename[200] = {0};
    sprintf(filename,"/bin/udevcode");
    unsigned char devicecode[16];
    unsigned char number[4];
    if(ReadBinFile(filename, UUID_MAGIC, devicecode, 16) > 0)
    {
        DebugPrint(0, "Read UUID ok, \n uuid    :");
        PrintHexLog(0, devicecode, 16);
    }
    else
    {
        DebugPrint(0, "Read UUID  fail, ");
    }



}
static int shell_uuid(int argc, char *argv[])
{
    TestDeviceCode();
    return 0;
}
DECLARE_SHELL_CMD("shell_uuid", shell_uuid, "shell_uuid");

void MakeRegisteCode(char * code)
{
    unsigned char buf[6] = {0};
    sprintf(code,"%02X%02X%02X%02X%02X%02X",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
}
int  SetGatewRegiste(const char *regcode)
{
    char rightCode[13] = {0};
    MakeRegisteCode(rightCode);
    if(strcmp(rightCode,regcode) == 0) //注册成功
    {
        ParaTermG.reg.autoflag = 1;
        memcpy(ParaTermG.reg.register_code,regcode,13);
        return 0;
    }
    else
    {
        ParaTermG.reg.autoflag = 1;
        //ParaTermG.reg.autoflag = 0;
        memset(ParaTermG.reg.register_code,0x00,13);
        return -1;
    }
}

void GetDeviceCode(unsigned char * deviceid,int *autoflag)
{
    *autoflag = ParaTermG.reg.autoflag;
}
static void LoadDefParaTerm(void)
{
    int i;
    /*广电*/


          ParaTermG.svraddr.net.ip[0] = 192;
          ParaTermG.svraddr.net.ip[1] = 168;
          ParaTermG.svraddr.net.ip[2] = 3;
    ParaTermG.svraddr.net.ip[3] = 187;
    ParaTermG.svraddr.net.port = 5000;//默认连接端口为9001
    ParaTermG.ledscreenPort = 28123;
    ParaTermG.MusicState = 0;

    sprintf(ParaTermG.strledscreenaddr,"192.168.3.187");
/*            ParaTermG.svraddr.net.ip[0] = 114;
            ParaTermG.svraddr.net.ip[1] = 112;
            ParaTermG.svraddr.net.ip[2] = 52;
        ParaTermG.svraddr.net.ip[3] = 104;
        ParaTermG.svraddr.net.port = 5000;//默认连接端口为9001
*/
    ParaTermG.svraddr_1.net.ip[0] = 114;
          ParaTermG.svraddr_1.net.ip[1] = 112;
          ParaTermG.svraddr_1.net.ip[2] = 52;
          ParaTermG.svraddr_1.net.ip[3] = 104;
          ParaTermG.svraddr_1.net.port = 5000;//默认连接端口为9001
   
          strcpy((char*)ParaTermG.apn,"CMNET");
    strcpy((char*)ParaTermG.cdma_usr,"CMNET");
    strcpy((char*)ParaTermG.cdma_pwd,"CMNET");

    /*网关*/
       ParaTermG.gate_addr.net.ip[0] = 192;
    ParaTermG.gate_addr.net.ip[1] = 168;
    ParaTermG.gate_addr.net.ip[2] = 0;
    ParaTermG.gate_addr.net.ip[3] = 1;



    ParaTermG.deviceid[0] = 0;
    ParaTermG.deviceid[1] = 7;
    ParaTermG.deviceid[2] = 5;
    ParaTermG.deviceid[3] = 5;
    ParaTermG.deviceid[4] = 1;
    ParaTermG.deviceid[5] = 4;
    ParaTermG.deviceid[6] = 0xD7;
    ParaTermG.first_start = 1;
    ParaTermG.Musicmonth = 7;
    ParaTermG.login_update_system = 0;
    ParaTermG.login_down_musice = 0;

/*
        ParaTermG.deviceid[0] = 0;
        ParaTermG.deviceid[1] = 0;
        ParaTermG.deviceid[2] = 0;
        ParaTermG.deviceid[3] = 0;
        ParaTermG.deviceid[4] = 0;
        ParaTermG.deviceid[5] = 0;
        ParaTermG.deviceid[6] = 1;
*/
    /*默认振铃次数*/
    ParaTermG.search_device = 0; //默认为5次
    /*任务开始时间*/
    ParaTermG.task_starthour = 0x01;//默认为1点启动
    /*定时任务执行间隔*/
    ParaTermG.task_startdev = 0x01;//默认为1小时间隔
    /*默认载波中继方式*/
    ParaTermG.plc_route_type = 0x00;//自动中继
    /*默认线损阀值*/
    ParaTermG.gate_linewaste[0] = 1;//默认为1%
    ParaTermG.gate_linewaste[1] = 0;
    /*重点用户数据上传时间*/
    ParaTermG.hour_upimpdata = 0xFF; //不上传
    /*日冻结上传时间*/
    ParaTermG.hour_updaydata = 0xFF;//不主动上传
    /*告警使能控制字*/
    ParaTermG.alarm_flag = 0;//不告警
    /*级联从终端地址*/
    memset((unsigned char*)ParaTermG.cascade_addr,0xFF,sizeof(ParaTermG.cascade_addr));//全部为无效值
    /*级联标记*/
    ParaTermG.cascade_flag = 1;//默认为主集中器，且不允许级联
    /*月末数据抄收开始时间*/
    ParaTermG.day_read_mondata = 0x01;//一号开始冻结
    /*月末数据抄收开始时间*/
    ParaTermG.hour_read_mondata = 0x01;// 1点
    /*月冻结上传开始时间*/
    ParaTermG.day_up_mondata = 0x03; // 3号
    ParaTermG.hour_up_mondata = 0x05;//5点上传

    ParaTermG.nor_pwd[0] = 0; //普通密码，缺省值为000000
    ParaTermG.nor_pwd[1] = 0;
    ParaTermG.nor_pwd[2] = 0;
    ParaTermG.com_pwd[0] = 0x11; //设置密码，缺省值为111111
    ParaTermG.com_pwd[1] = 0x11;
    ParaTermG.com_pwd[2] = 0x11;
    ParaTermG.adm_pwd[0] = 0;//管理员密码，缺省值为000000
    ParaTermG.adm_pwd[1] = 0;
    ParaTermG.adm_pwd[2] = 0;
    for(i=0;i<MAX_485PORT;i++){
    ParaTermG.port[i].baud = 4;
    ParaTermG.port[i].databits = 8;
    ParaTermG.port[i].parity = 1;
    ParaTermG.port[i].stopbits = 1;
    ParaTermG.port[i].func = i;
    }

    ParaTermG.reg.autoflag = 1;
    memset(ParaTermG.reg.register_code,0x00,16);
    //LoadParaCamra(); //加载摄像头信息
    /*end*/

}

 void StringToIp(const char *str, unsigned char *ip)
{
    int idx, i;

    ip[0] = ip[1] = ip[2] = ip[3] = 0;
    i = idx = 0;
    for(;0 != *str; str++) {
        if('.' == *str) {
            ip[idx] = i;
            i = 0;
            idx++;
            if(idx > 3) return;
        }
        else if(*str >= '0' && *str <= '9') {
            i *= 10;
            i += *str - '0';
        }
        else {
            ip[idx] = i;
            return;
        }
    }

    ip[idx] = i;
}

inline void IpToString(const unsigned char *ip, char *str)
{
    sprintf(str, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    DebugPrint(0, "save ip:%s\n", str);
}

inline void sms_phone_tonormal(int bytes, unsigned char *num, unsigned char *phone)
{
    int i;

    for (i=0; i<bytes; i++)
    {
        num[8-i-1] = num[bytes-1-i];
    }
    for (i=0; bytes<8; bytes++, i++)
    {
        num[i] = 0xaa;
    }

    for (i=0; i<8; i++)
    {
        phone[i] = num[8-1-i];
    }
}

/*
@para:phone-短信号码
        strphone-存储转化后的短信号码
@func:将短信号码由数字转为ASCII编码
@retn:无
@info:by ydl add 2011-10-17
*/

void sms_phone_numtoascii(unsigned char *phone, unsigned char *strphone)
{
    int i;
    int flag;
    int idx;

    idx = 0;
    flag = 0;
    for (i=0; i<16; i++)
    {
        if (flag==0)
        {
            if (phone[16-1-i] != 0 && phone[16-1-i]!=0xaa)
            {
                strphone[idx++] = phone[16-1-i]+0x30;
            }

            if (phone[16-1-i] == 0xaa)
            {
                flag = 1;
            }
        }
        else
        {
            if (phone[16-1-i]!=0xaa)
            {
                strphone[idx++] = phone[16-1-i]+0x30;
            }
        }
    }

    strphone[idx] = 0;
}
inline void sms_phone_tounnor(unsigned char *num, unsigned char *phone)
{
    int i;
    for (i=0; i<8; i++)
    {
        num[i] = phone[8-1-i];
    }
}

void Cascade_AddrChange(unsigned char *target, unsigned char *outadr,unsigned char flag)
{
    int i, tmp;
    if(1 == flag){
        memcpy(outadr, target, 16);
        for (i=0; i<4; i++)
        {
            tmp = outadr[i*4+2];
            outadr[i*4+2] = outadr[i*4+3];
            outadr[i*4+3] = tmp;
        }
    }
    else{
        memcpy(outadr, target, 4);
        {
            tmp = outadr[2];
            outadr[2] = outadr[3];
            outadr[3] = tmp;
        }

    }
}

unsigned char hard_info[18] = {
    0x04, 0x04, 0x04, 0x04, 0x10, 0xfb, 0x03, 0x01,
    0x55, 0x07, 0x00, 0x00, 0x11, 0x13, 0x01, 0x12,
    0x02, 0x0b,
};
extern int ReadDriverVersion(unsigned char * buf, int len);

/*
Func Description: 载入硬件信息
Func Param:
Func Return:
*/
static void load_hardinfo(void)
{
    unsigned char ver;
//    unsigned char hardinf[6];
    ver = VERSION_MINOR;
    ver=(ver<<4)&0xF0;
    hard_info[12] |= ver;
    ver = VERSION_MAJOR;
    ver <<= 4;
    ver |= VERSION_SUBMAJOR;
    hard_info[13] = ver;
/*
    ReadDriverVersion(hardinf,6);
    hard_info[16] = hardinf[0];
    hard_info[17] = hardinf[1];*/
    hextobcd(&hard_info[16], 2);
}


/**
* @brief 从文件中载入终端参数
* @return 0成功, 否则失败
*/

int LoadParaTerm(void)
{
    XINREF pf;
    char str[100];
    int i;
    struct hostent *mast_host;
    struct in_addr *sin_addr;
    DebugPrint(0, "  load param term...");
    DebugPrint(0, "  sizeof(term)=%d\r\n",sizeof(para_term_tG));

    LoadDefParaTerm();



    if(getuciConfigvar("sn",str) == 0)
    {
        PrintLog(0,"SN:%s\n",str);
        ParaTermG.deviceid[0] = str[0]-0x30;
        ParaTermG.deviceid[1] = str[1]-0x30;
        ParaTermG.deviceid[2] = str[2]-0x30;
        ParaTermG.deviceid[3] = str[3]-0x30;
        ParaTermG.deviceid[4] = str[4]-0x30;
        ParaTermG.deviceid[5] = str[5]-0x30;
        ParaTermG.deviceid[6] = str[6]-0x30;
        ParaTermG.deviceid[7] = str[7]-0x30;
    }
    if(getuciConfigvar("firststart",str) == 0)
    {
        ParaTermG.first_start = str[0]-0x30;
    }

    if(getuciConfigvar("musicmonth",str) == 0)
    {
        ParaTermG.Musicmonth = str[0]-0x30;
    }


    if(getuciConfigvar("master_addr",str) == 0)
    {
        PrintLog(0,"master_addr:%s\n",str);
        mast_host = gethostbyname(str);
        if(mast_host != NULL)
        {
            PrintLog(0,"Master IP Address :%s\n",inet_ntoa(*((struct in_addr *)mast_host->h_addr)));
            PrintLog(0,"Master HostName :%s\n",mast_host->h_name);
            sin_addr = (struct in_addr *)mast_host->h_addr;
            memcpy(ParaTermG.svraddr.net.ip,&sin_addr->s_addr,4);
            //ParaTermG.svraddr.net.ip[0] = sin_addr->s_addr.s_b1;
            //ParaTermG.svraddr.net.ip[1] = sin_addr->s_addr.s_b2;
            //ParaTermG.svraddr.net.ip[2] = sin_addr->s_addr.s_b3;
            //ParaTermG.svraddr.net.ip[3] = sin_addr->S_un_b.s_b4;
        }
    }

    if(getuciConfigvar("master_port",str) == 0)
    {
        PrintLog(0,"master_port:%s\n",str);
        ParaTermG.svraddr.net.port = atoi(str);

    }

    PrintLog(0,"music_volume:%s\n",str);
    if(getuciConfigvar("music_volume",str) == 0)
    {
        PrintLog(0,"music_volume:%s\n",str);
        ParaTermG.music_volume = atoi(str);
        setvolume(ParaTermG.music_volume);
    }
    else
    {
        PrintLog(0,"music_volume:%s\n",str);
        ParaTermG.music_volume = 20;
        setvolume(ParaTermG.music_volume);
    }
    sprintf(str,"ParaTermG.device_code = %s\n",ParaTermG.device_code);
    load_hardinfo();
    pf = XinOpen(PARAM_SAVE_PATH "/term.xin", 'r');
    if(NULL == pf) {
        pf = XinOpen(PARAM_BAK_PATH "/term.xin", 'r');
        if(NULL == pf) {
            DebugPrint(0, "no file\n");
    //        SaveParaTerm();
            return 1;
        }
    }

    /*读主站IP*/
    ParaTermG.svraddr.net.chn = XinReadInt(pf, "svraddrflag",0x04);//读上行连接方式
    if(XinReadString(pf, "svraddrIp", str, 24) > 0) {
        StringToIp(str, ParaTermG.svraddr.net.ip);
    }
    ParaTermG.svraddr.net.port = XinReadInt(pf, "svrport", 8300);

    /*读主站2IP*/
    ParaTermG.svraddr_1.net.chn = XinReadInt(pf, "svraddrflag1",0x04);//读上行连接方式
    if(XinReadString(pf, "svraddrIp1", str, 24) > 0) {
        StringToIp(str, ParaTermG.svraddr_1.net.ip);
    }
    ParaTermG.svraddr_1.net.port = XinReadInt(pf, "svrport1", 9001);

    /*读主站3IP*/
    ParaTermG.svraddr_2.net.chn = XinReadInt(pf, "svraddrflag2",0x04);//读上行连接方式
    if(XinReadString(pf, "svraddrIp2", str, 24) > 0) {
        StringToIp(str, ParaTermG.svraddr_2.net.ip);
    }
    ParaTermG.svraddr_2.net.port = XinReadInt(pf, "svrport2", 9001);

      /*读网关地址*/
          if(XinReadString(pf, "gateaddr", str, 24) > 0)
    {
        StringToIp(str, ParaTermG.gate_addr.net.ip);
    }

    /*by ydl add 2011-03-21*/
    ParaTermG.gate_addr.net.port = XinReadInt(pf, "gateport", 9001);

      /*读短信中心号码*/
    //XinReadHex(pf, "smsaddr", str,16);
    int rtn;
     rtn = XinReadHex(pf, "smsaddr", (unsigned char *)str, 8);
     if (rtn != -1)
     {
        sms_phone_tonormal(rtn, (unsigned char *)str, ParaTermG.sms_addr);
     }

    XinReadString(pf, "apn", ParaTermG.apn,16);
    /*by ydl modify 2011-05-15*/

    XinReadString( pf, "cdmausr", ParaTermG.cdma_usr,32 );
    XinReadString(pf, "cdmapwd", ParaTermG.cdma_pwd, 32);

    XinReadHex(pf, "norpwd", ParaTermG.nor_pwd, 3);//普通密码
    XinReadHex(pf, "compwd", ParaTermG.com_pwd, 3);//设置密码
    XinReadHex(pf, "admpwd", ParaTermG.adm_pwd,3);//管理员密码

    /*默认振铃次数*/
    ParaTermG.search_device = XinReadInt( pf, "search_device", 0);
    /*任务开始时间*/
    ParaTermG.task_starthour = XinReadInt(pf, "taskstarthour", 0x01);
    /*任务执行间隔*/
    ParaTermG.task_startdev = XinReadInt(pf, "taskstartdev", 0x01);//间隔一小时
    /*默认载波中继方式*/
    ParaTermG.plc_route_type = XinReadInt( pf, "plcroutetype", 0x00);//自动中继
    /*默认线损阀值*/
    XinReadHex( pf, "gatelinewaste",ParaTermG.gate_linewaste, 2);// 1表示0.1%
    /*重点用户数据上传时间*///保存为BCD码例如，如果是12点，则保存的是12而不是0x12
    ParaTermG.hour_upimpdata = XinReadInt( pf, "hourupimpdata", 0xFF);//不主动上传
    /*日冻结上传时间*/
    ParaTermG.hour_updaydata = XinReadInt( pf, "hourupdaydata", 0xFF);//不主动上传
    /*告警使能控制字*/
    ParaTermG.alarm_flag = XinReadInt( pf, "alarmflag", 0);//不告警

    /*级联从终端地址*/
    //XinReadHex(pf, "cascadeaddr", ParaTermG.cascade_addr, 16);
    unsigned char tmpbuf[16];
    XinReadHex(pf, "cascadeaddr", tmpbuf, 16);
    Cascade_AddrChange(tmpbuf, ParaTermG.cascade_addr,1);

    //加载注册信息
    ParaTermG.reg.autoflag = XinReadInt( pf, "AutoFlag", 0x01);
    //检查注册码是不是本机使用的
    XinReadString(pf,"RegeCode",ParaTermG.reg.register_code,13);
    char tempregcode[13];
    MakeRegisteCode(tempregcode);
    if(strcmp(ParaTermG.reg.register_code,tempregcode) == 0) //注册验证成功
    {
           ParaTermG.reg.autoflag = 1;
    }
    else
    {
        ParaTermG.reg.autoflag = 0;
    }
    ParaTermG.reg.autoflag = 1;

    /*级联标记*/
    ParaTermG.cascade_flag = XinReadInt( pf, "cascadeflag", 1);//默认为主集中器，且不允许级联
    /*月末数据抄收开始时间*/
    ParaTermG.day_read_mondata = XinReadInt( pf, "dayreadmondata", 0x01);//一号开始冻结
    /*月末数据抄收开始时间*/
    ParaTermG.hour_read_mondata = XinReadInt( pf, "hourreadmondata", 0x01);// 1点
    /*月冻结上传开始时间*/
    ParaTermG.day_up_mondata = XinReadInt( pf, "dayupmondata", 0x03); // 3号
    ParaTermG.hour_up_mondata = XinReadInt( pf, "hourupmondata", 0x05);//5点上传
//    printf("day_mont=%d,hour_mont=%d",ParaTermG.day_read_mondata,ParaTermG.hour_read_mondata);
//    printf("day_up_mont=%d,hour_up_mont=%d",ParaTermG.day_up_mondata,ParaTermG.hour_up_mondata);
    XinReadHex(pf,"peibian_addr",tmpbuf,4);
    Cascade_AddrChange(tmpbuf,ParaTermG.peibian_addr,0);
    XinReadHex(pf, "ct", ParaTermG.ct, 2);//配变CT
    for(i=0;i<MAX_485PORT;i++){
    sprintf((char *)tmpbuf,"baud%d",i);
       ParaTermG.port[i].baud = XinReadInt(pf, (char *)tmpbuf, 4); //默认为1200
    sprintf((char *)tmpbuf,"databits%d",i);
    ParaTermG.port[i].databits = XinReadInt(pf, (char *)tmpbuf, 8);//默认为8
    sprintf((char *)tmpbuf,"stopbits%d",i);
    ParaTermG.port[i].stopbits = XinReadInt(pf, (char *)tmpbuf, 1);//默认为1
    sprintf((char *)tmpbuf,"parity%d",i);
    ParaTermG.port[i].parity = XinReadInt(pf, (char *)tmpbuf, 1);//校验位
    sprintf((char *)tmpbuf,"func%d",i);
    ParaTermG.port[i].func= XinReadInt(pf, (char *)tmpbuf, i);//端口功能
    }



    DebugPrint(0, "ok\n");
    XinClose(pf);

    return 0;
}

/**
* @brief 保存终端参数
* @return 0成功, 否则失败
*/
int SaveParaTerm(void)
{

    //SaveParaCamra(); //保存摄像头信息
    XINREF pf;
    int bakup = 0;
    char str[48];
    int i;
    DebugPrint(0, "Save paraterm\n");

    pf = XinOpen(PARAM_SAVE_PATH "/term.xin", 'w');
    if(NULL == pf)
    {
            PrintLog(0,"open param_path...\n");
            goto mark_bakup;
    }

mark_save:


    XinWriteInt( pf, "svraddrflag", ParaTermG.svraddr.net.chn, 0);
    IpToString(ParaTermG.svraddr.net.ip, str);
       XinWriteString(pf,"svraddrIp", str);  
       XinWriteInt(pf, "svrport", ParaTermG.svraddr.net.port, 0);

    IpToString(ParaTermG.svraddr_1.net.ip, str);
       XinWriteString(pf,"svraddrIp2", str);
       XinWriteInt(pf, "svrport2", ParaTermG.svraddr_1.net.port, 0);

       IpToString(ParaTermG.svraddr_2.net.ip, str);
       XinWriteString(pf,"svraddrIp3", str);
       XinWriteInt(pf, "svrport3", ParaTermG.svraddr_2.net.port, 0); 

    sms_phone_tounnor((unsigned char *)str, ParaTermG.sms_addr);
    XinWriteHex(pf,"smsaddr", (unsigned char *)str, 8);

    /*by ydl add 2011-03-21*/
    IpToString(ParaTermG.gate_addr.net.ip, str);
       XinWriteString(pf,"gateaddr", str);
       XinWriteInt(pf, "gateport", ParaTermG.gate_addr.net.port, 0);

    XinWriteString(pf,"apn",ParaTermG.apn);


    XinWriteString( pf, "cdmausr", ParaTermG.cdma_usr);
    XinWriteString(pf, "cdmapwd", ParaTermG.cdma_pwd);

    XinWriteHex(pf, "norpwd", ParaTermG.nor_pwd,3);
    XinWriteHex(pf, "compwd", ParaTermG.com_pwd, 3);
    XinWriteHex(pf, "admpwd", ParaTermG.adm_pwd,3);

       XinWriteInt(pf,"search_device",ParaTermG.search_device,0);
       XinWriteInt(pf,"taskstarthour",ParaTermG.task_starthour,0);

       XinWriteInt(pf,"taskstartdev",ParaTermG.task_startdev,0);
       XinWriteInt(pf,"plcroutetype",ParaTermG.plc_route_type,0);
       XinWriteHex(pf,"gatelinewaste",ParaTermG.gate_linewaste,2);
       XinWriteInt(pf,"hourupimpdata",ParaTermG.hour_upimpdata,0);
       XinWriteInt(pf,"hourupdaydata",ParaTermG.hour_updaydata,0);
       XinWriteInt(pf,"alarmflag",ParaTermG.alarm_flag,0);


    unsigned char tmpbuf[16];
    Cascade_AddrChange(ParaTermG.cascade_addr, tmpbuf,1);
    XinWriteHex(pf, "cascadeaddr", tmpbuf, 16);
       //XinWriteHex(pf, "cascadeaddr", ParaTermG.cascade_addr, 16);
    XinWriteInt(pf,"cascadeflag",ParaTermG.cascade_flag,0);
    XinWriteInt(pf,"dayreadmondata",ParaTermG.day_read_mondata,0);
       XinWriteInt(pf,"hourreadmondata",ParaTermG.hour_read_mondata,0);
    XinWriteInt(pf,"dayupmondata",ParaTermG.day_up_mondata,0);
       XinWriteInt(pf,"hourupmondata",ParaTermG.hour_up_mondata,0);

    //XinWriteHex(pf,"smsaddr", ParaTermG.sms_addr,16);//短信中心号码

    Cascade_AddrChange(ParaTermG.peibian_addr,tmpbuf,0);
    XinWriteHex(pf,"peibian_addr",tmpbuf,4);
    XinWriteHex(pf, "ct", ParaTermG.ct,2);

    for(i=0;i<MAX_485PORT;i++){
    sprintf((char *)tmpbuf,"baud%d",i);
    XinWriteInt(pf,(char *)tmpbuf,ParaTermG.port[i].baud,0);
    sprintf((char *)tmpbuf,"databits%d",i);
    XinWriteInt(pf, (char *)tmpbuf,ParaTermG.port[i].databits, 0);//默认为8
    sprintf((char *)tmpbuf,"stopbits%d",i);
    XinWriteInt(pf, (char *)tmpbuf,ParaTermG.port[i].stopbits, 0);//默认为1
    sprintf((char *)tmpbuf,"parity%d",i);
    XinWriteInt(pf, (char *)tmpbuf, ParaTermG.port[i].parity,0);//校验位
    sprintf((char *)tmpbuf,"func%d",i);
    XinWriteInt(pf, (char *)tmpbuf, ParaTermG.port[i].func,0);//级联
    }

#if 0
    int i;
    for (i=0; i<PORTCFG_NUM; i++)
    {
        printf(str, "port_valid%d", i);
        XinWriteInt(pf, str, ParaTermG.port[i].valid, 0);

        printf(str, "port_frame%d", i);
        XinWriteInt(pf, str, ParaTermG.port[i].frame, 0);

        printf(str, "port_baud%d", i);
        XinWriteInt(pf, str, ParaTermG.port[i].baud, 0);

        printf(str, "port_func%d", i);
        XinWriteInt(pf, str, ParaTermG.port[i].func, 0);
    }
#endif
       XinClose(pf);

    /*end*/
mark_bakup:
    if(bakup) return 0;
    bakup = 1;
    pf = XinOpen(PARAM_BAK_PATH "/term.xin", 'w');
    if(NULL == pf)
    {
           PrintLog(0,"open bak fail... \n");
           return 1;
    }
    goto mark_save;
}

static void SetMstaSvrIp(cfg_svraddr_tG* des, char* res, int nLen)
{
     des->net.chn = res[0];
     des->net.unused = res[1];
     des->net.unused2 = res[2];
     des->net.ip[0] = res[3];
     des->net.ip[1] = res[4];
     des->net.ip[2] = res[5];
     des->net.ip[3] = res[6];
     char strPort[2];
     strPort[0] = res[8];  //反转后，高字节在前
     strPort[1] = res[7];
     des->net.port  = *(unsigned short*)strPort;
}

const char FormatHexChar1[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};
static void BcdToStr(unsigned char *dest, int destlen, unsigned char *scr, int scrlen)
{
    char *pstr=(char *)dest;
    int i=0;
    unsigned char uc;
    for(i=1; i<=scrlen; i++)
    {
        uc = *scr++;
        *pstr++ = FormatHexChar1[uc>>4];
        *pstr++ = FormatHexChar1[uc&0x0f];
     }
}






/*
*@brief 缓冲区反转
*param pstr 需要反转的缓冲区首地址
*param nlen 缓冲区有效字节的长度
*/  
//最多只能反127个字节
int  BuffRev(char* pstr,  int nLen)
{ 
        char str[128];
        if(nLen > 128)
           return 1;
        int nInx = nLen;
        int index = 0;
        for(; nInx >0; nInx--)
        {
            str[index++] = pstr[nInx-1];
        }
        memcpy(pstr,str,nLen);
        return 0;
}



