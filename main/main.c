#include "main.h"

void *menu_init(void)
{
    static int buffer[7][480][800] = {0};
    struct list_icon *menu_t = create_list_icon();  //初始化按钮图标管理结构体
    icon_init(menu_t, "./menu/menu.txt"); //导入图标组
    bmp_icon *p = menu_t->head;
    int x,y;
    for (int i = 0; i < 7; i++)
    {
        //装填
        for(y=0;y<480;y++)
        {
            for(x=0;x<800;x++)
            {
                char (*bmp_buf1)[800*3] = (void *)p->bmp->bmp_buf;
                buffer[i][y][x] = bmp_buf1[y][x*3] | bmp_buf1[y][x*3+1]<<8 | bmp_buf1[y][x*3+2]<<16;
            }
        }
        p= p->next;
    }
    return (void *)buffer;
}



int main(int argc, char **argv)
{
    /*硬件初始化*/
    LCD_addr = LCD_init();  //初始化LCD显示
    //fd_ts = touch_init("/dev/input/event0");    //初始化触摸屏
    /*字库初始化*/
    Init_Font();
//    /*地图初始化*/
//    map_show_init();
//    /*贪吃蛇初始化*/
//    snake = create_list();  //初始化贪吃蛇链表
//    food_init(); //食物初始化

    /*创建线程*/
    pthread_t scan_keyboard_t;    //键盘监听线程
    pthread_create(&scan_keyboard_t,NULL,scan_keyboard_task,NULL);
//    pthread_t map_showing_t;    //鸟瞰显示线程
//    pthread_create(&map_showing_t,NULL,map_showing_task,NULL);
//    pthread_t map_show_film;    //边界镜头跟拍线程
//    pthread_create(&map_show_film,NULL,map_show_film_task,NULL);
//    pthread_t snake_task_t;    //贪吃蛇主线程
//    pthread_create(&snake_task_t,NULL,snake_task,NULL);

    int (*buffer)[480][800] = menu_init();
    int i;
#if 1 //进入主线程（菜单控制）
    for(;;)
    {
        for(i=0; i<7; i++) {
            memcpy(LCD_addr,buffer[i],800*480*4);
            usleep(50000);
        }
    }
#endif
    return 0;
}
