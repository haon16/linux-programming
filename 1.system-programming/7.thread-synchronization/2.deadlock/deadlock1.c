//死锁情形1：同一个线程对一个锁反复lock

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex;

int main()
{
    pthread_mutex_init(&mutex, NULL);
    
    pthread_mutex_lock(&mutex);
    printf("before deadlock\n");
    pthread_mutex_lock(&mutex);
    printf("after deadlock\n");

    pthread_mutex_destroy(&mutex);
    return 0;
}