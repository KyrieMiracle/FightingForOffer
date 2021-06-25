#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ctype.h>
#include <fcntl.h>

#include "wrap.h"

#define MAXLINE 8192
#define SERV_PORT 1234

struct s_info{      //定义一个结构体，将地址结构跟cfd捆绑
    struct sockaddr_in cliaddr;
    int connfd;
};

void *do_work(void *arg)
{
    int n, i;
    struct s_info *ts = (struct s_info *)arg;
    char buf[MAXLINE];
    char str[INET_ADDRSTRLEN];

    while(1){
        n = Read(ts->connfd, buf, MAXLINE);
        if(n == 0){
            printf("the client %d close . . . \n", ts->connfd);
            break;
        }
        printf("received from %s at PORT %d \n",
            inet_ntop(AF_INET, &(ts->cliaddr.sin_addr), str, sizeof(str)),
            ntohs(ts->cliaddr.sin_port));
        for(i = 0; i < n; i++)
            buf[i] = toupper(buf[i]);
        write(STDOUT_FILENO, buf, n);
        write(ts->connfd, buf, n);
    }
    Close(ts->connfd);
    return (void *)0;
}

int main()
{
    struct sockaddr_in servaddr, cliaddr;
    socklen_t cliaddr_len;
    int listenfd, connfd;
    pthread_t tid;

    struct s_info ts[256];
    int i;

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //指定本地任意IP
    servaddr.sin_port = htons(SERV_PORT);

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    Listen(listenfd, 128);

    printf("Accepting client connect . . . \n");

    while(1){
        cliaddr_len = sizeof(cliaddr);
        connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);
        ts[i].cliaddr = cliaddr;
        ts[i].connfd = connfd;

        pthread_create(&tid, NULL, do_work, (void *)&ts[i]);  //参数3：其实最好是malloc申请一个地址再传，强行类型转换会丢数据
        pthread_detach(tid);  //子线程分离， 防止僵尸线程
        i++;
    }

    return 0;
}