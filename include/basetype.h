#ifndef _BASETYPE_H
#define _BASETYPE_H

//#define NULL 0

typedef void (*pfvoid_t)(void);

#define SCHAR    char
#define UCHAR    unsigned char
#define VCHAR    volatile unsigned char
#define SSHORT    short
#define USHORT    unsigned short
#define VSHORT    volatile unsigned short
#define SINT    int
#define UINT    unsigned int
#define VINT    volatile unsigned int
#define SLONG    long
#define ULONG    unsigned long
#define VLONG    volatile unsigned long
#define INT64    long long//__int64

typedef struct __attribute__((packed)){
    unsigned char year;
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char minute;
    unsigned char second;
} rtime_t;

typedef struct __attribute__((packed)){
    unsigned char year;
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char minute;
} mntime_t;

typedef union {
    unsigned short us;
    unsigned char uc[2];
} VARSHORT;

typedef union {
    unsigned int ul;
    unsigned char uc[4];
} VARLONG;

typedef union {
    float ft;
    unsigned char uc[4];
} VARFLOAT;

typedef union {
    double db;
    unsigned char uc[8];
} VARDOUBLE;

#define VARFLEX_ULONG    0
#define VARFLEX_INT    1
#define VARFLEX_UCHAR    2
#define VARFLEX_BCD    3
#define VARFLEX_USHORT    4
#define VARFLEX_SSHORT    5
#define VARFLEX_USHORTFLAG 6
typedef union {
    unsigned int ul;
    int si;
    unsigned char uc[4];
    unsigned short us;
    short ss;
} VARFLEX_4;

#endif /*_BASETYPE_H*/
