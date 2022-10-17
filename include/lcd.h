//
// Created by 10031 on 2022/9/29.
//

#ifndef __LCD_H
#define __LCD_H
#include <stdio.h>
#include <sys/mman.h>
/*open*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/*close read write*/
#include <unistd.h>

typedef int (*lcd_type)[800] ;

//显示屏映射地址全局变量
extern lcd_type LCD_addr;

//入口参数，文件描述符，返回值，映射内存地址
lcd_type LCD_init(void);

#endif //__LCD_H
