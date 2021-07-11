#include "types.h"
#include "stat.h"
#include "user.h"

int main(void) {
  char *p = 0; // 0 is just NULL in C
  printf(1, "%x\n", (unsigned int) *p); //deref null => BAD
  exit();
}