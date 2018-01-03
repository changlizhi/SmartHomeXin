#ifndef _RUNSTATE_H
#define _RUNSTATE_H

typedef struct {
    unsigned char flag;// 1有升级程序0没有升级程序
    unsigned char ver[8];
}softchg_stat_t;

typedef struct {
    softchg_stat_t softchg;     //版本变更

    struct {
        unsigned short head;        //循环表头索引
        unsigned short cur;          //循环表尾索引
        unsigned short snd;          //主动发送索引
        unsigned char  playstate;    //0--无播放 1--正在播放
    } alarm;  //告警文件状态
    unsigned char cnt_snderr; //告警重发次数。
    unsigned char alarmtime;//时钟告警每天只判断一次告警1:判断,0未判断
} runstate_tG;

#ifndef DEFINE_RUNSTATE
extern runstate_tG RunStateG;
#endif

/**
* @brief 获得修改运行状态指针
* @return 缓存区指针
*/
runstate_tG *RunStateModifyG(void);


/**
* @brief 保存运行状态
*/
void SaveRunState(void);
void ClearRunState(void);
#endif /*_RUNSTATE_H*/

