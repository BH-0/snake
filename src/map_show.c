#include "map_show.h"
//----------------------------------------------------------------
//此为地图处理程序
//----------------------------------------------------------------
pthread_rwlock_t map_buffer_rwlock; //读写锁
int map_buffer[MAP_HEIGHT][MAP_WIDTH]; //地图buffer 3*3

/*鸟瞰坐标*/
struct xy show_xy = {0};

//读写锁死锁保护函数
static void handler(void *arg)
{
    printf("[%u] is cancelled.\n",(unsigned)pthread_self());
    pthread_rwlock_t *pm = (pthread_rwlock_t*)arg;
    pthread_rwlock_unlock(pm);
}

//地图鸟瞰显示线程
//入口参数：起始坐标
void *map_showing_task(void *arg)
{
    static char key_buf = 0;
    char buf[128] = {0};
    pthread_detach(pthread_self()); //分离属性
    //当线程收到取消请求之后，立即响应,不需要遇到取消点
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    pthread_cleanup_push(handler,(void*)&map_buffer_rwlock);    //压栈
        int i;
        for (;;)
        {
            pthread_rwlock_rdlock(&map_buffer_rwlock);  //读锁
            for(i = 0; i < 480; i++)
            {
                memcpy(LCD_addr[i], &map_buffer[i+show_xy.y][show_xy.x], sizeof(int)*800);
            }
            pthread_rwlock_unlock(&map_buffer_rwlock);  //解锁

            /*这里可尝试挂起一点点时间,或者显示其他东西*/
            //积分显示
            sprintf(buf,"%d",game_points);
            if(game_points>=50)
            {
                Display_utf8(15,20,buf,0xff0000,2,0);   //红色
            }else if(game_points>=25)
            {
                Display_utf8(15,20,buf,0xffcc00,2,0);   //橙黄色
            }else
            {
                Display_utf8(15,20,buf,0x009432,2,0);   //绿色
            }
            if(scan_keyboard == 'w' || scan_keyboard == 'a' || scan_keyboard == 's' || scan_keyboard == 'd')
                key_buf = scan_keyboard;
            //键盘方向显示
            if(key_buf == 'w')
                Display_utf8(15,420,"↑",0x00ffff,2,0);
            if(key_buf == 'a')
                Display_utf8(15,420,"←",0x00ffff,2,0);
            if(key_buf == 's')
                Display_utf8(15,420,"↓",0x00ffff,2,0);
            if(key_buf == 'd')
                Display_utf8(15,420,"→",0x00ffff,2,0);


            usleep(30000);//刷新率控制30ms刷新一帧
        }
    pthread_cleanup_pop(0);    //弹栈，释放保护函数，但不执行此函数
}

//鸟瞰坐标修改
void map_show_xy(int x, int y)
{
    int x_f = 0;
    int y_f = 0;
//    x_f = x<0?0:x;
//    x_f = (x>(MAP_WIDTH-800))?(MAP_WIDTH-800):x;
//    y_f = y<0?0:y;
//    y_f = (y>(MAP_HEIGHT-480))?(MAP_HEIGHT-480):y;
    if(x<0)
        x_f = 0;
    else if(x>(MAP_WIDTH-800))
        x_f = (MAP_WIDTH-800);
    else
        x_f = x;
    if(y<0)
        y_f = 0;
    else if(y>(MAP_HEIGHT-480))
        y_f = (MAP_HEIGHT-480);
    else
        y_f = y;
    pthread_cleanup_push(handler,(void*)&map_buffer_rwlock);    //压栈
    pthread_rwlock_wrlock(&map_buffer_rwlock);  //写锁
    show_xy.x = x_f;
    show_xy.y = y_f;
    pthread_rwlock_unlock(&map_buffer_rwlock);  //解锁
    pthread_cleanup_pop(0);    //弹栈，释放保护函数，但不执行此函数
}

//边界镜头跟拍线程
void *map_show_film_task(void *arg)
{
    pthread_detach(pthread_self()); //分离属性
    //当线程收到取消请求之后，立即响应,不需要遇到取消点
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    int pdw = 100;  //跟拍边界与刷新宽度（必须为蛇身倍数）
    enum direction ls;  //镜头移动方向
    //enum direction ds;  //蛇头移动方向
    int x_s,y_s;    //蛇头坐标
    int x_f,y_f;    //鸟瞰坐标
    printf("x=%d y=%d\n",show_xy.x,show_xy.y);

    for(;;)
    {
        for(;;)
        {
            pthread_cleanup_push(handler,(void*)&snake_list_rwlock);    //压栈
            pthread_rwlock_rdlock(&snake_list_rwlock);  //读锁
            x_s = snake->tail->x;
            y_s = snake->tail->y;
            x_f = show_xy.x;
            y_f = show_xy.y;
            //ds = snake->tail->next_direction; //读取下一  次方向
            pthread_rwlock_unlock(&snake_list_rwlock);  //解锁
            pthread_cleanup_pop(0);    //弹栈，释放保护函数，但不执行此函数

            if(x_s < show_xy.x+pdw && x_f>=1)
            {
                printf("s_left x_s = %d, y_s = %d\n",x_s,y_s);
                ls = s_left;
                break;
            }
            if(x_s > show_xy.x+800-pdw && x_f<MAP_WIDTH-800)
            {
                printf("s_right x_s = %d, y_s = %d\n",x_s,y_s);
                ls = s_right;
                break;
            }
            if(y_s < show_xy.y+pdw && y_f>=1)
            {
                printf("s_up x_s = %d, y_s = %d\n",x_s,y_s);
                ls = s_up;
                break;
            }
            if(y_s > show_xy.y+480-pdw && y_f<MAP_HEIGHT-480)
            {
                printf("s_down x_s = %d, y_s = %d\n",x_s,y_s);
                ls = s_down;
                break;
            }

            usleep(100000);//一百毫秒扫描一次
        }
        int i;
        pthread_cleanup_push(handler,(void*)&snake_list_rwlock);    //压栈
        pthread_rwlock_wrlock(&snake_list_rwlock);  //读锁

        switch(ls)
        {
            case s_up:
                for(i=0; i<pdw; i++) {
                    map_show_xy(x_f, y_f - i); //跟拍
                    usleep(1000);   //镜头位移速度
                }
                break;
            case s_down:
                for(i=0; i<pdw; i++) {
                    map_show_xy(x_f, y_f + i); //跟拍
                    usleep(1000);   //镜头位移速度
                }
                break;
            case s_left:
                for(i=0; i<pdw; i++) {
                    map_show_xy(x_f - i, y_f); //跟拍
                    usleep(1000);   //镜头位移速度
                }
                break;
            case s_right:
                for(i=0; i<pdw; i++) {
                    map_show_xy(x_f + i, y_f); //跟拍
                    usleep(1000);   //镜头位移速度
                }
                break;
        }
        pthread_rwlock_unlock(&snake_list_rwlock);  //解锁
        pthread_cleanup_pop(0);    //弹栈，释放保护函数，但不执行此函数
    }

}


//地图初始化函数
void map_show_init(void)
{
    pthread_rwlock_init(&map_buffer_rwlock, NULL);  //读写锁初始化
    pthread_rwlock_wrlock(&map_buffer_rwlock);  //写锁

    memset((int *)map_buffer, MAP_COLOR, sizeof(map_buffer));    //清空地图

    /*边界绘制*/
    for (int j = 0; j <SNAKE_SIZE; j++)
        for(int i = 0; i < MAP_WIDTH; i++)
        {
            map_buffer[j][i] = 0xff6600;
            map_buffer[MAP_HEIGHT-SNAKE_SIZE+j][i] = 0xff6600;
        }
    for(int j=SNAKE_SIZE; j<MAP_HEIGHT-SNAKE_SIZE; j++)
        for(int i=0; i<SNAKE_SIZE; i++)
        {
            map_buffer[j][i] = 0xff6600;
            map_buffer[j][MAP_WIDTH-SNAKE_SIZE+i] = 0xff6600;
        }

    pthread_rwlock_unlock(&map_buffer_rwlock);  //解锁
}
