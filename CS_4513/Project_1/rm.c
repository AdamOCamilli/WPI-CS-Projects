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
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>          // Needed to get home directory (where dumpster is located)
#include <string.h>
#include <errno.h>
#include <dirent.h>       // Needed to parse directories

/* Official name of program (real rm.c has this, gotta look official) */
#define PROGRAM_NAME "rm"

/* Files probably should have less than 10 extensions */
#define MAX_EXTS 10

/* Maximum length of file paths already given as 4096 in limits.h */

/* Print various helpful debug messages */
#define DEBUG true

void print_help() {
  printf("Usage: rm [options] [files]\n");
  printf("Options: \n");
  printf("\t -f : Force a complete remove, i.e. default linux 'rm' or 'rm -r' process\n");
  printf("\t -h : Display help and use message\n");
  printf("\t -r : Remove directories recursively (copy to dumpster)\n");
}

int main(int argc, char **argv) {
  // First check environment variable is set
  if (getenv("DUMPSTER")  == NULL) {
    perror("Environmental variable DUMPSTER not detected\n");
    return 1;
  } if (argc <= 1) {
    printf("Usage: rm [options] [files]\n");
    printf("Must pass at least one file\n");
    return 1;
  }
    

  // Parse any options called with rm
  int opt_count = 0;
  int opt;
  while ((opt = getopt(argc, argv, "fhr")) != -1) {
    switch (opt) {
      case 'h':
	print_help();
	return 0;
 	break;
      case 'f':
        // Call real rm ...
	opt_count++;
	break;
      case 'r':
        // Set up recursion ...
	opt_count++;
	break;
    }
  }

  /* If no options, go ahead and move file(s) to dumpster */
  
  // First parse argv to get filenames
  char **files = (char **) malloc(sizeof(argv));
  char **realpaths = (char **) malloc(sizeof(argv) * PATH_MAX);  // Absolute paths of files to be found
  
  // Use stdlib function realpath() to find true paths
  for (int i = 0; i < argc - opt_count - 1; i++) { 
    files[i] = argv[i + opt_count + 1]; // +1 for ./rm + number of options

#ifdef DEBUG
    printf("Getting absolute path of %s\n",files[i]);
#endif

    realpaths[i] = realpath(files[i], realpaths[i]);

#ifdef DEBUG
    printf("Absolute path determined to be \n%s\n",realpaths[i]);
    printf("Errno: %d\n\n",errno);
#endif

  }

  // Now use rename to complete move operation, adding extension if necessary
  // Note if more than 9 instances of a file found, we must reject d
  int successes = 0;
  for (int i = 0; i < argc - opt_count - 1; i++) {
    char *path = strcat(getenv("DUMPSTER"), files[i]);
    printf("Path: %s\n",path);
#ifdef DEBUG
    printf("Path of file in dumpster determined to be \n%s\nMoving file...\n\n", path);
#endif
    
    if (realpaths[i] == NULL || path == NULL) {
      printf("Path came up as null for %s\nOperation aborted\n\n",files[i]);
      continue;
    }
    
    // Check if duplicate files exist in dumpster
    int duplicates = 0;
    if (access(path,F_OK) != -1) {
      duplicates++;
      for (int i = 1; i < 10; i++) {
	// To avoid altering path, add extension to temp variable each iteration
	// Redeclare it to avoid [path].1.2.3, etc.
	char *tmp_path = (char *) malloc(PATH_MAX);
	memset(tmp_path, 0, strlen(tmp_path)); // Clear in between iterations
	strcat(tmp_path,path);
	char ext[2];
	sprintf(ext,"%d",i);
	strcat(tmp_path,".");
	if (access(strcat(tmp_path,ext),F_OK) != -1) {
#ifdef DEBUG
	  printf("%s\nfound to exist\n\n",tmp_path);
#endif
	  duplicates++;
	} 
	free (tmp_path);
      }
      if (duplicates) {
	if (duplicates >= 10) {
	  printf("%s has too many duplicate files (%d)\n\n",files[i],duplicates);
	  continue;
	} else {
	  // Now adjust path accordingly
	  char ext[2];
	  sprintf(ext,"%d",duplicates);
	  strcat(path,".");
	  strcat(path,ext);        
#ifdef DEBUG
	  printf("%d duplicate file(s) found for %s in dumpster\n\n",duplicates,files[i]);
#endif
	}
      }
    }
    rename(realpaths[i], path); 
    successes++;
  }
   
  /* Free allocated variables */
  free(files);
  free(realpaths); 
  
  if (successes == 0) {
    printf("No files successfully moved\n");
    return -1;
  } else {
    if (successes == 1) {
      printf("%d file successfully moved\n",successes);
    } else {
      printf("%d files successfully moved\n",successes);
    }
    return 0;
  }
}
