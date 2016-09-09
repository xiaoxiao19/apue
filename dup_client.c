#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc,char *argv[])
{
  char buf[2047];
  if(argc <= 2)
    return 1;

  const char *ip = argv[1];
  int port = atoi(argv[2]);

  struct sockaddr_in serveraddr;
  bzero(&serveraddr,sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  inet_pton(AF_INET,ip,&serveraddr.sin_addr);
  serveraddr.sin_port = htons(port);

  int clientsock = socket(AF_INET,SOCK_STREAM,0);
  
  int ret = connect(clientsock,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
     read(clientsock,buf,sizeof(buf));
     printf("receive server data:%s\n",buf);
 close(clientsock);
}
