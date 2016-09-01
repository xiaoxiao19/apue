#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
  pid_t pc,pr;
  pc = fork();

  if(pc < 0)
    printf("error occured!\n");
  else if(pc == 0)
  {
    sleep(10);
    exit(0);
  }

}
