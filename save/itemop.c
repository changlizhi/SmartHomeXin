#include <string.h>

#include "param.h"
#include "include/debug.h"
#include "include/lib/datachg.h"
#include "include/param/term.h"
#include "include/param/unique.h"
typedef int (*itofunc_t)(unsigned char flag, itemop_t *op);

#define ITOFLAG_MASK    0x0f
#define ITOFLAG_FUNC    0    //由函数处理
#define ITOFLAG_SET00    1    //恒0
#define ITOFLAG_SETAA    2   //恒为0xaa
#define ITOFLAG_SETFF    3    //恒为0xff
#define ITOFLAG_BCD    4    //BCD码
#define ITOFLAG_COPY    5   //拷贝(不处理)
#define ITOFLAG_SHORTBCD    6    //BCD短整型
#define ITOFLAG_LONGBCD    7    //BCD长整型

#define ITOFLAG_RO    0x10   //可读
#define ITOFLAG_WO    0x20   //可写
#define ITOFLAG_WR    (ITOFLAG_RO|ITOFLAG_WO)   //可读写
#define ITOFLAG_HIGHLEVEL    0x40   //高权限
#define ITOFLAG_GROUP  0x80   //块操作

#define ITO_NEED_PROG_KEY 0x01

typedef struct {
    unsigned short id;   //数据项标识
    unsigned char flag;   //标志
    unsigned char savflag;   //保存区域标志
    unsigned short protech;
    unsigned short datalen;   //数据长度
    itofunc_t pfunc;   //函数或者储存区指针
} item_ele_t;

#define MASK_ITOID    0xfff0
#define ITEMGRP_FLAG_TERM    0x0000   //终端数据或者参数
#define ITEMGRP_FLAG_MET    0x1000    //表数据或者参数
#define ITEMGRP_FLAG_SPEC 0x2000    //用于校时数据项tn有可能为0xffff
#define ITEMGRP_FLAG_MASKFLAG    0xf000
#define ITEMGRP_FLAG_MASKOFFSET    0x0fff
typedef struct {
    unsigned short id;
    unsigned short flag;   //高4位为FLAG, 低12位为偏移基准值
    const item_ele_t *pitemlist;
} item_grp_t;

/*0x7500*/
const item_ele_t g_item_uni1[] = {
    {0x7500, ITOFLAG_WR|ITOFLAG_HIGHLEVEL|ITOFLAG_FUNC, SAVFLAG_PARAUNI, 0, 93, parauni_termip},
    {0x7501, ITOFLAG_WR|ITOFLAG_HIGHLEVEL|ITOFLAG_FUNC, SAVFLAG_PARAUNI, 0, 6, parauni_termmac},
    {0x7502, ITOFLAG_WR|ITOFLAG_HIGHLEVEL|ITOFLAG_FUNC, SAVFLAG_PARAUNI, 0, 1, paraterm_reset},
    {0x7503, ITOFLAG_WR|ITOFLAG_HIGHLEVEL|ITOFLAG_FUNC, SAVFLAG_PARAUNI, 0, 3, paraterm_sampling},
    {0x7504, ITOFLAG_WR|ITOFLAG_HIGHLEVEL|ITOFLAG_FUNC, SAVFLAG_PARAUNI, 0, sizeof(cfg_userinfo_tG), parauni_userinfo},

    {0, 0, 0, 0, 0, NULL},
};

/*0x8010*/
const item_ele_t g_item_addr[] = {
    {0x8010, ITOFLAG_WR|ITOFLAG_HIGHLEVEL|ITOFLAG_FUNC, SAVFLAG_PARATERM, 0, 9, paraterm_svraddr},
    {0x8011, ITOFLAG_WR|ITOFLAG_HIGHLEVEL|ITOFLAG_FUNC, SAVFLAG_PARATERM, 0, 9, paraterm_svraddr},
    {0x8012, ITOFLAG_WR|ITOFLAG_HIGHLEVEL|ITOFLAG_FUNC, SAVFLAG_PARATERM, 0, 9, paraterm_svraddr},
    {0x8013, ITOFLAG_WR|ITOFLAG_HIGHLEVEL|ITOFLAG_COPY, SAVFLAG_PARATERM, 0, 8, (itofunc_t)ParaTermG.sms_addr},
    {0x8014, ITOFLAG_WR|ITOFLAG_HIGHLEVEL|ITOFLAG_FUNC, SAVFLAG_PARATERM, 0, 8, paraterm_gateaddr},
    {0x8015, ITOFLAG_WR|ITOFLAG_HIGHLEVEL|ITOFLAG_FUNC, SAVFLAG_PARATERM, 0, 16, paraterm_apn},
    {0x8019, ITOFLAG_WR|ITOFLAG_HIGHLEVEL|ITOFLAG_FUNC, SAVFLAG_PARATERM, 0, 32, paraterm_usrname},
    {0x801a, ITOFLAG_WR|ITOFLAG_HIGHLEVEL|ITOFLAG_FUNC, SAVFLAG_PARATERM, 0, 32, paraterm_usrname},
    {0x801f,  ITOFLAG_WR|ITOFLAG_HIGHLEVEL|ITOFLAG_GROUP, 0, 0, 0, NULL},

    {0, 0, 0, 0, 0, NULL},
};
/*0x8020*/
const item_ele_t g_item_pwd[] = {
    {0x8020, ITOFLAG_WO|ITOFLAG_SET00, 0, 0, 3, (itofunc_t)ParaTermG.com_pwd},
    {0x8021, ITOFLAG_WO|ITOFLAG_COPY, SAVFLAG_PARATERM,  0,3, (itofunc_t)ParaTermG.com_pwd},
    {0x8022, ITOFLAG_WO|ITOFLAG_HIGHLEVEL|ITOFLAG_COPY, SAVFLAG_PARATERM, 0, 3, (itofunc_t)ParaTermG.adm_pwd},
    {0x802f, ITOFLAG_WO|ITOFLAG_HIGHLEVEL|ITOFLAG_GROUP, 0, 0, 0, NULL},

    {0, 0, 0, 0, 0, NULL},
};
/*0x8030*/
const item_ele_t g_item_time[] = {
    {0x8030, ITOFLAG_WR|ITOFLAG_FUNC, 0, 0, 6, paraterm_time},
    {0x803e, ITOFLAG_WR|ITOFLAG_FUNC, 0, 0, 2, paraterm_realtime},

    {0, 0, 0, 0, 0, NULL},
};

const item_ele_t g_item_port0[] = {
    {0x8700, ITOFLAG_WR|ITOFLAG_COPY, SAVFLAG_PARATERM,  0,1, (itofunc_t)&ParaTermG.port[0].baud},
    {0x8701, ITOFLAG_WR|ITOFLAG_COPY, SAVFLAG_PARATERM,  0,1, (itofunc_t)&ParaTermG.port[0].parity},
    {0x8702, ITOFLAG_WR|ITOFLAG_COPY, SAVFLAG_PARATERM,  0,1, (itofunc_t)&ParaTermG.port[0].databits},
    {0x8703, ITOFLAG_WR|ITOFLAG_FUNC, SAVFLAG_PARATERM,  0,1, paraterm_port_stopbit},
    {0x8704, ITOFLAG_WR|ITOFLAG_COPY, SAVFLAG_PARATERM,  0,1, (itofunc_t)&ParaTermG.port[0].func},
    {0x870f, ITOFLAG_WR|ITOFLAG_GROUP, 0,  0,0, NULL},

    {0, 0, 0, 0, 0, NULL},
};

const item_ele_t g_item_port1[] = {
    {0x8710, ITOFLAG_WR|ITOFLAG_COPY, SAVFLAG_PARATERM,  0,1, (itofunc_t)&ParaTermG.port[1].baud},
    {0x8711, ITOFLAG_WR|ITOFLAG_COPY, SAVFLAG_PARATERM,  0,1, (itofunc_t)&ParaTermG.port[1].parity},
    {0x8712, ITOFLAG_WR|ITOFLAG_COPY, SAVFLAG_PARATERM, 0, 1, (itofunc_t)&ParaTermG.port[1].databits},
    {0x8713, ITOFLAG_WR|ITOFLAG_FUNC, SAVFLAG_PARATERM,  0,1, paraterm_port_stopbit},
    {0x8714, ITOFLAG_WR|ITOFLAG_COPY, SAVFLAG_PARATERM,  0,1, (itofunc_t)&ParaTermG.port[1].func},
    {0x871f, ITOFLAG_WR|ITOFLAG_GROUP, 0, 0, 0, NULL},

    {0, 0, 0, 0,  0,NULL},
};

const item_ele_t g_item_port2[] = {
    {0x8720, ITOFLAG_WR|ITOFLAG_COPY, SAVFLAG_PARATERM,  0,1, (itofunc_t)&ParaTermG.port[2].baud},
    {0x8721, ITOFLAG_WR|ITOFLAG_COPY, SAVFLAG_PARATERM,  0,1, (itofunc_t)&ParaTermG.port[2].parity},
    {0x8722, ITOFLAG_WR|ITOFLAG_COPY, SAVFLAG_PARATERM, 0, 1, (itofunc_t)&ParaTermG.port[2].databits},
    {0x8723, ITOFLAG_WR|ITOFLAG_FUNC, SAVFLAG_PARATERM,  0,1, paraterm_port_stopbit},
    {0x8724, ITOFLAG_WR|ITOFLAG_COPY, SAVFLAG_PARATERM,  0,1, (itofunc_t)&ParaTermG.port[2].func},
    {0x872f, ITOFLAG_WR|ITOFLAG_GROUP, 0, 0, 0, NULL},

    {0, 0, 0, 0, 0, NULL},
};
#if 0
const item_ele_t g_item_port3[] = {
    {0x8730, ITOFLAG_WR|ITOFLAG_COPY, SAVFLAG_PARATERM,  0,1, (itofunc_t)&ParaTermG.port[2].baud},
    {0x8731, ITOFLAG_WR|ITOFLAG_FUNC, SAVFLAG_PARATERM, 0, 1, paraterm_port_chkbit},
    {0x8732, ITOFLAG_WR|ITOFLAG_FUNC, SAVFLAG_PARATERM, 0, 1, paraterm_port_databit},
    {0x8733, ITOFLAG_WR|ITOFLAG_FUNC, SAVFLAG_PARATERM,  0,1, paraterm_port_stopbit},
    {0x8734, ITOFLAG_WR|ITOFLAG_COPY, SAVFLAG_PARATERM, 0,1, (itofunc_t)&ParaTermG.port[2].func},
    {0x873f, ITOFLAG_WR|ITOFLAG_GROUP, 0, 0, 0, NULL},

    {0, 0, 0, 0, 0, NULL},
};
#endif
const item_ele_t g_item_port3[] = {
    {0x8730, ITOFLAG_WR|ITOFLAG_COPY, SAVFLAG_PARATERM, 0, 1, (itofunc_t)&ParaTermG.port[3].baud},
    {0x8731, ITOFLAG_WR|ITOFLAG_COPY, SAVFLAG_PARATERM,  0,1, (itofunc_t)&ParaTermG.port[3].parity},
    {0x8732, ITOFLAG_WR|ITOFLAG_COPY, SAVFLAG_PARATERM, 0, 1, (itofunc_t)&ParaTermG.port[3].databits},
    {0x8733, ITOFLAG_WR|ITOFLAG_FUNC, SAVFLAG_PARATERM,  0,1, paraterm_port_stopbit},
    {0x8734, ITOFLAG_WR|ITOFLAG_COPY, SAVFLAG_PARATERM,  0,1, (itofunc_t)&ParaTermG.port[3].func},
    {0x873f, ITOFLAG_WR|ITOFLAG_GROUP, 0,  0,0, NULL},

    {0, 0, 0, 0, 0, NULL},
};

const item_ele_t g_item_port8[] = {
    {0x8780, ITOFLAG_WR|ITOFLAG_COPY, SAVFLAG_PARATERM,  0,1, (itofunc_t)&ParaTermG.port[7].baud},
    {0x8781, ITOFLAG_WR|ITOFLAG_COPY, SAVFLAG_PARATERM,  0,1, (itofunc_t)&ParaTermG.port[7].parity},
    {0x8782, ITOFLAG_WR|ITOFLAG_COPY, SAVFLAG_PARATERM, 0, 1, (itofunc_t)&ParaTermG.port[7].databits},
    {0x8783, ITOFLAG_WR|ITOFLAG_FUNC, SAVFLAG_PARATERM,  0,1, paraterm_port_stopbit},
    {0x8784, ITOFLAG_WR|ITOFLAG_COPY, SAVFLAG_PARATERM,  0,1, (itofunc_t)&ParaTermG.port[7].func},
    {0x878f, ITOFLAG_WR|ITOFLAG_GROUP, 0, 0, 0, NULL},

    {0, 0, 0, 0, 0, NULL},
};
extern unsigned char hard_info[18];

/*0x8800*/
const item_ele_t g_item_ver[] = {
    {0x8809, ITOFLAG_RO|ITOFLAG_COPY, 0, 0, 8, (itofunc_t)&hard_info[8]},
    {0x880a, ITOFLAG_RO|ITOFLAG_COPY, 0, 0,2, (itofunc_t)&hard_info[16]},

    {0, 0, 0, 0,  0,NULL},
};

/*0x8810*/
const item_ele_t g_item_cfg[] = {
    {0x8810, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_COPY, SAVFLAG_PARATERM, 0, 1, (itofunc_t)&ParaTermG.search_device},
    {0x8815, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_PARATERM, 0, 1, paraterm_upreaddev},//(itofunc_t)&ParaTermG.task_starthour},
    {0x8816, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_PARATERM, 0, 1, paraterm_upreaddev},//(itofunc_t)&ParaTermG.task_startdev},
    {0x8817, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_COPY, SAVFLAG_PARATERM, 0, 1, (itofunc_t)&ParaTermG.plc_route_type},
    {0x881a, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_PARATERM, 0, 1, paraterm_updaydata},
    {0x881b, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_COPY, SAVFLAG_PARATERM, 0, 4, (itofunc_t)&ParaTermG.alarm_flag},

    {0x881c, ITOFLAG_RO|ITOFLAG_FUNC, 0, 0, 272, paraterm_scenelist},    //读取情景列表
    {0, 0, 0, 0,  0,NULL},
};

/*0x8820*/
const item_ele_t g_item_caspara[] = {
    {0x8820, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_PARATERM, 0, 2, paraterm_monlastread},
    {0x8821, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_PARATERM, 0, 2, paraterm_monlastup},
    //{0x8822, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_COPY, SAVFLAG_PARAUNI, 0, 4, (itofunc_t)ParaUniG.cascade_term.cascade_addr},
    {0x8822, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_PARATERM, 0, 4, paraterm_peibian_addr},
    {0x8823, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_COPY, SAVFLAG_PARATERM, 0, 2, (itofunc_t)ParaTermG.ct},
    {0, 0, 0, 0,  0,NULL},
};
/*0x883x情景信息*/  
const item_ele_t g_item_sceneinfo[] = {
    {0x8830, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_SCENE, 0, 370, paraterm_scene},
    {0x8831, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_SCENE, 0, 370, paraterm_scene},
    {0x8832, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_SCENE, 0, 370, paraterm_scene},
    {0x8833, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_SCENE, 0, 370, paraterm_scene},
    {0x8834, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_SCENE, 0, 370, paraterm_scene},
    {0x8835, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_SCENE, 0, 370, paraterm_scene},
    {0x8836, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_SCENE, 0, 370, paraterm_scene},
    {0x8837, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_SCENE, 0, 370, paraterm_scene},
    {0x8838, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_SCENE, 0, 370, paraterm_scene},
    {0x8839, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_SCENE, 0, 370, paraterm_scene},
    {0x883A, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_SCENE, 0, 370, paraterm_scene},
    {0x883B, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_SCENE, 0, 370, paraterm_scene},
    {0x883C, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_SCENE, 0, 370, paraterm_scene},
    {0x883C, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_SCENE, 0, 370, paraterm_scene},
    {0x883D, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_SCENE, 0, 370, paraterm_scene},
    {0x883E, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_SCENE, 0, 370, paraterm_scene},
    {0, 0, 0, 0,  0,NULL},
};
/*0x884x定时任务信息*/  
const item_ele_t g_item_timertaskinfo[] = {
    {0x8840, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_TIMERTASK, 0, 220, paraterm_timertasklist},
    {0x8841, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_TIMERTASK, 0, 374, paraterm_timertask},
    {0x8842, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_TIMERTASK, 0, 374, paraterm_timertask},
    {0x8843, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_TIMERTASK, 0, 374, paraterm_timertask},
    {0x8844, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_TIMERTASK, 0, 374, paraterm_timertask},
    {0x8845, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_TIMERTASK, 0, 374, paraterm_timertask},
    {0x8846, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_TIMERTASK, 0, 374, paraterm_timertask},
    {0x8847, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_TIMERTASK, 0, 374, paraterm_timertask},
    {0x8848, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_TIMERTASK, 0, 374, paraterm_timertask},
    {0x8849, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_TIMERTASK, 0, 374, paraterm_timertask},
    {0x884A, ITOFLAG_WR|ITOFLAG_RO|ITOFLAG_FUNC, SAVFLAG_TIMERTASK, 0, 374, paraterm_timertask},
    {0, 0, 0, 0,  0,NULL},
};


const item_grp_t g_item_grp[] = {
    {0x7500, ITEMGRP_FLAG_TERM, g_item_uni1},
    {0x8010, ITEMGRP_FLAG_TERM, g_item_addr},
    {0x8020, ITEMGRP_FLAG_TERM, g_item_pwd},
    {0x8030, ITEMGRP_FLAG_SPEC, g_item_time},
    {0x8700, ITEMGRP_FLAG_TERM, g_item_port0},
    {0x8710, ITEMGRP_FLAG_TERM, g_item_port1},
    {0x8720, ITEMGRP_FLAG_TERM, g_item_port2},
    {0x8730, ITEMGRP_FLAG_TERM, g_item_port3},
//    {0x8730, ITEMGRP_FLAG_TERM, g_item_port4},
    {0x8780, ITEMGRP_FLAG_TERM, g_item_port8},

    {0x8800, ITEMGRP_FLAG_TERM, g_item_ver},
    {0x8810, ITEMGRP_FLAG_TERM, g_item_cfg},
    {0x8820, ITEMGRP_FLAG_TERM, g_item_caspara},
    {0x8830, ITEMGRP_FLAG_TERM, g_item_sceneinfo},
    {0x8840, ITEMGRP_FLAG_TERM, g_item_timertaskinfo},


    {0, 0, NULL},
};

extern void SetSaveParamFlag(unsigned int flag);

static itemop_t g_itemop_bak;
int write_item(itemop_t *op)
{
    unsigned short group =  (op->id)&MASK_ITOID;
    int datalen;
    const item_grp_t *pgrp = g_item_grp;
    const item_ele_t *pitem;
    unsigned char *pdata;
    int rtn;
    unsigned int offset;
    unsigned short grpflag;

    op->actlen = 0;

    while(0 != pgrp->id)
    {
        if(group != pgrp->id)
        {
            pgrp++;
            continue;
        }

        grpflag = pgrp->flag & ITEMGRP_FLAG_MASKFLAG;
        offset = 0;
        /*测量点合法性检查*/
        if(ITEMGRP_FLAG_MET == grpflag)
        {
            if(op->mid >= MAX_METER) return ITORTN_INVALID;
            offset = pgrp->flag & ITEMGRP_FLAG_MASKOFFSET;
            offset *= op->mid;

        #if 0
            if (op->mid != 0xffff)/*op->mid=0xffff:有特殊含义*/
            {
                if (op->mid == 0 || op->mid > MAX_METP)
                {
                    DebugPrint(0, "mid:%d cid:%04x\n", op->mid, op->id);
                    return ITORTN_INVALID;
                }
            }
        #endif
        }
        else if(ITEMGRP_FLAG_SPEC == grpflag)
        {
            if(0 != op->mid && 0xffff != op->mid) return ITORTN_INVALID;
            offset = 0;
        }
        else  //term
        {
            if(0 != op->mid) return ITORTN_INVALID;
            offset = 0;
        }

        pitem = pgrp->pitemlist;
        while(0 != pitem->id)
        {
            if(op->id != pitem->id)
            {
                pitem++;
                continue;
            }

            datalen = (int)pitem->datalen;
            DebugPrint(0, "datalen:%d maxlen:%d\n", datalen, op->maxlen);
            if(datalen > op->maxlen) return ITORTN_INVALID;
            op->actlen = pitem->datalen;

            if(!(pitem->flag & ITOFLAG_WO)) return ITORTN_OPFAIL;
            if((pitem->flag&ITOFLAG_HIGHLEVEL) && (op->level != ITEMOP_HIGHLEVEL))
                return ITORTN_PWDERR;
            //if((pitem->protech&ITO_NEED_PROG_KEY)&&(program_status_get()==0))return ITORTN_OPFAIL;
            if(pitem->flag&ITOFLAG_GROUP)
            {
                int fail;

                pitem = pgrp->pitemlist;

                g_itemop_bak = *op;
                g_itemop_bak.actlen = 0;
                op->actlen = 0;

                fail = 0;
                while(pitem->id != op->id)
                {
                    g_itemop_bak.id = pitem->id;
                    rtn = write_item(&g_itemop_bak);
                    if(ITORTN_INVALID == rtn)
                        return ITORTN_INVALID;
                    else if(rtn) fail = rtn;

                    op->actlen += g_itemop_bak.actlen;
                    if(op->actlen > op->maxlen) return ITORTN_INVALID;
                    g_itemop_bak.buf += g_itemop_bak.actlen;
                    g_itemop_bak.maxlen -= g_itemop_bak.actlen;

                    pitem++;
                }

                return fail;
            }

            pdata = (unsigned char *)(pitem->pfunc);
            pdata += offset;

            switch(pitem->flag&ITOFLAG_MASK)
            {
            case ITOFLAG_FUNC:
                if(NULL == pitem->pfunc) return ITORTN_INVALID;
                rtn = (*(pitem->pfunc))(1, op);
                if(ITORTN_OK == rtn) SetSaveParamFlag(pitem->savflag);
                return rtn;

            case ITOFLAG_COPY:
                memcpy(pdata, op->buf, datalen);
                SetSaveParamFlag(pitem->savflag);
                return ITORTN_OK;

            case ITOFLAG_BCD:
                memcpy(pdata, op->buf, datalen);
                bcdtohex(pdata, datalen);
                SetSaveParamFlag(pitem->savflag);
                return ITORTN_OK;

            case ITOFLAG_SHORTBCD:
                {
                    unsigned short us = (unsigned short)bcdtol(op->buf, pitem->datalen);
                    depart_short(us, pdata);
                }
                SetSaveParamFlag(pitem->savflag);
                return ITORTN_OK;

            case ITOFLAG_LONGBCD:
                {
                    unsigned int ul = bcdtol(op->buf, pitem->datalen);
                    depart_long(ul, pdata);
                }
                SetSaveParamFlag(pitem->savflag);
                return ITORTN_OK;

            default: return ITORTN_OK;
            }
        }

        break;
    }

    return ITORTN_INVALID;
}

int read_item(itemop_t *op)
{
#if 1
    unsigned short group =  (op->id)&MASK_ITOID;
    int datalen;
    const item_grp_t *pgrp = g_item_grp;
    const item_ele_t *pitem;
    unsigned char *pdata;
    unsigned int offset;
    unsigned short grpflag;

    op->actlen = 0;


    //参数
    while(0 != pgrp->id)
    {
        if(group != pgrp->id)
        {
            pgrp++;
            continue;
        }

        grpflag = pgrp->flag & ITEMGRP_FLAG_MASKFLAG;
        offset = 0;
        if(ITEMGRP_FLAG_MET == grpflag)
        {
        #if 0
            if(op->mid >= MAX_METER) return ITORTN_INVALID;
            offset = pgrp->flag & ITEMGRP_FLAG_MASKOFFSET;
            offset *= op->mid;
        #endif
            return ITORTN_INVALID;//不读表项数据
        }
        else  //term
        {
            if(0 != op->mid) return ITORTN_INVALID;
            offset = 0;
        }

        pitem = pgrp->pitemlist;
        while(0 != pitem->id)
        {
            if(op->id != pitem->id)
            {
                pitem++;
                continue;
            }
            DebugPrint(0,"op->actlen =%d,pitem->datalen:%d\n ",op->actlen,pitem->datalen);
            datalen = (int)pitem->datalen;
            if(datalen > op->maxlen) return ITORTN_INVALID;
            if(!(pitem->flag & ITOFLAG_RO)) return ITORTN_OPFAIL;
            op->actlen = pitem->datalen;

            if(pitem->flag&ITOFLAG_GROUP)
            {
                pitem = pgrp->pitemlist;

                g_itemop_bak = *op;
                g_itemop_bak.actlen = 0;
                op->actlen = 0;

                while(pitem->id != op->id)
                {
                    g_itemop_bak.id = pitem->id;
                    g_itemop_bak.actlen = 0;
                    if(read_item(&g_itemop_bak))
                    {
                        op->actlen = 0;

                        return ITORTN_INVALID;
                    }

                    op->actlen += g_itemop_bak.actlen;
                    DebugPrint(0,"op->actlen =%d,g_itemop_bak->datalen:%d\n ",op->actlen,g_itemop_bak.actlen);
                    if(op->actlen > op->maxlen) return ITORTN_INVALID;
                    g_itemop_bak.buf += g_itemop_bak.actlen;
                    g_itemop_bak.maxlen -= g_itemop_bak.actlen;

                    pitem++;
                }

                return ITORTN_OK;
            }

            pdata = (unsigned char *)(pitem->pfunc);
            pdata += offset;

            switch(pitem->flag&ITOFLAG_MASK)
            {
            case ITOFLAG_FUNC:
                if(NULL == pitem->pfunc) return ITORTN_INVALID;
                if((*(pitem->pfunc))(0, op)) return ITORTN_INVALID;
                if(op->maxlen < op->actlen) return ITORTN_INVALID;
                return ITORTN_OK;

            case ITOFLAG_COPY:
                memcpy(op->buf, pdata, datalen);
                return ITORTN_OK;

            case ITOFLAG_BCD:
                memcpy(op->buf, pdata, datalen);
                hextobcd(op->buf, datalen);
                return ITORTN_OK;

            case ITOFLAG_SET00:
                memset(op->buf, 0, datalen);
                return ITORTN_OK;

            case ITOFLAG_SETAA:
                memset(op->buf, 0xaa, datalen);
                return ITORTN_OK;

            case ITOFLAG_SETFF:
                memset(op->buf, 0xff, datalen);
                return ITORTN_OK;

            case ITOFLAG_SHORTBCD:
                {
                    unsigned short us = make_short(pdata);
                    ltobcd((unsigned int)us, op->buf, pitem->datalen);
                }
                return ITORTN_OK;

            case ITOFLAG_LONGBCD:
                {
                    unsigned int ul = make_long(pdata);
                    ltobcd(ul, op->buf, pitem->datalen);
                }
                return ITORTN_OK;

            default: return ITORTN_INVALID;
            }
        }

        break;
    }
#endif
    return ITORTN_INVALID;
}


