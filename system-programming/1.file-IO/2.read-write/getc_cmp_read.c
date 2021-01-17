//系统调用和库函数比较：库函数实现单字符拷贝文本

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	FILE *fp1 = fopen(argv[1], "r");
	if(fp1 == NULL)
	{
		perror("file1 fopen error");
		exit(1);
	}

	FILE *fp2 = fopen(argv[2], "w");
	if(fp2 == NULL)
	{
		perror("file2 fopen error");
		exit(1);
	}

	int n;
	while((n = fgetc(fp1)) != EOF)
	{
		fputc(n, fp2);
	}
	
	fclose(fp1);
	fclose(fp2);
	return 0;
}

// strace 程序执行命令 ：可以追踪程序执行中所用到的系统调用

// 机制：预读入缓输出
