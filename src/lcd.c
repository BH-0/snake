#include "lcd.h"
//----------------------------------------------------------------
//此为LCD屏幕显示处理程序
//----------------------------------------------------------------

//显示屏映射地址全局变量
lcd_type LCD_addr = NULL;

//入口参数，文件描述符，返回值，映射内存地址
lcd_type LCD_init(void)
{
    //打开LCD
    int lcd_fd = open("/dev/fb0",O_RDWR);
    if(lcd_fd < 0)
    {
        perror("open lcd fail");
        return NULL;
    }

    //LCD的映射
    lcd_type LCD_addr;//定义int *类型，指针指向的是一个像素点
    LCD_addr = mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,lcd_fd,0);
    if(LCD_addr == NULL)
    {
        perror("mmap fail");
        close(lcd_fd);
        return NULL;
    }
    return LCD_addr;
}
