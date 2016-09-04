#include "pv.h"

void handlesem(key_t skey);

int main()
{
  key_t semkey = 0x200;
  int i;
  for(i=0;i<3;i++)
  {
    if(fork() == 0)
      handlesem(semkey);
  }
}

void handlesem(key_t skey)
{
  int semid;
  pid_t pid = getpid();

  if((semid = initsem(skey)) < 0)
    exit(1);

  printf("进程 %d 在临界资源区之前 \n",pid);
  p(semid);
  printf("进程 %d 在使用临界资源时，停止10s \n",pid);
  sleep(10);
  printf("进程 %d 退出临界区后 \n",pid);
  v(semid);

  printf("进程 %d 完全退出\n",pid);
  exit(0);

}




