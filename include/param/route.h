/**
* route.h -- 路由表参数
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-20
* 最后修改时间: 2009-5-20
*/

#ifndef _PARAM_ROUTE_H
#define _PARAM_ROUTE_H

#include "include/param/capconf.h"
//#include "include/param/term.h"

#define MAX_ROUTE_LEVEL        4  //最大路由级数
#define MAX_ROUTE_ONEMET        4  //一块表最多可配路由数

typedef struct {
    unsigned char level;  //级数
    unsigned char unuse;
    unsigned char addr[MAX_ROUTE_LEVEL*6];
} cfg_route_t;

typedef struct {
    unsigned char num;
    unsigned char unuse[3];
    cfg_route_t route[MAX_ROUTE_ONEMET];
} cfg_route_met_t;

#ifndef DEFINE_PARAROUTE
extern  cfg_route_met_t ParaRoute[MAX_METER];
#endif

#endif /*_PARAM_ROUTE_H*/

