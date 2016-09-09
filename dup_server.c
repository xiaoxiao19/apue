#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(int argc,char *argv[])
{
	if(argc <= 2)
	{
		printf("Usage:%s ip_address port_number\n",basename(argv[0]));
		return 1;
	}

	const char *ip = argv[1];
	int port = atoi(argv[2]);

	struct sockaddr_in address;
	bzero(&address,sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET,ip,&address.sin_addr);
	address.sin_port = htons(port);

    int sock = socket(PF_INET,SOCK_STREAM,0);
    assert(sock != -1);

    int ret = bind(sock,(struct sockaddr*)&address,sizeof(address));
    assert(ret != -1);

    ret = listen(sock,5);
    assert(ret != -1);

    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    int connfd = accept(sock,(struct sockaddr*)&client,client_addrlength);
    if(connfd < 0)
    {
    	printf("errno is:%d\n",errno);
    }
    else
    {
    	close(STDOUT_FILENO);
    	dup(connfd);
    	printf("abcd\n");
    	close(connfd);
    	/*首先关闭表转输出文件描述符STDOUT_FILENO(其值为1)，然后复制socket文件描述符connfd，
    	因为dup总是返回系统中最小的可用文件描述符，所以它的返回值实际上是1，即之前关闭的标准
    	文件描述符的值，这样一来，服务器输出到标准输出的内容（abcd）就会直接发送到与客户连接
    	对应的socket上，因此printf调用的输出将被客户端获得（而不是显示在服务器程序的终端上）
    	这既是CGI服务器的基本工作原理*/
    }
    close(sock);
    return 0;
}