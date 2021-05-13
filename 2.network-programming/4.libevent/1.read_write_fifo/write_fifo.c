//2021年5月11日12点46分
//利用libevent测试fifo读写

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <event2/event.h>

void write_cb(evutil_socket_t fd, short what, void *arg)
{
    static num = 0;
    char buf[BUFSIZ] = {0};
    sprintf(buf, "hello libevent %d", num++); 

    int len = write(fd, buf, strlen(buf)+1);

    sleep(1);
    
    return;
}

void sys_err(const char *str)
{
    perror(str);
    exit(1);
}

int main()
{
    //打开fifo的写端
    int fd = open("testfifo", O_WRONLY|O_NONBLOCK);
    if(fd == -1)
    {
        sys_err("open err");
    }

    //创建event_base
    struct event_base *base = event_base_new();

    //创建事件对象
    //struct event *ev = event_new(base, fd, EV_WRITE, write_cb, NULL);
    struct event *ev = event_new(base, fd, EV_WRITE|EV_PERSIST, write_cb, NULL);

    //添加事件到event_base上
    event_add(ev, NULL);

    //启动循环
    event_base_dispatch(base);      //while(1) ...

    //销毁event_base
    event_base_free(base);

    return 0;
}