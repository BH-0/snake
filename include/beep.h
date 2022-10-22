//
// Created by 10031 on 2022/10/22.
//

#ifndef __BEEP_H
#define __BEEP_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#define ON  0
#define OFF 1
#define BEEP_ON() ioctl(beep_fd, ON, 1)
#define BEEP_OFF() ioctl(beep_fd, OFF, 1)

extern int beep_fd;
extern int beep_cmd;   //发声使能
//蜂鸣器初始化
int beep_init(void);



#endif //__BEEP_H
