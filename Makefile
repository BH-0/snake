# syspath & tools
CC = arm-linux-gcc
GCC = gcc

#all:
#	make x86

# build target
TARGET_x86		:= ./bin/test_x86
TARGET_arm		:= ./bin/test_arm

# compile option
LIBS			+= -lm -lpthread
#CFLAGS			+= -rdynamic -Wall -pedantic -O0 -Wno-uninitialized
CFLAGS			+= -Wall -pedantic -O0 -Wno-uninitialized
LDFLAGS			+= $(LIBS)

#path
C_SOURCE = ./main/main.c ./src/*.c
INCLUDE_PATH = -I./include -I./lib/libjpeg
LIBRARY_PATH = -L./lib -lfont 
LIBRARY_JPEG_PATH = -L./lib/libjpeg -ljpeg

# build command
.PHONY: x86 arm clean

#all: mngdir_x86 $(TARGET_x86)
all: mngdir_x86 $(TARGET_x86)

mngdir_x86:
$(TARGET_x86):$(C_SOURCE)
	$(GCC) $^ -o $@ $(INCLUDE_PATH) $(LIBRARY_PATH) $(LIBRARY_JPEG_PATH) $(LDFLAGS) $(CFLAGS)
	@echo "*********************	Bulid $(TARGET_x86) complete	************************"


arm: mngdir_arm $(TARGET_arm)

mngdir_arm:
$(TARGET_arm):$(C_SOURCE)
	$(CC) $^ -o $@ $(INCLUDE_PATH) $(LIBRARY_PATH) $(LIBRARY_JPEG_PATH) $(LDFLAGS) $(CFLAGS)
	@echo "*********************	Bulid $(TARGET_arm) complete	************************"

clean:
	rm $(TARGET_x86) -fv
	rm $(TARGET_arm) -fv