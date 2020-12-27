//多个消费者一个生产者

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

        pthread_cond_signal(&has_data);              //唤醒阻塞在条件变量 has_data 上的线程

        sleep(rand()%3);
    }

    return NULL;
}

void *comsumer(void *arg)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);                     //加锁互斥量

        while(head == NULL)                             //多个消费者这边得是while才行，有数据后多个消费者被唤醒，其中一个消费者消耗数据后数据可能为空，其他消费者得继续判断，有数据才能接着访问
        {
            pthread_cond_wait(&has_data, &mutex);       //阻塞等待条件变量，解锁
        }                                               //pthread_cond_wait返回时，重新加锁mutex

        struct msg *mp = head;
        head = head->next;
         
        pthread_mutex_unlock(&mutex);                   //解锁互斥量
        printf("----------comsumer id: %lu : %d\n", pthread_self(), mp->num);

        free(mp);
        sleep(rand()%3);
    }

    return NULL;
}

int main()
{
    srand(time(NULL));

    pthread_t pid, cid1, cid2, cid3;
    int ret = pthread_create(&pid, NULL, producer, NULL);   //生产者
    if(ret != 0)
        err_thread(ret, "pthread_create producer error");

    ret = pthread_create(&cid1, NULL, comsumer, NULL);       //消费者
    if(ret != 0)
        err_thread(ret, "pthread_create comsumer error");

    ret = pthread_create(&cid2, NULL, comsumer, NULL);       //消费者
    if(ret != 0)
        err_thread(ret, "pthread_create comsumer error");
    
    ret = pthread_create(&cid3, NULL, comsumer, NULL);       //消费者
    if(ret != 0)
        err_thread(ret, "pthread_create comsumer error");

    pthread_join(pid, NULL);
    pthread_join(cid1, NULL);
    pthread_join(cid2, NULL);
    pthread_join(cid3, NULL);

    return 0;
}