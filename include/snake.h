//
// Created by 10031 on 2022/10/18.
//
#ifndef __SNAKE_H
#define __SNAKE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "map_show.h"
#include "keyboard.h"
#include "font.h"
#include "bmp.h"
#include "beep.h"
#include "icon.h"


//方向
enum direction{s_up,s_down,s_left,s_right};

//贪吃蛇链表节点
typedef struct snake_t{
    int x;  //左上角坐标,此坐标为地图像素实际坐标
    int y;  //左上角坐标,此坐标为地图像素实际坐标
    int color;  //蛇身颜色
    enum direction next_direction;  //下次蛇身生成的位置
    struct snake_t *prev;   //上一节点
    struct snake_t *next;   //下一节点
}snake_t;

//蛇身链表管理结构体
struct snake_list{
    snake_t *head;//保存链表的数据首结点的位置
    snake_t *tail;//保存链表的数据尾结点的位置
    int nodeNumber;//保存链表结点的数量
};

/*外部声明*/
extern struct snake_list *snake;
extern pthread_rwlock_t snake_list_rwlock; //读写锁
extern int game_points;
extern int game_points_list[10]; //积分表
extern int SNAKE_COLOR; //蛇身颜色

/*函数声明*/
//读写积分表
void game_points_rd(void);
//写积分表
void game_points_wr(void);
//有序插入积分表
//比列表大的数才会被插入
int game_points_insert(int data);
//显示积分列表
void game_points_list_show();
//显示简介
void about(void);
//显示设置
void set_menu(void);
//食物初始化
void food_init(void);
//显示和消除蛇身（4方向扫描）
//入口参数：想显示或消除的蛇身，1显示，0消除
void snake_show(snake_t *node, int cmd);

//snake主控线程
void *snake_task(void *arg);

//新建一个结点并初始化
//函数参数：管理结构体，节点数据
snake_t *new_node(struct snake_list *listHead,
                  int x,int y,int color,
                  enum direction next_direction);

//创建一条双向链表并初始化
struct snake_list *create_list();

//打印蛇身参数
void print_allToList_s(struct snake_list *listHead);

//尾插(链表不得为空)
void enter_snake_t(struct snake_list *listHead);

//头删(链表不得为空)
void del_snake_t(struct snake_list *listHead);

//销毁链表
void destory_list(struct snake_list *listHead);

//最高饱和度彩虹色每调用一次颜色加1
int rainbow(void);

#endif //__SNAKE_H
