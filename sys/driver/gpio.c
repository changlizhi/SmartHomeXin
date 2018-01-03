/**
* gpio.c -- GPIO驱动接口
* 
* 作者: zhuzhiqiang
* 创建时间: 2009-4-23
* 最后修改时间: 2009-4-23
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <poll.h>
#include "include/debug.h"

#include "include/sys/gpio.h"
 /****************************************************************
 * Constants
 ****************************************************************/
 
#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define POLL_TIMEOUT (3 * 1000) /* 3 seconds */
#define MAX_BUF 64

/****************************************************************
 * gpio_export
 ****************************************************************/
int gpio_export(unsigned int gpio)
{
    int fd, len;
    char buf[MAX_BUF];
 
    fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
    if (fd < 0) {
        perror("gpio/export");
        return fd;
    }
 
    len = snprintf(buf, sizeof(buf), "%d", gpio);
    write(fd, buf, len);
    close(fd);
 
    return 0;
}

/****************************************************************
 * gpio_unexport
 ****************************************************************/
int gpio_unexport(unsigned int gpio)
{
    int fd, len;
    char buf[MAX_BUF];
 
    fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
    if (fd < 0) {
        perror("gpio/export");
        return fd;
    }
 
    len = snprintf(buf, sizeof(buf), "%d", gpio);
    write(fd, buf, len);
    close(fd);
    return 0;
}

/****************************************************************
 * gpio_set_dir
 ****************************************************************/
int gpio_set_dir(unsigned int gpio, unsigned int out_flag)
{
    int fd;
    char buf[MAX_BUF];
 
    snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", gpio);
 
    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        perror("gpio/direction");
        return fd;
    }
 
    if (out_flag)
        write(fd, "out", 4);
    else
        write(fd, "in", 3);
 
    close(fd);
    return 0;
}

/****************************************************************
 * gpio_set_value
 ****************************************************************/
int gpio_set_value(unsigned int gpio, unsigned int value)
{
    int fd;
    char buf[MAX_BUF];
 
    snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
 
    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        perror("gpio/set-value");
        return fd;
    }
 
    if (value)
        write(fd, "1", 2);
    else
        write(fd, "0", 2);
 
    close(fd);
    return 0;
}

/****************************************************************
 * gpio_get_value
 ****************************************************************/
int gpio_get_value(unsigned int gpio, unsigned int *value)
{
    int fd;
    char buf[MAX_BUF];
    char ch;

    snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
 
    fd = open(buf, O_RDONLY);
    if (fd < 0) {
        perror("gpio/get-value");
        return fd;
    }
 
    read(fd, &ch, 1);

    if (ch != '0') {
        *value = 1;
    } else {
        *value = 0;
    }
 
    close(fd);
    return 0;
}


/****************************************************************
 * gpio_set_edge
 ****************************************************************/

int gpio_set_edge(unsigned int gpio, char *edge)
{
    int fd;
    char buf[MAX_BUF];

    snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/edge", gpio);
 
    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        perror("gpio/set-edge");
        return fd;
    }
 
    write(fd, edge, strlen(edge) + 1);
    close(fd);
    return 0;
}

/****************************************************************
 * gpio_fd_open
 ****************************************************************/

int gpio_fd_open(unsigned int gpio)
{
    int fd;
    char buf[MAX_BUF];

    snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
 
    fd = open(buf, O_RDONLY | O_NONBLOCK );
    if (fd < 0) {
        perror("gpio/fd_open");
    }
    return fd;
}

/****************************************************************
 * gpio_fd_close
 ****************************************************************/

int gpio_fd_close(int fd)
{
    return close(fd);
}

/**
* @brief 读取外部时钟
* @param 时钟变量指针
* @return 0成功, 否则失败
*/
int ExtClockRead(extclock_t *clock)
{
/*    AssertLog(-1==FidGpio, "invalid fid(%d)\n", FidGpio);

    if(sizeof(extclock_t) != read(FidGpio, (char *)clock, sizeof(extclock_t))) {
        ErrorLog("read ext clock fail\n");
        return 1;
    }
    else return 0;*/
/*    sysclock_t   sclock;
    if(SysClockRead(&sclock))
    {
        __android_log_print(ANDROID_LOG_INFO,"libsmarthome","read ext clock fail\n");
        return 1;
    }
    else
    {
        clock->year = sclock.year;
        clock->month = sclock.month;
        clock->day = sclock.day;
        clock->hour = sclock.hour;
        clock->minute = sclock.minute;
        clock->second = sclock.second;
        clock->week = sclock.week;
    }*/
    return 0;
}

/**
* @brief 设置外部时钟
* @param 时钟变量指针
* @return 0成功, 否则失败
*/
int ExtClockWrite(const extclock_t *clock)
{
/*    AssertLog(-1==FidGpio, "invalid fid(%d)\n", FidGpio);

    if(sizeof(extclock_t) != write(FidGpio, (const char *)clock, sizeof(extclock_t))) {
        ErrorLog("set ext clock fail\n");
        return 1;
    }
    else
    */
    system("hwclock --systohc");
    return 0;
}

/**
* @brief 读取驱动程序版本信息
*        版本信息如下:主版本号(1), 次版本号(1), 发布年月日(3)(BCD格式), 空(1)
* @param buf 缓存区指针
* @param len 缓存区长度
* @return 成功返回读取长度(6字节), 否则返回-1
*/
int ReadDriverVersion(unsigned char *buf, int len)
{

    memset(buf,0,6);

    return 6;
}
/**
* @brief 初始化GpioInit端口
* @return 0成功, 否则失败
*/
DECLARE_INIT_FUNC(GpioInit);
/*by ydl modify 2011-05-04*/

int GpioInit(void)
{

    gpio_export(GPIO_42);
    gpio_set_dir(GPIO_42, 1);
    gpio_set_value(GPIO_42,1);

    gpio_export(GPIO_39);
    gpio_set_dir(GPIO_39, 1);
    gpio_set_value(GPIO_39,0);

    gpio_export(GPIO_LED_NET);
    gpio_set_dir(GPIO_LED_NET, 1);
    gpio_set_value(GPIO_LED_NET,0);

    gpio_export(GPIO_LED_SYS);
    gpio_set_dir(GPIO_LED_SYS, 1);
    gpio_set_value(GPIO_LED_SYS,1);
    SET_INIT_FLAG(GpioInit);

    return 0;
}

