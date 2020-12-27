//互斥锁
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

pthread_mutex_t mutex;  //定义一把互斥锁    可以想象为一个int

void *fun(void *arg)
{
    srand(time(NULL));

    while (1)
    {
        pthread_mutex_lock(&mutex);     //加锁                  可以想象成，锁-- （1-- -> 0）
        printf("hello ");
        sleep(rand()%3);    //模拟长时间操作共享资源，导致CPU易主，产生与时间有关的错误
        printf("world\n");
        pthread_mutex_unlock(&mutex);   //解锁                  可以想象成，锁++ （0++- -> 1）
        sleep(rand()%3);
    }

    return NULL;
}

int main()
{
    srand(time(NULL));
    pthread_t tid;

    int ret = pthread_mutex_init(&mutex, NULL);     //初始化互斥锁      可以认为，锁的值为1
    if(ret != 0)
    {
        fprintf(stderr, "pthread_mutex_init error:%s\n", strerror(ret));
        exit(1);
    }

    pthread_create(&tid, NULL, fun, NULL);
    while (1)
    {
        pthread_mutex_lock(&mutex);     //加锁
        printf("HELLO ");
        sleep(rand()%3);
        printf("WORLD\n");
        pthread_mutex_unlock(&mutex);   //解锁
        sleep(rand()%3);
    }
    pthread_join(tid, NULL);
    pthread_mutex_destroy(&mutex);      //销毁互斥锁
    return 0;
}