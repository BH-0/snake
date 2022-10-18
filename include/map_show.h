#ifndef __MAP_SHOW_H
#define __MAP_SHOW_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "lcd.h"

#define SNAKE_SIZE 20   //蛇身大小,一定要为480和800的公因数
#define MAP_HEIGHT (480*3)      //地图高
#define MAP_WIDTH  (800*3)   //地图宽


/*全局声明*/
extern int map_buffer[MAP_HEIGHT][MAP_WIDTH];    //地图buffer
extern struct xy show_xy;   //鸟瞰坐标
extern pthread_rwlock_t map_buffer_rwlock; //读写锁

/*函数声明*/
//地图鸟瞰显示线程
//入口参数：起始坐标
void *map_showing_task(void *arg);

//地图初始化函数
void map_show_init(void);

//鸟瞰坐标修改
void map_show_xy(int x, int y);

#endif //__MAP_SHOW_H