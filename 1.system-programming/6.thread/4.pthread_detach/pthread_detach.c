//线程分离pthread_detach

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void* thread_fun(void *arg)
{
    printf("thread:pid = %d, tid = %lu\n", getpid(), pthread_self());
    return NULL;
}

int main()
{
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, thread_fun, NULL);
    if(ret != 0)
    {
        fprintf(stderr, "pthread_create error: %s\n", strerror(ret));   //线程中错误返回的就是错误码，用perror实际上是错的，perror底层是去找errno的值翻译成字符串然后再拼上我们所给的字符串
        exit(1);
    }
    
    ret = pthread_detach(tid);              //设置线程分离，线程终止会自动清理PCB,无需回收
    if(ret != 0)
    {
        fprintf(stderr, "pthread_detach error: %s\n", strerror(ret));
        exit(1);
    }

    sleep(1);

    ret = pthread_join(tid, NULL);
    //printf("pthread_join ret = %d\n", ret);       //线程分离后这边ret != 0，会去执行判断里面的语句，如果是perror(str)打印的是pthread_join error: Success    错误的
    if(ret != 0)                                    //改成strerror(ret)后打印的是pthread_join error: Invalid argument
    {
        fprintf(stderr, "pthread_join error: %s\n", strerror(ret));
        exit(1);
    }

    printf("main:pid = %d, tid = %lu\n", getpid(), pthread_self());

    return 0;    
}