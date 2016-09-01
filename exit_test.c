#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  pid_t pid;
  int count = 0;

  pid = vfork();
  if(pid == 0)
  {
    printf("child:count=%d\n",count);
    printf("child:getpid=%d\n",getpid());
    count = 1;
    printf("child:count=%d\n",count);
    //return 0;//会出现无限循环
    exit(0);
  }
  else
  {
    printf("\nfather:pid=%d\n",pid);
    printf("father:count=%d\n",count);
  }

  return 0;
}
