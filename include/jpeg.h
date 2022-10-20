//
// Created by 10031 on 2022/10/3.
//

#ifndef __JPEG_H
#define __JPEG_H
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include "jpeglib.h"
#include <sys/stat.h>
#include "bmp.h"

#define LCD_WIDTH  			800
#define LCD_HEIGHT 			480
#define FB_SIZE				(LCD_WIDTH * LCD_HEIGHT * 4)


//打开一张图片，入口参数路径，返回值指向图片管理结构体
bmp_t *open_jpeg(char *pjpg_path);

#endif //__JPEG_H
