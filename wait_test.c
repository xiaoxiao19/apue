#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
  pid_t pc,pr;
  pc = fork();

  if(pc < 0)//出现错误
     printf("error ecurred!\n");
  else if(pc == 0)//如果是子进程
  {
    printf("This is child process with pid of %d\n",getpid());
    sleep(10);
  }
  else//如果是父进程
  {
    pr = wait(NULL);
    printf("I catch a child process with pid of %d\n",pr);
  }
  exit(0);
}
