#include "map_show.h"
//----------------------------------------------------------------
//此为地图处理程序
//----------------------------------------------------------------
pthread_rwlock_t map_buffer_rwlock; //读写锁
int map_buffer[MAP_HEIGHT][MAP_WIDTH] = {0}; //地图buffer 3*3

/*鸟瞰坐标*/
struct xy{
    int x;
    int y;
}show_xy = {0};

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
    pthread_detach(pthread_self()); //分离属性
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
            //这里可尝试挂起一点点时间,或者显示其他东西
        }
    pthread_cleanup_pop(0);    //弹栈，释放保护函数，但不执行此函数
}

//鸟瞰坐标修改
void map_show_xy(int x, int y)
{
    int x_f = x<0?0:x;
    x_f = (x>(MAP_WIDTH-800))?(MAP_WIDTH-800):x;
    int y_f = y<0?0:y;
    y_f = (y>(MAP_HEIGHT-480))?(MAP_HEIGHT-480):y;
    pthread_rwlock_wrlock(&map_buffer_rwlock);  //写锁
    show_xy.x = x_f;
    show_xy.y = y_f;
    pthread_rwlock_unlock(&map_buffer_rwlock);  //解锁
}

//地图初始化函数
void map_show_init(void)
{
    pthread_rwlock_init(&map_buffer_rwlock, NULL);  //读写锁初始化
    pthread_rwlock_wrlock(&map_buffer_rwlock);  //写锁

    memset((int *)map_buffer, 0xff, sizeof(map_buffer));    //清空地图

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
