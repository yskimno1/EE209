/*-------------------------------*/
/* EE209 Assignment 2            */
/* Student Name : Kim Yunseong   */
/* Student number : 20150183     */
/* Program Name : sgrep.c        */
/*-------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* for skeleton code */
#include <unistd.h> /* for getopt */
#include "str.h"

#define FIND_STR        "-f"
#define REPLACE_STR     "-r"
#define DIFF_STR        "-d"

#define MAX_STR_LEN 1023

#define FALSE 0
#define TRUE  1

typedef enum {
  INVALID,
  FIND,
  REPLACE,
  DIFF
} CommandType;


/*--------------------------------------------------------------------*/
/* PrintUsage()
   print out the usage of the Simple Grep Program                     */
/*--------------------------------------------------------------------*/
void 
PrintUsage(const char* argv0) 
{
  const static char *fmt = 
    "Simple Grep (sgrep) Usage:\n"
    "%s [COMMAND] [OPTIONS]...\n"
    "\nCOMMNAD\n"
    "\tFind: -f [search-string]\n"
    "\tReplace: -r [string1] [string2]\n"
    "\tDiff: -d [file1] [file2]\n";

  printf(fmt, argv0);
}
/*-------------------------------------------------------------------*/
/* DoFind()
   Your task:
   1. Do argument validation 
   - String or file argument length is no more than 1023
   - If you encounter a command-line argument that's too long, 
   print out "Error: argument is too long"
   
   2. Read the each line from standard input (stdin)
   - If you encounter a line larger than 1023 bytes, 
   print out "Error: input line is too long" 
   - Error message should be printed out to standard error (stderr)
   
   3. Check & print out the line contains a given string (search-string)
      
   Tips:
   - fgets() is an useful function to read characters from file. Note 
   that the fget() reads until newline or the end-of-file is reached. 
   - fprintf(sderr, ...) should be useful for printing out error
   message to standard error

   NOTE: If there is any problem, return FALSE; if not, return TRUE  */
/*-------------------------------------------------------------------*/
int
DoFind(const char *pcSearch)
{
  /*
    This function gets each line of file, and search the word
    which is parameter of this function.
    I used StrSearch function mainly.

    buf is the array which gets each line of the file input.    */
  char buf[MAX_STR_LEN + 2]; 
  int len;
  if(StrGetLength(pcSearch)> MAX_STR_LEN){
    fprintf(stderr, "Error: argument is too long\n");
    return FALSE;
  }
  /* Read the line by line from stdin, Note that this is an example */
  while (fgets(buf, sizeof(buf), stdin)) {
    /* check input line length */
    if ((len = StrGetLength(buf)) > MAX_STR_LEN) {
      fprintf(stderr, "Error: input line is too long\n");
      return FALSE;
    }
    /* TODO: fill out this function */
    if( StrSearch(buf, pcSearch) != NULL){
      fprintf(stdout,"%s", buf);
    }
  }
  return TRUE;
}
/*-------------------------------------------------------------------*/
/* DoReplace()
   Your task:
   1. Do argument validation 
      - String length is no more than 1023
      - If you encounter a command-line argument that's too long, 
        print out "Error: argument is too long"
      - If word1 is an empty string,
        print out "Error: Can't replace an empty substring"
      
   2. Read the each line from standard input (stdin)
      - If you encounter a line larger than 1023 bytes, 
        print out "Error: input line is too long" 
      - Error message should be printed out to standard error (stderr)

   3. Replace the string and print out the replaced string

   NOTE: If there is any problem, return FALSE; if not, return TRUE  */
/*-------------------------------------------------------------------*/
int
DoReplace(const char *pcString1, const char *pcString2)
{
  /* This function is replacing (changing) pcString1 into pcString2.
     It gets two parameters to know which we should change and which
     we should change into.
     I made one more array, which names buftemp.
     By comparing the location of pointer buf and pointer StrSearch,
     We can know where should we put pcString2 in buftemp
     and It prints the first location(pointer) of buftemp.       
     
     buf gets each line of the file.

     bufcopy is the pointer of buftemp array, and bufpoint is
     the pointer of the buf array.
     pcStringtemp is the pointer of pcString, to copy pcString2
     into buftemp.                                                */

  
  char buf[MAX_STR_LEN + 2]; 
  int len;
  char* pcDest;
  int i, j=0;
  if ( pcString1 == NULL){
    fprintf(stderr, "Error: Can't replace an empty substring\n");
    return FALSE;
  }
  if (StrGetLength(pcString2)>MAX_STR_LEN){
    fprintf(stderr, "Error: argument is too long\n");
    return FALSE;
  }
  
  while (fgets(buf, sizeof(buf), stdin)) {

    /* check input line length */
    if ((len = StrGetLength(buf)) > MAX_STR_LEN) {
      fprintf(stderr, "Error: input line is too long\n");
      return FALSE;
    }
    char buftemp[MAX_STR_LEN + 2] = {'\0'};
    char* bufcopy = buftemp;
    char* bufpoint = buf;
    
    if (StrSearch(buf, pcString1) == NULL) {
      fprintf(stdout, "%s", buf);
    }
    else{
      while((pcDest = StrSearch(bufpoint, pcString1)) != NULL){
	if (bufpoint == pcDest){
	  StrCopy(bufcopy, pcString2);
	  for(i=0; i<StrGetLength(pcString2); i++){
	    bufcopy++;
	  }
	  for(j=0; j<StrGetLength(pcString1); j++){
	    bufpoint++;
	  }
	}
	else{
	  *bufcopy = *bufpoint;
	  bufcopy++;
	  bufpoint++;
	}
      }
      while(*bufpoint!='\n' && *bufpoint!=EOF){
	*bufcopy = *bufpoint;
	bufcopy++;
	bufpoint++;
      }
      *bufcopy = '\n';
    
      fprintf(stdout, "%s", buftemp);
    }
  }
  return TRUE;
}
/*-------------------------------------------------------------------*/
/* DoDiff()
   Your task:
   1. Do argument validation 
     - file name length is no more than 1023
     - If a command-line argument is too long, 
       print out "Error: arugment is too long" to stderr

   2. Open the two files
      - The name of files are given by two parameters
      - If you fail to open either file, print out error messsage
      - Error message: "Error: failed to open file [filename]\n"
      - Error message should be printed out to stderr

   3. Read the each line from each file
      - If you encounter a line larger than 1023 bytes, 
        print out "Error: input line [filename] is too long" 
      - Error message should be printed out to stderr

   4. Compare the two files (file1, file2) line by line 

   5. Print out any different line with the following format
      file1@linenumber:file1's line
      file2@linenumber:file2's line

   6. If one of the files ends earlier than the other, print out an
      error message "Error: [filename] ends early at line XX", where
      XX is the final line number of [filename].

   NOTE: If there is any problem, return FALSE; if not, return TRUE  */
/*-------------------------------------------------------------------*/
int
DoDiff(const char *file1, const char *file2)
{
  /* First of all, this function gets 2 files by parameter.
     It checks whether they have same number of lines,
     and check two lines by using StrCompare
     if it is nonzero, it means that two lines are different.
     So, by counting the line, we can print out line and each
     different lines in stdout. 
     It doesn't compare first,
     "because we have to show only error message
     if there is some errors".                                 */

  if (StrGetLength(file1)>MAX_STR_LEN){
    fprintf(stderr, "Error: argument is too long\n");
    return FALSE;
  }
  if (StrGetLength(file2)>MAX_STR_LEN){
    fprintf(stderr, "Error: argument is too long\n");
    return FALSE;
  }  
  FILE* diff1;
  diff1 = fopen(file1, "r");
  if(diff1 == NULL){
    fprintf(stderr, "Error: failed to open file [%s]\n", file1);
    fclose(diff1);
    return FALSE;
  }
  FILE* diff2;
  diff2 = fopen(file2, "r");
  if(diff2 == NULL){
    fprintf(stderr, "Error: failed to open file [%s]\n", file2);
    fclose(diff1);
    fclose(diff2);
    return FALSE;
  }
  int count1=0;
  int count2=0;

  char firstget[MAX_STR_LEN+2];
  char secondget[MAX_STR_LEN+2];
  while(fgets(firstget, sizeof(firstget), diff1)){
    if(StrGetLength(firstget) > MAX_STR_LEN){
      fprintf(stderr,"Error: input line [%s] is too long\n", file1);
      fclose(diff1);
      fclose(diff2);
      return FALSE;
    }
    count1++;
  }
  while(fgets(secondget, sizeof(secondget), diff2)){
    if(StrGetLength(secondget) > MAX_STR_LEN){
      fprintf(stderr,"Error: input line [%s] is too long\n", file2);
      fclose(diff1);
      fclose(diff2);
      return FALSE;
    }
    count2++;
  }
  if(count1 > count2){
    fprintf(stderr,"Error: [%s] ends early at ", file2);
    fprintf(stderr, "line %d, where %d is the ", count2, count1);
    fprintf(stderr, "final line number of [%s].\n", file1);	
    fclose(diff1);
    fclose(diff2);
    return FALSE;
  }
  else if(count1 < count2){
    fprintf(stderr,"Error: [%s] ends early at ", file1);
    fprintf(stderr, "line %d, where %d is the ", count1, count2);
    fprintf(stderr, "final line number of [%s].\n", file2);	
    fclose(diff1);
    fclose(diff2);
    return FALSE;
  }
  fclose(diff1);
  fclose(diff2);

  diff1 = fopen(file1, "r");

  diff2 = fopen(file2, "r");
  
  int count = 0;
  while(fgets(firstget, sizeof(firstget), diff1)){
    fgets(secondget, sizeof(secondget), diff2);
    count++;
    if(StrCompare(firstget, secondget)!=0){
      fprintf(stdout, "%s@%d:%s", file1, count, firstget);
      fprintf(stdout, "%s@%d:%s", file2, count, secondget);
    }
  }
  fclose(diff1);
  fclose(diff2);
  return TRUE;
}
/*-------------------------------------------------------------------*/
/* CommandCheck() 
   - Parse the command and check number of argument. 
   - It returns the command type number
   - This function only checks number of argument. 
   - If the unknown function is given or the number of argument is 
   different from required number, this function returns FALSE.
   
   Note: You SHOULD check the argument rule later                    */
/*-------------------------------------------------------------------*/ 
int
CommandCheck(const int argc, const char *argv1)
{
  int cmdtype = INVALID;
   
  /* check minimum number of argument */
  if (argc < 3)
    return cmdtype;
   
  /* check command type */ 
  if (strcmp(argv1, FIND_STR) == 0) {
    if (argc != 3)
      return FALSE;    
    cmdtype = FIND;       
  }
  else if (strcmp(argv1, REPLACE_STR) == 0) {
    if (argc != 4)
      return FALSE;
    cmdtype = REPLACE;
  }
  else if (strcmp(argv1, DIFF_STR) == 0) {
    if (argc != 4)
      return FALSE;
    cmdtype = DIFF;
  }
   
  return cmdtype;
}
/*-------------------------------------------------------------------*/
int 
main(const int argc, const char *argv[]) 
{
  int type, ret;
   
  /* Do argument check and parsing */
  if (!(type = CommandCheck(argc, argv[1]))) {
    fprintf(stderr, "Error: argument parsing error\n");
    PrintUsage(argv[0]);
    return (EXIT_FAILURE);
  }
   
  /* Do appropriate job */
  switch (type) {
  case FIND:
    ret = DoFind(argv[2]);
    break;
  case REPLACE:
    ret = DoReplace(argv[2], argv[3]);
    break;
  case DIFF:
    ret = DoDiff(argv[2], argv[3]);
    break;
  } 

  return (ret)? EXIT_SUCCESS : EXIT_FAILURE;
}
