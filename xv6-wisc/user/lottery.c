#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"

void printpstat() {
  struct pstat pst;
  getpinfo(&pst);
  printf(1, "total tickets: %d\n", pst.total_tickets);
  for (int i=0; i < NPROC; i++) {
    if (pst.state[i] == 3 || pst.state[i] == 4) { // RUNNABLE / RUNNING (pstat.h)
      printf(1, "[pid:%d] [tick:%d] [ticket: %d] [inuse: %d] [state: %d]\n",pst.pid[i], pst.ticks[i], pst.tickets[i], pst.inuse[i], pst.state[i]);
    }
  }
}

int
main(void)
{
  // pid x . : sleep + printpstat
  // pid x+1 : settickets(10);
  // pid x+2 : settickets(20);
  // pid x+3 : settickets(30); 
  int pid1 = fork();
  if (pid1 == 0) { // child process
    settickets(10);
    for(;;){}
  } else if (pid1 > 0) { // parent process
    int pid2 = fork();
    if (pid2 == 0) { 
      settickets(20);
      for(;;){}
    } else if (pid2 > 0){
      int pid3 = fork();
      if (pid3 == 0) {
        settickets(30);
        for(;;){}
      } else if (pid3 > 0) {
        for(;;) {
          sleep(1000);
          printpstat();
        } 
      }
    }
  }
  exit();
}
