/**
* uplink645.c -- 645通信
* 
* 作者: chengrongtao
* 创建时间: 2010-5-28
* 最后修改时间: 2010-5-28
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/debug.h"
#include "include/lib/bcd.h"
#include "include/sys/schedule.h"
#include "include/sys/task.h"
#include "include/sys/rs485.h"
#include "save/param.h"
#include "include/param/commport.h"
#include "include/param/term.h"

#include "include/debug.h"
#include "include/sys/timeal.h"
#include "qrcode.h"
#include "include/sys/uart.h"
#include "include/debug/shellcmd.h"
#include "include/lib/align.h"
#include "plmdb.h"
#include "uplink/svrcomm.h"
#include "include/uplink/svrnote.h"
#include "include/monitor/alarm.h"
#include "include/sys/gpio.h"


#define RS485II    0

typedef struct {
    unsigned char     reader[2];
    char             usercode[12];
    sysclock_t        starttime;
    sysclock_t        endtime;
    char            code[7];
    int              door[4];
}qrcode_pkt_t;//QRCODE 报文格式

extern void MakeAlarmG(const alarm_buf_t *pbuf);

unsigned char ShiftMM[20]= {3,14,20,50,30,2,44,1,32,38,3,46,37,8,41,45,33,31,43,44}; //案例移位密码
unsigned char SanMessageOut[66]={0};
qrcode_pkt_t  qrcodedata;
extern unsigned char SceneOnId;//开启当前情景

extern unsigned char MusicPause;//音乐暂停标志
extern unsigned char MusicPauseTmeout;//音乐暂停标志

int  Decrypt(unsigned char *bufSrc,unsigned int Len,unsigned char *SanMessageOut)
{
  unsigned char i, b,a,j,e,f,n;
  unsigned char TempMessage[66]={0};
  for( i=0;i< Len; i++)
   {
    if(bufSrc[i]=='U')                                 //去掉‘u’
    {
      memcpy(SanMessageOut,bufSrc+i+1,58);
      break;
    }
   }
   printf("Decrypt:%s",SanMessageOut);
   if(i==Len)
       return -1;
  //移位运算20次     把第a位数据移到第b位
    for(i=20 ; i>0 ; i-- )   //倒推算法
        {
       memcpy(TempMessage ,SanMessageOut,58) ;                //每次循环完要将SanMessageOut付给TempMessage

        //倒推算法
        b=i-1;                    //  第20数据        //第b位数据
        a = ShiftMM[b];       //取移位密码解密  17  // 第a位数据
        //将第17位数据移位到第20-21之间
        if(a<b)
         {
            e=0;
            f=0;
            n=TempMessage[a];
            for( j=0;j< 65; j++ )
            {
               if(j==a)                        //  第17个数据
                {
                  f= f+1  ;                   //   把18位继续赋值
                }
               if(j==b )                       //将第20位的数据等于第17位的
                {
                   SanMessageOut[e++]= n;
                    continue;
                }
                  SanMessageOut[e++]=TempMessage[f++];
            }
          }
          else if(a>b)
          {
            e=0;
            f=0;
            n=TempMessage[a-1];
            for( j=0;j< 65; j++ )
            {
               if(j==b)                       //将第20位的数据等于第17位的
                {
                   SanMessageOut[e++]= n;
                    continue;
                }
               if(j==a)                        //  第17个数据
                {
                   f= f+1  ;                   //   把18位继续赋值
                 }
                SanMessageOut[e++]=TempMessage[f++];
             }
            }
            else if(a==b)
             {
                 e=0;
                 f=0;
                 for( j=0;j< 67; j++ )
                 {
                   SanMessageOut[e++]=TempMessage[f++];
                 }
              }
    }
    for(j=0;j<58;j++)
    {
       printf("%c",SanMessageOut[j]);
    }
    return 0;
}
int checkqrcodedata()
{
    int  index = 0;

    memset(qrcodedata.reader,0,sizeof(qrcode_pkt_t));
    qrcodedata.usercode[0] = 'U';
    memcpy(&qrcodedata.usercode[1],&SanMessageOut[0],11);
    for(index = 11;index<58;index++)
        SanMessageOut[index]=SanMessageOut[index]-0x30;

    qrcodedata.starttime.year = SanMessageOut[13]*10;
    qrcodedata.starttime.year += SanMessageOut[14];
    qrcodedata.starttime.month = SanMessageOut[15]*10;
    qrcodedata.starttime.month += SanMessageOut[16];
    qrcodedata.starttime.day  = SanMessageOut[17]*10;
    qrcodedata.starttime.day += SanMessageOut[18];
    qrcodedata.starttime.hour = SanMessageOut[19]*10;
    qrcodedata.starttime.hour += SanMessageOut[20];
    qrcodedata.starttime.minute= SanMessageOut[21]*10;
    qrcodedata.starttime.minute += SanMessageOut[22];
    qrcodedata.starttime.second= SanMessageOut[23]*10;
    qrcodedata.starttime.second += SanMessageOut[24];

    qrcodedata.endtime.year = SanMessageOut[27]*10;
    qrcodedata.endtime.year += SanMessageOut[28];
    qrcodedata.endtime.month = SanMessageOut[29]*10;
    qrcodedata.endtime.month += SanMessageOut[30];
    qrcodedata.endtime.day  = SanMessageOut[31]*10;
    qrcodedata.endtime.day += SanMessageOut[32];
    qrcodedata.endtime.hour = SanMessageOut[33]*10;
    qrcodedata.endtime.hour += SanMessageOut[34];
    qrcodedata.endtime.minute= SanMessageOut[35]*10;
    qrcodedata.endtime.minute += SanMessageOut[36];
    qrcodedata.endtime.second= SanMessageOut[37]*10;
    qrcodedata.endtime.second += SanMessageOut[38];

    memcpy(qrcodedata.code,&SanMessageOut[39],7);


    qrcodedata.door[0] = SanMessageOut[46]*100;
    qrcodedata.door[0] += SanMessageOut[47]*10;
    qrcodedata.door[0] += SanMessageOut[48];

    qrcodedata.door[1] = SanMessageOut[49]*100;
    qrcodedata.door[1] += SanMessageOut[50]*10;
    qrcodedata.door[1] += SanMessageOut[51];

    qrcodedata.door[2] = SanMessageOut[52]*100;
    qrcodedata.door[2] += SanMessageOut[53]*10;
    qrcodedata.door[2] += SanMessageOut[54];

    qrcodedata.door[3] = SanMessageOut[55]*100;
    qrcodedata.door[3] += SanMessageOut[56]*10;
    qrcodedata.door[3] += SanMessageOut[57];




    sysclock_t  currenttime;
    SysClockReadCurrent(&currenttime);

    int timespan = SysClockDifference(&currenttime,&qrcodedata.endtime);
    int spanyear = currenttime.year - qrcodedata.endtime.year;
    if(spanyear>2 || spanyear < -2)
       system("hwclock -s");
    PrintHexLog(0,qrcodedata.code,7);

    if(memcmp(qrcodedata.code,ParaTermG.deviceid,7) == 0)
    {
        PrintLog(0,"本网点二维码\n");

    }
    else
    {
        PrintHexLog(0,qrcodedata.code,7);

        PrintLog(0,"非本网点二维码 %s,%s\n",qrcodedata.code,ParaTermG.deviceid);
        return ERROR_QRCODEUNKNOWN;
    }
    if(timespan>0)
    {
        return ERROR_QRCODETIMEOUT;
    }
    return 0;
}

int OpenDoor()
{
    int  index = 0;
    int  tmpindex=0;
    int  tmpdoor[4] = {0};
    int  devnum = 0;

    if(qrcodedata.reader[0] == 'G' && qrcodedata.reader[1] == 'M')
    {
        tmpdoor[0] = qrcodedata.door[0];
        tmpdoor[1] = qrcodedata.door[1];
        tmpdoor[2] = qrcodedata.door[2];
        tmpdoor[3] = qrcodedata.door[3];

        devnum = 0;
        for(index = 0;index<4;index++)
        {
            if(tmpdoor[index]!=0)
             {
                 ctrlstate[devnum].addr = tmpdoor[index];
                ctrlstate[devnum].runstate[0] = 0x88;

                devnum++;

             }

        }
        printf("ctrlnum = %d ,door[0]:%02x",devnum,ctrlstate[0].runstate[0]);

        ctrlnum = devnum;
    }
    else if(qrcodedata.reader[0] == 'J' && qrcodedata.reader[1] == 'M')
    {
        PrintLog(0,"开仓门。。\n");
        SceneOnId = 1;
        gpio_set_value(GPIO_DOOR,1);
        Sleep(300);
        gpio_set_value(GPIO_DOOR,0);

    }
    else if(qrcodedata.reader[0] == 'C' && qrcodedata.reader[1] == 'M')
    {
        PrintLog(0,"开仓门。。\n");
        gpio_set_value(GPIO_DOOR,1);
        Sleep(300);
        gpio_set_value(GPIO_DOOR,0);
    }
    return 0;
}

int PlayVoice(int code)
{
    char  Cmd[50] = {0};
    if(code == ERROR_QRCODETIMEOUT)
    {
        sprintf(Cmd,"%c%cspeek timeout",qrcodedata.reader[0],qrcodedata.reader[1]);
        MusicPauseTmeout = 10;

    }else if(ERROR_QRCODEUNKNOWN == code)
    {
        sprintf(Cmd,"%c%cspeek failed",qrcodedata.reader[0],qrcodedata.reader[1]);
        MusicPauseTmeout = 6;
    }
    else if(0 == code)
    {
        sprintf(Cmd,"%c%cspeek resultok",qrcodedata.reader[0],qrcodedata.reader[1]);
        MusicPauseTmeout = 6;
    }
    printf("%c, %s\n",qrcodedata.reader[0],Cmd);
    Rs485Send(0,Cmd,strlen(Cmd));
    return 0;
}
//检查二维码的合法性
int qrcodeValidity(unsigned char *qrcode)
{

    printf("SanMessageOut3:%s\n",qrcode);
    if(Decrypt(qrcode,66,SanMessageOut)==0) //解密成功
    {
        return checkqrcodedata();
    }
    else
        return -1;
}


/**
* @brief 服务器命令处理
* @param itf 接口编号
*/
void SvrMessageQrCode(unsigned char itf)
{

    return;
}


/**
* @brief 645任务--以集中器为表
*/
void *QrCodeTask(void *arg)
{

        unsigned char qrcodebuf[512]={0};

        int  recvlen = 0;
        int  recvindex = 0;
        memset(qrcodebuf, 0, 512);
        int  ret = 0;
        Sleep(100);

        while(1) {

            while(1)
            {
                recvindex = Rs485Recv(RS485II, &qrcodebuf[recvlen], 512);
                if(recvindex>0)
                  recvlen+=recvindex;
                else
                   break;
                Sleep(10);
            }
            if(recvlen > 50)  //收到二维码命令
            {

                if(qrcodebuf[0]=='G'&&qrcodebuf[1] == 'M') //柜门
                {
                    PrintLog(0,"柜门读头扫描到二维码\n");
                    ret = qrcodeValidity(qrcodebuf);
                    qrcodedata.reader[0] = 'G';
                    qrcodedata.reader[1] = 'M';
                    MusicPause = 1;
                    setvolume(0);
                    PlayVoice(ret);
                    if(ret == 0)
                        OpenDoor();


                }
                else if(qrcodebuf[0]=='J'&&qrcodebuf[1] == 'M') //柜门
                {
                    PrintLog(0,"仓体外读头扫描到二维码\n");
                    ret = qrcodeValidity(qrcodebuf);
                    qrcodedata.reader[0] = 'J';
                    qrcodedata.reader[1] = 'M';

                    if(ret == 0)
                        OpenDoor();
                    PlayVoice(ret);
                }
                else if(qrcodebuf[0]=='C'&&qrcodebuf[1] == 'M') //柜门
                {
                    PrintLog(0,"出仓体读头扫描到二维码\n");
                    ret = qrcodeValidity(qrcodebuf);
                    qrcodedata.reader[0] = 'C';
                    qrcodedata.reader[1] = 'M';
                    setvolume(0);
                    MusicPause = 1;
                    if(ret == 0)
                        OpenDoor();
                    PlayVoice(ret);
                }
                recvlen = 0;
            }
            else if(recvlen>0)
            {
                printf("SanMessageOut2:%s\n",qrcodebuf);
                if(strcmp("JMTAOTAOADMIN2350",qrcodebuf) == 0)
                  {
                      SceneOnId = 1;
                    gpio_set_value(GPIO_DOOR,1);
                    Sleep(300);
                    gpio_set_value(GPIO_DOOR,0);
                  }
                else if(strcmp("CMTAOTAOADMIN2350",qrcodebuf) == 0)
                 {
                     gpio_set_value(GPIO_DOOR,1);
                    Sleep(300);
                    gpio_set_value(GPIO_DOOR,0);
                 }
                recvlen = 0;
            }
            memset(qrcodebuf, 0, 512);
            if(exitflag)
              break;
            Sleep(10);
        }
    return 0;

}

/**
* @brief 通过读取二维码读头数据
* @return 成功0, 否则失败
*/
DECLARE_INIT_FUNC(QrCode);
int QrCode(void)
{

    SysCreateTask(QrCodeTask, NULL);
    SET_INIT_FLAG(QrCode);
    return 0;
}

