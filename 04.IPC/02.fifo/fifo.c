//fifo

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

void sys_error(const char *str)
{
	perror(str);
	exit(1);
}

int main()
{
	int ret = mkfifo("myfifo", 0644);
	if(ret == -1)
		sys_error("mkfifo error");

	return 0;
}
