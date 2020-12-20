//pthread_exit退出

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void sys_err(const char *str)
{
    perror(str);
    exit(1);
}

void* thread_fun(void *arg)
{
    int i = (int)arg;
    sleep(i);
    if(i == 2)
    {
        //exit(1);          //表示退出进程
        //return NULL;      //表示返回到调用者那里去
        pthread_exit(NULL);
    }
    printf("I'm %dth thread:pid = %d, tid = %lu\n", i+1, getpid(), pthread_self());
    return NULL;
}

int main()
{
    int i;
    int ret;
    pthread_t tid;

    for(i = 0; i < 5; i++)
    {
        ret = pthread_create(&tid, NULL, thread_fun, (void*)i);
        if(ret != 0)
            sys_err("pthread_create error");
    }

    //sleep(i);
    printf("I'm main thread:pid = %d, tid = %lu\n", getpid(), pthread_self());

    //return 0;    
    pthread_exit(NULL);     //退出主线程，其他线程正常运行，不需要sleep延迟主线程结束以输出子线程结果
}