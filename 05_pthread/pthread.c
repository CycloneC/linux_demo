#define _GNU_SOURCE
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

void *pthread_1(void *param)
{
    pthread_t tid = pthread_self(); //获取线程ID

    while (1)
    {
        sleep(1);
        printf("this is tid1 = %lu\n", (unsigned long)tid);
    }
}

void *pthread_2(void *param)
{
    int i = 0;
    pthread_t tid = pthread_self();
    pthread_t tid1 = *(pthread_t *)param;

    while (1)
    {
        sleep(1);
        printf("this is tid2 = %lu\n", (unsigned long)tid);

        i++;
        if (i >= 10)
        {
            pthread_cancel(tid1);   //使另一个线程退出
            pthread_exit(NULL);     //使本线程退出
        }
    }
}

int main(int argc, char const *argv[])
{
    int ret;
    pthread_t tid1, tid2;

    ret = pthread_create(&tid1, NULL, pthread_1, NULL);
    if (ret)
    {
        perror("pthread_create: ");
        return -1;
    }
    printf("create tid1=%lu\n", (unsigned long)tid1);

    ret = pthread_create(&tid2, NULL, pthread_2, &tid1);
    if (ret)
    {
        perror("pthread_create: ");
        return -1;
    }
    printf("create tid2=%lu\n", (unsigned long)tid2);

    /* 线程资源回收 阻塞方式*/
    pthread_join(tid1, NULL);
    printf("join tid1\n");

    /* 线程资源回收 非阻塞方式*/
    while (1)
    {
        if (pthread_tryjoin_np(tid2, NULL) == 0)
        {
            printf("join tid2\n");
            break;
        }
        
    }

    return 0;
}
