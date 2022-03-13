
files = $(wildcard *.c) #找出方法文件所有的.c文件
objs = $(patsubst %.c, %.o, $(files)) #将.c替换为.o  作为目标的依赖文件
dep_files := $(patsubst %.c, .%.o.d, $(files)) #将.c加上前缀和后缀  作为c文件的头文件依赖文件
dep_files := $(wildcard $(dep_files)) #取存在的文件

CFLAGS = -Werror -Iinclude #添加编译选项

TARGET := test

$(TARGET) : $(objs)
	gcc -o $@ $^ 

#如果依赖文件存在，则include这文件，即当这些文件改变，会重新编译
ifneq ($(dep_files),)
include $(dep_files)
endif

%.o : %.c 
	gcc $(CFLAGS) -c -o $@ $< -MD -MF .$@.d

clean : 
	rm -f $(shell find -name "*.o")
	rm -f $(TARGET)

distclean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(TARGET)

.PHONY : clean