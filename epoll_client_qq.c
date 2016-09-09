#define _GNU_SOURCE 1
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <poll.h>
#include <fcntl.h>

#define BUFFER_SIZE 64

int main(int argc,char *argv[])
{
	if(argc <= 2)
	{
		printf("Usage %s ip_address port_number\n",basename(argv[0]));
		return 1;
	}

	const char *ip = argv[1];
	int port = atoi(argv[2]);

	struct sockaddr_in serveraddr;
	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET,ip,&serveraddr.sin_addr);
	serveraddr.sin_port = htons(port);

	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	assert(sockfd >= 0);

	if(connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr)) < 0)
	{
		printf("connection failed\n");
		close(sockfd);
		return 1;
	}

	struct pollfd fds[2];
	//注册文件描述符0（标准输入）和文件描述符sockfd上的可读事件
	fds[0].fd = 0;
	fds[0].events = POLLIN;
	fds[0].revents = 0;
	fds[1].fd = sockfd;
	fds[1].events = POLLIN | POLLRDHUP;
	fds[1].revents = 0;

	char read_buf[BUFFER_SIZE];
	int pipefd[2];
	int ret = pipe(pipefd);
	assert(ret != -1);
        while(1)
	{
		ret = poll(fds,2,-1);
		if(ret < 0)
		{
			printf("poll failure\n");
			break;
		}

		if(fds[1].revents & POLLRDHUP)
		{
			printf("server close the connection\n");
			break;
		}

		else if(fds[1].revents & POLLIN)
		{
			memset(read_buf,'\0',BUFFER_SIZE);
			recv(fds[1].fd,read_buf,BUFFER_SIZE-1,0);
			printf("%s\n",read_buf);
		}

		if(fds[0].revents & POLLIN)
		{
			//使用splice将用户输入的数据直接写到sockfd上（零拷贝）
			//将0（STDIN 标准输入）上流入的客户数据重定向到管道中
			ret = splice(0,NULL,pipefd[1],NULL,32768,SPLICE_F_MORE|SPLICE_F_MOVE);
			//将管道中的输出重定向到sockfd客户连接文件描述符中
			ret = splice(pipefd[0],NULL,sockfd,NULL,32768,SPLICE_F_MORE|SPLICE_F_MOVE);
		}
	}
	close(sockfd);
	return 0;
}
