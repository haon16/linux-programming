//线程间全局变量

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int var = 100;

void *thread_fun(void *arg)
{
    var = 200;
    printf("thread change var = %d\n", var);
    
    return NULL;
}

int main()
{
    printf("At first, var = %d\n", var);

    pthread_t tid;
    pthread_create(&tid, NULL, thread_fun, NULL);
    sleep(1);

    printf("After pthread_create, var = %d\n", var);

    return 0;
}