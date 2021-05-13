//2021年5月12日12点39分
//libevent实现基本的TCP服务器-客户端

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>

//读缓冲回调
void read_cb(struct bufferevent *bev, void *ctx)
{
    char buf[1024] = {0};

    bufferevent_read(bev, buf, sizeof(buf));
    printf("receive from client: %s", buf);

    char *p = "receive ok!\n";

    //写数据给客户端
    bufferevent_write(bev, p, strlen(p)+1);

    sleep(1);
}

//写缓冲回调
void write_cb(struct bufferevent *bev, void *ctx)
{
    printf("成功写数据给客户端，写缓冲区回调函数被调用\n");
}

//事件回调
void event_cb(struct bufferevent *bev, short what, void *ctx)
{
    if(what & BEV_EVENT_EOF)
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

//监听器回调：有客户端连接上服务器会产生回调
void listener_cb(struct evconnlistener *listener , evutil_socket_t fd, struct sockaddr *addr, int len, void *ptr)
{
    printf("connect new client\n");

    struct event_base *base = (struct event_base*)ptr;

    //创建bufferevent事件
    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

    //给bufferevent设置回调
    bufferevent_setcb(bev, read_cb, write_cb, event_cb, NULL);

    //启用bufferevent的读缓冲，默认是disable的
    bufferevent_enable(bev, EV_READ);
}

int main()
{
    //服务器地址信息
    struct sockaddr_in serv_adr;
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons(9999);
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);

    //创建event_base
    struct event_base *base = event_base_new();

    //创建服务器监听器
    struct evconnlistener *listener = evconnlistener_new_bind(base, listener_cb, base, LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, 36, (struct sockaddr*)&serv_adr, sizeof(serv_adr));

    //启动循环监听
    event_base_dispatch(base);

    //释放资源
    evconnlistener_free(listener);
    event_base_free(base);

    return 0;
}