/**
* version.h -- 版本号头文件
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-3-31
* 最后修改时间: 2009-5-6
*/

#ifndef _VERSION_H
#define _VERSION_H

//工程版本号

//广标规约版本信息 
//版本号为BCD码，从01-99。
#define VERSION_MAJOR        01     //主版本号
#define VERSION_SUBMAJOR    01     //次版本号
#define VERSION_MINOR        04    //小版本

//版本发布日期
#if 0
#define RELEASE_DATE_YEAR    0x09
#define RELEASE_DATE_MONTH    0x05
#define RELEASE_DATE_DAY    0x05
#else
extern const unsigned char _mk_year;
extern const unsigned char _mk_month;
extern const unsigned char _mk_day;
extern const unsigned char _mk_hour;
extern const unsigned char _mk_minute;
#endif
//应生产要求，增加软件包版本号
//注意每次正式发布前须更新added by yzl. 2011.6.15
#define VERSION_PACK_MAJOR        03
#define VERSION_PACK_MINOR        01
#define VERSION_PACK_HARD        4
/*
@规约类型，由宏控制某些省的特殊需求。
@若要选定某省，请打开相应的宏控制,并重新编译。
*/

#define    TYPE_NWTY            0    //南网通用()
#define    TYPE_S3C2440          1     //linux Arm9版本
#define    TYPE_S3C6410          2     //linux Arm9版本
#define    TYPE_ANDROID          3     //Android 版本

#define     GWVERSION                   TYPE_NWTY/*控制每个版本*/


extern const char *VerStringProj[];
#define STRING_PROJECT        VerStringProj[GWVERSION]
extern const char *hard_cpu;//CPU类型
extern const char *hard_type;//硬件类型

#endif /*_VERSION_H*/

