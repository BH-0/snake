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
    /*贪吃蛇初始化*/
    snake = create_list();  //初始化贪吃蛇链表

    /*创建线程*/
    pthread_t map_showing_t;    //鸟瞰显示线程
    pthread_create(&map_showing_t,NULL,map_showing_task,NULL);
    pthread_t map_show_film;    //边界镜头跟拍线程
    pthread_create(&map_show_film,NULL,map_show_film_task,NULL);
    pthread_t scan_keyboard_t;    //键盘监听线程
    pthread_create(&scan_keyboard_t,NULL,scan_keyboard_task,NULL);
    pthread_t snake_task_t;    //贪吃蛇主线程
    pthread_create(&snake_task_t,NULL,snake_task,NULL);


#if 1
    for(int i=0; i<SNAKE_SIZE; i++) //食物
    {
        pthread_rwlock_wrlock(&map_buffer_rwlock);
        for (int j=0; j<SNAKE_SIZE; j++) {

            map_buffer[400 + i][700 + j] = 0xff0000;
        }
        pthread_rwlock_unlock(&map_buffer_rwlock);
    }
    for(int i=0; i<SNAKE_SIZE; i++) //食物
    {
        pthread_rwlock_wrlock(&map_buffer_rwlock);
        for (int j=0; j<SNAKE_SIZE; j++) {

            map_buffer[440 + i][700 + j] = 0xff0000;
        }
        pthread_rwlock_unlock(&map_buffer_rwlock);
    }
    for(int i=0; i<SNAKE_SIZE; i++) //食物
    {
        pthread_rwlock_wrlock(&map_buffer_rwlock);
        for (int j=0; j<SNAKE_SIZE; j++) {

            map_buffer[440 + i][740 + j] = 0xff0000;
        }
        pthread_rwlock_unlock(&map_buffer_rwlock);
    }
    for(int i=0; i<SNAKE_SIZE; i++) //食物
    {
        pthread_rwlock_wrlock(&map_buffer_rwlock);
        for (int j=0; j<SNAKE_SIZE; j++) {

            map_buffer[400 + i][740 + j] = 0xff0000;
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
