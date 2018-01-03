#ifndef _ITEMOP_H
#define _ITEMOP_H


#define ITEMOP_LOWLEVEL    0
#define ITEMOP_HIGHLEVEL    0x11
typedef struct {
    unsigned short id;  //数据项标识
    unsigned short maxlen;   //缓存区最大长度
    unsigned char *buf;   //缓存指针
    unsigned short actlen;   //操作后实际数据长度
    //unsigned char mid;   //测量点号
    unsigned short mid;
    unsigned char level;   //操作级别
    unsigned char itf;   //操作所在接口
} itemop_t;


#define ITORTN_OK    0   //数据项读或写成功
#define ITORTN_INVALID    1   //非法数据项操作
#define ITORTN_PWDERR    2   //密码错误
#define ITORTN_OPFAIL    3   //数据项写失败
#define ITORTN_OPTIMEOUT 4

int write_item(itemop_t *op);
int read_item(itemop_t *op);

//void set_savflag(unsigned char flag);
//void clear_savflag(void);
//void deal_savflag(void);
#endif

