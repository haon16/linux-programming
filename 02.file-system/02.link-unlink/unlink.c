//unlink函数是删除一个dentry

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    int fd = open("temp.txt", O_RDWR|O_CREAT|O_TRUNC, 0644);
    if(fd < 0)
    {
        perror("open temp error");
        exit(1);
    }

    int ret;
    ret = unlink("temp.txt");       //具备了被释放的条件
    if(ret < 0)
    {
        perror("unlink error");
        exit(1);
    }

    char *p1 = "test of unlink\n";
    char *p2 = "after write something\n";
    ret = write(fd, p1, strlen(p1));
    if(ret == -1)
        perror("write error");

    printf("write ok\n");
    ret = write(fd, p2, strlen(p2));
    if(ret == -1)
        perror("write error");

    printf("Enter any key continue\n");
    getchar();

    //p1[3] = 'H';     //段错误，会提前结束程序，如果在程序尾unlink，则temp.txt不能正确释放，会被保留，如果放在open后则正常

    close(fd);

    /*
    ret = unlink("temp.txt");       //具备了被释放的条件
    if(ret < 0)
    {
        perror("unlink error");
        exit(1);
    }
    */

    return 0;
}