#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define __NR_cs3013_syscall1 377
#define __NR_cs3013_syscall2 378
#define __NR_open 5
#define __NR_close 6
#define __NR_read 3

long testCall1(void) {
  return (long) syscall(__NR_cs3013_syscall1);
}

long testCall2(unsigned short *target_pid, unsigned short *output) {
  return (long) syscall(__NR_cs3013_syscall2, target_pid, output);
}

long testOpen(void) {
  return (long) syscall(__NR_open,"virus.txt",0x0000); //0x0000 is value of read only flag
}

long testClose(int fd) {
  return (long) syscall(__NR_close,fd);
}

long testRead(int fd) {
  char buf[1200]; // arbitrary buffer size
  size_t count = 1;
  long bytes_read = 0;
  while ((syscall(__NR_read,fd,buf,count)) > 0) 
    bytes_read++;
  return bytes_read; 
}

int main() {
  long fd = testOpen();
  printf("The return values of the system calls are:\n");
  printf("\tcs3013_syscall1: %ld\n", testCall1());
  printf("\tOpen: %ld\n", fd);
  printf("Errno: %s\n", strerror(errno));
  printf("\tRead: %ld\n", testRead(fd));
  printf("\tClose: %ld\n", testClose(fd));

  unsigned short sample_pid = 10000;
  unsigned short *output = (unsigned short *)malloc(sizeof(unsigned short));
  testCall2(&sample_pid, output);
  printf("Given %d, kernel returned %d\n", sample_pid, *output);
  return 0;
}
