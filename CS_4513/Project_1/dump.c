/* 'dump' - An accompanying function to rm that deletes all file in the user's dumpster directory 
   located at '~/.dumpster'

   The following options are supported:
       -v : Be verbose
       -h : Display help and use message

   Returns 0 upon success, 1 upon pre-failure, -1 upon bug failure

   Author: Adam Camilli (aocamilli@wpi.edu)

 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <utime.h>

#include <string.h>
#include <errno.h>
#include <dirent.h>       // Needed to parse directories

#include "dumpster.h"

int main(int argc, char **argv) {
  // First check environment variable is se
  char *dumpster = (char *) malloc(sizeof(char*));
  if ((dumpster = getenv("DUMPSTER"))  == NULL) {
    printf("Environmental variable DUMPSTER not detected\n");
    return 1;
  }

  // Parse any command line options
  int opt_count = 0;
  int opt;
  int really_remove = 0;
  int VERBOSE = 0;
  while ((opt = getopt(argc, argv, "vh")) != -1) {
    switch (opt) {
      case 'v':
	VERBOSE = 1;
	opt_count++;
	break;
      case 'h':
	printf("Usage: dump [options]\n");
	printf("Options: \n");
	printf("\t -v : Be verbose (list all important actions)\n");
	printf("\t -h : Display help message\n");
	return 0;
 	break;
    }
  }

  // Change working directory to dumpster
  if (chdir(dumpster) != 0) {
    if (VERBOSE) {
      printf("Couldn't change working directory to dumpster\n");
      printf("Errno: %d\n",errno);
    }
    return -1;
  }
  
  char *buf = (char *) malloc(PATH_MAX);
  
  if (clear_dir(dumpster, getcwd(buf, PATH_MAX), VERBOSE) != 0) {
    if (VERBOSE) {
      printf("Couldn't dump all dumpster contents\n");
      printf("Errno: %d\n",errno);
    }
    return -1;
  }
  
  return 0;

}
