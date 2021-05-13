//2021年5月12日17点27分
//libevent实现基本的TCP服务器-客户端

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <event2/bufferevent.h>
#include <event2/event.h>

//读缓冲回调
void read_cb(struct bufferevent *bev, void *ctx)
{
    char buf[1024] = {0};
    bufferevent_read(bev, buf, sizeof(buf));

    printf("receive from server : %s", buf);

    //bufferevent_write(bev, buf, strlen(buf)+1);
    sleep(1);
}

//写缓冲回调
void write_cb(struct bufferevent *bev, void *ctx)
{
    printf("成功写数据给服务器，写缓冲区回调函数被调用\n");
}

//事件回调
void event_cb(struct bufferevent *bev, short what, void *ctx)
{
    if(what & BEV_EVENT_CONNECTED)
    {
        printf("connect server ok\n");  
        return;
    }
    else if(what & BEV_EVENT_EOF)
    {
        printf("connection closed\n");
    }
    else if(what & BEV_EVENT_ERROR)
    {
        printf("some other error\n");
    }

    bufferevent_free(bev);
    printf("buffevent资源已经被释放\n");
}

//客户端与用户交互，从终端读取数据写给服务器
void read_terminal(evutil_socket_t fd, short what, void *arg)
{   
    //读数据
    char buf[1024] = {0};
    int len = read(fd, buf, sizeof(buf));

    struct bufferevent* bev = (struct bufferevent*)arg;
    //发送数据
    bufferevent_write(bev, buf, len);
}

int main()
{
    //创建event_base
    struct event_base *base = event_base_new();

    //创建bufferevent事件
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

    //服务器地址信息
    struct sockaddr_in serv_adr;
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons(9999);
    inet_pton(AF_INET, "127.0.0.1", &serv_adr.sin_addr.s_addr);

    //连接服务器
    bufferevent_socket_connect(bev, (struct sockaddr*)&serv_adr, sizeof(serv_adr));

    //设置回调
    bufferevent_setcb(bev, read_cb, write_cb, event_cb, NULL);

    //设置读回调生效
    bufferevent_enable(bev, EV_READ);

    //创建事件
    struct event *ev = event_new(base, STDIN_FILENO, EV_READ|EV_PERSIST, read_terminal, bev);

    //添加事件
    event_add(ev, NULL);

    //启动循环
    event_base_dispatch(base);

    //资源释放
    event_free(ev);
    event_base_free(base);

    return 0;
}