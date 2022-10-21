#include "snake.h"
//----------------------------------------------------------------
//此为贪吃蛇处程序
//----------------------------------------------------------------

/*贪吃蛇全局变量*/
struct snake_list *snake;
pthread_rwlock_t snake_list_rwlock; //读写锁
int game_points = 0; //游戏积分
int game_points_list[10] = {0}; //积分表
int SNAKE_COLOR = 0xffffff; //蛇身颜色


//读写锁死锁保护函数
static void handler(void *arg)
{
    printf("[%u] is cancelled.\n",(unsigned)pthread_self());
    pthread_rwlock_t *pm = (pthread_rwlock_t*)arg;
    pthread_rwlock_unlock(pm);
}

//读写积分表
void game_points_rd(void)
{
    FILE *fp = fopen("./menu/game_points.txt","r+");//切记这里用"r+"不要用"w+"
    if(fp == NULL)
    {
        perror("fopen fail");
        return ;
    }
    char buf[10240] = {0};
    fread(buf,1,1024,fp);

    //按行读取
    char*p;
    p=strtok(buf,"\n");
//    if(p != NULL && *p != '\0' && *p != '#')
//        printf("%s\n",p);
    int i = 0;
    while (p)
    {
        if(p != NULL && *p != '\0' && *p != '#'&& *p != '\n'&&(strlen(p) > 2))
        {
            char name[8] = {0};
            char points[32] = {0};
            sscanf(p, "%[^':']:%s",name,points);
            game_points_list[i] = atoi(points);
            i++;
            if(i>9)
                break;
        }
        p=strtok(NULL,"\n");
    }
    fclose(fp);
}

//写积分表
void game_points_wr(void)
{
    FILE *fp = fopen("./menu/game_points.txt","w+");
    if(fp == NULL)
    {
        perror("fopen fail");
        return ;
    }
    char buf[1024] = {0};
    int i = 0;
    strcpy(buf,"#积分排行，名次，分数\n");
    for(i = 0; i<10; i++)
    {
        sprintf(buf,"%s%d:%d\n",buf,i+1,game_points_list[i]);
    }
    printf("%s\n",buf);
    fwrite(buf,1,strlen(buf),fp);
    fclose(fp);
}

//有序插入积分表
//比列表大的数才会被插入
int game_points_insert(int data)
{
    int i = 0;
    int j;
    for(i = 0; i<10; i++)
    {
        if(data == game_points_list[i])
            return -1;
        else if(data>game_points_list[i])
            break;
    }
    if(i>=10)
        return -1;
    else
    {
        j = i;
        for(i=9;i>j;i--)
        {
            game_points_list[i] = game_points_list[i-1];
        }
        game_points_list[j] = data;
    }
    return 0;
}

//显示积分列表
void game_points_list_show()
{
    //显示背景
    bmp_t *bp = open_bmp("./menu/game_points_list.bmp");
    show_bmp(LCD_addr,bp,0,0);
    destroy_bmp_t(bp);
    char buf[32] = {0};

    //显示当前积分
    sprintf(buf,"%d",game_points);
    Display_utf8(600, 390, buf, 0xff8faf, 4, 1);

    //显示积分榜
    sprintf(buf,"%d",game_points_list[0]);
    Display_utf8(170, 57, buf, 0xff0000, 4, 1);
    sprintf(buf,"%d",game_points_list[1]);
    Display_utf8(155, 120, buf, 0xff8800, 3, 1);
    sprintf(buf,"%d",game_points_list[2]);
    Display_utf8(145, 172, buf, 0x91cc52, 2, 1);

    for(int i=3; i<10;i++)
    {
        sprintf(buf,"%d",game_points_list[i]);
        Display_utf8(128, 208+((i-3)*31), buf, 0xffffff, 2, 0);
    }


    for(;;)
    {
        if(scan_keyboard == '\n')
        {
            scan_keyboard = 0; //清空按键
            return;
        }
    }
}

//显示简介
void about(void)
{
    //显示背景
    bmp_t *bp = open_bmp("./menu/about.bmp");
    show_bmp(LCD_addr,bp,0,0);
    destroy_bmp_t(bp);

    for(;;)
    {
        if(scan_keyboard == '\n')
        {
            scan_keyboard = 0; //清空按键
            return;
        }
    }
}

//显示设置
void set_menu(void)
{
    //显示背景
    bmp_t *bp = open_bmp("./menu/set.bmp");
    show_bmp(LCD_addr,bp,0,0);
    destroy_bmp_t(bp);

    for(;;)
    {
        if(scan_keyboard == '\n')
        {
            scan_keyboard = 0; //清空按键
            return;
        }
    }
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
    unsigned int refresh_rate = 100+(2500*(50-(game_points>50?50:game_points))/50);   //蛇身动画每一帧的速度控制，越大越丝滑但越慢

    pthread_cleanup_push(handler,(void*)&snake_list_rwlock);    //压栈
    pthread_rwlock_rdlock(&snake_list_rwlock);  //读锁
    direction_f = node->next_direction; //扫描方向
    if (cmd == 0)
        color = MAP_COLOR;
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
                usleep(refresh_rate);//刷新率控制
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
                usleep(refresh_rate);//刷新率控制
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
                usleep(refresh_rate);//刷新率控制
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
                usleep(refresh_rate);//刷新率控制
            }
            break;
    }
}

//食物初始化
void food_init(void)
{
    //初始食物坐标
    int food_buf[][2] = {{400,700},{440,700},{440,740},{400,740},{420,680},
                         {420,740},{440,720},{460,720},{420,760},{420,720}};
    int l = 0;  //多个食物刷新
    int x,y;
    pthread_rwlock_wrlock(&map_buffer_rwlock);  //写锁
    for(l=0; l<(sizeof(food_buf)/8); l++)
    {
        for (int i = 0; i < SNAKE_SIZE; i++) //食物
        {
            for (int j = 0; j < SNAKE_SIZE; j++) {
                x = food_buf[l][0];
                y = food_buf[l][1];
                map_buffer[x + i][y + j] = 0xff0000;
            }
        }
    }
    pthread_rwlock_unlock(&map_buffer_rwlock);  //解锁
}

//食物刷新
//传入一个种子，随机刷新一次食物，种子可以是被吃掉食物的坐标
void food(unsigned int seed_x,unsigned int seed_y)
{
    srand(seed_x*seed_y);
    //刷新范围从 SNAKE_SIZE 到 MAP_WIDTH - SNAKE_SIZE
    int x_b = rand()%(MAP_WIDTH-(SNAKE_SIZE*2)+1)+SNAKE_SIZE;
    int y_b = rand()%(MAP_HEIGHT-(SNAKE_SIZE*2)+1)+SNAKE_SIZE;
    //抹零
    x_b -= x_b%SNAKE_SIZE;
    y_b -= y_b%SNAKE_SIZE;
    int sum = 0;
    //重叠重刷
    while((map_buffer[y_b][x_b]&0xffffff) != MAP_COLOR)    //不为白色
    {
        //刷新范围从 SNAKE_SIZE 到 MAP_WIDTH - SNAKE_SIZE
        x_b = rand()%(MAP_WIDTH-(SNAKE_SIZE*2)+1)+SNAKE_SIZE;
        y_b = rand()%(MAP_HEIGHT-(SNAKE_SIZE*2)+1)+SNAKE_SIZE;
        //抹零
        x_b -= x_b%SNAKE_SIZE;
        y_b -= y_b%SNAKE_SIZE;
        sum++;
        printf("食物刷新失败：%d次\n",sum);
    }
    pthread_cleanup_push(handler,(void*)&map_buffer_rwlock);    //压栈
    pthread_rwlock_wrlock(&map_buffer_rwlock);  //写锁
    for(int i=0; i<SNAKE_SIZE; i++) //食物
    {
        for (int j=0; j<SNAKE_SIZE; j++)
        {
            map_buffer[y_b + i][x_b + j] = 0xff0000;
        }
    }
    pthread_rwlock_unlock(&map_buffer_rwlock);  //解锁
    pthread_cleanup_pop(0);    //弹栈，释放保护函数，但不执行此函数
    printf("(%d,%d)食物已刷新!\n",x_b,y_b);
}

//奖罚判定，判断吃到食物还是撞墙，还是撞到自己
//入口参数：蛇的朝向
//返回值：1吃到食物，0正常移动，-1碰撞
int snake_decision(enum direction d)
{
    switch(d)
    {
        case s_up:
            if((map_buffer[snake->tail->y-SNAKE_SIZE][snake->tail->x]&0xffffff) == 0xff0000)  //吃到
            {
                printf("nice~\n");
                food(snake->tail->x*snake->head->x,snake->tail->y*snake->head->y);
                game_points++;
                return 1;
            }else if((map_buffer[snake->tail->y-SNAKE_SIZE][snake->tail->x]&0xffffff) != MAP_COLOR)   //白色
            {
                printf("game over!\n");
                return -1;
            }
            break;
        case s_down:
            if((map_buffer[snake->tail->y+SNAKE_SIZE][snake->tail->x]&0xffffff) == 0xff0000)  //吃到
            {
                printf("nice~\n");
                food(snake->tail->x*snake->head->x,snake->tail->y*snake->head->y);
                game_points++;
                return 1;
            }
            else if((map_buffer[snake->tail->y+SNAKE_SIZE][snake->tail->x]&0xffffff) != MAP_COLOR)   //白色
            {
                printf("game over!\n");
                return -1;
            }
            break;
        case s_left:
            if((map_buffer[snake->tail->y][snake->tail->x-SNAKE_SIZE]&0xffffff) == 0xff0000)  //吃到
            {
                printf("nice~\n");
                food(snake->tail->x*snake->head->x,snake->tail->y*snake->head->y);
                game_points++;
                return 1;
            }
            else if((map_buffer[snake->tail->y][snake->tail->x-SNAKE_SIZE]&0xffffff) != MAP_COLOR)  //白色
            {
                printf("game over!\n");
                return -1;
            }
            break;
        case s_right:
            if((map_buffer[snake->tail->y][snake->tail->x+SNAKE_SIZE]&0xffffff) == 0xff0000)  //吃到
            {
                printf("nice~\n");
                food(snake->tail->x*snake->head->x,snake->tail->y*snake->head->y);
                game_points++;
                return 1;
            }else if((map_buffer[snake->tail->y][snake->tail->x+SNAKE_SIZE]&0xffffff) != MAP_COLOR) //白色
            {
                printf("game over!\n");
                return -1;
            }
            break;
    }
    return 0;
}

//snake主控线程
void *snake_task(void *arg)
{
    //pthread_detach(pthread_self()); //分离属性
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
        {
            usleep(100+200000*(50-(game_points>50?50:game_points))/50);//正常
        }

        if(scan_keyboard == 'w' && snake->tail->next_direction != s_down)
            snake->tail->next_direction = s_up;
        if(scan_keyboard == 'a' && snake->tail->next_direction != s_right)
            snake->tail->next_direction = s_left;
        if(scan_keyboard == 's' && snake->tail->next_direction != s_up)
            snake->tail->next_direction = s_down;
        if(scan_keyboard == 'd' && snake->tail->next_direction != s_left)
            snake->tail->next_direction = s_right;
        if(scan_keyboard == 'p')    //暂停游戏
        {
            printf("暂停游戏\n");
            scan_keyboard = 0; //清空按键
            pthread_cleanup_push(handler,(void*)&map_buffer_rwlock);    //压栈
            pthread_rwlock_wrlock(&map_buffer_rwlock);  //写锁
            Display_utf8(325,200,"暂停",0x25ccf7,4,1);   //橙黄色
            for(;;)
            {
                if(scan_keyboard == 'p')    //继续游戏
                    break;
            }
            pthread_rwlock_unlock(&map_buffer_rwlock);  //解锁
            pthread_cleanup_pop(0);    //弹栈，释放保护函数，但不执行此函数
            printf("\n继续游戏\n");
        }
        scan_keyboard = 0; //清空按键
        int decision = snake_decision(snake->tail->next_direction); //碰撞检测
        if(decision == 1)    //碰撞判断
        {
//            for(int i = 0; i <50; i++)
//            {
//                rainbow();
//            }
//            SNAKE_COLOR = rainbow(); //改变蛇身颜色
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
        if(decision == -1)    //结束游戏
        {
            pthread_exit(0);
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
        snake_t *newNode=new_node(listHead,200+(SNAKE_SIZE*sum),240,SNAKE_COLOR,s_right);
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
void print_allToList_s(struct snake_list *listHead)
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
            newNode = new_node(listHead, prev_x, prev_y-SNAKE_SIZE, SNAKE_COLOR, s_up);
            break;
        case s_down:
            newNode = new_node(listHead, prev_x, prev_y+SNAKE_SIZE, SNAKE_COLOR, s_down);
            break;
        case s_left:
            newNode = new_node(listHead, prev_x-SNAKE_SIZE, prev_y, SNAKE_COLOR, s_left);
            break;
        case s_right:
            newNode = new_node(listHead, prev_x+SNAKE_SIZE, prev_y, SNAKE_COLOR, s_right);
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


//最高饱和度彩虹色每调用一次颜色加1
int rainbow(void)
{
    static int color = 0x000000ff;  //从蓝色开始
    static int mode = 0;
    switch (color)
    {
        case 0x00ff0000:
            mode = 0;   //0x00ff++00
            break;
        case 0x00ffff00:
            mode = 1;   //0x00--ff00
            break;
        case 0x0000ff00:
            mode = 2;   //0x0000ff++
            break;
        case 0x0000ffff:
            mode = 3;   //0x0000--ff
            break;
        case 0x000000ff:
            mode = 4;   //0x00++00ff
            break;
        case 0x00ff00ff:
            mode = 5;   //0x00ff00--
            break;
        default:
            break;
    }

    switch (mode)
    {
        case 0:
            color += 0x00000100;   //0x00ff++00
            break;
        case 1:
            color -= 0x00010000;   //0x00--ff00
            break;
        case 2:
            color += 0x00000001;   //0x0000ff++
            break;
        case 3:
            color -=  0x00000100;   //0x0000--ff
            break;
        case 4:
            color += 0x00010000;   //0x00++00ff
            break;
        case 5:
            color -= 0x00000001;   //0x00ff00--
            break;
        default:
            break;
    }
    return color;
}