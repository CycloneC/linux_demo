#define _GNU_SOURCE
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

static sem_t sem;               //信号量
static pthread_mutex_t mutex;   //互斥量 一般申请全局变量
static char share_buf[1024] = {0};

void *pthread_1(void *param)
{
    while (1)
    {
        /* 等待通知 */
        sem_wait(&sem);

        pthread_mutex_lock(&mutex);
        printf("recv: %s\n", share_buf);
        pthread_mutex_unlock(&mutex);
    }
}

void *pthread_2(void *param)
{
    char buf[1024] = {0};

    while (1)
    {
        if (fgets(buf, 1024, stdin))
        {
            pthread_mutex_lock(&mutex);
            memcpy(share_buf, buf, 1024);
            pthread_mutex_unlock(&mutex);

            /* 通知接收线程 */
            sem_post(&sem);
        }
    }
}

int main(int argc, char const *argv[])
{
    int ret;
    pthread_t tid1, tid2;

    ret = sem_init(&sem, 0, 0);//初始化信号量
    if (ret != 0)
    {
        perror("sem_init: ");
        return -1;
    }

    ret = pthread_mutex_init(&mutex, NULL);//初始化互斥量
    if (ret != 0)
    {
        perror("pthread_mutex_init: ");
        return -1;
    }

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

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_mutex_destroy(&mutex);
    sem_destroy(&sem);
    return 0;
}
