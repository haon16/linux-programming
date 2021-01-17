//open函数测试

#include <unistd.h>		//open
#include <fcntl.h>		//O_RDONLY头文件
#include <stdio.h>		//printf
#include <errno.h>
#include <string.h>

int main()
{
	int fd1 = open("./test1.txt", O_RDONLY);
	//int fd = open("./test.cp", O_RDONLY|O_CREAT, 0644);		//创建文件时指定文件权限 文件权限 = mode & ~umask
	//int fd = open("./test.txt", O_RDONLY|O_TRUNC);			//文件截断为0
	int fd = open("./text.cp", O_RDONLY|O_CREAT|O_TRUNC, 0644);	//如果文件存在，截断文件后以只读模式访问，如果文件不存在，则先创建再以只读访问
	

	printf("fd = %d\n", fd);
	if(fd1 == -1)
	{
		printf("fd1 = %d, errno = %d:%s\n", fd1, errno, strerror(errno));
	}

	close(fd);

	return 0;
}
