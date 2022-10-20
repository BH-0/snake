#ifndef __BMP_H
#define __BMP_H

#include <stdio.h>
/*open*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/*close read write*/
#include <unistd.h>
/*mmap*/
#include <sys/mman.h>
#include <stdlib.h>
#include <math.h>   //编译时需要链接数学库 -lm
#include "lcd.h"
#include "touch.h"

//一维数组坐标转换
#define BMP_BUF(y,x) (*(bmp->bmp_buf + (y)*bmp->width*3 + (x)))


typedef struct{
    char *path; //路径
    int height;  //高
    int width;  //宽
    char *bmp_buf; //数组指针
}bmp_t;

//全局触摸坐标
extern int x_ts;
extern int y_ts;


//打开一张图片，入口参数路径，返回值指向首字节指针，从上至下扫描
bmp_t *open_bmp(char *path);

//显示图片，入口参数：LCD映射地址，bmp图片地址
void show_bmp(int (*LCD_addr)[800], bmp_t *bmp, int x_start, int y_start);

//从图片1至图片2的过渡动画(往右滚动),速度从0.01~1，1为全速,direction:方向：0从左到右 1从右到左
//只能传入800*480的图片
void show_bmp_plus(int (*LCD_addr)[800], char (*bmp_buf1)[800*3], char (*bmp_buf2)[800*3], float speed, int direction);

//销毁一个图片结构体
void destroy_bmp_t(bmp_t *pic);

#endif
