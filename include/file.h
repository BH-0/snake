//
// Created by 10031 on 2022/10/9.
//

#ifndef __FILE_H
#define __FILE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <linux/input.h>
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
#include "picture.h"

//文件双向链表节点
typedef struct file_t {
    void *p;    //内存占位用
    char find_name[256]; //文件名
    struct file_t *prev;
    struct file_t *next;
}file_t;

struct file_link{
    file_t *head;
    file_t *tail;
    int nodeNumber;//保存链表结点的数量
};

//文件名排序前比较算法
//入口参数：对比对象，与被对比对象
//返回值：大于diff_name返回0，小于diff_name返回1，同名返回-1
//排序是大的放后面，小的放前面，有序插入时直接遍历找到第一个比他大的，并插在他前面
int find_name_diff(char *object_name, char *diff_name);

//读取某文件的文件名
//入口参数：文件路径
char *get_find_name(char *find_path);

//文件后缀判断函数
int file_suffix_judgment_pro(char *filename,char *file_suffix);

//创建一条文件双向链表并初始化
//入口参数：目录路径，文件后缀
struct file_link *file_link_init(char *directory_path,char *file_suffix);

//链表遍历并打印
void print_allToFileList(struct file_link *listHead);

//销毁链表
void destroy_file_link(struct file_link *listHead);

#endif //__FILE_H
