#define TRUE 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>


#define MAX_ARGS 32
#define MAX_CMD_LENGTH 128

/** Forks and executes a system command passed in as a string. 
 * Assumes all arguments/paths are already added.
 */
void execute(char* cmd) {

  struct timeval start,end;
  gettimeofday(&start,NULL);

  int pid = fork();

  char* tok;
  char* new_cmd_copy = strdup(cmd);

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
      fprintf(stderr, "Invalid Command\n"); // exec shouldn't return 
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
void type_prompt(char* newcmds[], int num_comd) {
  fflush(stdin);
  printf("G'Day Commander! What command would you like me to run?\n");
  printf("\t0.  whoami\t:  Prints out the result of the whoami command.\n");
  printf("\t1.  last\t:  Prints out the result of the last command.\n");
  printf("\t2.  ls\t:  Prints out the result of a listing on a user-specified path.\n");

  int i;
  if (num_comd > 0) {
    for (i = 0; i < num_comd; i++) {
      printf("\t%d.  %s\t: User added command.\n",3+i,newcmds[i]);
    }
  }
  

  printf("\ta.  add command\t: Adds a new command to the menu.\n");
  printf("\tc.  Change Directory\t: causes your program to change the working directory to the specified directory.\n");
  printf("\te.  Exit\t: Causes your shell to terminate.\n");
  printf("\tp.  Print Working Directory\t: prints the directory associated with your process.\n");
  printf("Option?: ");
}


/** Phase 2 of Project 1: Simply runs a list of commands in a menu-driven manner. 
 * Time code borrowed from (http://www.cs.loyola.edu/~jglenn/702/S2008/Projects/P3/time.html)
 */
int main(int argc, char**argv[]) {

  int run = 1;
	
  char* newcmds[50]; // new commands added by user
  int cmds_added = 0; // total new commands added by user
  
  while(run) {

    char cmd[100]; // arbitrary buffer size
    long cmd_num;
    char* cmd_char;
    char arg[100];
    char path[100];

    type_prompt(newcmds,cmds_added);
    fgets(cmd,100,stdin);

    // Check if user entered a number
    char* non_num = (char*)malloc(4);
    cmd_num = strtol(cmd,&non_num,10);
    if (cmd == non_num) { // if input was not numerical (needed since strlol returns 0 for '0' and non-numbers)
      cmd_num = -1; // then don't call numbered commands
    }
    
    if (cmd_num == 0) { // whoami command
      printf("\n-- Who Am I? --\n");
      execute("whoami");
    } else if (cmd_num == 1) { // last command
      printf("-- Last Logins --\n");
      execute("last");
    } else if (cmd_num == 2) { // ls command
      printf("\n-- Directory Listing --\n");
      printf("Arguments?: ");
      fgets(arg,100,stdin);
      arg[strlen(arg)-1] = 0; //remove newline char
      strcat(arg," "); //add space
      printf("Path?: ");
      fgets(path,100,stdin);
      path[strlen(path)-1] = 0; //remove newline char
      strcat(path," "); //add space
      char exec_str[128];
      strcpy(exec_str,"ls ");
      strcat(exec_str,arg);
      strcat(exec_str,path);
      execute(exec_str);
    } else if (cmd_num > 2) {
      printf("-- Command: %s --\n",newcmds[cmd_num-3]);
      execute(newcmds[cmd_num-3]);
    } else if (strcmp(cmd,"c\n") == 0) { // cd command
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
    } else if (strcmp(cmd,"e\n") == 0) { // exit command
      struct timeval start,end;
      gettimeofday(&start,NULL);

      printf("Logging you out commander.\n");
      run = 0;
      
      gettimeofday(&end,NULL);
      struct rusage r;
      getrusage(RUSAGE_SELF,&r);
      printf("-- Statistics --\n");
      printf("Elapsed Time: %ld\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
      printf("Page Faults: %ld\n",r.ru_majflt);
      printf("Page Faults (reclaimed): %ld\n\n",r.ru_minflt);
    } else if (strcmp(cmd,"p\n") == 0) { // pwd command
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
    } else if (strcmp(cmd,"a\n") == 0) { // add new command
      char* new_cmd = (char*)malloc(MAX_CMD_LENGTH * sizeof(char));
      
      printf("-- Add a command --\n");
      printf("Command to add?: ");
      fgets(new_cmd,MAX_CMD_LENGTH,stdin);

      // Remove trailing newline character
      new_cmd[strlen(new_cmd) - 1] = '\0';
      
      newcmds[cmds_added] = new_cmd; //Add new command to the list
      cmds_added++;
      printf("Okay, added with ID %d!\n\n",2+cmds_added);	
    } else {
      printf("Bad input, please input something on list of commands.\n");
    }
  } return 0;
}
