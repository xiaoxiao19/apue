#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <unistd.h>
#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 10

//将文件描述符设置为非阻塞的
int setnonblocking(int fd)
{
	int old_option = fcntl(fd,F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd,F_SETFL,new_option);
	return old_option;
}

//将文件描述符fd上的EPOLLIN注册到epollfd指示的epoll内核时间表中
//参数enable_et指定是否对fd启用ET模式
void addfd(int epollfd,int fd,int enable_et)
{
	struct  epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN;
	if(enable_et)
		event.events |= EPOLLET;
	epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
	setnonblocking(fd);
}

void It(struct epoll_event *events,int number,int epollfd,int listenfd)
{
	char buf[BUFFER_SIZE];
        int i;
	for(i=0;i<number;i++)
	{
		int sockfd = events[i].data.fd;
		if(sockfd == listenfd)
		{
			struct sockaddr_in *client_address;
			socklen_t client_addrlength = sizeof(client_address);
			int connfd = accept(listenfd,(struct sockaddr*)&client_address,&client_addrlength);
			addfd(epollfd,connfd,0);
		}
		else if(events[i].events & EPOLLIN)
		{
			//只要socket读缓存中还有未读出的数据，这段代码就被触发
			printf("event trigger once\n");
			memset(buf,'\0',BUFFER_SIZE);
			int ret = recv(sockfd,buf,BUFFER_SIZE-1,0);
			if(ret <= 0)
			{
				close(sockfd);
				continue;
			}
			printf("get %d bytes of content:%s\n",ret,buf);
		}
		else
			printf("something else happened\n");
	}
}


void et(struct epoll_event *events,int number,int epollfd,int listenfd)
{
	char buf[BUFFER_SIZE];
        int i;
	for(i=0;i< number;i++)
	{
		int sockfd = events[i].data.fd;
		if(sockfd == listenfd)
		{
			struct sockaddr_in *client;
			socklen_t client_addrlength = sizeof(client);
			int connfd = accept(listenfd,(struct sockaddr*)&client,&client_addrlength);
			assert(connfd != -1);
			addfd(epollfd,connfd,1);
		}
		else if(events[i].events & EPOLLIN)
		{
			//这段代码不会被重复触发，所以我们循环读取数据，以确保socket读缓存中的所有数据读出
			printf("event trigger once\n");
			while(1)
			{
				memset(buf,'\0',BUFFER_SIZE);
				int ret = recv(sockfd,buf,BUFFER_SIZE-1,0);
				if(ret < 0)
				{
					if((errno == EAGAIN) || (errno == EWOULDBLOCK))
					{
						printf("read later\n");
						break;
					}
					close(sockfd);
					break;
				}
				else if(ret == 0)
					close(sockfd);
	                        else
		                	printf("get %d bytes of content:%s\n",ret,buf);
			}
                }
	        else
		    printf("something else happend\n");
	}
}


int main(int argc,char *argv[])
{
	if(argc <= 2)
		return 1;
	const char *ip = argv[1];
	int port = atoi(argv[2]);

	int ret = 0;
	struct sockaddr_in address;
	bzero(&address,sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET,ip,&address.sin_addr);
	address.sin_port = htons(port);

	int listenfd = socket(PF_INET,SOCK_STREAM,0);
	assert(listenfd != -1);

	ret = bind(listenfd,(struct sockaddr *)&address,sizeof(address));
	assert(ret != -1);

	ret = listen(listenfd,5);
	assert(ret != -1);

        struct	epoll_event events[MAX_EVENT_NUMBER];
	int epollfd = epoll_create(5);
	assert(epollfd != -1);
	addfd(epollfd,listenfd,1);

	while(1)
	{
		int ret = epoll_wait(epollfd,events,MAX_EVENT_NUMBER,-1);
		if(ret < 0)
		{
			printf("epoll failure\n");
			break;
		}
		//It(events,ret,epollfd,listenfd);
		et(events,ret,epollfd,listenfd);
	}
	close(listenfd);
	return 0;
}











