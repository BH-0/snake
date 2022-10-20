#include "file.h"
//----------------------------------------------------------------
//此为目录、文件管理程序
//----------------------------------------------------------------

//文件名排序前比较算法
//入口参数：对比对象，与被对比对象
//返回值：大于diff_name返回0，小于diff_name返回1，同名返回-1
//排序是大的放后面，小的放前面，有序插入时直接遍历找到第一个比他大的，并插在他前面
int find_name_diff(char *object_name, char *diff_name)
{
    int i = 0;
    //先跳过前缀相同的部分
    while(object_name[i] == diff_name[i] && object_name[i] != '\0' && diff_name[i] != '\0' && i < 255)
        i++;
    if(object_name[i] > diff_name[i])
        return 0;
    else if(object_name[i] < diff_name[i])
        return 1;
    else
        return -1; //名称全等
}

//读取某文件的文件名
//入口参数：文件路径
char *get_find_name(char *find_path)
{
    static char path_buf[512] = {0};
    strcpy(path_buf, find_path);
    char *ptr = NULL;
    char *p = NULL;
    p = strtok(path_buf,"/");
    if (p)
        ptr = p;
    while (p)
    {
        p = strtok(NULL,"/");
        if(p)
            ptr = p;
    }
    return ptr;
}

//文件后缀判断函数
int file_suffix_judgment_pro(char *filename,char *file_suffix)
{
    int filename_len = strlen(filename);
    int file_suffix_len = strlen(file_suffix);
    int i = 0;
    if(filename_len<file_suffix_len)
    {
        return -1;
    }
    while(i<=file_suffix_len)
    {
        if(filename[filename_len-i] != file_suffix[file_suffix_len-i])
        {
            return 0;
        }
        i++;
    }
    return 1;
}

//创建节点,创建前先判断文件是否为指定的文件类型
//入口参数：文件路径
file_t *new_file(char *find_name)
{
    file_t *newfile = malloc(sizeof(file_t));
    if(newfile == NULL)
    {
        printf("malloc newfile error\n");
        return NULL;
    }
    newfile->next = NULL;
    newfile->prev = NULL;
    strcpy(newfile->find_name,find_name);

    return newfile;
}

//创建一条文件双向链表并初始化
//入口参数：目录路径，文件后缀
struct file_link *file_link_init(char *directory_path,char *file_suffix)
{
    //创建管理结构体
    struct file_link *listHead = malloc(sizeof(struct file_link));
    if(listHead==NULL)
    {
        printf("malloc listHead error\n");
        return NULL;
    }

    listHead->head = listHead->tail = NULL;
    listHead->nodeNumber = 0;

    //打开目录扫描目录下的文件
    //打开目录
    DIR *dp = opendir(directory_path);
    if(dp == NULL)
    {
        perror("opendir fail");
        free(listHead);
        return NULL;
    }

    //切换到目录里面去
    chdir(directory_path);

    struct dirent *ep = NULL;
    while(1)
    {
        ep = readdir(dp);
        if(ep == NULL)  //说明已经读取到最后一项了
        {
            break;
        }
        if(ep->d_name[0] == '.' || ep->d_type == DT_DIR)  //如果文件名的第一个字符是. 或者遇到了目录
        {
            continue;
        }
        if(file_suffix_judgment_pro(ep->d_name,file_suffix) == 0)    //判读文件类型再加入链表
        {
            continue;
        }

        //新建节点
        file_t *newPic = NULL;
        //创建节点
        newPic = new_file(ep->d_name);
        //有序插入
        insert_list_sort((struct pic_link *)listHead, (pic_t *)newPic);

    }
    chdir("..");
    closedir(dp);

    return listHead;
}

//链表遍历并打印
void print_allToFileList(struct file_link *listHead)
{
//遍历链表，将链表中的每个结点的数据打印出来
    file_t *p = listHead->head;
    int i = 0;
    while(i < listHead->nodeNumber)
    {
        printf("print:%s\n",p->find_name);
        p = p->next;
        i++;
    }
}

//销毁链表
void destroy_file_link(struct file_link *listHead)
{
    if(listHead == NULL)
        return;
    if(listHead->nodeNumber == 0)
    {

        free(listHead);
        return;
    }
    if(listHead->head == NULL)
    {
        free(listHead);
        return;
    }

    file_t *dp = NULL;
    file_t *p = listHead->head;
    int i = 0;

    while(i < listHead->nodeNumber)
    {
        dp = p;
        //printf("--%s\n",p->find_name);
        free(dp);
        p = p->next;
        i++;
    }
    free(listHead);

}