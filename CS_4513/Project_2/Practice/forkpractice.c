/*
  Just some much needed practice with fork() , wait(), exec(), etc.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Turn off asserts
//#define NDEBUG 

int main(int argc, char** argv) {
  printf("Only the original parent gets to say hello\n\n");

  pid_t id = fork();
  if (id < 0) { perror("fork() failed\n"); exit(1); }
  else if (id == 0) {
    printf("I'm the child of %d\n",getppid());
    exit(123);
  } else if (id > 0) {
    int status;
    waitpid(id, &status, 0);
    printf("I'm the parent of child %d\n", id);
  }
  
  /*
  // Found online: Really bad "sleepsorter" 
  while (--argc > 1 && !fork());
  int val  = atoi(argv[argc]);
  sleep(val);
  printf("%d\n", val);
  */
  return 0;
}
