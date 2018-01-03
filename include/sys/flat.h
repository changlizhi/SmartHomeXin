/**
* flat.h -- 平滑储存
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-5-15
* 最后修改时间: 2009-5-15
*/

#ifndef _SYS_FLAT_H
#define _SYS_FLAT_H

//copy from driver.注意，若driver中结构改动，则此处需更新
#define FLATID_START        0
#define FLATID_IMETENE    FLATID_START
#define FLATID_PULSE        1
#define FLATID_RUNSTATE    2
#define FLATID_END        3
#define MAX_SECTOR        FLATID_END

#define BUFFER_SIZE        1024

typedef struct {
    unsigned int id;
    unsigned short add_start;
    unsigned short add_max;
} sector_conf_t;


/**
* @brief 读取FLAT文件数据
* @param sector 文件扇区号(0~2)
* @param buf 缓存区指针
* @param len 缓存区长度
* @return 成功返回实际读取长度, 失败返回-1
*/
int ReadFlatFile(unsigned int sector, unsigned char *buf, int len);

/**
* @brief 写入FLAT文件数据
* @param sector 文件扇区号(0~2)
* @param buf 缓存区指针
* @param len 缓存区长度
* @return 成功返回实际写入长度, 失败返回-1
*/
int WriteFlatFile(unsigned int sector, const unsigned char *buf, int len);

#endif /*_SYS_FLAT_H*/


