//三种杀死线程的方法

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

void* thread_fun1(void *arg)
{   
    printf("thread 1 return\n");
    return (void*)111;
}

void* thread_fun2(void *arg)
{   
    printf("thread 2 return\n");
    pthread_exit((void*)222);
}

void* thread_fun3(void *arg)
{   
    /*
    while(1)
    {
        printf("thread 3: going to die in 3 seconds\n");  //pthread_cancel会在进入内核后选择一个契机终止线程，printf和sleep底层都有系统调用，所以能进入内核进而杀死线程
        sleep(1);                                         
    }
    */

    /*
    while(1)                                             //去掉上两个函数且不加取消点，则无法杀死线程
    {
    }
    */

    /*
    while(1)
    {
        pthread_testcancel();                           //自己添加取消点，之后可正常杀死线程
    }
    */
   
    return (void*)666;
}

int main()
{
    pthread_t tid;
    void *retval = NULL;

    pthread_create(&tid, NULL, thread_fun1, NULL);
    pthread_join(tid, &retval);
    printf("thread 1 exit code = %d\n\n", (int)retval);

    pthread_create(&tid, NULL, thread_fun2, NULL);
    pthread_join(tid, &retval);
    printf("thread 2 exit code = %d\n\n", (int)retval);

    pthread_create(&tid, NULL, thread_fun3, NULL);
    sleep(3);
    pthread_cancel(tid);
    pthread_join(tid, &retval);
    printf("thread 3 exit code = %d\n\n", (int)retval);     // pthread_join返回值-1表示异常死亡

    return 0;    
}