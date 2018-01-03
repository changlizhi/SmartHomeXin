
#include "include/basetype.h"
#include "include/debug.h"
#include "include/sys/schedule.h"
#include "include/lib/datachg.h"
#include "uplink/uplink_pkt.h"
#include "uplink/uplink_dl.h"
#include "uplink/svrcomm.h"
#include "include/param/term.h"


int faal_sendpkt(unsigned char itf, faalpkt_t *pkt)
{
    int len;

//    DebugPrint(LOGTYPE_SHORT, "sendpkt  itf=%d,svrcomm_itf=%d\n", itf, SvrCommInterface);
    len = faal_makepkt(itf, pkt);
    if(-1 == len) return 1;
#if 0 
    if((itf == svrcomm_itf) && (para_uni.bcompress) && (len > 32))
    {
        int ziplen;

    /*    ziplen = EnData((unsigned char *)pkt, len, EXE_COMPRESS_NEW);
        if(ziplen < len)
        {
            if(FAAL_NEEDPRINT(itf)) faal_print_zippkt((faal_zippkt_t *)SendBuf, "SEND ZIP");

            if((*g_faalitf[itf].rawsend)(SendBuf, ziplen)) return 1;
            else return 0;
        }*/
    }

#endif

    if(UPLINK_NEEDPRINT(itf)) faal_printpkt(pkt, "SEND:");
     if((*UplinkInterface[itf].rawsend)((unsigned char *)pkt, len))
     {
        return 1;
     }
    return 0;
}

/*
@func:帧拆分发送
@info:by ydl add 2011-03-21
*/
int faal_mulfram_sendpkt(unsigned char itf, faalpkt_t *pkt, unsigned char frameid)
{
    int len;
    int iseq;
    unsigned short tmp;
    /*设置帧内序号*/

    len = faal_makepkt(itf, pkt);
    if(-1 == len) return 1;

    if(UPLINK_NEEDPRINT(itf)) faal_printpkt(pkt, "SEND");

     if((*UplinkInterface[itf].rawsend)((unsigned char *)pkt, len))
     {

        return 1;
    }


    /*非单帧或尾帧延时100ms*/
    if (frameid != 0 &&  frameid != 7)
    {
        Sleep(20);//
    }
    return 0;
}

int faal_makepkt(unsigned char itf, faalpkt_t *pkt)
{
    int i, len;
    unsigned char *puc;
    unsigned char chk;
    unsigned short crc=0;
    len = (int)makepkt_short(pkt->len);
    len &= 0xffff;
    if(len > FAAL_SNDMAX(itf)) return(-1);

    pkt->head = pkt->dep = FAAL_HEAD;
    pkt->ver = 0x01;
    for(i=0;i<4;i++)
        pkt->addr[i] = ParaTermG.deviceid[i*2]<<4 | ParaTermG.deviceid[i*2+1];
    pkt->cmd |= FAALMASK_DIR;

    len += LEN_FAAL_HEAD;
    puc = (unsigned char *)pkt;
    chk = 0;

    crc = GetCRC16(pkt,len);
    *puc++ = crc>>8;
    *puc++ = crc;
    *puc = FAAL_TAIL;
    len += 3;

    return(len);
}

extern int GetLogType(void);
void faal_printpkt(faalpkt_t *pkt, char *prompt)
{
    unsigned short len;
    int logolvl;

    logolvl = GetLogType();
    //if(logolvl < LOGTYPE_SHORT) return;
    len = makepkt_short(pkt->len);
    PrintLog(0, "%s: cmd=%02XH, len=%d.\r\n", prompt, pkt->cmd, len);
    //if(logolvl != LOGTYPE_UPLINK) return;
    len += LEN_FAAL_HEAD+2;
    print_charbuf((unsigned char *)pkt, len);
    return;
}



