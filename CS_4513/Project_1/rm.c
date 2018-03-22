/* 'rm' - A transparent replacement of /bin/rm which moves file(s) to the user's dumpster directory 
   located at '~/.dumpster'
   
   It does not replace /bin/rm, but rather appears first in the /bin/ path so as to be called first.
   It performs one of two actions:
       1) If the file is not on the same partition as the dumpster directory, the file is moved.
       2) If the file is on the same partition as the dumpster directory, it is hard-linked (renamed)
   If a file is already contained within the dumpster, it is given extenstion '.[num]', where [num] is
   [1-9]. If more duplicates are attempted to be added, they will be rejected (remain at their
   current location).

   All file permissions, including access times, are preserved.

   The following options are supported:
       -f : Force a complete remove, i.e. default linux 'rm' or 'rm -r' process
       -h : Display help and use message
       -r : Remove directories recursively (copy to dumpster)

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

void print_help() {
  printf("Usage: rm [options] [files]\n");
  printf("Options: \n");
  printf("\t -v : Verbose, print major actions that take place within program\n");
  printf("\t -f : Force a complete remove, i.e. default linux 'rm' or 'rm -r' process\n");
  printf("\t -h : Display help and use message\n");
  printf("\t -r : Remove directories recursively (copy to dumpster)\n");
}

int main(int argc, char **argv) {
  // First check environment variable is set
  char *dumpster = (char *) malloc(sizeof(char*));
  if ((dumpster = getenv("DUMPSTER"))  == NULL) {
    printf("Environmental variable DUMPSTER not detected\n");
    return 1;
  } if (argc <= 1) {
    printf("Usage: rm [options] [files]\n");
    printf("Must pass at least one file\n");
    return 1;
  }
    

  // Parse any options called with rm
  int opt_count = 0;
  int opt;
  int really_remove = 0;
  int VERBOSE = 0;
  while ((opt = getopt(argc, argv, "vfhr")) != -1) {
    switch (opt) {
      case 'v':
	VERBOSE = 1;
	opt_count++;
	break;
      case 'h':
	print_help();
	return 0;
 	break;
      case 'f':
	really_remove = 1;
	opt_count++;
	break;
      case 'r':
        // Set up recursion ...
	opt_count++;
	break;
    }
  }

/**************************************************** 
  
 If no options, go ahead and move or copy file(s) to dumpster.
 First parse argv to find true file paths
 
*****************************************************/
  
  char **files = (char **) malloc(sizeof(argv));
  char **abspaths = (char **) malloc(sizeof(argv) * PATH_MAX);  // Absolute paths of files to be found
  
  // Use stdlib function realpath() to find true paths
  for (int i = 0; i < argc - opt_count - 1; i++) { 
    files[i] = argv[i + opt_count + 1]; // +1 for ./rm + number of options

    if (VERBOSE) {
      printf("Getting absolute path of %s\n",files[i]);
    }

    // Works if files are in current directory
    abspaths[i] = realpath(files[i], abspaths[i]);
    // Else utilize dirname()
    if (abspaths[i] == NULL) {
      // Make sure we have permission to move file
      if (errno == EACCES || errno == EPERM) {
	if (VERBOSE) {
	  printf(": Insufficient permission to access %s\n",files[i]);
	}
	continue;
      } else if (errno == ENOENT) {
	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd)) == NULL) {	 
	  if (VERBOSE) {
	    printf("Somehow couldn't get current working directory\n");
	  }
	  continue;
	} else {
	  while (abspaths[i] == NULL) {
	    char dpath[PATH_MAX];
	    memset(dpath,0,sizeof(dpath));
	    printf("What absolute directory is %s in? (exit to exit)\n",files[i]);
	    fgets(dpath,PATH_MAX,stdin);
	    dpath[strcspn(dpath, "\n")] = 0; // Remove trailing newline
	    if (strcmp(dpath,"exit") == 0) {
	      printf("Exiting...\n");
	      break;
	    }
	    if (abspaths[i] != NULL)
	      abspaths[i] = in_dir(files[i], dpath, cwd, 0, VERBOSE);
	  }
	  if (abspaths[i] == NULL) {
	    if (VERBOSE) 
	      printf("User gave no input as to where %s is, ignoring it\n",files[i]);
	    continue;
	  }
	}
      }
    }
    if (VERBOSE) {
      printf("Absolute path determined to be %s\n",abspaths[i]);
      if (abspaths[i] == NULL)
	printf("Errno: %d\n",errno);
    }
  }
  
  
/***************************************************

 Now find out which device each file is on 

****************************************************/

  struct stat statbuf;

  if (VERBOSE) {
    printf("Getting stat() for dumpster at %s\n", dumpster);
  }
  
  stat(dumpster,&statbuf);
  int dumpster_dev = statbuf.st_dev;

  char **same_dev_paths = (char **) malloc(sizeof(abspaths));
  char **same_dev_names = (char **) malloc(sizeof(files));
  char **diff_dev_paths = (char **) malloc(sizeof(abspaths));
  char **diff_dev_names = (char **) malloc(sizeof(files));
  int same_count = 0;
  int diff_count = 0;

  for (int i = 0; i < argc - opt_count - 1; i++) {
    memset(&statbuf, 0, sizeof(statbuf));
    stat(abspaths[i],&statbuf);
    if (statbuf.st_dev == dumpster_dev) {
      same_dev_paths[i] = abspaths[i];
      same_dev_names[i] = files[i];
      same_count++;
    } 
  } 
  for (int i = 0; i < argc - opt_count - 1; i++) {
    memset(&statbuf, 0, sizeof(statbuf));
    stat(abspaths[i],&statbuf);
    if (statbuf.st_dev != dumpster_dev) {
      diff_dev_paths[i] = abspaths[i];
      diff_dev_names[i] = files[i];
      diff_count++;
    } 
  }

  //For files on the same partition, use rename() syscall
  for (int i = 0; i < same_count; i++)
    same_dev_move(same_dev_names[i], same_dev_paths[i], dumpster, 10, VERBOSE);

  //For files on a different partition, use fcopy() function
  for (int i = 0; i < diff_count; i++)
    diff_dev_move(diff_dev_names[i], diff_dev_paths[i], dumpster, VERBOSE);

  free(files);
  free(abspaths);
  free(same_dev_paths);
  free(same_dev_names);
  free(diff_dev_paths);
  free(diff_dev_names);

  return 0;
}
