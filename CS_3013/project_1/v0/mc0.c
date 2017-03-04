#define TRUE 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>
#include <wait.h>

struct cmd {
  const char* name;
};

/** Prints shell prompt. */
void type_prompt() {
  fflush(stdin);
  printf("G'Day Commander! What command would you like me to run?\n");
  printf("\t0.  whoami\t:  Prints out the result of the whoami command.\n");
  printf("\t1.  last\t:  Prints out the result of the last command.\n");
  printf("\t2.  ls\t:  Prints out the result of a listing on a user-specified path.\n");
  printf("Option?: ");
} 
  
/** Phase 1 of Project 1: Simply runs a list of commands in a menu-driven manner. 
 * time code borrowed from (http://www.cs.loyola.edu/~jglenn/702/S2008/Projects/P3/time.html)
 */
int main(int argc, char**argv[]) {
  while(TRUE) {

    int cmd_num = -1;

    char input[4];
    char arg[100];
    char path[100];

    type_prompt();
    fgets(input,4,stdin);
    sscanf(input,"%d",&cmd_num);
    
    if (cmd_num == 0) { // if command is successfully read
      printf("-- Who Am I? --\n");
      struct timeval start,end;
      gettimeofday(&start,NULL);

      int pid = fork();
      if (pid < 0) {
	fprintf(stderr,"Fork failed!\n");
      } else if (pid == 0) {
	char* stuff[2] = {"why do i need null",NULL}; // no args used on whoami
	execvp("whoami",stuff);
        fprintf(stderr,"Invalid command!\n"); // exec shouldn't return
      } else if (pid > 0) {
	wait(NULL);
	gettimeofday(&end,NULL);
	struct rusage r;
	getrusage(RUSAGE_SELF,&r);
	printf("-- Statistics --\n");
	printf("Elapsed Time: %ld\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
	printf("Page Faults: %ld\n",r.ru_majflt);
	printf("Page Faults (reclaimed): %ld\n\n",r.ru_minflt);
      }

    } else if (cmd_num == 1) { // last command
      printf("-- Last Logins --\n");
      struct timeval start,end;
      gettimeofday(&start,NULL);

      int pid = fork();
      if (pid < 0) {
        fprintf(stderr,"Fork failed!\n");
      } else if (pid == 0) {
	char* stuff[2] = {"why do i need null",NULL}; // no args used on last
	execvp("last",stuff);
        fprintf(stderr,"Invalid command!\n"); // exec shouldn't return
      } else if (pid > 0) {
	wait(NULL);
	gettimeofday(&end,NULL);
	struct rusage r;
	getrusage(RUSAGE_SELF,&r);
	printf("-- Statistics --\n");
	printf("Elapsed Time: %ld\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
	printf("Page Faults: %ld\n",r.ru_majflt);
	printf("Page Faults (reclaimed): %ld\n\n",r.ru_minflt);
      }
    } else if (cmd_num == 2) { // ls command
      printf("-- Directory Listing --\n");
      printf("Arguments?: ");
      fgets(arg,100,stdin);
      if (strlen(arg) == 0) { // if no args given
	arg[0] = 0;
      } else {
	arg[strlen(arg)-1] = 0; // remove newline character
      }
      printf("Path?: ");
      fgets(path,100,stdin);
      if (strlen(path) == 0) { // if no path given
	path[0] = 0;
      } else {
	path[strlen(arg)-1] = 0; // remove new line character
      }

      char* exec_str[] = {"ls",arg,path,NULL};
      
      struct timeval start,end;
      gettimeofday(&start,NULL);
      int pid = fork();
      if (pid < 0) {
	fprintf(stderr,"Fork failed!\n");
      } else if (pid == 0) {
        execvp(exec_str[0],exec_str);
        fprintf(stderr,"Invalid command!\n"); // exec shouldn't return
      } else if (pid > 0) {
	wait(NULL);
	gettimeofday(&end,NULL);
	struct rusage r;
	getrusage(RUSAGE_SELF,&r);
	printf("-- Statistics --\n");
	printf("\nElapsed Time: %ld\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
	printf("Page Faults: %ld\n",r.ru_majflt);
	printf("Page Faults (reclaimed): %ld\n\n",r.ru_minflt);
      }  
    } else {
      printf("Bad input, please input something on list of commands.\n");
    }
  } return 0;
}
