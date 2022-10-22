#include "beep.h"

int beep_fd;    //蜂鸣器文件描述符
int beep_cmd = 0;   //发声使能
int tone1[7] = {25000, 20000, 15000, 10000, 5000, 2000, 1}; //半周 单位us,第一个为低音7

void beep_us(int us)
{
    BEEP_ON();
    usleep(10);
    BEEP_OFF();
    usleep(us);
}

//发声线程
void *beep_voice(void *arg)
{
    pthread_detach(pthread_self()); //分离属性
    for(;;) {
        if (beep_cmd == 1)  //模式1 吃到东西
        {
            BEEP_ON();
            usleep(20000);
            BEEP_OFF();
            beep_cmd = 0;
        }else if (beep_cmd == 2)    //模式2 撞车
        {
            for(int i = 0; i <10; i++)
                beep_us(tone1[0]);
            BEEP_OFF();
            beep_cmd = 0;
        }
        else if (beep_cmd == 3)    //模式3 菜单按钮
        {
            for(int i = 0; i <5; i++)
                beep_us(tone1[5]);
            BEEP_OFF();
            beep_cmd = 0;
        }
        else
        {
            BEEP_OFF();
        }
    }
}


//蜂鸣器初始化
int beep_init(void)
{
    // 打开蜂鸣器
    beep_fd = open("/dev/beep", O_RDWR);
    if(beep_fd == -1)
    {
        perror("open:");
        return -1;
    }
    //初始化发声线程
    pthread_t beep_voice_t;
    pthread_create(&beep_voice_t,NULL,beep_voice,NULL);

    return 0;
}




