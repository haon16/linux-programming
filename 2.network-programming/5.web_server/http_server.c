//2021年5月18日22点50分
//web服务器的简单实现

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <errno.h>
#include <dirent.h>

#define MAXSIZE 2048

void epoll_run(int port);
int init_listen_fd(int port, int epfd);
void do_accept(int lfd, int epfd);
void do_read(int cfd, int epfd);
int get_line(int cfd, char *buf, int size);
void disconnect(int cfd, int epfd);
void http_request(int cfd, const char *file);
void send_error(int cfd, int status, char* desc, char* text);
const char *get_file_type(const char *name);
void send_respond_head(int cfd, int status, char *descrip, const char *type, int len);
void send_file(int cfd, const char *file);
void send_dir(int cfd, const char *dir);

int main(int argc, char *argv[])
{
    //命令行参数获取端口和server提供的目录
    if(argc < 3)
    {
        printf("Usage: %s <port> <path>\n", argv[0]);
        exit(1);
    }

    //获取用户输入的端口
    int port = atoi(argv[1]);

    //改变进程工作目录
    int ret = chdir(argv[2]);
    if(ret != 0)
    {
        perror("chdir() error");
        exit(1);
    }

    //启动epoll监听
    epoll_run(port);

    return 0;
}

void epoll_run(int port)
{
    //创建一个epoll监听树根
    int epfd = epoll_create(MAXSIZE);
    if(epfd == -1)
    {
        perror("epoll_create() error");
        exit(1);
    }

    //创建lfd并添加至监听树
    int lfd = init_listen_fd(port, epfd);

    struct epoll_event all_events[MAXSIZE];

    while (1)
    {
        //监听节点对应事件
        int ret = epoll_wait(epfd, all_events, MAXSIZE, -1);
        if(ret == -1)
        {
            perror("epoll_wait() error");
            exit(1);
        }

        for(int i = 0; i < ret; i++)
        {
            //只处理读事件，其他事件默认不处理
            struct epoll_event *pev = &all_events[i];

            //不是读事件
            if(!(pev->events & EPOLLIN))
                continue;

            if(pev->data.fd == lfd)         //接受连接请求
            {
                do_accept(lfd, epfd);
            }
            else                            //读数据
            {
                do_read(pev->data.fd, epfd);
            }
        }
    }
}

int init_listen_fd(int port, int epfd)
{
    //创建监听的套接字lfd
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd == -1)
    {
        perror("socket() error");
        exit(1);
    }

    //创建服务器地址结构
    struct sockaddr_in serv_adr;
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons(port);
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);

    //设置端口复用
    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    //绑定地址
    int ret = bind(lfd, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
    if(ret == -1)
    {
        perror("bind() error");
        exit(1);
    }

    //设置监听上限
    ret = listen(lfd, 128);
    if(ret == -1)
    {
        perror("listen() error");
        exit(1);
    }

    //将lfd添加到epoll树上
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = lfd;

    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
    if(ret == -1)
    {
        perror("epoll_ctl() add lfd error");
        exit(1);
    }

    return lfd;
}

void do_accept(int lfd, int epfd)
{
    struct sockaddr_in clnt_adr;
    socklen_t clnt_adr_len = sizeof(clnt_adr);

    int cfd = accept(lfd, (struct sockaddr*)&clnt_adr, &clnt_adr_len);
    if(cfd == -1)
    {
        perror("accept() error");
        exit(1);
    }

    //打印客户端IP+port
    char client_ip[64] = {0};
    printf("new client IP: %s, Port: %d, cfd = %d\n", inet_ntop(AF_INET, &clnt_adr.sin_addr.s_addr, client_ip, sizeof(client_ip)), ntohs(clnt_adr.sin_port), cfd);

    //设置cfd非阻塞
    int flag = fcntl(cfd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(cfd, F_SETFL, flag);

    //将新节点cfd挂到epoll监听树上
    struct epoll_event ev;
    ev.data.fd = cfd;
    ev.events = EPOLLIN | EPOLLET;      //边缘触发

    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
    if(ret == -1)
    {
        perror("epoll_ctl() add cfd error");
        exit(1);
    }
}

void do_read(int cfd, int epfd)
{
    char line[1024] = {0};
    int len = get_line(cfd, line, sizeof(line));
    if(len == 0)
    {
        printf("client disconnect\n");
        disconnect(cfd, epfd);
    }
    else if(len == -1)
    {
        perror("get_line() error");
    }
    else
    {
        printf("请求行数据：%s", line);
        while(1)    //把缓冲区数据读完
        {
            char buf[1024] = {0};
            len = get_line(cfd, buf, sizeof(buf));
            if(len == -1)
                break;

            //printf("len = %d, buf = %s\n", len, buf);
            //sleep(1);
        }

        if(strncasecmp(line, "GET", 3) == 0)
        {
            http_request(cfd, line);
            disconnect(cfd, epfd);
        }
    }
}

//获取一行以\r\n结尾的数据（http协议每行以/r/n结尾）
int get_line(int cfd, char *buf, int size)
{
    int i = 0;
    char c = '\0';
    int n;
    while((i < size-1) && (c != '\n'))
    {
        n = recv(cfd, &c, 1, 0);
        //printf("n = %d, %c\n", n, c);
        if(n > 0)
        {
            if(c == '\r')
            {
                n = recv(cfd, &c, 1, MSG_PEEK);
                if((n > 0) && (c == '\n'))
                {
                    recv(cfd, &c, 1, 0);
                    //printf("pre:\\r, cur:\\n \n");
                }
                else
                {
                    c = '\n';
                }
            }
            buf[i] = c;
            i++;
        }
        else
        {
            c = '\n';
        }
    }
    buf[i] = '\0';
    if(-1 == n)
        i = n;

    return i;
}

void disconnect(int cfd, int epfd)
{
    int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, NULL);
    if(ret != 0)
    {
        perror("epoll_ctl() delete cfd error");
        exit(1);
    }
    close(cfd);
}

void http_request(int cfd, const char *line)
{
    //拆分http请求行
    char method[16], path[256], protocol[16];
    sscanf(line, "%[^ ] %[^ ] %[^ ]", method, path, protocol);      //正则[^ ] 非空的任意字符
    printf("method=%s, path=%s, protocol=%s\n", method, path, protocol);

    char *file = path + 1;      //去掉path中的'/'获取访问的文件名
    if(strcmp(path, "/") == 0)  //未指定访问的资源，默认显示资源目录中的内容
    {
        file = "./";
    }

    //判断文件是否存在
    struct stat sbuf;
    int ret = stat(file, &sbuf);
    if(ret != 0)
    {
        perror("stat() error");
        send_error(cfd, 404, "Not Found", "No such file or direntry");
        return;
    }

    if(S_ISREG(sbuf.st_mode))       //文件
    {
        //发送消息报头
        send_respond_head(cfd, 200, "OK", get_file_type(file), sbuf.st_size);   //sbuf.st_size或者-1
        //发送文件内容
        send_file(cfd, file);
    }
    else if(S_ISDIR(sbuf.st_mode))  //目录
    {
        send_respond_head(cfd, 200, "OK", get_file_type("./html"), -1);
        send_dir(cfd, file);
    }
}

void send_error(int cfd, int status, char* desc, char* text)
{
    char buf[4096] = {0};
    sprintf(buf, "%s %d %s\r\n", "HTTP/1.1", status, desc);
    sprintf(buf+strlen(buf), "Content-Type:%s\r\n", "text/html");
    sprintf(buf+strlen(buf), "Content-Length:%d\r\n", -1);
    sprintf(buf+strlen(buf), "Connection: close\r\n");
    send(cfd, buf, strlen(buf), 0);
    send(cfd, "\r\n", 2, 0);

    memset(buf, 0, sizeof(buf));

    sprintf(buf, "<html><head><title>%d %s</title></head>\n", status, desc);
    sprintf(buf+strlen(buf), "<body bgcolor=\"#cc99cc\"><h4 align=\"center\">%d %s</h4>\n", status, desc);
    sprintf(buf+strlen(buf), "%s\n", text);
    sprintf(buf+strlen(buf), "<hr>\n</body>\n</html>\n");
    send(cfd, buf, strlen(buf), 0);

    return;
}

const char *get_file_type(const char *name)
{
    //自右向左查找‘.’字符，如不存在返回NULL
    char *dot = strrchr(name, '.');
    if(dot == NULL)
        return "text/plain; charset=utf-8";
    if(strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
        return "text/html; charset=utf-8";
    if(strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
        return "image/jpeg";
    if(strcmp(dot, ".gif") == 0)
        return "image/gif";
    if(strcmp(dot, ".mp3") == 0)
        return "audio/mpeg";
    if(strcmp(dot, ".avi") == 0)
        return "video/x-msvideo";
    
    return "text/plain; charset=utf-8";
}

/*
HTTP/1.1 200 OK
Server: xhttpd
Date: Fri, 18 Jul 2014 14:34:26 GMT
Content-Type: text/plain; charset=iso-8859-1(必选项)
Content-Length: 32 (要么不写或者传-1，要写务必精确)
Content-Language: zh-CN
Last-Modified: Fri, 18 Jul 2014 08:36:36 GMT
Connection: close
【空行】
*/
//回发http协议应答:客户端的fd, 错误号，错误描述，回发文件类型，文件长度
void send_respond_head(int cfd, int status, char *desc, const char *type, int len)
{
    char str[100];
    sprintf(str, "Content-Type: %s", type);

    char buf[1024] = {0};
    //状态行
    sprintf(buf, "HTTP/1.1 %d %s\r\n", status, desc);
    //消息报头
    sprintf(buf+strlen(buf), "%s\r\n", str);
    sprintf(buf+strlen(buf), "Content-Length: %d\r\n", len);
    send(cfd, buf, strlen(buf), 0);
    //空行
    send(cfd, "\r\n", 2, 0);
}

//发送文件给浏览器
void send_file(int cfd, const char *file)
{
    int fd = open(file, O_RDONLY);
    if (fd == -1)
    {
        perror("open() error");
        send_error(cfd, 404, "Not Found", "No such file or direntry");
        return;
    }
    
    int n = 0;
    int ret;
    char buf[4096] = {0};
    while((n = read(fd, buf, sizeof(buf))) > 0)
    {
        ret = send(cfd, buf, n, 0);
        if(ret == -1)
        {
            printf("errno = %d\n", errno);
            if(errno == EAGAIN)
            {
                printf("-----EAGAIN\n");
                continue;
            }
            else if(errno == EINTR)
            {
                printf("-----EINTR\n");
                continue;
            }
            else
            {
                perror("send() error");
                exit(1);
            }
        }

        if(ret < 4096)
            printf("send ret: %d\n", ret);
    }

    close(fd);
}

//发送目录内容
void send_dir(int cfd, const char *dirname)
{
    //拼一个html页面<table></table>
    char buf[4094] = {0};

    sprintf(buf, "<html><head><title>目录名：%s</title></head>", dirname);
    sprintf(buf+strlen(buf), "<body><h1>当前目录：%s</h1><table>", dirname);

    char enstr[1024] = {0};
    char path[1024] = {0};

    //目录项二级指针
    struct dirent** ptr;
    int num = scandir(dirname, &ptr, NULL, alphasort);

    //遍历
    for(int i = 0; i < num; ++i)
    {
        char *name = ptr[i]->d_name;
        
        //拼接文件的完整路径
        sprintf(path, "%s/%s", dirname, name);
        printf("path = %s\n", path);
        struct stat st;
        stat(path, &st);

        encode_str(enstr, sizeof(enstr), name);

        if(S_ISREG(st.st_mode))
        {
            sprintf(buf+strlen(buf), "<tr><td><a href=\"%s\">%s</a></td><td>%ld</td></tr>", enstr, name, (long)st.st_size);
        }
        else if(S_ISDIR(st.st_mode))
        {
            sprintf(buf+strlen(buf), "<tr><td><a href=\"%s/\">%s/</a></td><td>%ld</td></tr>", enstr, name, (long)st.st_size);
        }
        int ret = send(cfd, buf, strlen(buf), 0);
        if(ret == -1)
        {
            printf("errno = %d\n", errno);
            if(errno == EAGAIN)
            {
                printf("-----EAGAIN\n");
                continue;
            }
            else if(errno == EINTR)
            {
                printf("-----EINTR\n");
                continue;
            }
            else
            {
                perror("send() error");
                exit(1);
            }
            memset(buf, 0, sizeof(buf));
        }

        sprintf(buf+strlen(buf), "</table></body></html>");
        send(cfd, buf, strlen(buf), 0);
        printf("dir message send ok\n");
    }
}

//传文本数据可直接用命令测试：curl http://127.0.0.1:9999/hello.c