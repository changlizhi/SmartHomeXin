/**
* capconf.h -- 容量配置
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-10
* 最后修改时间: 2009-5-10
*/

#ifndef _PARAM_CAPCONF_H
#define _PARAM_CAPCONF_H

#define MAX_CET                      10   //多功能表最大个数
#define MAX_METER            255 //最大电表/交流采样数
#define MAX_METP            MAX_METER   //最大测量点数#define MAX_METP1            64 //用于测量点
#define MAX_CENMETP        1     //最大多功能测量点数, 载波表计的测量点号只能大于这个

#define MAX_PLCMET            (MAX_METP-MAX_CENMETP)  //最大载波表数
#define PLC_BASEMETP        (MAX_CENMETP+1)  //载波表起始测量点号(表号) 从2号表开始为普通载波表

#define IMPORTANT_BASEMETP            1201
#ifdef DEBUGMODE
#define MAX_IMPORTANT_USER        16//6   //最大重点用户表数
#else
#define MAX_IMPORTANT_USER        6   //最大重点用户表数
#endif
#define IMPORTANT_ENDMETP              (IMPORTANT_BASEMETP+MAX_IMPORTANT_USER)

#define METYPE_ELEMET 0x01/*单相电子表*/
#define METYPE_MENMET 2/*机械表*/
#define METYPE_SIMFUNCMET 3/*简易多功能表*/
#define METYPE_MULFUNCMET 8/*多功能总表*/
#endif /*_PARAM_CAPCONF_H*/

