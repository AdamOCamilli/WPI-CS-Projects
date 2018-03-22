/* 'dv' - An accompanying function to rm that searches for a file in the user's dumpster directory 
   located at '~/.dumpster' and transfers it to current working directory

   The following options are supported:
       -v : Be verbose
       -h : Display help and use message
       -a : Get all files with the same original name (i.e. foo.1, foo.2, ...)

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
  // First check environment variable is set
  char *dumpster = (char *) malloc(sizeof(char*));
  if ((dumpster = getenv("DUMPSTER"))  == NULL) {
    printf("Environmental variable DUMPSTER not detected\n");
    return 1;
  } else if (argc <= 1) {
    printf("Usage: dv [options] [files]\n");
    printf("Options: \n");
    printf("\t -v : Be verbose (list all important actions)\n");
    printf("\t -h : Display help message\n");
    printf("\t -a : Retrieve all files with this name\n");
    printf("Must pass at least one file\n");
    return 1;
  }

  // Parse any command line options
  int opt_count = 0;
  int opt;
  int ALL_FILES = 0;
  int VERBOSE = 0;
  while ((opt = getopt(argc, argv, "vha")) != -1) {
    switch (opt) {
    case 'v':
      VERBOSE = 1;
      opt_count++;
      break;
    case 'h':
      printf("Usage: dv [options] [files]\n");
      printf("Options: \n");
      printf("\t -v : Be verbose (list all important actions)\n");
      printf("\t -h : Display help message\n");
      printf("\t -a : Retrieve all files with this name\n");
      opt_count++;
      break;
    case 'a':
      ALL_FILES = 1;
      opt_count++;
      break;
    }
  }
  
  // Get current working directory (where retrieved files will be put)
  char *cwd = (char *) malloc(PATH_MAX);
  if (getcwd(cwd, PATH_MAX) == NULL) {
    if (VERBOSE) {
      printf("Somehow couldn't get current working directory\n");
      printf("Errno: %d\n",errno);
    }
    return -1;
  }
  
  char **files = (char **) malloc(sizeof(argv));
  char **abspaths = (char **) malloc(sizeof(argv) * PATH_MAX);  // Absolute paths of files to be found

  for (int i = 0; i < argc - opt_count - 1; i++) {
    files[i] = argv[opt_count + i + 1];
  }
  
  char *path = (char *) malloc(PATH_MAX);
  for (int i = 0; i < argc - opt_count - 1; i++) {
    memset(path,0,PATH_MAX);
    if (ALL_FILES) {
      for (int j = 0; j < 10; j++) {	
	memset(path, 0, PATH_MAX); // Clear between iterations
	if (j == 0) {
	  if ((path = in_dir(files[i], dumpster, dumpster, 0,VERBOSE)) != NULL) {
	    if (VERBOSE) {
	      printf("Retrieving file %s from dumpster to %s\n",files[i],cwd);
	    }
	    if (same_dev_move(files[i], path, cwd, PATH_MAX, VERBOSE) != 0) {
	      if (VERBOSE)
		printf("Couldn't retrieve %s\n",files[i]);
	    } else 
	      if (VERBOSE)
		printf("Retrieved %s into %s\n",files[i],cwd);
	  }
	  continue;
	}
	strncpy(path,files[i],PATH_MAX);
	char ext[2];
	sprintf(ext,"%d",j);
	strcat(path,".");
	strcat(path,ext);
	if ((path = in_dir(path, dumpster, dumpster,0,VERBOSE)) != NULL) {
	  if (VERBOSE) {
	    printf("Retrieving file %s from dumpster to %s\n",files[i],cwd);
	  }
	  if (same_dev_move(files[i], path, cwd, PATH_MAX, VERBOSE) != 0) {
	    if (VERBOSE)
	      printf("Couldn't retrieve %s\n",files[i]);
	  } else 
	    if (VERBOSE)
	      printf("Retrieved %s into %s\n",files[i],cwd);
	} else {
	  if (VERBOSE)
	    printf("Done retrieving copies of %s\n",files[i]);
	  break;
	}
      } 
    } else if ((path = in_dir(files[i], dumpster, dumpster, 0,VERBOSE)) != NULL) {
      if (VERBOSE) {
	printf("Retrieving file %s from dumpster to %s\n",files[i],cwd);
      }
      if (same_dev_move(files[i], path, cwd, PATH_MAX, VERBOSE) != 0) {
	if (VERBOSE)
	  printf("Couldn't retrieve %s\n",files[i]);
      } else 
	if (VERBOSE)
	  printf("Retrieved %s into %s\n",files[i],cwd);
    }
  }
  
  free(path);
  free(cwd);
  free(files);
  free(abspaths);
  
  return 0;
}
