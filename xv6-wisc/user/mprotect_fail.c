/* mprotect/munprotect failure should not affect the system */
#include "types.h"
#include "stat.h"
#include "user.h"
#define PGSIZE 0x1000
#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a) ((char*)((((unsigned int)(a)) & ~(PGSIZE-1))))
#define check(exp, msg) if(exp) {} else {\
  printf(1, "%s:%d check (" #exp ") failed: %s\n", __FILE__, __LINE__, msg);\
  exit();}


int
main(int argc, char *argv[])
{
  char a;
  int ppid = getpid();

  // failed system call should not affect anything

  mprotect((void *)0x1004, 1);
  a = *(char*)0x1004;
  munprotect((void *)0x1004, 1);
  a = *(char*)0x1004;

  mprotect((void *)0x1000, 0);
  a = *(char*)0x1000;
  munprotect((void *)0x1000, 0);
  a = *(char*)0x1000;

  void* heap = sbrk(0);
  mprotect(heap - PGSIZE, 2);
  a = *(char*)(heap-PGSIZE); // should not be protected
  if (fork() == 0) {
    a = *(char*)heap; // outside address space
    // should not reach here
    printf(1, "TEST FAILED\n");
    kill(ppid);
    exit();
  } else {
    wait();
  }
  sbrk(PGSIZE);
  ((char*)heap)[0] = a; // should not be protected

  printf(1, "PASSED TEST!\n");
  exit();
}