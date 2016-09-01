#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
 
static volatile sig_atomic_t sigflag; 
static sigset_t newmask, oldmask, zeromask;
/* signal handler for SIGUSR1 and SIGUSR2 */
static void sig_usr(int signo)
{
 sigflag = 1;
 return;
}
void TELL_WAIT()
{
 if(signal(SIGUSR1, sig_usr) == SIG_ERR)
  printf("signal SIGUSR1 error\n");
 if(signal(SIGUSR2, sig_usr) == SIG_ERR)
  printf("signal SIGUSR2 error\n");
  
 sigemptyset(&zeromask);
 
 sigemptyset(&newmask);
 sigaddset(&newmask, SIGUSR1);
 sigaddset(&newmask, SIGUSR2);
 
 /* block SIGUSR1 and SIGUSR2, and save current signal mask */
 if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
  printf("SIG_BLOCK error\n");
}
void TELL_PARENT(pid_t pid)
{
 kill(pid, SIGUSR2); /* tell parent we are done */
}
void WAIT_PARENT()
{
 while(sigflag == 0)
  sigsuspend(&zeromask); /* wait for parent */
 
 sigflag = 0;
 
 /* reset signal mask */
 if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
  printf("SIG_SETMASK error\n");
}
void TELL_CHILD(pid_t pid)
{
 kill(pid, SIGUSR1);
}
void WAIT_CHILD()
{
 while(sigflag == 0)
  sigsuspend(&zeromask); /* wait for parent */
 
 sigflag = 0;
 
 /* reset signal mask */
 if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
  printf("SIG_SETMASK error\n");
}
void do_task(char *task_str)
{
 printf("%s\n", task_str);
}
/* parent goes first program */
int main()
{
 pid_t pid;
 
 TELL_WAIT();
 
 pid = fork();
 if(pid < 0) {
  printf("fork error\n");
 }
 else if(pid == 0) {
  WAIT_PARENT();
  do_task("child task\n");
 }
 else {
  do_task("parent task\n");
  TELL_CHILD(pid);
 }
 
 return 0;
}
/* child goes first program*/
int main()
{
 pid_t pid;
 
 TELL_WAIT();
 
 pid = fork();
 if(pid < 0) {
  printf("fork error\n");
 }
 else if(pid == 0) {
  do_task("child task\n");
  TELL_PARENT(getppid());
 }
 else {
  WAIT_CHILD();
  do_task("parent task\n");
 }
 
 return 0;
}