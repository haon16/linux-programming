//2021年5月11日12点00分
//利用libevent测试fifo读写

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <event2/event.h>

void read_cb(evutil_socket_t fd, short what, void *arg)
{
    char buf[BUFSIZ] = {0};
    int len = read(fd, buf, sizeof(buf));

    printf("what = %s, read from write : %s\n", what & EV_READ ? "read满足":"read不满足", buf);
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
    //创建fifo
    unlink("testfifo");
    mkfifo("testfifo", 0644);

    //打开fifo的读端
    int fd = open("testfifo", O_RDONLY|O_NONBLOCK);
    if(fd == -1)
    {
        sys_err("open err");
    }

    //创建event_base
    struct event_base *base = event_base_new();

    //创建事件对象
    struct event *ev = event_new(base, fd, EV_READ|EV_PERSIST, read_cb, NULL);

    //添加事件到event_base上
    event_add(ev, NULL);

    //启动循环
    event_base_dispatch(base);      //while(1) ...

    //销毁event_base
    event_base_free(base);

    return 0;
}