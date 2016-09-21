#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
	const char *pathname = "m_fifo";
	int pipd_fd = -1;
	int bytes;
	cahr buf[20];

	strcpy(buf,"hello world!");
	mkfifo(pathname,0777);

	pipe_fd = open(pathname,O_WRONLY);
	printf("Process %d result %d\n",getpid(),pipe_fd);
	write(pipe_fd,buf,strlen(buf));
	printf("Process %d finished!\n",getpid());
	return 0;
}