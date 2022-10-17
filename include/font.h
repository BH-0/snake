#ifndef __FONT__
#define __FONT__

#include "utf8_gb2312.h"

extern int Init_Font(void);  //显示字体库前先调用本函数进行初始化

extern void UnInit_Font(void); //程序退出前，调用本函数

extern int Clean_Area(int X,//x坐标起始点
                 int Y,//y坐标起始点
				 int width,//绘制的宽度
				 int height,//绘制的高度
				 unsigned long color);//往屏幕指定区域填充颜色
				 
extern int Display_characterX(unsigned int x,//x坐标起始点
                         unsigned int y,//y坐标起始点
						 unsigned char *string,//GB2312 中文字符串
						 unsigned int color ,//字体颜色值
						 int size);//字体放大倍数 1~8

//重新封装字库函数，使其支持utf-8字符串
int Display_utf8(unsigned int x,//x坐标起始点
                 unsigned int y,//y坐标起始点
                 const char *UTF8,//UTF8 中文字符串
                 unsigned int color ,//字体颜色值
                 int size,//字体放大倍数 1~8
                 int shadow);//文字阴影 0关闭 1开启
#endif