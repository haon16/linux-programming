//2021年3月14日12点44分
//网络中的ET和LT模式

#include "../../base.h"
#include <strings.h>

#define MAXLINE 10

int main(int argc, char *argv[])
{
    int sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_adr;
    bzero(&serv_adr, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons(9999);
    inet_pton(AF_INET, "127.0.0.1", &serv_adr.sin_addr.s_addr);
    
    Connect(sockfd, (struct sockaddr*)&serv_adr, sizeof(serv_adr));

    char ch = 'a';
    char buf[MAXLINE];
    int i;
    while(1)
    {
        //aaaa\n
        for(i = 0; i < MAXLINE/2; i++)
        {
            buf[i] = ch;
        }
        buf[i-1] = '\n';
        ch++;

        //bbbb\n
        for(; i < MAXLINE; i++)
        {
            buf[i] = ch;
        }
        buf[i-1] = '\n';
        ch++;
        //aaaa\nbbbb\n
        write(sockfd, buf, sizeof(buf));
        sleep(5);
    }

    Close(sockfd);
    return 0;
}
