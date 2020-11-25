//循环创建多个子进程

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    int i;
    for(i = 0; i < 5; i++)
    {
        if(fork() == 0)
            break;
    }

    if(i == 5)
    {
        //sleep(1);
        printf("I'm parent\n");
    }
    else
        printf("I'm %dth child\n", i+1);

    return 0;
}