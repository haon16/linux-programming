//信号捕捉函数signal

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void sys_err(const char* str)
{
    perror(str);
    exit(1);
}

void sig_catch(int signo)
{
    printf("catch you!!! %d\n", signo);
    return;
}

int main()
{
    signal(SIGINT, sig_catch);

    while(1);
    
    return 0;
}