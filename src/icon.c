#include "icon.h"
//----------------------------------------------------------------
//此为图标按钮相关程序
//----------------------------------------------------------------

//图标管理结构体指针全局变量
struct list_icon *start_in;
struct list_icon *start_out;

//批量初始化图标，入口参数：管理结构体指针，图标列表文件路径
void icon_init(struct list_icon *listHead,char * txt_path)
{
    FILE *fp = fopen(txt_path,"r+");//切记这里用"r+"不要用"w+"
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
    while (p)
    {
        if(p != NULL && *p != '\0' && *p != '#'&& *p != '\n'&&(strlen(p) > 7))
        {
            char buf_p[1024] = {0};
            char f_name[512] = {0};
            char name[8] = {0};
            char x[8] = {0};
            char y[8] = {0};
            sscanf(p, "%[^','],%[^','],%[^','],%s",f_name,name,x,y);
            sprintf(buf_p,"./menu/%s.bmp",f_name);
            insert_list_icon(listHead, buf_p, atoi(name), atoi(x), atoi(y));
        }
        p=strtok(NULL,"\n");
    }
    fclose(fp);
}

//初始化图标管理结构体
struct list_icon *create_list_icon()
{
    //1、申请头结点的动态内存空间并且初始化（定义一个管理结构体的变量）
    struct list_icon *listHead=malloc(sizeof(struct list_icon));
    if(listHead==NULL)
    {
        printf("malloc listHead error\n");
        return NULL;
    }
    listHead->head=NULL;
    listHead->tail=NULL;
    listHead->icon_number=0;
    return listHead;
}

//打开一张图片，插入到链表后，入口参数：图标管理结构体，路径，图标名称，返回值指向图标结构体
bmp_icon *insert_list_icon(struct list_icon *listHead, char *path, int name, int x, int y)
{
    //1、新建结点并初始化
    bmp_icon *new_icon = malloc(sizeof(bmp_icon));
    if(new_icon == NULL)
    {
        printf("malloc new_icon error\n");
        return NULL;
    }
//初始化新建结点
    new_icon->bmp = open_bmp(path);
    new_icon->name = name;
    new_icon->x = x;
    new_icon->y = y;

    if(listHead->icon_number == 0) //第一个
    {
        listHead->head = listHead->tail = new_icon;
    }
    else  //尾插
    {
        listHead->tail->next = new_icon;
        listHead->tail = new_icon;
    }
    listHead->icon_number++;
    return new_icon;
}

//寻找某个图标，返回此图标结构体指针
bmp_icon *find_icon(struct list_icon *listHead, int name)
{
    bmp_icon *p = listHead->head;
    int i = 0;
    while (i < listHead->icon_number)
    {
        if(p->name == name)
            break;
        p = p->next;
        i++;
    }
    if(i == listHead->icon_number)
    {
        printf("icon name not find\n");
        return NULL;
    }else
        return p;
}

//显示单个图标，入口参数：LCD映射地址，图标结构体
void display_icon(int (*LCD_addr)[800], bmp_icon *icon)
{
    show_bmp(LCD_addr, icon->bmp, icon->x, icon->y);
}


//显示单个或一串图标，入口参数：LCD映射地址，图标管理结构体，某个图标名称（如果为空，则全部显示）
int display_icons(int (*LCD_addr)[800], struct list_icon *listHead, int name)
{
    bmp_icon *p = listHead->head;
    if(name == 0)    //显示全部
    {
        int i = 0;
        while (i < listHead->icon_number)
        {
            display_icon(LCD_addr, p); //显示图标
            p = p->next;
            i++;
        }
    }else   //显示单张
    {
        p = find_icon(listHead, name);
        if(p != NULL)
        {
            display_icon(LCD_addr, p); //显示图标
        }else
            return -1;

    }
    return 0;
}

//检测是否有按动触发图标，入口参数：图标管理结构体，触摸坐标，返回值：图标名称
int touch_button(struct list_icon *listHead, int x, int y)
{
    bmp_icon *p = listHead->head;
    int i = 0;
    while (i < listHead->icon_number)
    {
        if(x>p->x && x<(p->x+p->bmp->width) && y>p->y && y<(p->y+p->bmp->height))
            break;
        i++;
        p = p->next;
    }
    if(i == listHead->icon_number)
    {
        return -1; //按到了其他地方
    }else
    {
        return p->name;
    }
}

//销毁一组图标，入口参数：图标管理结构体
void del_icon(struct list_icon *listHead)
{
    bmp_icon *p = listHead->head;
    bmp_icon *p_next = listHead->head;
    int i = 0;
    if(listHead == NULL)
        return;
    while (i < listHead->icon_number)
    {
        p_next = p->next;
        free(p->bmp->bmp_buf);
        p->bmp->bmp_buf = NULL;
        free(p->bmp);
        p->bmp = NULL;
        free(p);
        p = p_next;
        i++;
    }
    listHead->head = listHead->tail = NULL;
    free(listHead);
}
