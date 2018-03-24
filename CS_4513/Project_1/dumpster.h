#ifndef ___DUMPSTER_H__
#define ___DUMPSTER_H__ "dumpster.h"

/* Removes last extension of string */
int remove_num(char* mystr);

/* Write to timing file */
void write_to_timings(char *metric, double measurement, char *ftime_name);

/* Checks directory and, recursively, its sub-directories for a file up to a certain depth
   Note: Considers file extension (i.e., not basename(file)) when checking for match 
   Returns the absolute path of match if found, null otherwise
*/
char * in_dir(char *fname, char *dpath, char *orig_dir, int depth, int VERBOSE);

/* Clears directory */
int clear_dir(char *dpath, char *orig_dir, int VERBOSE);

/* My own 'copy' function to move files from one partition to another 
   src should be a file path on another partition, while dest should point to
   a newly created file in dumpster.
   All permissions/accesstimes are preserved.
 */
int fcopy(char *src, char *dest, int VERBOSE, int timed);

int same_dev_move(char *fname, char *fpath, char *dumppath, int limited_copies, int VERBOSE, int timing);

/* 
   Modified rm for files on a different device (partition) than the dumpster 
   Use fcopy() to complete move operation, adding extension if necessary
   Note if more than 9 instances of a file found, we must reject following instances
*/
int diff_dev_move(char *fname, char *fpath, char *dumppath, int VERBOSE, int timed);

#endif
