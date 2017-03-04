#define MAX_ARGS 32
#define MAX_CMD_LENGTH 128

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include <time.h>
#include <wait.h>
#include <sys/resource.h>
#include <errno.h>

/** Struct and helper functions from a stack overflow thread on creating a dynamic array in C. 
 * http://stackoverflow.com/questions/3536153/c-dynamically-growing-array
 */
typedef struct {
  size_t used;
  size_t size;
  char* array[];
} Array;

void initArray(Array* a, size_t initSize) {
  a->used = 0;
  a->size = initSize;
  a->array = 
}

void insertArray(Array* a, char* elt) {
  if (a->used == a->size) {
      a->size++;
      a->array = (char **)realloc(a->array,a->size * sizeof(char*));
    } a->array[a->used++] = elt;
}

void freeArray(Array* a) {
  free(a->array);
  a->array = NULL;
  a->used = a->size = 0;
}

/** Forks and executes a system command passed in as a string. 
 * Assumes all arguments/paths are already added.
 */
void execute(Array* cmd, int num) {

  struct timeval start,end;
  gettimeofday(&start,NULL);
        
  int pid = fork();

  char* tok;
  char* new_cmd_copy = strdup(cmd->array[num]);
  char** exec_str = (char**)calloc(MAX_ARGS, sizeof(char*));
  int arg_index = 0;

  while ((tok = strtok(new_cmd_copy, " ")) != NULL) {
    exec_str[arg_index++] = tok;
    //printf("%s\n",tok);
    new_cmd_copy = NULL;
  }

  exec_str[arg_index] = NULL;
  
  if (pid < 0) {
    fprintf(stderr,"Fork failed!\n");
  } else if (pid == 0) {
    execvp(exec_str[0],exec_str);
    fprintf(stderr,"Invalid command!\n"); // exec shouldn't return
  } else {
     wait(NULL);
     gettimeofday(&end,NULL);
     struct rusage r;
     getrusage(RUSAGE_SELF,&r);
     printf("-- Statistics --\n");
     printf("\nElapsed Time: %ld\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
     printf("Page Faults: %ld\n",r.ru_majflt);
     printf("Page Faults (reclaimed): %ld\n\n",r.ru_minflt);
  }

  free(exec_str);
  
}

/** Prints shell prompt. */
void type_prompt(Array* newcmds) {
  int ch;
  printf("G'Day Commander! What command would you like me to run?\n");
  printf("\t0.  whoami\t:  Prints out the result of the whoami command.\n");
  printf("\t1.  last\t:  Prints out the result of the last command.\n");
  printf("\t2.  ls\t:  Prints out the result of a listing on a user-specified path.\n");
  int i = 0;
  if (newcmds->used > 0) {
    for (i = 0; i < newcmds->used; i++) {
      printf("\t%d.  %s\t: User added command.\n",3+i,newcmds->array[i]);
    }
  }
  printf("\ta.  add command\t: Adds a new command to the menu.\n");
  printf("\tc.  Change Directory\t: causes your program to change the working directory to the specified directory.\n");
  printf("\te.  Exit\t: Causes your shell to terminate.\n");
  printf("\tp.  Print Working Directory\t: prints the directory associated with your process.\n");
  printf("Option?: ");
} 

/** Phase 2 of Project 1: Runs a list of commands in a menu-driven manner and allows user to add their own commands.
 * Time code borrowed from (http://www.cs.loyola.edu/~jglenn/702/S2008/Projects/P3/time.html)
 */
int main(int argc, char**argv[]) {
  
  int shell = 1;

  Array* native_cmds = (Array*)malloc(sizeof(Array*)); // standard commands
  initArray(native_cmds,3);
  Array* newcmds = (Array*)malloc(sizeof(Array*)); // new commands added by user
  initArray(newcmds,1);
  int cmds_added = 0; // total number of new commands added by user
  
  while(shell) {

    char cmd[100]; 
    int cmd_num = 0;
    char* cmd_char;
    char arg[100];
    char path[100];

    type_prompt(newcmds);
    fgets(cmd,100,stdin);

    // Check if user entered a number
    char* non_num = (char*)malloc(100);
    cmd_num = strtol(cmd,&non_num,10);
    if (cmd == non_num) { // if input was not numerical (needed since strlol returns 0 for '0' and non-numbers)
      cmd_num = -1; // then don't call numbered commands
    }
    
    if (cmd_num == 0) { // whoami command
      printf("\n-- Who Am I? --\n");
      insertArray(native_cmds,"whoami");
      execute(native_cmds, (native_cmds->used) - 1);
    } else if (cmd_num == 1) { // last command
      insertArray(native_cmds,"last");
      printf("-- Last Logins --\n");
      execute(native_cmds,(native_cmds->used) - 1);
    } else if (cmd_num == 2) { // ls command
      printf("\n-- Directory Listing --\n");
      printf("Arguments?: ");
      fgets(arg,sizeof(arg),stdin);
      arg[strlen(arg)-1] = 0; // remove newline char
      strcat(arg, " "); //add space
      printf("Path?: ");
      fgets(path,sizeof(path),stdin);
      path[strlen(path)-1] = 0; // remove newline char
      strcat(path, " "); // add space

      char exec_str[128];
      strcpy(exec_str,"ls ");
      strcat(exec_str,arg);
      strcat(exec_str,path);
      char* dup = strdup(exec_str);

      insertArray(native_cmds,dup);
      execute(native_cmds, (native_cmds->used) - 1);
    } else if (cmd_num > 2) {
      if (cmd_num > newcmds->used - 3) {
	printf("Sorry commander, this number is not assigned to a command yet!\n");
      } else {
	printf("-- User Command %d --\n",cmd_num);
	printf("Executing: %s\n",newcmds->array[cmd_num - 3]); // bad, this is command 0 on newcmds but is displaced as 3/4/...
	execute(newcmds,cmd_num - 3);
      }
    } else if (cmd[0] == 'a' && isspace(cmd[1])) {
      char new_cmd[MAX_CMD_LENGTH];
      printf("-- Add a command --\n");
      printf("Command to add?: ");
      fgets(new_cmd,sizeof(new_cmd),stdin);
      // Remove trailing newline character
      new_cmd[strlen(new_cmd)-1] = 0;
      printf("NEW CMD: %s\n",new_cmd);
      insertArray(newcmds,new_cmd);
      printf("ARR: %s\n",newcmds->array[0]);
      cmds_added++;
      printf("Okay, added with ID %d!\n\n",2+cmds_added);
    } else if (cmd[0] == 'c' && isspace(cmd[1])) { //cd command
      struct timeval start,end;
      gettimeofday(&start,NULL);
      // execute inline
      char dest[200];
      printf("-- Change Working Directory --\n");
      printf("Enter new directory: ");
      fgets(dest,200,stdin);
      dest[strlen(dest)-1] = 0;
      chdir(dest);
      // done executing inline
      gettimeofday(&end,NULL);
      struct rusage r;
      getrusage(RUSAGE_SELF,&r);
      printf("-- Statistics --\n");
      printf("Elapsed Time: %ld\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
      printf("Page Faults: %ld\n",r.ru_majflt);
      printf("Page Faults (reclaimed): %ld\n\n",r.ru_minflt);
    } else if (cmd[0] == 'e' && isspace(cmd[1])) { // exit command
      struct timeval start,end;
      gettimeofday(&start,NULL);

      printf("Logging you out commander.\n");
      shell = 0;
      
      gettimeofday(&end,NULL);
      struct rusage r;
      getrusage(RUSAGE_SELF,&r);
      printf("-- Statistics --\n");
      printf("Elapsed Time: %ld\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
      printf("Page Faults: %ld\n",r.ru_majflt);
      printf("Page Faults (reclaimed): %ld\n\n",r.ru_minflt);
    } else if (cmd[0] == 'p' && isspace(cmd[1])) { // pwd command
      struct timeval start,end;
      gettimeofday(&start,NULL);
      
      char cwd[200];
      printf("-- Current Working Directory --\n");
      getcwd(cwd,sizeof(cwd));
      printf("%s\n",cwd);
      
      gettimeofday(&end,NULL);
      struct rusage r;
      getrusage(RUSAGE_SELF,&r);
      printf("-- Statistics --\n");
      printf("Elapsed Time: %ld\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
      printf("Page Faults: %ld\n",r.ru_majflt);
      printf("Page Faults (reclaimed): %ld\n\n",r.ru_minflt); 
    } else {
      printf("Bad input, please input something on list of commands.\n");
    }
  }
  freeArray(newcmds);
  freeArray(native_cmds);
  return 0;
}
