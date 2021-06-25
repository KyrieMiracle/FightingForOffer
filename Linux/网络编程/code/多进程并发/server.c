#include "wrap.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <ctype.h>
#define  SERV_PORT 1234

void catch_child(int signum)
{
	while(waitpid(0, NULL, WNOHANG)>0);
	return ;
}

int main()
{
	int lfd, cfd; 
	struct sockaddr_in srv_addr, clt_addr;
	socklen_t clt_addr_len;
	pid_t pid;
	int ret;
	char buf[BUFSIZ];
	int connect_count = 0;

	bzero(&srv_addr, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(SERV_PORT);
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	lfd = Socket(AF_INET, SOCK_STREAM, 0);

	Bind(lfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));

	Listen(lfd, 128);

	clt_addr_len = sizeof(clt_addr);

	while(1){
		cfd = Accept(lfd, (struct sockaddr *)&clt_addr, &clt_addr_len);
		pid = fork();
		connect_count++;
		if(pid < 0) perr_exit("fork error");
		else if(pid == 0){
			printf("%d connected\n", connect_count);
			close(lfd);
			break;
		}
		else{
			struct sigaction act;
			act.sa_handler = catch_child;
			sigemptyset(&act.sa_mask);
			act.sa_flags = 0;

			ret = sigaction(SIGCHLD,&act, NULL );
			if(ret != 0)
				perr_exit("sigaction error");
			close(cfd);
			continue;
		}
	}

	if(pid == 0){
		while(1){
			ret = read(cfd,buf,sizeof(buf));
			if(ret == 0){
				close(cfd);
				exit(1);
			}
			for(int i = 0; i < ret; i++)
			{
				buf[i] = toupper(buf[i]);
			}
			write(cfd, buf, ret);
			write(STDOUT_FILENO, buf, ret);
		}	
	}




	return 0;
}
