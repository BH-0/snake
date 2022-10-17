#include "font.h"
//----------------------------------------------------------------
//此为字库相关程序
//----------------------------------------------------------------

//重新封装字库函数，使其支持utf-8字符串
int Display_utf8(unsigned int x,//x坐标起始点
                 unsigned int y,//y坐标起始点
                 const char *UTF8,//UTF8 中文字符串
                 unsigned int color ,//字体颜色值
                 int size,//字体放大倍数 1~8
                 int shadow)//文字阴影 0关闭 1开启
{
    unsigned char temp[256] = {0};

    Utf8ToGb2312(UTF8, strlen(UTF8), temp); //转码
    if(shadow != 0)
        Display_characterX(x+1,y+1,temp,0,size);    //显示阴影
    return Display_characterX(x,y,temp,color,size);
}
