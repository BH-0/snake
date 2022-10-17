#ifndef __TOUCH_H
#define __TOUCH_H
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>//触摸屏的头文件

#define test 0

enum SLIDE
{
    click,
    left,
    right,
    up,
    down
};

//触摸文件描述符全局变量（用于触摸坐标读取）
extern int fd_ts;

//初始化触摸
int touch_init(char *path);

//获取触摸坐标：入口参数：触摸文件描述符，xy坐标，返回值：1按下，0松开
//阻塞函数
int get_xy(int fd, int *x_ts, int *y_ts);

//获取触摸坐标：入口参数：触摸文件描述符，xy坐标，返回值：滑动或点击
//阻塞函数
enum SLIDE get_xy_plus(int fd, int *x_ts, int *y_ts);

#endif 
