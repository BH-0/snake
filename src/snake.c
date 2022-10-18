#include "snake.h"
//----------------------------------------------------------------
//此为贪吃蛇处程序
//----------------------------------------------------------------

/*贪吃蛇全局变量*/
struct snake_list *snake;
pthread_rwlock_t snake_list_rwlock; //读写锁

//读写锁死锁保护函数
static void handler(void *arg)
{
    printf("[%u] is cancelled.\n",(unsigned)pthread_self());
    pthread_rwlock_t *pm = (pthread_rwlock_t*)arg;
    pthread_rwlock_unlock(pm);
}

//显示和消除蛇身（4方向扫描）
//入口参数：想显示或消除的蛇身，1显示，0消除
void snake_show(snake_t *node, int cmd)
{
    int color = 0;  //扫描颜色
    int x = 0;
    int y = 0;
    int i,j;
    enum direction direction_f;

    pthread_cleanup_push(handler,(void*)&snake_list_rwlock);    //压栈
    pthread_rwlock_rdlock(&snake_list_rwlock);  //读锁
    direction_f = node->next_direction; //扫描方向
    if (cmd == 0)
        color = 0xffffff;
    else
        color = node->color;
    x = node->x;
    y = node->y;
    pthread_rwlock_unlock(&snake_list_rwlock);  //解锁
    pthread_cleanup_pop(0);    //弹栈，释放保护函数，但不执行此函数

    //分情况扫描
    switch (direction_f)
    {
        case s_up:
            for(j=0; j<SNAKE_SIZE; j++)
            {
                pthread_cleanup_push(handler,(void*)&map_buffer_rwlock);    //压栈
                pthread_rwlock_wrlock(&map_buffer_rwlock);  //写锁
                for (i = 0; i<SNAKE_SIZE; i++)
                {
                    map_buffer[y+SNAKE_SIZE-j-1][x+i] = color;
                }
                pthread_rwlock_unlock(&map_buffer_rwlock);  //解锁
                pthread_cleanup_pop(0);    //弹栈，释放保护函数，但不执行此函数
                usleep(1500);//刷新率控制5ms刷新一帧
            }
            break;
        case s_down:
            for(j=0; j<SNAKE_SIZE; j++)
            {
                pthread_cleanup_push(handler,(void*)&map_buffer_rwlock);    //压栈
                pthread_rwlock_wrlock(&map_buffer_rwlock);  //写锁
                for (i = 0; i<SNAKE_SIZE; i++)
                {
                    map_buffer[y+j][x+i] = color;
                }
                pthread_rwlock_unlock(&map_buffer_rwlock);  //解锁
                pthread_cleanup_pop(0);    //弹栈，释放保护函数，但不执行此函数
                usleep(1500);//刷新率控制5ms刷新一帧
            }
            break;
        case s_left:
            for(i=0; i<SNAKE_SIZE; i++)
            {
                pthread_cleanup_push(handler,(void*)&map_buffer_rwlock);    //压栈
                pthread_rwlock_wrlock(&map_buffer_rwlock);  //写锁
                for (j = 0; j<SNAKE_SIZE; j++)
                {
                    map_buffer[y+j][x+SNAKE_SIZE-i-1] = color;
                }
                pthread_rwlock_unlock(&map_buffer_rwlock);  //解锁
                pthread_cleanup_pop(0);    //弹栈，释放保护函数，但不执行此函数
                usleep(1500);//刷新率控制5ms刷新一帧
            }
            break;
        case s_right:
            for(i=0; i<SNAKE_SIZE; i++)
            {
                pthread_cleanup_push(handler,(void*)&map_buffer_rwlock);    //压栈
                pthread_rwlock_wrlock(&map_buffer_rwlock);  //写锁
                for (j = 0; j<SNAKE_SIZE; j++)
                {
                    map_buffer[y+j][x+i] = color;
                }
                pthread_rwlock_unlock(&map_buffer_rwlock);  //解锁
                pthread_cleanup_pop(0);    //弹栈，释放保护函数，但不执行此函数
                usleep(1500);//刷新率控制5ms刷新一帧
            }
            break;
    }
}

//奖罚判定，判断吃到食物还是撞墙，还是撞到自己
//入口参数：蛇的朝向
//返回值：1吃到食物，0正常移动，-1碰撞
int snake_decision(enum direction d)
{
    switch(d)
    {
        case s_up:
            if(map_buffer[snake->tail->y-SNAKE_SIZE][snake->tail->x] == 0 ||    //撞自己
                    map_buffer[snake->tail->y-SNAKE_SIZE][snake->tail->x] == 0xff6600)  //撞墙
            {
                printf("game over!\n");
                for(;;);
            }
            else if(map_buffer[snake->tail->y-SNAKE_SIZE][snake->tail->x] == 0xff0000)  //吃到
            {
                printf("nice~\n");
                return 1;
            }
            break;
        case s_down:
            if(map_buffer[snake->tail->y+SNAKE_SIZE][snake->tail->x] == 0 ||    //撞自己
               map_buffer[snake->tail->y+SNAKE_SIZE][snake->tail->x] == 0xff6600)  //撞墙
            {
                printf("game over!\n");
                for(;;);
            }
            else if(map_buffer[snake->tail->y+SNAKE_SIZE][snake->tail->x] == 0xff0000)  //吃到
            {
                printf("nice~\n");
                return 1;
            }
            break;
        case s_left:
            if(map_buffer[snake->tail->y][snake->tail->x-SNAKE_SIZE] == 0 ||    //撞自己
               map_buffer[snake->tail->y][snake->tail->x-SNAKE_SIZE] == 0xff6600)  //撞墙
            {
                printf("game over!\n");
                for(;;);
            }
            else if(map_buffer[snake->tail->y][snake->tail->x-SNAKE_SIZE] == 0xff0000)  //吃到
            {
                printf("nice~\n");
                return 1;
            }
            break;
        case s_right:
            if(map_buffer[snake->tail->y][snake->tail->x+SNAKE_SIZE] == 0 ||    //撞自己
               map_buffer[snake->tail->y][snake->tail->x+SNAKE_SIZE] == 0xff6600)  //撞墙
            {
                printf("game over!\n");
                for(;;);
            }
            else if(map_buffer[snake->tail->y][snake->tail->x+SNAKE_SIZE] == 0xff0000)  //吃到
            {
                printf("nice~\n");
                return 1;
            }
            break;
    }
    return 0;
}

//snake主控线程
void *snake_task(void *arg)
{
    pthread_detach(pthread_self()); //分离属性
    int length_buf = 0;
    snake_t *p = snake->head;
    for(int i=0; i<snake->nodeNumber; i++)
    {
        snake_show(p,1);    //显示蛇宝宝
        p = p->next;
    }
    for(;;)
    {
        if((scan_keyboard == 'w' && snake->tail->next_direction == s_up)||
                (scan_keyboard == 'a' && snake->tail->next_direction == s_left)||
                (scan_keyboard == 's' && snake->tail->next_direction == s_down)||
                (scan_keyboard == 'd' && snake->tail->next_direction == s_right))
        {
            usleep(10); //同向加速
        }
        else
            usleep(200000);//正常

        if(scan_keyboard == 'w' && snake->tail->next_direction != s_down)
            snake->tail->next_direction = s_up;
        if(scan_keyboard == 'a' && snake->tail->next_direction != s_right)
            snake->tail->next_direction = s_left;
        if(scan_keyboard == 's' && snake->tail->next_direction != s_up)
            snake->tail->next_direction = s_down;
        if(scan_keyboard == 'd' && snake->tail->next_direction != s_left)
            snake->tail->next_direction = s_right;
        scan_keyboard = 0; //清空按键

        if(snake_decision(snake->tail->next_direction) == 1)    //碰撞判断
        {
            enter_snake_t(snake);   //尾插
            snake_show(snake->tail, 1);  //显示头
            length_buf+=5;  //变长次数
        }else if(length_buf != 0)   //还有长度，继续变长
        {
            enter_snake_t(snake);   //尾插
            snake_show(snake->tail, 1);  //显示头
            length_buf--;
        }
        else
        {
            enter_snake_t(snake);   //尾插
            snake_show(snake->tail, 1);  //显示头
            snake_show(snake->head, 0);  //消除尾
            del_snake_t(snake); //头删
        }
    }
}


/*贪吃蛇链表*/
//新建一个结点并初始化
//函数参数：管理结构体，节点数据
snake_t *new_node(struct snake_list *listHead,
        int x,int y,int color,
        enum direction next_direction)
{
    int x_f = 0;
    int y_f = 0;
    /*限制蛇的位置范围*/
    if(x<0)
        x_f = 0;
    else if(x>(MAP_WIDTH-SNAKE_SIZE))
        x_f = (MAP_WIDTH-SNAKE_SIZE);
    else
        x_f = x;
    if(y<0)
        y_f = 0;
    else if(y>(MAP_HEIGHT-SNAKE_SIZE))
        y_f = (MAP_HEIGHT-SNAKE_SIZE);
    else
        y_f = y;


//1.新建一个结点（申请一个结构体的内存空间）
    snake_t *newNode = malloc(sizeof(snake_t));
    if(newNode == NULL)
    {
        printf("malloc newNode error\n");
        return NULL;
    }
//2、初始化新建结点
    newNode->x = x_f;
    newNode->y = y_f;
    newNode->color = color;
    newNode->next_direction = next_direction;
    newNode->prev = NULL;
    newNode->next = NULL;

//    pthread_cleanup_push(handler,(void*)&snake_list_rwlock);    //压栈
//    pthread_rwlock_wrlock(&snake_list_rwlock);  //写锁  //锁中锁
//链表的结点数加1
    listHead->nodeNumber++;
//    pthread_rwlock_unlock(&snake_list_rwlock);  //解锁
//    pthread_cleanup_pop(0);    //弹栈，释放保护函数，但不执行此函数
    return newNode;
}

//创建一条双向链表并初始化
struct snake_list *create_list()
{
    pthread_rwlock_init(&snake_list_rwlock, NULL);  //读写锁初始化
    pthread_rwlock_wrlock(&snake_list_rwlock);  //写锁
    struct snake_list *listHead=malloc(sizeof(struct snake_list));
    if(listHead==NULL)
    {
        printf("malloc listHead error\n");
        return NULL;
    }
    listHead->head=NULL;
    listHead->tail=NULL;
    listHead->nodeNumber=0;

    /*初始化蛇宝宝*/
    int sum = 0;
    while(sum<5)    //设定蛇身初始长度
    {
        //新建结点
        //设定蛇出生地及默认方向（出生地坐标必须为蛇身尺寸的倍数）
        snake_t *newNode=new_node(listHead,200+(SNAKE_SIZE*sum),240,0,s_right);
        if(newNode == NULL)
        {
            printf("[%d]newNode error\n",__LINE__);//打印错误的行数
            break;//退出while循环
        }

        //新建结点插入链表中
        //从无到有（新建的第一个结点就是首结点）
        if(listHead->head==NULL)
        {
            listHead->head = listHead->tail = newNode;
        }else  //从少到多（默认尾插）
        {
            //链表的尾结点的next指向新结点
            listHead->tail->next = newNode;
            //新结点的prev指向尾结点
            newNode->prev = listHead->tail;  //此时tail还是指向上一个节点
            //更新尾结点
            listHead->tail = newNode;
        }
        sum++;
    }
    pthread_rwlock_unlock(&snake_list_rwlock);  //解锁
    return listHead;
}

//打印蛇身参数
void print_allToList(struct snake_list *listHead)
{
//遍历链表，将链表中的每个结点的数据打印出来
    pthread_cleanup_push(handler,(void*)&snake_list_rwlock);    //压栈
    pthread_rwlock_rdlock(&snake_list_rwlock);  //读锁
    snake_t *p = listHead->head;
    while(p)
    {
        switch(p->next_direction) {
            case s_up:
                printf("(%d,%d)↑\n", p->x,p->y);
                break;
            case s_down:
                printf("(%d,%d)↓\n", p->x,p->y);
                break;
            case s_left:
                printf("(%d,%d)←\n", p->x,p->y);
                break;
            case s_right:
                printf("(%d,%d)→\n", p->x,p->y);
                break;
        }
        p = p->next;
    }
    pthread_rwlock_unlock(&snake_list_rwlock);  //解锁
    pthread_cleanup_pop(0);    //弹栈，释放保护函数，但不执行此函数
}

//尾插(链表不得为空)
void enter_snake_t(struct snake_list *listHead)
{
    int prev_x = 0;
    int prev_y = 0;
    pthread_cleanup_push(handler,(void*)&snake_list_rwlock);    //压栈
    pthread_rwlock_rdlock(&snake_list_rwlock);  //读锁
    if(listHead->nodeNumber == 0)
        return;
    prev_x = listHead->tail->x;
    prev_y = listHead->tail->y;
    pthread_rwlock_unlock(&snake_list_rwlock);  //解锁
    pthread_cleanup_pop(0);    //弹栈，释放保护函数，但不执行此函数
    snake_t *newNode = NULL;
    //新建节点
    switch(listHead->tail->next_direction){ //判断上一次的方向
        case s_up:
            newNode = new_node(listHead, prev_x, prev_y-SNAKE_SIZE, 0, s_up);
            break;
        case s_down:
            newNode = new_node(listHead, prev_x, prev_y+SNAKE_SIZE, 0, s_down);
            break;
        case s_left:
            newNode = new_node(listHead, prev_x-SNAKE_SIZE, prev_y, 0, s_left);
            break;
        case s_right:
            newNode = new_node(listHead, prev_x+SNAKE_SIZE, prev_y, 0, s_right);
            break;
    }
    if(newNode == NULL)
    {
        printf("[%d]newNode error\n",__LINE__);//打印错误的行数
        return;//退出
    }

    pthread_cleanup_push(handler,(void*)&snake_list_rwlock);    //压栈
    pthread_rwlock_wrlock(&snake_list_rwlock);  //写锁
    //尾插
    //链表的尾结点的next指向新结点
    listHead->tail->next = newNode;
    //新结点的prev指向尾结点
    newNode->prev = listHead->tail;  //此时tail还是指向上一个节点
    //更新尾结点
    listHead->tail = newNode;
    pthread_rwlock_unlock(&snake_list_rwlock);  //解锁
    pthread_cleanup_pop(0);    //弹栈，释放保护函数，但不执行此函数
}

//头删(链表不得为空)
void del_snake_t(struct snake_list *listHead)
{
    pthread_cleanup_push(handler,(void*)&snake_list_rwlock);    //压栈
    pthread_rwlock_wrlock(&snake_list_rwlock);  //写锁
    if(listHead->nodeNumber == 0)
        return;
    snake_t *p = listHead->head;
    //更新首结点
    listHead->head = p->next;
    //删除结点的下一个结点的prev指向NULL
    p->next->prev = NULL;
    //删除结点的next指向NULL
    p->next = NULL;
    //删除的结点数-1
    listHead->nodeNumber--;
    //释放空间
    free(p);
    pthread_rwlock_unlock(&snake_list_rwlock);  //解锁
    pthread_cleanup_pop(0);    //弹栈，释放保护函数，但不执行此函数
}

//销毁链表
void destory_list(struct snake_list *listHead)
{
    pthread_rwlock_wrlock(&snake_list_rwlock);  //写锁
    if(listHead->nodeNumber == 0)
    {
        free(listHead);
        return;
    }
    snake_t *p = listHead->head;
    snake_t *dp = listHead->head;
    int i = listHead->nodeNumber;
    while (i--)
    {
        p = p->next;
        free(dp);
        dp = p;
    }
    listHead->head = listHead->tail = NULL;
    listHead->nodeNumber = 0;
    free(listHead);
    pthread_rwlock_unlock(&snake_list_rwlock);  //解锁
}
