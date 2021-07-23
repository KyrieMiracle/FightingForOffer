#include <iostream>


#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "wrap.h"
#define MAX_EVENTS 1024
#define BUFLEN 4096
#define SERV_PORT 8080

void recvdata(int fd, int events, void *arg);
void senddata(int fd, int events, void *arg);




struct myevent_s{
    int fd;
    int event;
    void *arg;
    void(*call_back)(int fd, int events, void *arg);
    int status;
    char buf[BUFLEN];
    int len;
    long last_active;
};

int g_efd;
myevent_s g_events[MAX_EVENTS + 1];


//initialize the members of "myevent_s" 

void eventset(struct myevent_s *ev, int fd, void (*call_back)(int, int, void*), void *arg){
    ev->fd = fd;
    ev->call_back = call_back;
    ev->event = 0;
    ev->arg = arg;
    ev->status = 0;
    //memset(ev->buf, 0, sizeof(ev->buf));
    //ev->len = 0;
    ev->last_active = time(NULL);
}






void eventadd(int efd, int events, myevent_s *ev){
    epoll_event epv = {0,{0}};
    int op;
    epv.data.ptr = ev;
    epv.events = ev->event = events;

    if(ev->status == 1){
        op = EPOLL_CTL_MOD;
    }else{
        op = EPOLL_CTL_ADD;
        ev->status = 1;
    }

    if(epoll_ctl(efd, op, ev->fd, &epv) < 0){
        printf("event add failed [fd = %d], events [%d]\n", ev->fd, events);
    }else{
        printf("event add ok [fd = %d], op = %d, event[%0X]\n", ev->fd, op, events);
    }

    return;
}



void eventdel(int efd, myevent_s *ev){
    epoll_event epv = {0,{0}};
    
    if(ev->status != 1) return;

    epv.data.ptr = ev;
    ev->status = 0;
    epoll_ctl(efd, EPOLL_CTL_DEL, ev->fd, &epv);

    return;
}





void acceptconn(int lfd, int events, void *arg){
    sockaddr_in cin;
    socklen_t len = sizeof(cin);
    int cfd, i;

    if((cfd = accept(lfd, (sockaddr *)&cin, &len)) == -1){
        if(errno != EAGAIN && errno != EINTR){

        }
        printf("%s: accept, %s\n", __func__, strerror(errno));
        return;
    }

    do{
        for(i = 0; i < MAX_EVENTS;i++)
            if(g_events[i].status == 0)
                break;
        if(i == MAX_EVENTS){
            printf("%s: max connect limit[%d]\n", __func__, MAX_EVENTS);
            break;
        }        

        int flag = 0;
        if((flag = fcntl(cfd, F_SETFL, O_NONBLOCK)) < 0){
            printf("%s: fcntl nonblocking failed, %s\n", __func__, strerror(errno));
            break;
        }

        eventset(&g_events[i], cfd, recvdata, &g_events[i]);
        eventadd(g_efd, EPOLLIN, &g_events[i]);

    }while(0);

    printf("new connect [%s:%d][time:%ld], pos[%d]\n", 
            inet_ntoa(cin.sin_addr), ntohs(cin.sin_port), g_events[i].last_active, i);
    return ;
}





void recvdata(int fd, int events, void *arg){
    myevent_s *ev = (myevent_s *)arg;
    int len;

    len = recv(fd, ev->buf, sizeof(ev->buf), 0);

    eventdel(g_efd, ev);

    if(len > 0){
        ev->len = len;
        ev->buf[len] = '\0';
        printf("client[%d]:%s\n", fd, ev->buf);

        eventset(ev, fd, senddata, ev);
        eventadd(g_efd, EPOLLOUT, ev);
    }else if(len == 0){
        close(ev->fd);
        printf("[fd=%d] pos[%ld], closed\n", fd, ev-g_events);
    } else {
        close(ev->fd);
        printf("recv[fd=%d] error[%d]:%s\n", fd, errno, strerror(errno));
    }
    return;
}






void senddata(int fd, int events, void *arg){
    myevent_s *ev = (myevent_s *)arg;
    int len;

    len = send(fd, ev->buf, ev->len, 0);

    if(len > 0){
        printf("send[fd = %d], [%d]%s\n", fd, len, ev->buf);
        eventdel(g_efd, ev);
        eventset(ev, fd, recvdata, ev);
        eventadd(g_efd, EPOLLIN, ev);
    }else{
        close(ev->fd);
        eventdel(g_efd, ev);
        printf("send[fd=%d] error %s\n", fd, strerror(errno));
    }
    return;
}











void initlistensocket(int efd, short port){
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(lfd, F_SETFL, O_NONBLOCK);

    eventset(&g_events[MAX_EVENTS], lfd, acceptconn, &g_events[MAX_EVENTS]);

    eventadd(efd, EPOLLIN, &g_events[MAX_EVENTS]);

    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = INADDR_ANY;

    bind(lfd, (sockaddr *)&sin, sizeof(sin));

    listen(lfd, 20);

    return;
}





int main(int argc, char *argv[])
{
    unsigned short port = SERV_PORT;

    if(argc == 2)   port = atoi(argv[1]);

    g_efd = epoll_create(MAX_EVENTS + 1);
    if(g_efd <= 0)
        printf("create efd in %s err %s\n", __func__, strerror(errno));

    initlistensocket(g_efd, port);

    epoll_event events[MAX_EVENTS + 1];
    printf("server running:port[%d]\n", port);

    int checkpos = 0, i;
    while(1){
        long now = time(NULL);
        for(i = 0; i < 100; i++, checkpos++){
            if(checkpos == MAX_EVENTS) checkpos = 0;

            if(g_events[checkpos].status != 1) continue;

            long durantion = now - g_events[checkpos].last_active;

            if(durantion > 60){
                close(g_events[checkpos].fd);
                printf("[fd=%d] timeout\n", g_events[checkpos].fd);
                eventdel(g_efd, &g_events[checkpos]);
            }
        }

        int nfd = epoll_wait(g_efd, events, MAX_EVENTS + 1, 1000);
        if(nfd < 0){
            printf("epoll_wait error, exit\n");
            break;
        }

        for(i = 0; i < nfd; i++){
            myevent_s *ev = (myevent_s *)events[i].data.ptr;
            if((events[i].events & EPOLLIN) && (ev->event & EPOLLIN)){
                ev->call_back(ev->fd, events[i].events, ev->arg);
            }
            if((events[i].events & EPOLLOUT) && (ev->event & EPOLLOUT)) {         //写就绪事件
                ev->call_back(ev->fd, events[i].events, ev->arg);
            }
        }
    }

    return 0;
}