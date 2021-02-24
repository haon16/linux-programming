//循环创建多个子线程:错误示例

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
    int i = *((int*)arg);           //解引用得到主线程中i的值，pthread_create底层会使用系统调用clone，子线程运行会慢于主线程，i++值会改变
    sleep(i);
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
        ret = pthread_create(&tid, NULL, thread_fun, (void*)&i);    //传参采用地址传递
        if(ret != 0)
            sys_err("pthread_create error");
    }

    sleep(6);
    printf("I'm main thread:pid = %d, tid = %lu\n", getpid(), pthread_self());

    return 0;    
}