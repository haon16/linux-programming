//读写锁

#define _XOPEN_SOURCE 500     //要加上这个宏才可使用 rwlock相关类型及函数 usleep
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int counter;
pthread_rwlock_t rwlock;

//3个线程不定时写同一全局资源，5个线程不定时读同一全局资源
void *thread_write(void *arg)
{
    int t;
    int i = (int)arg;
    while(1)
    {
        pthread_rwlock_wrlock(&rwlock);     //以写模式加锁，写独占
        t = counter;
        usleep(1000);       //一睡眠会失去CPU，读线程可能获得这把锁，但因为写线程是独占的，所以此时这把锁不会被读线程加锁成功（睡着的时候也抱着这把锁）
        printf("write %d:%lu: counter=%d ++counter=%d\n", i, pthread_self(), t, ++counter);
        pthread_rwlock_unlock(&rwlock);
        usleep(10000);
    }
    return NULL;
}

void *thread_read(void *arg)
{
    int i = (int)arg;
    while(1)
    {
        pthread_rwlock_wrlock(&rwlock);     //读线程间，读锁共享
        printf("read %d:%lu: counter=%d\n", i, pthread_self(), counter);
        pthread_rwlock_unlock(&rwlock);
        usleep(2000);
    }
    return NULL;
}

int main()
{
    pthread_t tid[8];
    pthread_rwlock_init(&rwlock, NULL);

    int i;
    for(i = 0; i < 3; i++)
        pthread_create(&tid[i], NULL, thread_write, (void*)i);
    
    for(i = 0; i < 5; i++)
        pthread_create(&tid[i+3], NULL, thread_read, (void*)i);
    
    for(i = 0; i < 8; i++)
        pthread_join(tid[i], NULL);

    pthread_rwlock_destroy(&rwlock);

    return 0;
}

