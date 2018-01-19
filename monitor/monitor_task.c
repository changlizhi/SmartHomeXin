/**
* monitor_task.c -- 监测模块任务
* 
* 作者: yangzhilong
* 创建时间: 2009-10-30
* 最后修改时间: 2009-10-30
*/
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>

#include <sys/types.h>  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>  
#include <unistd.h> 
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <poll.h>
#include <sys/stat.h>
#include "include/lib/datachg.h"

#include "include/basetype.h"
#include "include/debug/shellcmd.h"
#include "include/debug.h"
#include "include/param/term.h"
#include "include/sys/schedule.h"
#include "include/sys/gpio.h"
#include "include/sys/reset.h"
#include "include/sys/timeal.h"
#include "include/sys/timer.h"
#include "include/sys/task.h"
#include "include/sys/cycsave.h"
#include "include/sys/syslock.h"
#include "include/lib/bcd.h"
#include "downlink/plmdb.h"
#include "include/uplink/svrnote.h"
#include "../uplink/svrcomm.h"


/*返回str2第一次出现在str1中的位置(下表索引),不存在返回-1*/
int indexOf(char *str1,char *str2)
{
    char *p=str1;
    int i=0;
    p=strstr(str1,str2);
    if(p==NULL){
        return -1;
    }
    else{
        while(str1!=p)
        {
            str1++;
            i++;
        }
    }
    return i;
}

int post(char *ip,int port,char *page,char *msg,char *recvline){
    int sockfd,n;

    struct sockaddr_in servaddr;

    char content[4096];
    char content_page[50];
    sprintf(content_page,"POST /%s HTTP/1.1\r\n",page);
    char content_host[50];
    sprintf(content_host,"HOST: %s\r\n",ip);

    char content_type[] = "Content-Type: application/x-www-form-urlencoded\r\n";
    char content_len[50];
    sprintf(content_len,"Content-Length: %d\r\n\r\n",strlen(msg));
    sprintf(content,"%s%s%s%s%s",content_page,content_host,content_type,content_len,msg);
    printf("content---%s\n",content);

    if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        printf("sockfd2---%d\n",sockfd);
        printf("socket error\n");
        return -1;
    }

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if(inet_pton(AF_INET,ip,&servaddr.sin_addr) <= 0){
        printf("transfer ip err");
        printf("ip---%s\n",ip);
        return -1;
    }

    if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0){
        printf("connect error\n");
        return -1;
    }

    write(sockfd,content,strlen(content));
    n = read(sockfd,recvline,1024);
    printf("n---%d\n",n);

    int ind = indexOf(recvline,"config");
    printf("ind---%d\n",ind);

    if(n < 0){
        printf("read error\n");
        return -1;
    }
    return 0;
}
// 全局常量定义
const char * base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char padding_char = '=';

/*编码代码
* const unsigned char * sourcedata， 源数组
* char * base64 ，码字保存
*/

/** 在字符串中查询特定字符位置索引
* const char *str ，字符串
* char c，要查找的字符
*/
inline int num_strchr(const char *str, char c) //
{
    const char *pindex = strchr(str, c);
    if (NULL == pindex){
        return -1;
    }
    return pindex - str;
}
/* 解码
* const char * base64 码字
* unsigned char * dedata， 解码恢复的数据
*/
int base64_decode(const char * base64, unsigned char * dedata)
{
    int i = 0, j=0;
    int trans[4] = {0,0,0,0};
    for (;base64[i]!='\0';i+=4){
        // 每四个一组，译码成三个字符
        trans[0] = num_strchr(base64char, base64[i]);
        trans[1] = num_strchr(base64char, base64[i+1]);
        // 1/3
        dedata[j++] = ((trans[0] << 2) & 0xfc) | ((trans[1]>>4) & 0x03);

        if (base64[i+2] == '='){
            continue;
        }
        else{
            trans[2] = num_strchr(base64char, base64[i + 2]);
        }
        // 2/3
        dedata[j++] = ((trans[1] << 4) & 0xf0) | ((trans[2] >> 2) & 0x0f);

        if (base64[i + 3] == '='){
            continue;
        }
        else{
            trans[3] = num_strchr(base64char, base64[i + 3]);
        }

        // 3/3
        dedata[j++] = ((trans[2] << 6) & 0xc0) | (trans[3] & 0x3f);
    }

    dedata[j] = '\0';

    return 0;
}



#define IPSTR "10.0.0.104"
#define PORT 8989
#define BUFSIZE 1024

//int main(int argc, char **argv)
//char cilent_main();
char cilent_main(char buf[BUFSIZE]);

char cilent_main(char buf[BUFSIZE])
{
        int sockfd, ret, i, h;
        struct sockaddr_in servaddr;
        //char str1[4096], str2[4096], buf[BUFSIZE], *str;
        char str1[4096], str2[4096], *str;
        socklen_t len;
        fd_set   t_set1;
        char *toke;
        char *ps[1024];
        int t = 0;
        int j = 0;
        struct timeval  tv;


        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
                printf("创建网络连接失败,本线程即将终止---socket error!\n");
                exit(0);
        }

        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(PORT);
        if (inet_pton(AF_INET, IPSTR, &servaddr.sin_addr) <= 0 ){
                printf("创建网络连接失败,本线程即将终止--inet_pton error!\n");
                exit(0);
        };

        if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
                printf("连接到服务器失败,connect error!\n");
                exit(0);
        }
        printf("与远端建立了连接\n");
        memset(str2, 0, 4096);
        strcat(str2, "theDataToPost");
        str=(char *)malloc(128);
        len = strlen(str2);
        sprintf(str, "%d", len);

        memset(str1, 0, 4096);
        strcat(str1, "GET /mhsy_web/huoqulianjie?sn=1234 HTTP/1.1\n");
        strcat(str1, "Host:10.0.0.118\n");
 //       strcat(str1, "Host:IPSTR\n");
        strcat(str1, "Content-Type: text/html\n");
        strcat(str1, "Content-Length: ");
        strcat(str1, str);
        strcat(str1, "\n\n");
        printf("%s\n",str);

        strcat(str1, str2);
        strcat(str1, "\r\n\r\n");
        printf("%s\n",str1);

        printf("****************\n");
        ret = write(sockfd,str1,strlen(str1));
        if (ret < 0) {
                printf("发送失败！错误代码是%d，错误信息是'%s'\n",errno, strerror(errno));
                exit(0);
        }else{
                printf("消息发送成功，共发送了%d个字节！\n\n", ret);
        }
        printf("****************\n");
         printf("*****************%s\n",str1);
        FD_ZERO(&t_set1);
        FD_SET(sockfd, &t_set1);

        while(1){
                sleep(2);
                tv.tv_sec= 0;
                tv.tv_usec= 0;
                h= 0;
                printf("--------------->1\n");
                h= select(sockfd +1, &t_set1, NULL, NULL, &tv);
                printf("--------------->2\n");


                if (h < 0) {
                        close(sockfd);
                        printf("在读取数据报文时SELECT检测到异常，该异常导致线程终止！\n");
                        return -1;
                }

                if (h > 0){
                        memset(buf, 0, 1024);
                        i= read(sockfd, buf,1024 );
                        if (i<0){
                                close(sockfd);
                                printf("读取数据报文时发现远端关闭，该线程终止！\n");
                                return -1;
                        }
                }

                        printf("********1********\n");
                        printf("%s\n", buf);
                        return buf;
                        /*printf("*********1*******\n");

                        char *c = strtok(buf,"\n");
                        while(c)
                        {
                                ps[t++] = c;
                                c = strtok(NULL,"\n");

                        }
                        for(j =0 ;j<t;j++)
                        {
                            printf("%s\n",ps[j]);
                        }*/
        }
        close(sockfd);


//        return 0;
}



#include "include/monitor/alarm.h"
#include "downlink/plcomm.h"
//static int TimerIdPowerOff = -1;
#define USB_DISK   "/dev/sda1"
#define TMP_UDISK    TEMP_PATH"udisk.tmp" //监时文件
#define UDISKPARAM     "fdisk -l /dev/sd?" //检测U盘挂载盘符

#define TMP_SERIAL    TEMP_PATH"serial.tmp" //监时文件

unsigned char termtousb=0;//0:可以用U盘更新程序到终端,1从终端拷贝数据到U盘
unsigned char udiskname[96];//盘符名
static int USBLockId = -1;//资源锁
static int USBUartLockId = -1;//资源锁
int udisk = -1;//文件名//盘符名文件ID

extern unsigned char hard_info[18];
extern unsigned char     audiourl[512];

extern unsigned char       UpdateAudiourlFlag;

unsigned char wifi_down_musice_state=0;
unsigned char wifi_update_system_state=0;
/**
* @brief 校准系统时钟
*/

extern int  getuci(const char *varname, char *buffer,char *mingling);
extern int  setuci(const char *varname, char *buffer,char *mingling);
extern int  getuciConfigvar(const char *varname, char *buffer);
extern int  setuciConfigvar(const char *varname, char *buffer);
extern void MakeAlarmG(alarm_buf_t *pbuf);
extern void itoa(int i, char *str);

extern void SaveAlarm(alarm_buf_t *pbuf);
extern void UpdateAlarm(alarm_buf_t *pbuf);
extern alarm_buf_t * GetCurrentAlarm();
static int currentButtonState = -1; //0--为停止状态，1--为播放状态

static void SysClockCilibrate(void)
{
    sysclock_t clock;

    if(ReadExternSysClock(&clock)) return;

    SysClockSet(&clock);
}

#define CYCSAVE_TIMEOUT        (7*3500)    // 7 hour
#define CYCSAVE_FIRSTCNT        (3*3500)    // 3 hour
#define DBCLR_TIMEOUT            (16*3500)  // 16 hour
#define DBCLR_FIRSETCNT        (2*3500)   // 2 hour
#define CLKCHK_TIMEOUT            (5*3500)   // 5 hour

//播放音频文件 
//filename 文件名称
//type 文件类型1--音频治疗文件，0--语音提示文件
static int PlayVoice(const char * filename,int type)
{
    char cmd[128]={0};
    char dir[80] = {0};
    system("killall -9 madplay");
    system("killall -9 aplay");
    int filestat = 0;
    if(type == 1)
    {
        filestat = getFileDays();//判断音频文件是否有效
        PrintLog(0,"getFileDays %d",filestat);
        if(filestat !=0)
        {
            //音频文件失效，播放提示音
            sprintf(cmd,"aplay /tmp/mounts/SD-P1/voice/musicefileInvalid.wav  &");
            system(cmd);
            Sleep(1);
            //切换音频播放开关
            gpio_set_value(GPIO_39,1);
            gpio_set_value(GPIO_42,1);
            return -1;
        }
        else if(access("/tmp/mounts/SD-P1/play/shock.mp3",F_OK)==0)
        {
            //音频有效，则循环播放音频文件
            sprintf(cmd,"madplay /tmp/mounts/SD-P1/play/%s -r &",filename);
            system(cmd);
            Sleep(1);
            //切换音频播放开关
            gpio_set_value(GPIO_39,0);
            gpio_set_value(GPIO_42,0);
            return 0;
        }
        else
        {
            //无音频文件，播放提示音
            sprintf(cmd,"aplay /tmp/mounts/SD-P1/voice/musicefileInvalid.wav  &");
            system(cmd);
            Sleep(1);
            gpio_set_value(GPIO_39,1);
            gpio_set_value(GPIO_42,1);
            return -1;
        }



    }
    else if(type == 0)
    {
        //播放指定文件名的提示音
        sprintf(cmd,"aplay /tmp/mounts/SD-P1/voice/%s  &",filename);

        system(cmd);
        Sleep(1);
        gpio_set_value(GPIO_39,1);
        gpio_set_value(GPIO_42,1);
        return 0;

    }
}

/**
* @brief 监测任务
*/
extern void DbaseClear(void);
//更新音频播放记录和统计播放时长，存入文件中，在设备登录后用于上传工作参数
static void *UpdateAlarmTask_Monitor(void *arg)
{
    static int times = 0;
    while(1){//每100毫秒监测一次

        if(currentButtonState == 1)//如果是播放按键为按下
        {
            times++;
            if(times >= 6)
            {
                PrintLog(0,"UpdateAlarmTask_Monitor");
                UpdateAlarm(GetCurrentAlarm());//更新播放时间
                times = 0;
            }
        }
        else if(currentButtonState == 0 ||currentButtonState == -1)
        {
            times = 0;
        }
        Sleep(100);


        if(exitflag)
            break;
    }
    return 0;
}


static void *NetLedTask_Monitor(void *arg)
{
    static int lednet = 1;
    int  times = 0;
    while(1){
        if(UpdateAudiourlFlag == 1)
        {
            gpio_set_value(GPIO_LED_NET,1);
            Sleep(10);
            gpio_set_value(GPIO_LED_NET,0);
            Sleep(10);
        }
        else if(SvrCommLineState == LINESTAT_ON)
        {
            gpio_set_value(GPIO_LED_NET,0);
            Sleep(100);
        }
        else if(SvrCommLineState == LINESTAT_OFF)
        {
            times++;
            if(times>1000);
            {
                 gpio_set_value(GPIO_LED_NET,lednet);
                times = 0;
                if(lednet == 0)
                    lednet = 1;
                else
                    lednet = 0;
            }

        }
        Sleep(1000);
        if(exitflag)
            break;
    }
    return 0;
}

//下载音乐文件任务监测
static void *SysLedTask_Monitor(void *arg)
{
    while(1){
        gpio_set_value(GPIO_LED_SYS,1);
            Sleep(100);
        gpio_set_value(GPIO_LED_SYS,0);
        Sleep(100);
        if(exitflag)
            break;
    }
    return 0;
}

//下载音乐文件任务监测
static void *DownLoadMusicTask_Monitor(void *arg)
{
    char downloadcmd[512] = {0};
    struct    stat     buf;
    int     reseult;
    int     filestat = 0;
    int     firstDownTaskCheck = 1;
    while(1){//每10毫秒监测一次
        //如果音频更新为1或者第一次更新为1或者登录状态为1
        if(UpdateAudiourlFlag ||(1 == firstDownTaskCheck)||(1==ParaTermG.login_update_system)) //进入下载状态
        {
            reseult = stat("/tmp/mounts/SD-P1/music.zip",&buf);//检测压缩文件的状态

            PrintLog(0,"music.zip size:%d,create time:%s",buf.st_size,ctime(&buf.st_ctime));
            PrintLog(0," in downloadMusic Task stop play...\n");


             if(1 == firstDownTaskCheck)//如果是第一次下载，则说明还没有下载，此值有文件配置才对，否则一直会进入此条件
             {
                 wifi_down_musice_state = 1;
                PrintLog(0," in firstDownTaskCheck Task stop play...\n");
                system("wifi up");
                 memset(downloadcmd,0,512);
                sprintf(downloadcmd,"sh /opt/work/musicdownload.sh");
                system(downloadcmd);
                firstDownTaskCheck = 0;
                wifi_down_musice_state = 0;
             }
             if(1==ParaTermG.login_update_system)
             {
                 wifi_down_musice_state = 1;
                PrintLog(0," in firstDownTaskCheck Task stop play...\n");
                system("wifi up");
                 memset(downloadcmd,0,512);
                sprintf(downloadcmd,"sh /opt/work/musicdownload.sh");
                system(downloadcmd);
                ParaTermG.login_update_system = 0;
                wifi_down_musice_state = 0;
             }
             else
             {
                wifi_down_musice_state = 1;
                system("wifi up");
                PlayVoice("startdownload.wav",0);
                currentButtonState = 0;

                memset(downloadcmd,0,512);

                //base64解密之后得到url


                sprintf(downloadcmd,"sh /opt/work/musicdownload.sh %s",audiourl);
                system(downloadcmd);
                PlayVoice("enddownload.wav",0);
                system("wifi up");
                PrintLog(0,"downloadMusic Task Finshed...\n");

                wifi_down_musice_state = 0;

                UpdateAudiourlFlag = 0;
            }



        }
        Sleep(10);
        if(exitflag)
            break;
    }
    return 0;
}
//系统远程升级监测任务
static void *UpdateSystemTask_Monitor(void *arg)
{
    char systemcheckcmd[512] = {0};
    int  checktime = 600;
        while(1){
             if(checktime>600)//每10分钟必须更新一次
             {
                wifi_update_system_state = 1;
                 PrintLog(0," in UpdateSystemTask_Monitor Task stop play...\n");
                memset(systemcheckcmd,0,512);
                sprintf(systemcheckcmd,"sh /opt/work/ftup.sh");
                system(systemcheckcmd);
                wifi_update_system_state = 0;
                checktime=0;
             }
             if(1==ParaTermG.login_update_system)//扫描到系统更新标记时也要更新
             {
                wifi_update_system_state = 1;
                 PrintLog(0," in UpdateSystemTask_Monitor Task stop play...\n");
                memset(systemcheckcmd,0,512);
                sprintf(systemcheckcmd,"sh /opt/work/ftup.sh");
                system(systemcheckcmd);
                ParaTermG.login_update_system = 0;
                wifi_update_system_state = 0;//阻止wifi关闭
                checktime=0;//重新计时
             }
            Sleep(1000);
            checktime++;
            if(exitflag)//此值永远为0，是做什么用的呢？
                break;
        }
        return 0;

}


//监测播放按键按下事件
static void *PlayTask_Pressdown(void *arg)
{

    static int presstimes = 0;
    unsigned int value = 0;
    int  playstate = 0;
    gpio_export(GPIO_PLAY);
    gpio_set_dir(GPIO_PLAY, 0);
    gpio_set_edge(GPIO_PLAY, "rising");

    currentButtonState  = 0;
    PrintLog(0,"ParaTermG.first_start %d...\n",ParaTermG.first_start);
    if(ParaTermG.first_start)
    {
        PlayVoice("welcome.wav",0);

    }
    else
    {
        PlayVoice("welcome1.wav",0);
    }
    system("sh /opt/work/unzipmusic.sh");

    while (1) {

            gpio_get_value(GPIO_PLAY,&value);
            if(value == 0 )
            {
                presstimes++;


            }
            else if(value == 1)
            {
                if(presstimes>5)
                {

                    if(currentButtonState == 0)
                    {
                        if(UpdateAudiourlFlag == 0)
                        {

                            PlayVoice("startplay.wav",0);

                            Sleep(600);
                            playstate = PlayVoice("shock.mp3",1);
                            if(playstate == 0)
                            {
                                MakeAlarmG(GetCurrentAlarm());
                                currentButtonState = 1;
                                PrintLog(0,"play button press to start play...\n");
                                system("wifi up");
                                SvrCommLineState = LINESTAT_OFF;
                            }

                        }
                        else if(UpdateAudiourlFlag == 1)
                        {
                            PlayVoice("downloading.wav",0);
                        }

                    }
                    else if(currentButtonState == 1)
                    {
                        if(UpdateAudiourlFlag == 1)
                        {
                            PlayVoice("downloading.wav",0);
                        }
                        else if(UpdateAudiourlFlag == 0)
                        {
                            PrintLog(0,"play button press to stop play...\n");
                            system("killall -9 madplay");
                            PrintLog(0,"play button press to stop play1...\n");

                            SaveAlarm(GetCurrentAlarm());
                            gpio_set_value(GPIO_42,1);
                            gpio_set_value(GPIO_39,0);
                            //system("wifi up");
                            PrintLog(0,"play button press to stop play2...\n");

                            PlayVoice("stopplay.wav",0);
                            PrintLog(0,"play button press to stop play3...\n");

                            currentButtonState = 0;
                        }
                    }
                    presstimes = 0;
                }
            }
        Sleep(1);
    if(exitflag)
    {

        break;
    }
    }
    return 0;
}
static void wifitodnamy(){//仅供公司测试时使用，一旦wifi设置失败了，可以长按+号5秒进行公司的wifi链接
    sleep(600);
    char systemcheckcmd[512] = {0};
    memset(systemcheckcmd,0,512);
    sprintf(systemcheckcmd,"sh /www/cgi-bin/setwifi dnamy 2017dnamy");//测试控制器
    system(systemcheckcmd);
}

//监测音量按键按下事件
static void *VolumeBtn_Pressdown(void *arg)
{


    static int pressaddtimes = 0;
    static int presssubtimes = 0;
    static int currentVolume = 9;
    unsigned char VolumeLevel[10]={0};
    VolumeLevel[0] = 0;
    VolumeLevel[1] = 40;
    VolumeLevel[2] = 70;
    VolumeLevel[3] = 80;
    VolumeLevel[4] = 90;
    VolumeLevel[5] = 100;
    VolumeLevel[6] = 110;
    VolumeLevel[7] = 120;
    VolumeLevel[8] = 124;
    VolumeLevel[9] = 127;
    char   cmd[100];
    int gpio_fdadd;
    int gpio_fdsub;

    unsigned int value = 0;

    gpio_export(GPIO_KEY_ADD);
    gpio_set_dir(GPIO_KEY_ADD, 0);
    gpio_set_edge(GPIO_KEY_ADD, "rising");
    gpio_fdadd = gpio_fd_open(GPIO_KEY_ADD);

    gpio_export(GPIO_KEY_SUB);
    gpio_set_dir(GPIO_KEY_SUB, 0);
    gpio_set_edge(GPIO_KEY_SUB, "rising");
    gpio_fdsub = gpio_fd_open(GPIO_KEY_SUB);

    PrintLog(0,"play current volume is %d...\n",VolumeLevel[currentVolume]);

    while (1) {

            gpio_get_value(GPIO_KEY_ADD,&value);
//            if(value == 0 )
//            {
//                pressaddtimes++;
//                if(pressaddtimes >400 && presssubtimes<100)
//                {
//                    //if(currentButtonState == 0)
//                    {
//                        PlayVoice("enablewifi.wav",0);
//                        sprintf(cmd,"wifi up");
//                        system(cmd);
//                        wifitodnamy();//仅供测试使用
//                        pressaddtimes = 0;
//                    }
//                }
//
//            }
            if(value == 1)
            {
                if(presssubtimes>800 && pressaddtimes >800)
                {
                    PrintLog(0,"1 presssubtimes is %d...pressaddtimes is %d\n",presssubtimes,pressaddtimes);
                    sprintf(cmd,"uci set wireless.ap.encryption=\'none\'");
                    system(cmd);
                    sprintf(cmd,"uci delete wireless.ap.key");
                    system(cmd);
                    sprintf(cmd,"uci commit");
                    system(cmd);
                    sprintf(cmd,"uci -c/opt/ft set ftconfig.@ftconfig[0].firststart=1");
                    system(cmd);
                    sprintf(cmd,"uci -c/opt/ft commit");
                    sprintf(cmd,"wifi up");
                    system(cmd);
                    sprintf(cmd,"wifi up");
                    system(cmd);
                    PlayVoice("resetwifi.wav",0);
                    presssubtimes = 0;
                    pressaddtimes = 0;
                }
                if(pressaddtimes>5)
                {
                    if(currentVolume<9)
                        currentVolume++;

                    PrintLog(0,"play current volume is %d...\n",VolumeLevel[currentVolume]);
                    memset(cmd,0,100);
                    sprintf(cmd,"amixer cset numid=9,iface=MIXER,name=\'Headphone Playback Volume\' %d",VolumeLevel[currentVolume]);
                    system(cmd);
                    pressaddtimes = 0;
                }
            }

            gpio_get_value(GPIO_KEY_SUB,&value);
            if(value == 0 )
            {
                presssubtimes++;
                if(presssubtimes >400 &pressaddtimes <100)
                {
                    //if(currentButtonState == 0)
                    {

                    PlayVoice("disablewifi.wav",0);
                    sprintf(cmd,"wifi up");
                        system(cmd);
                        presssubtimes = 0;
                    }
                }


            }
            else if(value == 1)
            {
                if(presssubtimes>800 && pressaddtimes >800)
                {
                    PrintLog(0,"1 presssubtimes is %d...pressaddtimes is %d\n",presssubtimes,pressaddtimes);
                    sprintf(cmd,"uci set wireless.ap.encryption=\'none\'");
                    system(cmd);
                    sprintf(cmd,"uci delete wireless.ap.key");
                    system(cmd);
                    sprintf(cmd,"uci commit");
                    system(cmd);
                    sprintf(cmd,"uci -c/opt/ft set ftconfig.@ftconfig[0].firststart=1");
                    system(cmd);
                    sprintf(cmd,"uci -c/opt/ft commit");

                    sprintf(cmd,"wifi up");
                    system(cmd);
                    sprintf(cmd,"wifi up");
                    system(cmd);
                    PlayVoice("resetwifi.wav",0);
                    presssubtimes = 0;
                    pressaddtimes = 0;
                }
                if(presssubtimes>5)
                {

                    if(currentVolume>0)
                        currentVolume--;
                    PrintLog(0,"play current volume is %d...\n",VolumeLevel[currentVolume]);
                    memset(cmd,0,100);
                    sprintf(cmd,"amixer cset numid=9,iface=MIXER,name=\'Headphone Playback Volume\' %d",VolumeLevel[currentVolume]);
                    system(cmd);
                    presssubtimes = 0;
                }
            }

        Sleep(1);
    if(exitflag)
    {
      gpio_fd_close(gpio_fdadd);
      gpio_fd_close(gpio_fdsub);
        break;
    }
    }
    gpio_fd_close(gpio_fdadd);
    gpio_fd_close(gpio_fdsub);
    return 0;
}
static void trans(long sec){
	long hour,min,day;
	day=sec/3600*24;     //计算时 3600进制
	hour=sec/3600;     //计算时 3600进制
	min=(sec%3600)/60;   //计算分  60进制
	sec=(sec%3600)%60;   //计算秒  余下的全为秒数
	PrintLog(0,"shijian-----%ld Tian:%ld XIAOSHI:%ld FEN:%ld MIAO\n",day,hour,min,sec);
}

static void cishushixiao(){
    //startup.sh中启动时会执行一次
    char needstr[100]={""};

    char *canshu[]={
        "uci -c/opt/ft get ftconfig.@ftconfig[0].%s 2>&1",//0
        "uci -c/opt/ft get bofangcishu.@bofangcishu[0].%s 2>&1",//1
        "uci -c/opt/ft set bofangcishu.@bofangcishu[0].%s=%s",//2
        "0"//3
    };

    const char *varcishu="cishu";
    getuci(varcishu,needstr,canshu[1]);
    PrintLog(0,"cishu-----:%s\n",needstr);

    char cmd[512] = {0};
    memset(cmd,0,512);
    int cs = atoi(needstr);
    if(cs > 3 && cs <= 5){//测试用3
        sprintf(cmd,"aplay /tmp/mounts/SD-P1/voice/5.wav  &");
        system(cmd);
        Sleep(50);
        gpio_set_value(GPIO_39,1);
        gpio_set_value(GPIO_42,1);
        Sleep(1000);//等待10秒读文字
    }
    if(cs > 5){//如果播放次数大于130则删除音频，测试用5次
        PrintLog(0,"shanchu shock.mp3\n");
        sprintf(cmd,"rm -rf /tmp/mounts/SD-P1/play/shock.mp3");
        system(cmd);
        Sleep(50);
    }
}

static void *Yinpinguoqi(void *arg){
    char *lujing = "/tmp/mounts/SD-P1/play/shock.mp3";
    while(1){
        uciuse();
        PrintLog(0,"lujing-----%s\n",lujing);
        time_t t;
        t=time(0);//当前时间秒数

        PrintLog(0,"dangqian shijian-----%ld\n",t);
        trans(t);
        struct stat buf;
        int result;
        //获得文件状态信息
        result =stat(lujing, &buf );
        //显示文件状态信息
        if( result != 0 ){
            PrintLog(0,"wenjian chucuo" );//并提示出错的原因，如No such file or directory（无此文件或索引）
        }
        else
        {
            PrintLog(0,"wenjian daxiao: %d\n", buf.st_size);
            PrintLog(0,"chuangjian shijian : %s\n", ctime(&buf.st_ctime));
            PrintLog(0,"fangwen shijian : %s\n", ctime(&buf.st_atime));
            PrintLog(0,"xiugai shijian: %s\n", ctime(&buf.st_mtime));
        }

        Sleep(600);
    }
}

static int Kaishizhendong()
{
    char cmd[512] = {0};
    memset(cmd,0,512);
    //音频有效，则循环播放音频文件
    sprintf(cmd,"ash /opt/work/zengjiacishu.sh");//播放前增加一次次数
    system(cmd);
    Sleep(50);
    sprintf(cmd,"madplay /tmp/mounts/SD-P1/play/shock.mp3 -r &");
    system(cmd);
    Sleep(50);
    //切换音频播放开关
    gpio_set_value(GPIO_39,0);
    gpio_set_value(GPIO_42,0);
    currentButtonState = 1;
    return 0;
}
static int Bofangcishu()
{
    char cmd[512] = {0};
    memset(cmd,0,512);
    //音频有效，则循环播放音频文件
    sprintf(cmd,"madplay /tmp/mounts/SD-P1/play/shock.mp3 -r &");
    system(cmd);
    Sleep(50);
    //切换音频播放开关
    gpio_set_value(GPIO_39,0);
    gpio_set_value(GPIO_42,0);
    currentButtonState = 1;
    return 0;
}
//更新音频播放记录和统计播放时长，存入文件中，在设备登录后用于上传工作参数
static void *GengxinBofangShijian(void *arg)
{
    while(1){
        int bofang = (currentButtonState == 1);
        if(bofang)//如果是播放状态
        {
            time_t t;
            t=time(0);//当前时间秒数
            char dangqianshijian[100]={0};
            sprintf(dangqianshijian,"%ld",t);
            PrintLog(0,"dangqianshijian---shuzi:%ld,dangqianshijian---zifu:%s\n",t,dangqianshijian);

            char needstr[100];

            const char *snvarname="sn";
            getuciConfigvar(snvarname,needstr);
            PrintLog(0,"sn---needstr---:%s\n",needstr);

            //这里如果服务端发起更新命令或者初次开机播放，初始值为11111就需要更新一次
            const char * kaishishijian="kaishishijian";
            getuciConfigvar(kaishishijian,needstr);

            int kaishile = (strcmp(needstr, "11111") == 0);
            PrintLog(0,"needstr111---:%s\n",needstr);
            PrintLog(0,"kaishile---:%d\n",kaishile);

            if (kaishile){
                setuciConfigvar(kaishishijian,dangqianshijian);
                PrintLog(0,"huoqukaishishijian---needstr---:%s\n",needstr);
            }
            //每1分钟更新一次，没有其他条件
            const char * jieshushijian="jieshushijian";
            setuciConfigvar(jieshushijian,dangqianshijian);
            getuciConfigvar(jieshushijian,needstr);
            PrintLog(0,"huoqujieshushijian---needstr---:%s\n",needstr);

            sprintf(dangqianshijian,"uci -c/opt/ft commit");
            system(dangqianshijian);
            Sleep(10);
            sprintf(dangqianshijian,"ash /opt/work/scbofangshijian.sh");//进行上传
            system(dangqianshijian);
        }
        Sleep(600);//每6秒监测一次
    }
    return 0;
}


static void *BofangYinpin(void *arg)
{
    gpio_export(GPIO_PLAY);
    gpio_set_dir(GPIO_PLAY, 0);
    gpio_set_edge(GPIO_PLAY, "rising");
    char cmd[512] = {0};
    memset(cmd,0,512);
    cishushixiao();//在播放之前进行检测是否达到播放次数，达到则删除音频
    Sleep(600);
    while (1) {
        char *lujing = "/tmp/mounts/SD-P1/play/shock.mp3";
        if(access(lujing,F_OK)==0){
            sprintf(cmd,"aplay /tmp/mounts/SD-P1/voice/2.wav  &");
            system(cmd);
            Sleep(600);
            gpio_set_value(GPIO_39,1);
            gpio_set_value(GPIO_42,1);
            Sleep(600);
            Kaishizhendong();
            break;
        }
        else
        {
            //无音频文件，播放提示音
            sprintf(cmd,"aplay /tmp/mounts/SD-P1/voice/6.wav &");
            system(cmd);
            Sleep(600);
            gpio_set_value(GPIO_39,1);
            gpio_set_value(GPIO_42,1);
        }
        Sleep(600);
    }
    return 0;
}


static void *Bofangzanting(void *arg)
{
    static int presstimes = 0;
    unsigned int value = 0;
    gpio_export(GPIO_PLAY);
    gpio_set_dir(GPIO_PLAY, 0);
    gpio_set_edge(GPIO_PLAY, "rising");

    currentButtonState  = 0;
    while (1) {
        gpio_get_value(GPIO_PLAY,&value);
        if(value == 0 )
        {
            presstimes++;
        }
        else if(value == 1)
        {
            if(presstimes>5)
            {

                PrintLog(0,"currentButtonState=======%d \n",currentButtonState);
                if(currentButtonState == 0)
                {
                    Kaishizhendong();
                    PrintLog(0,"bofangzhong jilu daxiao...\n");
                }
                else if(currentButtonState == 1)
                {
                    PrintLog(0,"guanbi yinpin...\n");
                    system("killall -9 madplay");
                    PrintLog(0,"guanbi yinpin chenggong...\n");
                    currentButtonState = 0;
                }
                presstimes = 0;
            }
        }
        Sleep(1);
        if(exitflag)
        {
            break;
        }
    }
    return 0;
}

//监测音量按键按下事件
static void *Yinliangzengjian(void *arg)
{
    static int pressaddtimes = 0;
    static int presssubtimes = 0;
    static int currentVolume = 9;
    unsigned char VolumeLevel[10]={0};
    VolumeLevel[0] = 0;
    VolumeLevel[1] = 40;
    VolumeLevel[2] = 70;
    VolumeLevel[3] = 80;
    VolumeLevel[4] = 90;
    VolumeLevel[5] = 100;
    VolumeLevel[6] = 110;
    VolumeLevel[7] = 120;
    VolumeLevel[8] = 124;
    VolumeLevel[9] = 127;
    char   cmd[100];
    int gpio_fdadd;
    int gpio_fdsub;

    unsigned int value = 0;

    gpio_export(GPIO_KEY_ADD);
    gpio_set_dir(GPIO_KEY_ADD, 0);
    gpio_set_edge(GPIO_KEY_ADD, "rising");
    gpio_fdadd = gpio_fd_open(GPIO_KEY_ADD);

    gpio_export(GPIO_KEY_SUB);
    gpio_set_dir(GPIO_KEY_SUB, 0);
    gpio_set_edge(GPIO_KEY_SUB, "rising");
    gpio_fdsub = gpio_fd_open(GPIO_KEY_SUB);

    PrintLog(0,"dangqian yinliang is %d...\n",VolumeLevel[currentVolume]);

    while (1) {
        gpio_get_value(GPIO_KEY_ADD,&value);
        if(value == 0 )//按下
        {
            pressaddtimes++;
        }
        else if(value == 1)
        {
            if(pressaddtimes>5)//消抖
            {
                if(currentVolume<9)
                    currentVolume++;

                PrintLog(0,"vol++ dangqian yinliang %d...\n",VolumeLevel[currentVolume]);
                memset(cmd,0,100);
                sprintf(cmd,"amixer cset numid=9,iface=MIXER,name=\'Headphone Playback Volume\' %d",VolumeLevel[currentVolume]);
                system(cmd);
                pressaddtimes = 0;
            }
        }

        gpio_get_value(GPIO_KEY_SUB,&value);
        if(value == 0 )//按下
        {
            presssubtimes++;
        }
        else if(value == 1)
        {
            if(presssubtimes > 5)//消抖
            {

                if(currentVolume > 3)//最小音量不能小于3，否则振动就非常小了
                    currentVolume--;
                PrintLog(0,"vol-- dangqian yinliang %d...\n",VolumeLevel[currentVolume]);
                memset(cmd,0,100);
                sprintf(cmd,"amixer cset numid=9,iface=MIXER,name=\'Headphone Playback Volume\' %d",VolumeLevel[currentVolume]);
                system(cmd);
                presssubtimes = 0;
            }
        }
        Sleep(1);
        if(exitflag)
        {
          gpio_fd_close(gpio_fdadd);
          gpio_fd_close(gpio_fdsub);
          break;
        }
    }
    gpio_fd_close(gpio_fdadd);
    gpio_fd_close(gpio_fdsub);
    return 0;
}
static void *Chmodzhixing(void *arg){
    char cmd[512] = {0};
    memset(cmd,0,512);
    sprintf(cmd,"chmod +x /www/cgi-bin/*");
    system(cmd);
    Sleep(15);
    sprintf(cmd,"chmod +x /opt/work/*");
    system(cmd);
    Sleep(15);
}

static void *XiazaiYinpin(void *arg){

}

static void *ShezhiSn(void *arg){
    while(1){
        char cmd[512] = {0};
        memset(cmd,0,512);
        sprintf(cmd,"ash /opt/work/macdizhi.sh");
        system(cmd);
        Sleep(600);
    }
}
static void *Gengxinmima(void *arg){
    while(1){
        char cmd[512] = {0};
        memset(cmd,0,512);
        sprintf(cmd,"ash /opt/work/gengxinmima.sh");
        system(cmd);
        Sleep(600);
    }
}

//识别网络是否成功然后上传
static void *ShangchuanShuju(void *arg){
    //识别网络是否成功然后上传
    while(1){
        PrintLog(0,"kaishi qingqiu lianwang!!!\n");
        Sleep(1200);
        char msg[] = "Ceshilianwang=ceshi";
        char ip[] = "kzq.amchis.com";
        int port = 8989;
        char page[] = "sn/ceshilianwang";
        char recvline[1024];
        int cg = post(ip,port,page,msg,recvline);
        if(cg == 0){
            PrintLog(0,"recvline---%s\n",recvline);
            char cmd[512] = {0};
            memset(cmd,0,512);
            sprintf(cmd,"ash /opt/work/shangchuansn.sh");
            system(cmd);
            Sleep(15);

            sprintf(cmd,"ash /opt/work/shangchuanshijian.sh");
            system(cmd);
            Sleep(15);
        }
    }
}
//还要加一个mtd功能执行控制器初始化以免控制器无法联网255.255.0.0 TODO
static void *Chongxinshaolu(void *arg){
    while(1){//每两分钟监测一次网络是否可达服务器，如果可执行则
        char cmd[512] = {0};
        memset(cmd,0,512);
        sprintf(cmd,"ash /opt/work/chongxinshaolu.sh");
        system(cmd);
        Sleep(600);


    }
}
//增加一个12小时重启功能，很多数据需要启动时执行，比如删除，比如更新，
//使用着的时候不会操作，但记录了很多数据，重启时读取那些数据完成功能，最不济就免费用一天而已
static void *Chongqi(void *arg){
    int x=0;
    while(1){
        if(x > 60000){//十分钟重启
            char cmd[512] = {0};
            memset(cmd,0,512);
            sprintf(cmd,"reboot");
            system(cmd);
            break;
        }
        x += 100;
        PrintLog(0,"jishu---x:%d\n",x);//每秒打印一次计数
        Sleep(100);//线程睡眠1秒
    }
}

DECLARE_INIT_FUNC(MonitorTaskInit);
int MonitorTaskInit(void)
{

    RunStateInit();
    currentButtonState=0;
//    SysCreateTask(PlayTask_Pressdown, NULL);//音频播放键按下时任务
    SysCreateTask(Chmodzhixing, NULL);//播放音频的任务
    SysCreateTask(BofangYinpin, NULL);//播放音频的任务
    SysCreateTask(Bofangzanting, NULL);//播放暂停功能
    SysCreateTask(Yinliangzengjian, NULL);//音量增减功能
    SysCreateTask(Chongqi, NULL);//重新启动的任务
//    SysCreateTask(Yinpinguoqi, NULL);//设置音频过期，等时钟芯片做好之后再做
    SysCreateTask(GengxinBofangShijian, NULL);//播放音频的任务
    SysCreateTask(ShezhiSn, NULL);//设置sn
    SysCreateTask(Gengxinmima, NULL);//更新密码
//    SysCreateTask(ShangchuanShuju, NULL);//上传sn的功能
//    SysCreateTask(XiazaiYinpin, NULL);//上传sn的功能
//    AlarmInit();//初始化时间文件alm不要了，用uci 来set
//    SysCreateTask(UpdateSystemTask_Monitor, NULL);//系统更新任务
//    SysCreateTask(UpdateAlarmTask_Monitor, NULL);//更新播放时间
//    SysCreateTask(DownLoadMusicTask_Monitor, NULL);//音乐下载，内部有协议通信方法
//    SysCreateTask(NetLedTask_Monitor, NULL);
//    SysCreateTask(SysLedTask_Monitor, NULL);
//    SysCreateTask(VolumeBtn_Pressdown, NULL);
    SET_INIT_FLAG(MonitorTaskInit);
    return 0;
}


