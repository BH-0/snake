//
// Created by 10031 on 2022/9/30.
//

#ifndef __PICTURE_H
#define __PICTURE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*open*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
/*close read write*/
#include <unistd.h>
/*mmap*/
#include <sys/mman.h>
#include <stdlib.h>
#include <math.h>   //编译时需要链接数学库 -lm
#include "file.h"
#include "lcd.h"
#include "bmp.h"
#include "jpeg.h"

#define BMP_TYPE 0x4d42
#define JPEG_TYPE 0xd8ff

//一维数组坐标转换
//#define PIC_BUF(y,x) (*(pic->bmp_buf + (y)*pic->width*3 + (x)))
//#define PIC_RE_BUF(y,x) (*(pic_re->bmp_buf + (y)*pic_re->width*3 + (x)))
//#define w3(w) ((int)((w)+0.5)-(((int)((w)+0.5))%3))

#define BG_COLOR 0xfaf6e9

//双向链表成员节点
typedef struct pic_t{
    bmp_t *pic;
    char find_name[256];    //图片文件名
    struct pic_t *prev; //存储是上一个结点的地址
    struct pic_t *next; //存储是下一个结点的地址
}pic_t;

//双向链表管理结构体
struct pic_link{
    struct pic_t *head;//保存链表的数据首结点的位置
    struct pic_t *tail;//保存链表的数据尾结点的位置
    int nodeNumber;//保存链表结点的数量
};

//图库管理结构体全局变量
extern struct pic_link *gallery;

//任意尺寸重建图片
//可将小图片等比例变大或大图片等比例缩小,注意及时free返回指针
//入口参数：图片管理结构体，指定图片大小，背景填充颜色
//返回值：一个新的图片结构体
bmp_t *pic_rebuild_pro(bmp_t *pic, unsigned int width, unsigned int height, unsigned int bgcolor);

//识别图片文件类型
int pic_find_identify(char *path);

//新建节点
pic_t *new_pic(bmp_t *pic, char *find_name);

//有序插入
void insert_list_sort(struct pic_link *listHead, pic_t *newPic);

//创建一条图片双向链表并初始化
struct pic_link *pic_link_init(void);

//链表遍历
void print_allToList(struct pic_link *listHead);

#endif //__PICTURE_H
