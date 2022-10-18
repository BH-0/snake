#include "main.h"

int main(int argc, char **argv)
{
    /*硬件初始化*/
    LCD_addr = LCD_init();  //初始化LCD显示
    fd_ts = touch_init("/dev/input/event0");    //初始化触摸屏
    /*字库初始化*/
    Init_Font();
    /*地图初始化*/
    map_show_init();

    /*创建线程*/
    pthread_t map_showing_t;    //鸟瞰显示线程
    pthread_create(&map_showing_t,NULL,map_showing_task,NULL);

#if 1
    for(int i=0; i<SNAKE_SIZE; i++)
    {
        pthread_rwlock_wrlock(&map_buffer_rwlock);
        for (int j=0; j<SNAKE_SIZE; j++) {

            map_buffer[400 + i][700 + j] = 0xffffff;
        }
        pthread_rwlock_unlock(&map_buffer_rwlock);
    }
    for(int i=0; i<SNAKE_SIZE; i++)
    {
        pthread_rwlock_wrlock(&map_buffer_rwlock);
        for (int j=0; j<SNAKE_SIZE; j++) {

            map_buffer[400 + i][700 + j] = 0;
        }
        pthread_rwlock_unlock(&map_buffer_rwlock);
    }



#endif

#if 1 //进入主线程
    for(;;)
    {

    }
#endif
    return 0;
}
