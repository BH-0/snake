//
// Created by 10031 on 2022/10/18.
//
#ifndef __KEYBOARD_H
#define __KEYBOARD_H
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>

/*全局变量声明*/
extern char scan_keyboard;

//键盘监听线程
void *scan_keyboard_task(void *arg);

#endif //SNAKE_KEYBOARD_H
