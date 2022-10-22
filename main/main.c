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
    /*读出积分表*/
    game_points_rd();

    /*创建线程*/
    pthread_t scan_keyboard_t;    //键盘监听线程
    pthread_create(&scan_keyboard_t,NULL,scan_keyboard_task,NULL);

    /*初始化蜂鸣器*/
    beep_init();
#if 0

    for(;;);
#endif

    int (*buffer)[480][800] = menu_init();
    int i;
    struct list_icon *key1_t = create_list_icon();  //初始化按钮图标管理结构体
    icon_init(key1_t, "./menu/key1.txt"); //导入图标组
    char key_buf = 0;   //键盘按键
    int key_bit = 1;    //菜单选项buf
#if 1 //进入主线程（菜单控制）
    for(;;)
    {
        for(;;) {
            for (i = 0; i < 7; i++) {
                /*判断按键*/
                if (scan_keyboard == 'w' || scan_keyboard == 'a' || scan_keyboard == 's' || scan_keyboard == 'd' ||
                    scan_keyboard == '\n') {
                    key_buf = scan_keyboard;
                    scan_keyboard = 0;
                    beep_cmd = 3;   //按钮声
                }
                if (key_buf == 's' || key_buf == 'd') {
                    key_bit++;
                    key_bit = (key_bit > 4 ? 1 : key_bit);
                } else if (key_buf == 'w' || key_buf == 'a') {
                    key_bit--;
                    key_bit = (key_bit < 1 ? 4 : key_bit);
                }
                if (key_buf != '\n') {
                    key_buf = 0;
                    /*显示背景*/
                    memcpy(LCD_addr, buffer[i], 800 * 480 * 4);

                    /*显示按键*/
                    display_icons(LCD_addr, key1_t, key_bit);
                    switch (key_bit) {
                        case 1:
                            Display_utf8(363, 150, "开始游戏", 0xff8faf, 1, 0);
                            break;
                        case 2:
                            Display_utf8(363, 212, "游戏简介", 0xff8faf, 1, 0);
                            break;
                        case 3:
                            Display_utf8(363, 275, "游戏设置", 0xff8faf, 1, 0);
                            break;
                        case 4:
                            Display_utf8(363, 336, "积分排行", 0xff8faf, 1, 0);
                            break;
                    }
                } else   //回车进入对应功能
                {
                    if (key_bit == 1) {
                        beep_cmd = 3;   //按钮声
                        break;  //开始游戏
                    }
                    else if (key_bit == 2)
                    {
                        beep_cmd = 3;   //按钮声
                        key_buf = 0;
                        about();    //简介
                        beep_cmd = 3;   //按钮声
                    }
                    else if (key_bit == 3)
                    {
                        beep_cmd = 3;   //按钮声
                        key_buf = 0;
                        set_menu();    //设置
                        beep_cmd = 3;   //按钮声
                    }
                    else if (key_bit == 4)
                    {
                        beep_cmd = 3;   //按钮声
                        key_buf = 0;
                        game_points_list_show();    //显示积分表
                        beep_cmd = 3;   //按钮声
                    }
                }
                usleep(80000);
            }
            if(key_buf == '\n' && key_bit == 1){
                key_buf = 0;
                break;
            }
        }

        /*********进入主程序********/
        system("mplayer -af volume=-10 -softvol -softvol-max 80 \
                            -geometry 0:0 -zoom -x 800 -y 480 -cache 8192 -slave \
                           -quiet ./menu/1_Trim.mp4 "); //过场动画

        game_points = 0;    //积分清零
        /*地图初始化*/
        map_show_init();
        /*贪吃蛇初始化*/
        snake = create_list();  //初始化贪吃蛇链表
        food_init(); //食物初始化

        /*创建线程*/
        pthread_t map_showing_t;    //鸟瞰显示线程
        pthread_create(&map_showing_t,NULL,map_showing_task,NULL);
        pthread_t map_show_film;    //边界镜头跟拍线程
        pthread_create(&map_show_film,NULL,map_show_film_task,NULL);
        pthread_t snake_task_t;    //贪吃蛇主线程
        pthread_create(&snake_task_t,NULL,snake_task,NULL);

        /*游戏结束*/
        pthread_join(snake_task_t,NULL);    //等待游戏结束
        /*抢锁*/
        pthread_rwlock_wrlock(&map_buffer_rwlock);  //写锁
        pthread_rwlock_wrlock(&snake_list_rwlock);  //写锁
        sleep(2);   //三秒后退出
        //取消其他线程
        pthread_cancel(map_show_film);  //取消跟拍
        pthread_cancel(map_showing_t);  //取消鸟瞰
        pthread_rwlock_unlock(&snake_list_rwlock);  //解锁
        pthread_rwlock_unlock(&map_buffer_rwlock);  //解锁

        /*资源销毁*/
        destory_list(snake);    //销毁蛇
        snake = NULL;

        system("mplayer -af volume=-10 -softvol -softvol-max 80 \
                            -geometry 0:0 -zoom -x 800 -y 480 -cache 8192 -slave \
                           -quiet ./menu/2_Trim.mp4 "); //过场动画


        //写入积分表
        if(game_points_insert(game_points) == 0)
            game_points_wr();
        game_points_list_show();    //显示积分表

        show_xy.x = 0;  //鸟瞰坐标清零
        show_xy.y = 0;
        printf("end\n");
        //for(;;);
    }
#endif
    return 0;
}
