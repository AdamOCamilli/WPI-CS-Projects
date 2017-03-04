#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define __NR_cs3013_syscall2 378
#define SHRT_MAX 32767

typedef struct ancestry {
  pid_t ancestors[10];
  pid_t siblings[100];
  pid_t children[100];
} ancestry;

long testCall2(unsigned short *target_pid, struct ancestry *response) {
  return (long) syscall(__NR_cs3013_syscall2, target_pid, response);
}

int main(int argc, char *argv[]) {

  if (argc != 2) {
    printf("Must pass in one PID\n");
    return -1;
  } 

  long input = strtol(argv[1],NULL,0);
  unsigned short target = (unsigned short) input;

  if (errno || input <= 0 || input > SHRT_MAX) {
    printf("Must pass in valid short as PID\n");
    return -1;
  }

  
  struct ancestry *target_hierarchy = (struct ancestry *)malloc(sizeof(struct ancestry));
  
  printf("testcall2 returned: %d\n",testCall2(&target,target_hierarchy));

  free(target_hierarchy);
  return 0;
}
