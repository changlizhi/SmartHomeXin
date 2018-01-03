/**
* gpio -- GPIO驱动接口头文件
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-4-23
* 最后修改时间: 2009-4-23
*/

#ifndef _GPIO_H
#define _GPIO_H

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define POLL_TIMEOUT (3 * 1000) /* 3 seconds */
#define MAX_BUF 64

#define YMK_STATE0    0x01
#define YMK_STATE1    0x02
#define YMK_STATE2    0x04
#define YMK_STATE3    0x08
#define YMK_STATE4    0x10
#define RS1            0x20
#define RS0            0x40
#define MP_STATUS    0x80 //市电检测

//GPIO管脚号
//x = 0~31
#define GPIO_PA(x)        ((unsigned char)0x00|((x)&0x1f))
#define GPIO_PB(x)        ((unsigned char)0x20|((x)&0x1f))
#define GPIO_PC(x)        ((unsigned char)0x40|((x)&0x1f))
#define GPIO_PD(x)        ((unsigned char)0x60|((x)&0x1f))

//外部时钟格式
typedef struct {
    unsigned char century;
    unsigned char year;
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char minute;
    unsigned char second;
    unsigned char week;
} extclock_t;

int gpio_export(unsigned int gpio);

int gpio_unexport(unsigned int gpio);

int gpio_set_dir(unsigned int gpio, unsigned int out_flag);

int gpio_set_value(unsigned int gpio, unsigned int value);

int gpio_get_value(unsigned int gpio, unsigned int *value);

int gpio_set_edge(unsigned int gpio, char *edge);

int gpio_fd_open(unsigned int gpio);

int gpio_fd_close(int fd);
/**
* @brief 读取外部时钟
* @param 时钟变量指针
* @return 0成功, 否则失败
*/
int ExtClockRead(extclock_t *clock);

/**
* @brief 设置外部时钟
* @param 时钟变量指针
* @return 0成功, 否则失败
*/
int ExtClockWrite(const extclock_t *clock);

/**
* @brief 读取驱动程序版本信息
*        版本信息如下:主版本号(1), 次版本号(1), 发布年月日(3)(BCD格式), 空(1)
* @param buf 缓存区指针
* @param len 缓存区长度
* @return 成功返回读取长度(6字节), 否则返回-1
*/
int ReadDriverVersion(unsigned char *buf, int len);

#define GPIO_LED                    GPIO_PB(11)       //LED灯


#define GPIO_39                        GPIO_PB(7)    //语音提示

#define GPIO_42                        GPIO_PB(10) //振子输出

#define GPIO_DOOR                        GPIO_PA(17)  //大门
#define GPIO_PWDOWN                        GPIO_PA(13)  //掉电检测
#define GPIO_PLAY                        GPIO_PB(8)  //播放音频文件按钮
#define GPIO_KEY_ADD                    GPIO_PA(15)
#define GPIO_KEY_SUB                    GPIO_PA(14)
#define GPIO_LED_NET                    GPIO_PA(16)
#define GPIO_LED_SYS                    GPIO_PA(17)

#define GPIO_MOTOR                        GPIO_PC(23)  //电机

#endif /*_GPIO_H*/

