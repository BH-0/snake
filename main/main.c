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
    pthread_t map_show_film;    //鸟瞰显示线程
    pthread_create(&map_show_film,NULL,map_show_film_task,NULL);



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

    print_allToList(snake); //打印
    printf("\n");
    snake_t *p = snake->head;
    for(int i=0; i<snake->nodeNumber; i++)
    {
        snake_show(p,1);
        p = p->next;
    }

    sleep(1);
    int i = 0;
    char buf = {0};
    while(1)//350
    {
        //usleep(20000);
        usleep(10);
        scanf("%c",&buf);
        if(buf == 'w' && snake->tail->next_direction != s_down)
            snake->tail->next_direction = s_up;
        if(buf == 'a' && snake->tail->next_direction != s_right)
            snake->tail->next_direction = s_left;
        if(buf == 's' && snake->tail->next_direction != s_up)
            snake->tail->next_direction = s_down;
        if(buf == 'd' && snake->tail->next_direction != s_left)
            snake->tail->next_direction = s_right;
//        if(i==10)
//            snake->tail->next_direction = s_down;
//        else if(i==68)
//            snake->tail->next_direction = s_right;
//        else if(i==162)
//            snake->tail->next_direction = s_up;
//        else if(i==231)
//            snake->tail->next_direction = s_left;
        enter_snake_t(snake);   //尾插
        snake_show(snake->tail,1);  //显示头
        snake_show(snake->head,0);  //消除尾
        del_snake_t(snake); //头删

        i++; //
    }
    printf("end\n");
#endif

#if 1 //进入主线程
    for(;;)
    {

    }
#endif
    return 0;
}
