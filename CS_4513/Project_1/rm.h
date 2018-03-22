#ifndef RM_H_
#define RM_H_ 

/* Max file size we copy */
int FILE_MAX = 1024*1024; 
/* How far we recurse into subdirectories to find files */
int DEPTH_MAX = 3; 


/* Checks directory and, recursively, its sub-directories for a file up to a certain depth
   Note: Considers file extension (i.e., not basename(file)) when checking for match 
   Returns the absolute path of match if found, null otherwise
*/
char * in_dir(char *fname, char *dpath, char *orig_dir, int depth);
/* My own 'copy' function to move files from one partition to another 
   src should be a file path on another partition, while dest should point to
   a newly created file in dumpster.
   All permissions/accesstimes are preserved.
 */
int fcopy(char *src, char *dest);

int same_dev_move(char *fname, char *fpath, char *dumppath, int limited_copies);

#endif
