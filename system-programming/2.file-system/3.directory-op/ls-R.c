//递归遍历目录实现

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

void isFile(char *filename);

void readDir(char *dir)
{
    //打开目录
    DIR *dp = opendir(dir);
    if(dp == NULL)
    {
        perror("opendir error");
        return;
    }

    //读取目录项
    char path[256] = {0};
    struct dirent *sdp;
    while((sdp = readdir(dp)) != NULL)
    {
        if(strcmp(sdp->d_name, ".") == 0 || strcmp(sdp->d_name, "..") == 0)
            continue;
        
        //目录项本身不可访问，拼接目录、目录项
        sprintf(path, "%s/%s", dir, sdp->d_name);
        //判断文件类型，目录递归进入，文件显示名字/大小
        isFile(path);
    }

    closedir(dp);
    return;
}

void isFile(char *filename)
{
    //获取文件属性，判断文件类型
    struct stat buf;
    int ret = stat(filename, &buf);
    if(ret == -1)
    {
        perror("stat error");
        return;
    }

    //目录文件
    if(S_ISDIR(buf.st_mode))
    {
        readDir(filename);
    }

    //普通文件
    printf("%s\t%ld\n", filename, buf.st_size);

    return;
}

int main(int argc, char *argv[])
{
    //判断命令行参数
    if(argc == 1)
    {
        isFile(".");
    }
    else
    {
        isFile(argv[1]);
    }

    return 0;
}