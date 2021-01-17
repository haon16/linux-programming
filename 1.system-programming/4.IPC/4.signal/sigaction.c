//信号捕捉函数sigaction

#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void sys_err(const char* str)
{
    perror(str);
    exit(1);
}

void sig_catch(int signo)           //回调函数
{
    if(signo == SIGINT)
    {
        printf("catch SIGINT!!! %d\n", signo);
        sleep(10);
    }
    else if(signo == SIGBUS)
        printf("catch SIGBUS!!! %d\n", signo);
    return;
}

int main()
{
    struct sigaction act, oldact;

    act.sa_handler = sig_catch;         //设置回调函数
    sigemptyset(&act.sa_mask);          //清空sa_mask屏蔽字，只在sig_catch工作时有效
    act.sa_flags = 0;                   //默认值

    int ret = sigaction(SIGINT, &act, &oldact);     //注册信号捕捉函数
    if(ret == -1)
        sys_err("sigaction error");
    
    ret = sigaction(SIGBUS, &act, &oldact);
    if(ret == -1)
        sys_err("sigaction error");

    while(1);
    
    return 0;
}