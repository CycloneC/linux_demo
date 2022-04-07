#include <stdio.h>
#include <stdlib.h>

int func(int a)
{
    int *p = 0;

    *p = a; //空指针赋值，此处程序异常退出
}

int main(int argc, char const *argv[])
{
    int a = 0;

    printf("start...\n");
    func(a);
    printf("end...\n");

    return 0;
}
