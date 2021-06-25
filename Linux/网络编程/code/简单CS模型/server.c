#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <ctype.h>

#define SERV_PORT 9527

void sys_err(const char *str){
	perror(str);
	exit(1);
}

int main(int argc, char *argv[]){
	
	int lfd = 0, cfd = 0;
	int ret; 
	char buf[BUFSIZ], client_IP[1024];

	struct sockaddr_in serv_addr, clit_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORTr);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	socklen_t clit_addr_len;
//1
	lfd = socket(AF_INET, SOCK_STREAM, 0);
	if(lfd = -1)
		sys_err("socket error");

//2
	bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
//3
	listen(lfd, 128);
//4  
	clit_addr_len = sizeof(clit_addr);
	cfd = accept(lfd, (struct sockaddr *)&clit_addr, &clit_addr_len);
	if(cfd == -1)
		sys_err("accept error");
	printf("client ip:%s port:%d\n", 
		inet_ntop(AF_INET, &clit_addr.sin_addr.s_addr, client_IP, sizeof(client_IPl)),
		ntohs(clit_addr.sin_port));

//5
	while(1){
		ret = read(cfd, buf, sizeof(buf));
		write(STDOUT_FILENO, buf, ret);
//6
		for(int i = 0; i < ret; i++){
			buf[i] = toupper(buf[i]);
		}
//7
		write(cfd, buf, ret);
	}
//8
	close(lfd);
	close(cfd);


	return 0;
}