#include "bmp.h"
//----------------------------------------------------------------
//此为bmp图片相关程序
//----------------------------------------------------------------

int x_ts = 0;
int y_ts = 0;

//打开一张图片，入口参数路径，返回值指向bpm管理结构体
bmp_t *open_bmp(char *path)
{
    //创建bpm管理结构体
    bmp_t *bmp = malloc(sizeof(bmp_t));
    bmp->path = path;
    //打开bmp图
    int fd_bmp;
    fd_bmp = open(bmp->path,O_RDWR);
    if(fd_bmp < 0)
    {
        perror("open bmp fail");
        free(bmp);
        bmp = NULL;
        return NULL;
    }

    lseek(fd_bmp, 0x12, SEEK_SET); //偏移到记录宽高的位置

    read(fd_bmp, &bmp->width, 4);   //800
    read(fd_bmp, &bmp->height, 4);   //480
    printf("%s: height=%d, width=%d\n", path, bmp->height, bmp->width);
    if((bmp->height*bmp->width)%4 != 0) //像素点不为4的倍数
    {
        printf("%s:Error, the image pixels in multiples of four!\n",path);
        free(bmp);
        close(fd_bmp);
        return NULL;
    }
    if((bmp->height*bmp->width)>2560*2560) //图片超大
    {
        printf("%s:Error, the image size is too big!\n",path);
        free(bmp);
        close(fd_bmp);
        return NULL;
    }

    //去除掉它的头54个字节
    lseek(fd_bmp,54,SEEK_SET);

    //存储bmp图片的buffer800*480
    bmp->bmp_buf = malloc(bmp->width*bmp->height*3);
    //读bmp图
    read(fd_bmp, (char *)bmp->bmp_buf, bmp->width*bmp->height*3);
    //上下翻转
    char buf= 0;
    int y,x;
    for(y=0; y<bmp->height/2; y++)
        for(x=0; x<bmp->width*3; x++)
        {
            //y 0 = 479  1 = 478  239 = 240
            buf = BMP_BUF(y,x);
            BMP_BUF(y,x) = BMP_BUF(bmp->height-1-y,x);
            BMP_BUF(bmp->height-1-y,x) = buf;
        }
    //关闭bmp
    close(fd_bmp);
    return bmp;
}

//显示图片，入口参数：LCD映射地址，bmp图片地址,显示坐标
void show_bmp(int (*LCD_addr)[800], bmp_t *bmp, int x_start, int y_start)
{
    //将buf里面的数据通过指针addr填充到LCD中
    int x;//x表示横轴
    int y;//y表示纵轴


    for(y=0;y<bmp->height;y++)
        for(x=0;x<bmp->width;x++)
        {
            if((x+x_start)>799 || (y+y_start)>479 || (x+x_start)<0 || (y+y_start)<0)
                continue;

            LCD_addr[y+y_start][x+x_start] = \
                    BMP_BUF(y,x*3) | BMP_BUF(y,x*3+1)<<8 | BMP_BUF(y,x*3+2)<<16;

            /*
                1.要将buf[]里面的数据由BGR换成RGB
                2.要将3个字节封装成4个字节
            */
            //*(addr+y*800+x) = buf[]
            //			0     = buf[0] | buf[1]<<8 | buf[2]<<16
            //			1     = buf[3] | buf[4]<<8 | buf[5]<<16
            //		    2     = buf[6] | buf[7]<<8 | buf[8]<<16
            //			800   = buf[2400] |buf[2401]<<8 |buf[2402] <<16
        }
#if 0
    printf("x=%d,y=%d\n",x_start,y_start);
#endif
}



//从图片1至图片2的过渡动画(往右滚动),速度从0.01~1，1为全速,direction:方向：0从左到右 1从右到左
//只能传入800*480的图片
void show_bmp_plus(int (*LCD_addr)[800], char (*bmp_buf1)[800*3], char (*bmp_buf2)[800*3], float speed, int direction)
{
    int x;//x表示横轴
    int y;//y表示纵轴
    float z;//帧

    if(speed<0.01)
        speed = 0.01;
    if(speed>1)
        speed = 1;
    float speed_buf = powf(800, speed);

    int LCD_buf1[480][800] = {0};
    int LCD_buf2[480][800] = {0};
    //装填
    for(y=0;y<480;y++)
    {
        for(x=0;x<800;x++)
        {
            LCD_buf1[y][x] = bmp_buf1[y][x*3] | bmp_buf1[y][x*3+1]<<8 | bmp_buf1[y][x*3+2]<<16;
            LCD_buf2[y][x] = bmp_buf2[y][x*3] | bmp_buf2[y][x*3+1]<<8 | bmp_buf2[y][x*3+2]<<16;
        }
    }
    //显示
    if(direction == right)  //从左往右
    {
        for(z=1;z<800;z+=powf(speed_buf, (800-z/1.3)/800))//这里可以调节动画末尾缓冲速度
            for(y=0;y<480;y++)
                for(x=0;x<800;x++)
                {
                    if(x<=((int)z-1))
                        LCD_addr[y][x] = LCD_buf2[y][799-((int)z-1)+x];
                    else
                        LCD_addr[y][x] = LCD_buf1[y][x-((int)z-1)];
                }
    }else if(direction == left)  //从右往左
    {
        for(z=1;z<800;z+=powf(speed_buf, (800-z/1.3)/800))//这里可以调节动画末尾缓冲速度
            for(y=0;y<480;y++)
                for(x=0;x<800;x++)
                {
                    if(x<=799-((int)z-1))
                        LCD_addr[y][x] = LCD_buf1[y][x+((int)z-1)];
                    else
                        LCD_addr[y][x] = LCD_buf2[y][x-(799-((int)z-1))];
                }
    }else if(direction == down)  //从上往下
    {
        for(z=1;z<480;z+=powf(speed_buf, (480-z/1.3)/480))//这里可以调节动画末尾缓冲速度
            for(y=0;y<480;y++)
                for(x=0;x<800;x++)
                {
                    if(y<=((int)z-1))
                        LCD_addr[y][x] = LCD_buf2[479-((int)z-1)+y][x];
                    else
                        LCD_addr[y][x] = LCD_buf1[y-((int)z-1)][x];
                }
    }else if(direction == up)  //从下往上
    {
        for(z=1;z<480;z+=powf(speed_buf, (480-z/1.3)/480))//这里可以调节动画末尾缓冲速度
            for(y=0;y<480;y++)
                for(x=0;x<800;x++)
                {
                    if(y<=479-((int)z-1))
                        LCD_addr[y][x] = LCD_buf1[y+((int)z-1)][x];
                    else
                        LCD_addr[y][x] = LCD_buf2[y-(479-((int)z-1))][x];
                }
    }

    for(y=0;y<480;y++)  //最后一帧
        for(x=0;x<800;x++)
        {
                LCD_addr[y][x] = LCD_buf2[y][x];
        }

}

//销毁一个图片结构体
void destroy_bmp_t(bmp_t *pic)
{
    if(pic == NULL)
        return;
    if(pic->bmp_buf == NULL)
    {
        free(pic);
        return;
    }
    char *dp = pic->bmp_buf;
    free(dp);
    pic->bmp_buf = NULL;
    free(pic);
}