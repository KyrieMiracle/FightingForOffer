#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

void sys_err(const char *str)
{
	perror(str);
	exit(1);
}

int main(int argc, char *argv[])
{
	pid_t pid;
	int ret, fd;

	pid = fork();
	if(pid > 0)      //父进程终止
		exit(0);

	pid = setsid();  //创建新会话
	if(pid == -1)
		sys_err("setsid error");
	
	ret = chdir("/home/ubuntu"); //改变工作目录位置
	if(ret == -1)
		sys_err("chdir error");

	umask(0022); //改变文件访问权限掩码

	close(STDIN_FILENO);//关闭文件描述符 0
	fd = open("/dev/null", O_RDWR);// fd --> 0
	if(fd == -1)
		sys_err("open error");
	dup2(fd, STDOUT_FILENO); //重定向 stdout 和 stderr
	dup2(fd, STDOUT_FILENO);

	while(1);

	return 0;
}
