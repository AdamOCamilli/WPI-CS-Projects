/* 'dumpster' - A collection of functions for the dumpster commands.
   
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

/* Maximum length of file paths already given as 4096 in constant PATH_MAX in limits.h */
// PATH_MAX = 4096

/* Max file size we copy */
#define FILE_MAX 1024*1024 
/* How far we recurse into subdirectories to find files */
#define DEPTH_MAX 3

/**************************************************************

                        Helper functions

**************************************************************/

/* Checks directory and, recursively, its sub-directories for a file up to a certain depth
   Note: Considers file extension (i.e., not basename(file)) when checking for match 
   Returns the absolute path of match if found, null otherwise
*/
char * in_dir(char *fname, char *dpath, char *orig_dir, int depth, int VERBOSE) {
  // Base case
  if (VERBOSE) {
    printf("Directory depth = %d\n",depth);
  }
  if (depth > DEPTH_MAX)
    return NULL;
  
  DIR *p_dir;
  struct dirent *entry;
  // Check directory exists
  chdir(dpath);
  if ((p_dir = opendir(dpath)) == NULL) {
    if (VERBOSE) {
      printf("Can't open directory %s\nErrno: %d\n",dpath,errno);
    }
    chdir(orig_dir);
    return NULL;
  }
  
  struct stat statbuf;  
  // Start parsing
  char *abspath = (char *) malloc (PATH_MAX);
  char *temp_dir = (char *) malloc (PATH_MAX);
  while ((entry = readdir(p_dir)) != NULL) {
    memset(temp_dir, 0, sizeof(temp_dir)); // Clear between iterations
    lstat(entry->d_name, &statbuf);
    // If we find another directory
    if (S_ISDIR(statbuf.st_mode)) {
      // Ignore . .. and hidden directories
      if (entry->d_name[0] == '.')
	continue;
      // Concatenate original directory onto new subdirectory in temp variable
      strcat(temp_dir, dpath);
      printf("temp_dir: %s\n",temp_dir);
      if (temp_dir[strlen(temp_dir) - 1] != '/')
	strcat(temp_dir, "/");
      strcat(temp_dir,entry->d_name);
      if (VERBOSE) {
	printf("Recursing to subdirectory %s\n",temp_dir);
      }
      // Recurse, pass in working directory as orig_dir to switch back to when function terminates
      if ((abspath = in_dir(fname, temp_dir, dpath, depth + 1, VERBOSE)) != NULL) {
	chdir(orig_dir);
	return abspath;
      }
    } else if (strcmp(fname,entry->d_name) == 0) {
      abspath = realpath(entry->d_name,abspath);
      chdir(orig_dir);
      return abspath;
    }
  }

  if (VERBOSE) {
    printf("File not found in directory or subdirectories (depth=%d)\n",depth);
  }
  
  chdir(orig_dir);
  //free(abspath);
  //free(temp_dir);
  return NULL;
}

int clear_dir(char *dpath, char *orig_dir, int VERBOSE) {
  DIR *p_dir;
  struct dirent *entry;
  // Check directory exists
  chdir(dpath);
  if ((p_dir = opendir(dpath)) == NULL) {
    if (VERBOSE) {
      printf("Can't open directory %s\nErrno: %d\n",dpath,errno);
    }
    chdir(orig_dir);
    return -1;
  }
  
  struct stat statbuf;  
  // Start parsing
  char *abspath = (char *) malloc (PATH_MAX);
  char *temp_dir = (char *) malloc (PATH_MAX);
  while ((entry = readdir(p_dir)) != NULL) {
    memset(temp_dir, 0, sizeof(temp_dir)); // Clear between iterations
    lstat(entry->d_name, &statbuf);
    // If we find another directory
    if (S_ISDIR(statbuf.st_mode)) {
      // Ignore . .. and hidden directories
      if (entry->d_name[0] == '.')
	continue;
      // If it is an empty directory, just delete it
      if (rmdir(entry->d_name) == 0) {
	if (VERBOSE) {
	  printf("Unlinked empty directory %s\n",entry->d_name);
	}
	continue;
      }
      // Concatenate original directory onto new subdirectory in temp variable
      strcat(temp_dir, dpath);
      printf("temp_dir: %s\n",temp_dir);
      if (temp_dir[strlen(temp_dir) - 1] != '/')
	strcat(temp_dir, "/");
      strcat(temp_dir,entry->d_name);
      if (VERBOSE) {
	printf("Recursing to subdirectory %s\n",temp_dir);
      }
      // Recursively 
      clear_dir(temp_dir, dpath, VERBOSE);
      if (rmdir(entry->d_name) == 0) {
	if (VERBOSE) {
	  printf("Unlinking empty directory %s\n",entry->d_name);
	}
	continue;
      } else if (VERBOSE) {
	printf("Failed to remove all files in directory %s\n",entry->d_name);
	chdir(orig_dir);
	closedir(p_dir);
	return -1;
      }
    }
    if (VERBOSE) {
      printf("Unlinking file %s\n",entry->d_name);
    }
    unlink(entry->d_name);
  }
  
  chdir(orig_dir);
  //free(abspath);
  //free(temp_dir);
  closedir(p_dir);
  return 0;
}


/* My own 'copy' function to move files from one partition to another 
   src should be a file path on another partition, while dest should point to
   a newly created file in dumpster.
   All permissions/accesstimes are preserved.
 */
int fcopy(char *src, char *dest, int VERBOSE) {
  
  FILE *old = fopen(src,"r+");
  FILE *new = fopen(dest,"r+");

  // Verify paths are valid
  if (old == NULL) {
    if (VERBOSE) {
      printf(": %s not a valid source file -fcopy()\n",src);
      printf(": errno = %d\n",errno);
    }
    return 1;
  } else if (new == NULL) { // i.e. dumpster was full
    if (VERBOSE) {
      printf("No valid destination file -fcopy()\n");
      printf("errno = %d\n",errno);
    }
    return 1;  
  }

  // Mark original files' access times.
  struct stat statbuf;  
  struct utimbuf *old_times; 
  lstat(src,&statbuf);

  old_times->actime = statbuf.st_atim.tv_nsec; // Last access (nanoseconds)
  old_times->modtime = statbuf.st_mtim.tv_nsec; // Last modification (nanoseconds)
  

  // Read old file into new
  char buf[FILE_MAX];
  size_t bytes;
  while ((bytes = fread(buf, 1, sizeof(buf), old)) > 0)
    fwrite(buf, 1, bytes, new);
  
  // Close files, unlink old file
  fclose(old);
  fclose(new);
  unlink(src);
  // Adjust access times
  utime(dest,old_times);

  return 0;
}

/**************************************************************

  Functions to move files from the same or different partitions 
  to dumpster

**************************************************************/

/* 
   Modified move operation for file on the same device (partition) as the dest directory 
   Use rename to complete move operation, adding extension if necessary
   Note if more than 9 instances of a file found, we must reject following instances
*/
int same_dev_move(char *fname, char *fpath, char *dumppath, int limited_copies, int VERBOSE) {

  // Path of file in new directory
  char *newpath = (char *) malloc(PATH_MAX);
  strncpy(newpath, dumppath, PATH_MAX);
  
  if (VERBOSE) {
    printf("New directory path: %s\n", newpath);
  }

  struct stat statbuf;
  lstat(dumppath, &statbuf);
  // If a directory
  if (S_ISDIR(statbuf.st_mode)) {
    if (newpath[strlen(newpath)-1] != '/') {
      strcat(newpath,"/");
    }
  }
    
  strcat(newpath,fname);
  if (VERBOSE) {
    printf("New path of file determined to be %s\nMoving file...\n", newpath);
  }
    
  // Check against null
  if (fpath == NULL || newpath == NULL) {
    printf("Path came up as null for %s\nOperation aborted\n",fname);
    return -1;
  }
    
  // Check if duplicate files exist (if required, such as in)
  if (limited_copies) {
    int duplicates = 0;
    if (access(newpath,F_OK) != -1) {
      duplicates++;
      for (int i = 1; i < 10; i++) {
	// To avoid altering path, add extension to temp variable each iteration
	// Redeclare it to avoid [path].1.2.3, etc.
	char *tmp_path = (char *) malloc(PATH_MAX);
	memset(tmp_path, 0, strlen(tmp_path)); // Clear in between iterations
	strcat(tmp_path, newpath);
	char ext[2];
	sprintf(ext,"%d",i);
	strcat(tmp_path,".");
	if (access(strcat(tmp_path,ext),F_OK) != -1) {
	  if (VERBOSE) {
	    printf("%s\nfound to exist in %s\n",tmp_path,dumppath);
	  }
	  duplicates++;
	} 
	free (tmp_path);
      }
      
      if (duplicates) {
	if (duplicates >= limited_copies) {
	  printf("%s has too many duplicate files (%d) in %s\n",fname,duplicates,dumppath);
	  free(newpath);
	  return 1;
	} else {
	  // Now adjust path accordingly
	  char ext[2];
	  sprintf(ext,"%d",duplicates);
	  strcat(newpath,".");
	  strcat(newpath,ext);        
	  if (VERBOSE) {
	    printf("%d duplicate file(s) found for %s in %s\n",duplicates,fname,dumppath);
	  }
	}
      }
    }
  }

  if (rename(fpath, newpath) != 0) 
    if (VERBOSE) 
      printf("%s could not be moved to %s\nErrno = %d\n",fname,dumppath,errno);
      
  if (VERBOSE)
    printf("%s successfully renamed and moved to %s\n",fname,dumppath);
  free(newpath);
  return 0;
}

/* 
   Modified rm for files on a different device (partition) than the dumpster 
   Use fcopy() to complete move operation, adding extension if necessary
   Note if more than 9 instances of a file found, we must reject following instances
*/
int diff_dev_move(char *fname, char *fpath, char *dumppath, int VERBOSE) {
  
  // Path of file in dumpster
  char *newpath = dumppath;

  if (VERBOSE) {
    printf("Dumpster path: %s\n",newpath);
  }
    
  if (VERBOSE) {
    printf("Path of file in dumpster determined to be \n%s\nMoving file...\n", newpath);
  }
    
  // Check against null
  if (fpath == NULL || newpath == NULL) {
    printf("No path found for %s\nOperation aborted\n",fname);
    return -1;
  }
    
  // Check if duplicate files exist in dumpster
  int duplicates = 0;
  if (access(newpath,F_OK) != -1) {
    duplicates++;
    for (int i = 1; i < 10; i++) {
      // To avoid altering path, add extension to temp variable each iteration
      // Redeclare it to avoid [path].1.2.3, etc.
      char *tmp_path = (char *) malloc(PATH_MAX);
      memset(tmp_path, 0, strlen(tmp_path)); // Clear in between iterations
      strcat(tmp_path, newpath);
      char ext[2];
      sprintf(ext,"%d",i);
      strcat(tmp_path,".");
      if (access(strcat(tmp_path,ext),F_OK) != -1) {
	if (VERBOSE) {
	  printf(": %s\nfound to exist\n",tmp_path);
	}
	duplicates++;
      } 
      free (tmp_path);
    }
    
    if (duplicates) {
      if (duplicates >= 10) {
	printf("%s has too many duplicate files (%d) in dumpster\n",fname,duplicates);
	return 1;
      } else {
	// Now adjust path accordingly
	char ext[2];
	sprintf(ext,"%d",duplicates);
	strcat(newpath,".");
	strcat(newpath,ext); 	
	if (VERBOSE) {
	  printf("%d duplicate file(s) found for %s in dumpster\n",duplicates,fname);
	}	  
      }
    }
  }
  
  fcopy(fpath, newpath, VERBOSE); 
  printf("%s successfully copied and renamed and moved to dumpster\n", fname);
  return 0;
}


