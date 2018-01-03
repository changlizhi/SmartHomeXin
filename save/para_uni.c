#include <string.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <errno.h>

#include "itemop.h"
#include "para_uni.h"
#include "include/param/unique.h"

/*
Func Description: 终端以太网IP
Func Param:
    flag: 0-读, 1-写
    op: 操作定义指针
Func Return: ITORTN_*
*/
int parauni_termip(unsigned char flag, itemop_t *op)
{
    if(0 == flag)
    {
        memcpy(&op->buf[0],&ParaUniG.termip.autogetip,93);

        /*op->buf[0] = ParaUniG.termip.autogetip;
        memcpy(&op->buf[1], ParaUniG.termip.ipterm, 4);
        memcpy(&op->buf[5], ParaUniG.termip.maskterm, 4);
        memcpy(&op->buf[9], ParaUniG.termip.ipgatew, 4);
        memcpy(&op->buf[13], ParaUniG.termip.username, 32);
        memcpy(&op->buf[45], ParaUniG.termip.pwd, 32);
        memcpy(&op->buf[77], (unsigned char *)&ParaUniG.termip.portlisten, 2);*/

    }
    else
    {
        memcpy(&ParaUniG.termip.autogetip,&op->buf[0],93);
/*        ParaUniG.termip.autogetip =op->buf[0];
        memcpy(ParaUniG.termip.ipterm, &op->buf[1], 4);
        memcpy(ParaUniG.termip.maskterm, &op->buf[5], 4);
        memcpy(ParaUniG.termip.ipgatew, &op->buf[9], 4);
        memcpy(ParaUniG.termip.username, &op->buf[13], 32);
        memcpy(ParaUniG.termip.pwd, &op->buf[45], 32);
        memcpy((unsigned char *)&ParaUniG.termip.portlisten, &op->buf[77], 2);*/

    }

    return 0;
}

int parauni_userinfo(unsigned char flag, itemop_t *op)
{
    if(0 == flag)
    {
        memcpy(op->buf, &ParaUniG.userinfo, sizeof(cfg_userinfo_tG));
    }
    else
    {
        memcpy( &ParaUniG.userinfo, op->buf,sizeof(cfg_userinfo_tG));

    }

    return 0;
}
int parauni_termmac(unsigned char flag, itemop_t *op)
{
    if(0 == flag)
    {
        memcpy(op->buf, ParaUniG.addr_mac, 6);
    }
    else
    {
        memcpy(ParaUniG.addr_mac, op->buf, 6);

    }

    return 0;
}
