//alarm函数

#include <stdio.h>
#include <unistd.h>

int main()
{
    alarm(1);
    for(int i = 0; ; i++)
    {
        printf("%d\n", i);
    }

    return 0;
}

/*
查看运行时间的差别
 time ./alarm 
 time ./alarm > out

实际执行时间 = 系统时间 + 用户时间 + 等待时间
程序运行的瓶颈在于IO,优化程序，首选优化IO
*/