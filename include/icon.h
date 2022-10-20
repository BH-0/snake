//
// Created by 10031 on 2022/9/30.
//

#ifndef __ICON_H
#define __ICON_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/mman.h>
#include <stdlib.h>
#include "lcd.h"
#include "bmp.h"
#include "touch.h"

//图标名称
#define START_IN 1
#define START_OUT 2

typedef struct bmp_icon{
    bmp_t *bmp;  //图标图片
    int name; //图标名称
    int x; //图标右上角位置
    int y;  //图标右上角位置
    struct bmp_icon *next; //下一个图标
}bmp_icon;

//图标管理结构体
struct list_icon{
    struct bmp_icon *head;//保存链表的数据首结点的位置
    struct bmp_icon *tail;//保存链表的数据尾结点的位置
    int icon_number;//保存链表结点的数量
};

//图标管理结构体指针全局变量
extern struct list_icon *start_in;
extern struct list_icon *start_out;


//批量初始化图标，入口参数：管理结构体指针，图标列表文件路径
void icon_init(struct list_icon *listHead,char * txt_path);

//初始化图标管理结构体
struct list_icon *create_list_icon();

//打开一张图片，插入到链表后，入口参数：图标管理结构体，路径，图标名称，返回值指向图标结构体
bmp_icon *insert_list_icon(struct list_icon *listHead, char *path, int name, int x, int y);

//寻找某个图标，返回此图标结构体指针
bmp_icon *find_icon(struct list_icon *listHead, int name);

//显示单个图标，入口参数：LCD映射地址，图标结构体
void display_icon(int (*LCD_addr)[800], bmp_icon *icon);

//显示单个或一串图标，入口参数：LCD映射地址，图标管理结构体，某个图标名称（如果为空，则全部显示）
int display_icons(int (*LCD_addr)[800], struct list_icon *listHead, int name);

//检测是否有按动触发图标，入口参数：图标管理结构体，触摸坐标，返回值：图标名称
int touch_button(struct list_icon *listHead, int x, int y);

//销毁一组图标，入口参数：图标管理结构体
void del_icon(struct list_icon *listHead);

#endif //__ICON_H
