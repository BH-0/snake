#include "keyboard.h"
//----------------------------------------------------------------
//此程序专门用于键盘按键监听
//----------------------------------------------------------------
/*按键全局变量声明*/
char scan_keyboard = 0;

//键盘监听线程，监听shell窗口键盘按键输入
void *scan_keyboard_task(void *arg)
{
    pthread_detach(pthread_self()); //分离属性
    for(;;) {
        printf("         \033[?25l\033[%dD",20);  //锁定光标
        struct termios new_settings;
        struct termios stored_settings;
        tcgetattr(0, &stored_settings);            // 读取原始配置信息
        new_settings = stored_settings;
        new_settings.c_lflag &= (~ICANON);        // 屏蔽整行缓存，不需要回车，输入单个字符即可输出
        new_settings.c_cc[VTIME] = 0;
        new_settings.c_cc[VMIN] = 1;
        tcsetattr(0, TCSANOW, &new_settings);    // 设置新的配置信息

        scan_keyboard = getchar();

        tcsetattr(0, TCSANOW, &stored_settings);    // 恢复原始配置信息
    }
}