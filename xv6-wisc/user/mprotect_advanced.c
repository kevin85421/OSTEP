/* mprotect/munprotect behaviors on fork */
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
  int fd;
  unlink("tmp.out");
  int ppid = getpid();

  if (fork() == 0) {
    char *addr = malloc(PGSIZE * 4); // test on heap
    mprotect(addr, 2);
    int cpid = getpid();
    if (fork() == 0) {
      addr[PGSIZE] = 'a';
      // should not reach here
      printf(1, "line %d: child process does not inherit protected page\n", __LINE__);
      printf(1, "TEST FAILED\n");
      kill(cpid);
      kill(ppid);
      exit();
    } else {
      wait();
    }
  } else {
    wait();
  }

  if (fork() == 0) {
    char *addr = malloc(PGSIZE * 4); // test on heap
    mprotect(addr, 2);
    if (fork() == 0) {
      munprotect(addr, 2);
      addr[PGSIZE] = 'a';
      // should reach here
      fd = open("tmp.out", O_CREATE|O_RDWR);
      printf(1, "reached here\n");
      write(fd, "a", 1);
      close(fd);
      exit();
    } else {
      wait();
    }
    addr[PGSIZE] = 'a';
    // should not reach here
    printf(1, "line %d: child process unprotects parent's page\n", __LINE__);
    printf(1, "TEST FAILED\n");
    kill(ppid);
    exit();
  } else {
    wait();
  }

  fd = open("tmp.out", O_RDWR);
  if (fd < 0) {
    printf(1, "addr should be writable after munprotect\n");
    printf(1, "TEST FAILED\n");
  } else {
    struct stat st;
    fstat(fd, &st);
    if (st.size == 1) printf(1, "PASSED TEST!\n");
    else printf(1, "TEST FAILED\n");
  }
  unlink("tmp.out");
  exit();
}