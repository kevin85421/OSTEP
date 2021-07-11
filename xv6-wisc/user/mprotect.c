#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"

#define PGSIZE 0x1000
#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a) ((char*)((((unsigned int)(a)) & ~(PGSIZE-1))))
#define check(exp, msg) if(exp) {} else {\
  printf(1, "%s:%d check (" #exp ") failed: %s\n", __FILE__, __LINE__, msg);\
  exit();}


int
main(int argc, char *argv[])
{
  int ppid = getpid();
  if (fork() == 0) {
    char addr = 'a'; // test on stack        
    char* align = PGROUNDDOWN((uint *)&addr);
    mprotect(align, 1);
    // printf should page fault, since calling function needs to modify stack
    printf(1, "line %d: failed to protect the stack\n", __LINE__);
    printf(1, "TEST FAILED\n");
    kill(ppid);
    exit();
  } else {
    wait();
  }

  if (fork() == 0) {
    char *addr = malloc(1); // test on heap
    *addr = 'a';
    char *align = PGROUNDDOWN((uint *)addr);
    mprotect(align, 1);
    (*addr)++;
    // should not reach here
    printf(1, "line %d: failed to protect the heap\n", __LINE__);
    printf(1, "TEST FAILED\n");
    kill(ppid);
    exit();
  } else {
    wait();
  }

  if (fork() == 0) {
    char *addr = malloc(PGSIZE * 4); // test on heap
    mprotect(addr, 3);
    addr[PGSIZE] = 'a';
    // should not reach here
    printf(1, "line %d: failed to protect the heap for more than one page\n", __LINE__);
    printf(1, "TEST FAILED\n");
    kill(ppid);
    exit();
  } else {
    wait();
  }
}