/* mprotect/munprotect arguments */
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
  // page alignment
  check(mprotect((void *)0x1004, 1) == -1, "not page-aligned addr should return -1");
  check(munprotect((void *)0x1004, 1) == -1, "not page-aligned addr should return -1");

  // positive len
  check(mprotect((void *)0x1000, 0) == -1, "len should be positive integer");
  check(munprotect((void *)0x1000, 0) == -1, "len should be positive integer");
  check(mprotect((void *)0x1000, -2) == -1, "len should be positive integer");
  check(munprotect((void *)0x1000, -2) == -1, "len should be positive integer");

  // address space
  void* heap = sbrk(0);
  check(munprotect(heap, 1) == -1, "out of address space");
  check(mprotect(heap, 1) == -1, "out of address space");
  check(munprotect(heap-PGSIZE, 2) == -1, "out of address space");
  check(mprotect(heap-PGSIZE, 2) == -1, "out of address space");
  sbrk(PGSIZE * 2);  // grow heap
  check(mprotect(heap, 1) == 0, "no longer out of address space");
  check(munprotect(heap, 1) == 0, "no longer out of address space");
  check(mprotect(heap, 3) == -1, "out of address space");
  check(munprotect(heap, 3) == -1, "out of address space");

  printf(1, "PASSED TEST!\n");
  exit();
}