/**
* flat.c -- 平滑储存
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-10
* 最后修改时间: 2009-5-15
*/

#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <stdint.h>

#include "include/environment.h"
#include "include/debug.h"
#include "include/lib/crc.h"
#include "include/sys/syslock.h"
#include "include/sys/flat.h"



static int FlatFid = -1;

#define FLAT_MAGIC        0x39a6
typedef struct {
    unsigned short magic;
    unsigned short crc;
} rec_head_t;

static sector_conf_t SectorConfig[MAX_SECTOR] = {
    {FLATID_IMETENE, 0, 1024},            // 1024K
    {FLATID_PULSE, 1024, 256},          // 256K
    {FLATID_RUNSTATE, 1280, 512},      // 512K
};
#define REC_DATASIZE(sector)    (SectorConfig[sector].add_max-sizeof(rec_head_t))




static int LockIdFlat;
#define FLAT_LOCK        LockSysLock(LockIdFlat)
#define FLAT_UNLOCK        UnlockSysLock(LockIdFlat)


/**
* @brief 平滑储存初始化
* @return 成功0, 否则失败
*/
DECLARE_INIT_FUNC(FlatInit);
int FlatInit(void)
{
    //SysInitMutex(&FlatMutex);
    LockIdFlat = RegisterSysLock();

/*    FlatFid = open("/dev/fram", O_RDWR);
    if(-1 == FlatFid) {
        printf("can not open fram driver.\n");
        return 1;
    }*/

    SET_INIT_FLAG(FlatInit);
    return 0;
}

/**
* @brief 检查记录的合法性
* @param buf 缓存区指针
* @param len 缓存区长度
* @return 合法记录返回1, 非法记录返回0, 空记录返回-1
*/
static inline int ValidRecord(const unsigned char *buf, int len)
{
    unsigned short crc;
    rec_head_t *phead = (rec_head_t *)buf;
    int i;

    if(phead->magic != FLAT_MAGIC) {
        if(phead->magic == 0xffff && phead->crc == 0xffff) goto mark_empty;
        else return 0;
    }

    crc = CalculateCRC(buf+sizeof(rec_head_t), len-sizeof(rec_head_t));
    if(crc != phead->crc) return 0;

    return 1;

mark_empty:
    buf += sizeof(rec_head_t);
    len -= sizeof(rec_head_t);
    for(i=0; i<len; i++) {
        if(0xff != *buf++) return 0;
    }

    return -1;
}


/**
* @brief 读取FLAT文件数据
* @param sector 文件扇区号
* @param buf 缓存区指针
* @param len 缓存区长度
* @return 成功返回实际读取长度, 失败返回-1
*/
int ReadFlatFile(unsigned int sector, unsigned char *buf, int len)
{
    int rtn = 0;
    int rdlen = 0;
    unsigned char memcache[BUFFER_SIZE];

    AssertLogReturn(FlatFid<0, -1, "invalid flat id\n");
    AssertLogReturn(sector >= MAX_SECTOR, -1, "invalid sector(%d)\n", sector);
    AssertLog(len<=0, "invalid len(%d:%d)\n", sector, len);
    AssertLogReturn(len > REC_DATASIZE(sector), -1, "invalid record size(%d:%d)\n", sector, REC_DATASIZE(sector));

    memset(memcache, 0x0, sizeof(memcache));
    rdlen = len + sizeof(rec_head_t);

    FLAT_LOCK;
    ioctl(FlatFid, 0, &sector); //选择写入的扇区
    rtn = read(FlatFid, (char *)memcache, rdlen);
    FLAT_UNLOCK;

    if(1 != ValidRecord(memcache, rdlen) || rtn<4)
    {
        DebugPrint(0, "validRecord\n");
        return -1;
    }
    else
    {
        memcpy(buf, memcache+4, len);
        return rtn-4;
    }
}

/**
* @brief 写入FLAT文件数据
* @param sector 文件扇区号
* @param buf 缓存区指针
* @param len 缓存区长度
* @return 成功返回实际写入长度, 失败返回-1
*/
int WriteFlatFile(unsigned int sector, const unsigned char *buf, int len)
{
    rec_head_t head;
    int rtn = 0;
    unsigned char memcache[BUFFER_SIZE];
    unsigned char* pmem = memcache;

    AssertLogReturn(FlatFid<0, -1, "invalid flat id(%d)\n", sector);
    AssertLogReturn(sector >= MAX_SECTOR, -1, "invalid sector(%d)\n", sector);
    AssertLogReturn(len <= 0, -1, "invalid len(%d:%d)\n", sector, len);
    AssertLogReturn(len > REC_DATASIZE(sector), -1, "invalid record size(%d:%d)\n", sector, REC_DATASIZE(sector));

    head.magic = FLAT_MAGIC;
    head.crc = CalculateCRC(buf, len);

    memset(pmem, 0x0, sizeof(memcache));
    memcpy(pmem, (void*)&head, sizeof(head)); pmem += sizeof(head);
    memcpy(pmem, buf, len);

    FLAT_LOCK;
    ioctl(FlatFid, 0, &sector); //选择写入的扇区
    rtn = write(FlatFid, memcache, sizeof(head)+len);
    FLAT_UNLOCK;

    //DebugPrint(0, "writeflat id is %d, rtn is %d\n", sector, rtn);
    return rtn;
}


