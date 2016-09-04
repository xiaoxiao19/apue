#include "pv.h"

int main()
{
  key_t semkey_A = 0x200;
  key_t semkey_B = 0x220;
  key_t semkey_C = 0x240;

  int semid_A,semid_B,semid_C;
  if((semid_A=initsem(semkey_A)) < 0) exit(1);
  if((semid_B=initsem(semkey_B)) < 0) exit(1);
  if((semid_C=initsem(semkey_C)) < 0) exit(1);
  printf("A 进程A的信号量标识符%d,它的初始值为%d\n",
          semid_A,semctl(semid_A,0,GETVAL));
  printf("B 进程B的信号量标识符%d,它的初始值为%d\n",
          semid_B,semctl(semid_B,0,GETVAL));
  printf("C 进程C的信号量标识符%d,它的初始值为%d\n",
          semid_C,semctl(semid_C,0,GETVAL));

  int count = 0;
  if(fork() != 0)
  {
    int i;
    for(i=0;i<10;i++)
    {
      p(semid_A);
      printf("A  进程A的信号量值为%d\n",semctl(semid_A,0,GETVAL));
      v(semid_B);
    }
  }
  else
  {
    int j;
    for(j=0;j<10;j++)
    {
      p(semid_B);
      printf("B  进程B的信号量值为%d\n",semctl(semid_B,0,GETVAL));
      count++;
      if(count == 3)
      {
        v(semid_C);
        printf("C  进程C的信号量值为%d,count=%d\n",semctl(semid_C,0,GETVAL),count);
        v(semid_A);
        count = 0;
      }
      else
        v(semid_A);
    }
  }
  if((semctl(semid_A,0,IPC_RMID)) < 0)
  {
    perror("semctl error");
    exit(1);
  }
  if((semctl(semid_B,0,IPC_RMID)) < 0)
  {
    perror("semctl error");
    exit(1);
  }
  if((semctl(semid_C,0,IPC_RMID)) < 0)
  {
    perror("semctl error");
    exit(1);
  }
}
