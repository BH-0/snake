# snake

该项目基于SP6816平台，属于智能音影系统的子项目

![](https://files.catbox.moe/25wlop.png)

![](https://files.catbox.moe/bso072.png)

![](https://files.catbox.moe/ngm3hq.png)

文件结构

```
│   .gitignore
│   Makefile
│   README.md
│
├───.idea
│       .gitignore
│       misc.xml
│       vcs.xml
│       workspace.xml
│
├───.vscode
├───bin 编译后的程序
│       test_arm
│
├───include
│       beep.h
│       bmp.h
│       file.h
│       font.h
│       icon.h
│       jpeg.h
│       keyboard.h
│       lcd.h
│       main.h
│       map_show.h
│       picture.h
│       snake.h
│       touch.h
│       utf8_gb2312.h
│
├───lib
│   │   libfont.a 字库
│   │
│   └───libjpeg jpeg库
│           jconfig.h
│           jerror.h
│           jmorecfg.h
│           jpeglib.h
│           libjpeg.a
│           libjpeg.la
│           libjpeg.so
│           libjpeg.so.9
│           libjpeg.so.9.1.0
│
├───main 主函数
│       main.c
│
├───project_home
│   │   test_arm 编译后的程序
│   │
│   └───menu 菜单界面图标
│           1_Trim.mp4
│           2_Trim.mp4
│           about.bmp
│           game_points.txt
│           game_points_list.bmp
│           Image1.bmp
│           Image2.bmp
│           Image3.bmp
│           Image4.bmp
│           Image5.bmp
│           Image6.bmp
│           Image7.bmp
│           key1.bmp
│           key1.txt
│           menu.txt
│           s1.bmp
│           s2.bmp
│           s3.bmp
│           s4.bmp
│           s5.bmp
│           set.bmp
│           set.txt
│
└───src 模块程序
        beep.c
        bmp.c
        file.c
        font.c
        GB2312.c
        icon.c
        jpeg.c
        keyboard.c
        lcd.c
        map_show.c
        picture.c
        snake.c
        touch.c
        utf8_gb2312.c
```

