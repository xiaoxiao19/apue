#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
	const char *pathname = "m_fifo";
	char buf[20];
	int pipe_fd;

	pipe_fd = open(pathname,O_RDONLY);
	printf("Process %d result %d\n",getpid(),pipe_fd);
	read(pipe_fd,buf,sizeof(buf));
	close(pipe_fd);
	printf("Process %d finished,\n read content:%s\n",getpid(),buf);

	return 0;
}