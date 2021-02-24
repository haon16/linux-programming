//条件变量实现生产者消费者

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void err_thread(int ret, char *str)
{
    if(ret != 0)
    {
        fprintf(stderr, "%s:%s\n", str, strerror(ret));
        pthread_exit(NULL);
    }
}

struct msg
{
    int num;
    struct msg *next; 
};
struct msg *head;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;      //定义并且初始化一个互斥量
pthread_cond_t has_data = PTHREAD_COND_INITIALIZER;     //定义并且初始化一个条件变量

void *producer(void *arg)
{
    while(1)
    {
        struct msg *mp = malloc(sizeof(struct msg));
        mp->num = rand() % 1000 + 1;                    //模拟生产一个数据
        printf("-----produce %d\n", mp->num);

        pthread_mutex_lock(&mutex);                     //加锁互斥量
        mp->next = head;                                //写公共区域
        head = mp;
        pthread_mutex_unlock(&mutex);                   //解锁互斥量

        pthread_cond_signal(&has_data);                 //唤醒阻塞在条件变量 has_data 上的线程

        sleep(rand()%3);
    }

    return NULL;
}

void *comsumer(void *arg)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);                     //加锁互斥量

        if(head == NULL)                                //只有一个消费者时，这边if和while都行，无区别
        {
            pthread_cond_wait(&has_data, &mutex);       //阻塞等待条件变量，解锁
        }                                               //pthread_cond_wait返回时，重新加锁mutex

        struct msg *mp = head;
        head = head->next;
        
        pthread_mutex_unlock(&mutex);                   //解锁互斥量
        printf("----------comsumer %d\n", mp->num);

        free(mp);
        sleep(rand()%3);
    }

    return NULL;
}

int main()
{
    srand(time(NULL));

    pthread_t pid, cid;
    int ret = pthread_create(&pid, NULL, producer, NULL);   //生产者
    if(ret != 0)
        err_thread(ret, "pthread_create producer error");

    ret = pthread_create(&cid, NULL, comsumer, NULL);       //消费者
    if(ret != 0)
        err_thread(ret, "pthread_create comsumer error");

    pthread_join(pid, NULL);
    pthread_join(cid, NULL);

    return 0;
}