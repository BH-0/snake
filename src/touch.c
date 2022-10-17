#include "touch.h"
//----------------------------------------------------------------
//此为触摸屏触摸处理程序
//----------------------------------------------------------------
//触摸文件描述符全局变量（用于触摸坐标读取）
int fd_ts = 0;

//初始化触摸
int touch_init(char *path)
{
    //打开触摸屏
    int fd;
    fd=open(path,O_RDWR);//要用系统IO open函数
    if(fd <0)
    {
        perror("open ts fail");
    }
    return fd;
}

//获取触摸坐标：入口参数：触摸文件描述符，xy坐标，返回值：1按下，0松开
//阻塞函数
int get_xy(int fd, int *x_ts, int *y_ts)
{
    struct input_event ts;
        while(1)
        {
            //读触摸屏信息--阻塞函数（点击触摸屏之后才会往下执行）
            read(fd,&ts,sizeof(struct input_event));
            if(ts.type==EV_ABS && ts.code==ABS_X)
            {
                *x_ts = ts.value*800/1024;
            }
            if(ts.type==EV_ABS && ts.code==ABS_Y)
            {
                *y_ts = ts.value*480/600;
            }
            if(ts.type==EV_KEY && ts.code== BTN_TOUCH && ts.value==0)//松发
            {
#if test
                printf("start:x=%d, y=%d\n", *x_ts, *y_ts);//坐标值
#endif
                return 0;
            }
            if(ts.type==EV_KEY && ts.code== BTN_TOUCH && ts.value==1)//按发
            {
#if test
                printf("end:x=%d, y=%d\n",*x_ts, *y_ts);//坐标值
#endif
                return 1;
            }
        }
}

//获取触摸坐标：入口参数：触摸文件描述符，xy坐标，返回值：滑动或点击
//阻塞函数
enum SLIDE get_xy_plus(int fd, int *x_ts, int *y_ts)
{
    struct input_event ts;
    int x_start = 0;
    int x_end = 0;
    int y_start = 0;
    int y_end = 0;
    while(1)
    {
        while(1)
        {
            //读触摸屏信息--阻塞函数（点击触摸屏之后才会往下执行）
            read(fd,&ts,sizeof(struct input_event));
            if(ts.type==EV_ABS && ts.code==ABS_X)
            {
                *x_ts = ts.value*800/1024;
            }
            if(ts.type==EV_ABS && ts.code==ABS_Y)
            {
                *y_ts = ts.value*480/600;
            }
            if(ts.type==EV_KEY && ts.code== BTN_TOUCH && ts.value==1)//按发
            {
#if test
                printf("start:x=%d, y=%d\n", *x_ts, *y_ts);//坐标值
#endif
                x_start = *x_ts;
                y_start = *y_ts;
                break;
            }

        }
        while(1)
        {
            //读触摸屏信息--阻塞函数（点击触摸屏之后才会往下执行）
            read(fd,&ts,sizeof(struct input_event));
            if(ts.type==EV_ABS && ts.code==ABS_X)
            {
                *x_ts = ts.value*800/1024;
            }
            if(ts.type==EV_ABS && ts.code==ABS_Y)
            {
                *y_ts = ts.value*480/600;
            }
            if(ts.type==EV_KEY && ts.code== BTN_TOUCH && ts.value==0)//松发
            {
#if test
                printf("end:x=%d, y=%d\n",*x_ts, *y_ts);//坐标值
#endif
                x_end = *x_ts;
                y_end = *y_ts;
                break;
            }
        }
        if (x_end - x_start > 100)
            return right;
        else if (x_end - x_start < -100)
            return left;
        else if (y_end - y_start > 100)
            return down;
        else if (y_end - y_start < -100)
            return up;
        else
            return click;
    }
}




