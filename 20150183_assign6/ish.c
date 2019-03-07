/* Name : Kim Yunseong
   ID : 20150183
   
   Assignment 6        */
#define _POSIX_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <wait.h>
#include <dirent.h>
#include <signal.h>
#include <fcntl.h>

#include "dynarray.h"


enum {MAX_INPUT_LENGTH = 1024};
enum {FALSE = 0, TRUE};
enum {START=0, IN_WORD, IN_QUOTE, ERROR};

int sig_count = 0;
int value = 0;

/* Handler Function */
static void mysigquitHandler(int iSig){
  alarm(5);
  sig_count++;
  if(sig_count - value == 2) exit(0);
  fprintf(stdout,"Type Ctrl-\\ again within 5 seconds to exit\n");
  fflush(stdout);
}

static void myHandler() {
  value++;
}
/* Reset Array
   reset the all arraies NULL
 */
void resetArray(char array[]){
  int i;
  for(i=0; i<MAX_INPUT_LENGTH; i++){
    array[i] = '\0';
  }
}
/*-----------------------------------------------------------
void Execution_stage
This function is called by do_lex function.
This function do all of works in execution stage.
It returns nothing, and its parameters are Dynamic Array and
specialState, which determines |, <, > the filename or special
characters.

-------------------------------------------------------------*/
void Execution_stage(DynArray_T oDynArray, int* specialState){
  //  const char* format = "./";
  int i=0;
  pid_t pid;
  int status;
  void (*pfRet)(int);
  
  while(i<oDynArray->iLength){
    int j=0;
    char** argv = (char **)calloc(oDynArray->iLength+1, sizeof(char*));  

    for( ; i<oDynArray->iLength && specialState[i]!=1; i++){
      argv[j] = oDynArray->ppvArray[i];
      j++;
    }
    /* check built-in functions */
    if(strcmp("cd", argv[0])==0){
      char* path;
      if(argv[1]==NULL){
	path = (char *)(getenv("HOME"));
      }
      else{
	path = argv[1];
      }

      int result =  chdir(path);
      if(result<0){
	fprintf(stderr, "./ish: Not a directory\n");
      }
      free(argv);
    }
    else if(strcmp("setenv", argv[0])==0){
      if(argv[2]==NULL){
	if(argv[1]==NULL){
	  fprintf(stderr, "./ish : need more parameter\n");
	  free(argv);
	}
	else{
	  setenv(argv[1], "", 1);
	  free(argv);
	}
      }
      else{
	setenv(argv[1], argv[2], 1);
	free(argv);
      }
    }
    else if(strcmp("unsetenv", argv[0])==0){
      if(argv[1]==NULL){
	fprintf(stderr, "./ish: unsetenv takes one parameter\n");
	free(argv);
      }
      else{
	unsetenv(argv[1]);
	free(argv);
      }
    }
    else if(strcmp("exit", argv[0])==0){
      free(argv);
      exit(0);
    }
    else{ // not built-in, now, check pipe and redirections.
      if(specialState[i]>=1){ // if we should redirect
	if(strcmp(oDynArray->ppvArray[i], "|")==0){
	  //pipe
	  int pid, fds[2];
	  if (pipe(fds) == -1) exit(-1);

	  pid = fork();
	  if(pid == 0){ // child
	    pfRet = signal(SIGINT, SIG_DFL);
	    assert(pfRet != SIG_ERR); // go to default action

	    close(fds[1]);
	    dup2(fds[0], 0);
	    close(fds[0]);
  
	    execvp(argv[0], argv);
	    fprintf(stderr, "%s: No such file or directory\n", argv[0]);
	    
	    exit(0);
	  }
	  else{ // parent
	    close(fds[0]);
	    dup2(fds[1], 1);
	    close(fds[1]);
	    
	    wait(&status);
	    i++;
	    free(argv);
	    continue;
	  }
	}
	else if(strcmp(oDynArray->ppvArray[i], "<")==0){
	  char** stdinargv =
	    (char **)calloc(2, sizeof(char*));  
	  i++;
	  while(specialState[i]==0 && i<oDynArray->iLength){
	    stdinargv[0] = argv[0];
	    stdinargv[1] = oDynArray->ppvArray[i];
	    i++;
	  
	    pid = fork();
	    if(pid == 0){ // child
	      pfRet = signal(SIGINT, SIG_DFL);
	      assert(pfRet != SIG_ERR); // go to default action
	    
	      execvp(stdinargv[0], stdinargv);
	      fprintf(stderr, "%s: %s: No such file or directory\n",
		      stdinargv[0], stdinargv[1]);
	      exit(0); //cannot reach here
	    }
	    else{
	      wait(&status);
	      continue;
	    }
	  }
	  free(argv);
	  free(stdinargv);
	  continue;
	}
	else if(strcmp(oDynArray->ppvArray[i], ">")==0){
	  int fd;
	  pid = fork();
	  if(pid == 0){
	    fd = creat(oDynArray->ppvArray[i+1], 0600);
	    close(1);
	    dup(fd);
	    close(fd);
	    execvp(argv[0], argv);
	    exit(0); //cannot reach here
	  }
	  else{
	    wait(&status);
	    i++;
	    i++;
	    free(argv);
	    continue;
	  }
	}
	assert(0);
      }
      else{ // don't need to redirect
	fflush(stdout);
	pid = fork();
	if(pid == -1){ //fail
	  fprintf(stderr, "Fork failed\n");
	  assert(0);
	}
	else if(pid == 0){//child
	  pfRet = signal(SIGINT, SIG_DFL);
	  assert(pfRet != SIG_ERR); // go to default action
	
	  execvp(argv[0], argv);
	  fprintf(stderr, "%s: No such file or directory\n", argv[0]);
	  exit(0);
	}
	// in parent
	pid = wait(&status);
	free(argv);
      }
    }
  }
}
/*do_lex
  This function does all lexical analysis.
  It is done by input arguments and input lines.
  It returns 1 if success, 0 if fail.
  (actually, return value doesn't matter)
 */
int do_lex(char* buff, int argc, char *argv[]){ // similar to main
  if(strcmp(buff, "\n")==0){ // special case if buff is '\n'
    return 1;
  }
  
  
  DynArray_T oDynArray = DynArray_new(0);
  assert(oDynArray!=NULL);
    
  int len = strlen(buff);
  char tokentemp[MAX_INPUT_LENGTH] = {'\0'}; // temporary putting letters
  char* tokentempptr = tokentemp;
  int specialState[MAX_INPUT_LENGTH] = {0}; //check the state of token
  //whether that is filename or special character

  int specialindex = 0;
  int state = START;
  int i=0;
  char c;

  for(i=0; i<len; i++){
    c = buff[i];
    if(isspace(c)){
      if(state==START) continue;
      else if(state==IN_WORD){
	DynArray_add(oDynArray, strdup(tokentemp));
	resetArray(tokentemp);
	specialindex++;
	tokentempptr = tokentemp;
	state = START;
	continue;
      }
      else if(state == IN_QUOTE){
	*tokentempptr = c;
	tokentempptr++;
      }
    }
    else if(c=='|' || c=='<' || c=='>'){
      if(state == START){
	*tokentempptr = c;
	DynArray_add(oDynArray, strdup(tokentemp));
	resetArray(tokentemp);
	specialState[specialindex] = 1;
	specialindex++;
	tokentempptr = tokentemp;
	//keep state START
      }
      else if(state == IN_QUOTE){
	*tokentempptr = c;
	tokentempptr++;
      }
      else if(state == IN_WORD){
	DynArray_add(oDynArray, strdup(tokentemp));
	resetArray(tokentemp);
	tokentempptr = tokentemp;
	specialindex++;
	  
	*tokentempptr = c;
	DynArray_add(oDynArray, strdup(tokentemp));
	resetArray(tokentemp);
	specialState[specialindex] = 1;
	specialindex++;
	  
	tokentempptr = tokentemp;
	state = START;
      }
    }
    else if(c=='\"'){
      if(state == START || state == IN_WORD){
	state = IN_QUOTE;
      }
      else if(state == IN_QUOTE){
	state = IN_WORD;
      }
    }
    else{ // valuable letter
      if(state == IN_WORD || state == IN_QUOTE){
	*tokentempptr = c;
	tokentempptr++;
      }
      else if(state == START){
	state = IN_WORD;
	*tokentempptr = c;
	tokentempptr++;
      }
    }  
  }
  if(state==IN_QUOTE){
    fprintf(stderr, "%s: Could not find quote pair\n", argv[0]);
    state = ERROR;
    DynArray_add(oDynArray, strdup(tokentemp));
    resetArray(tokentemp);
    tokentempptr = tokentemp;
    DynArray_free(oDynArray);
    return 0;
  }
  if(state!=START){
    DynArray_add(oDynArray, strdup(tokentemp));
    resetArray(tokentemp);
    tokentempptr = tokentemp;
  }
  /*--------------------------------------------------------*/
  if(strcmp(oDynArray->ppvArray[0], "exit")==0){
    if(oDynArray->iLength >1){
      fprintf(stderr,
	      "%s: exit does not take any parameters\n", argv[0]);
      DynArray_free(oDynArray);
      return 0;
    }
    else{
      fprintf(stdout, "\n");
      exit(0);
    }
  }

  int pipestate=0;
  int stdinstate=0;
  int stdoutstate = 0;
  int breakcount = 0;
  for(i=0; i<oDynArray->iLength; i++){
    if(specialState[i]==1){
      if(i==0){
	fprintf(stderr,
		"%s: Missing command name\n",
		argv[0]);
	breakcount++;
	break;
      }
      else if(i==oDynArray->iLength -1){
	fprintf(stderr,
		"%s: Pipe or redirection destination not specified\n",
		argv[0]);
	breakcount++;
	break;	  
      }
      else{//i>0 && i<iLength-1 (not at the first and last)	  
	if( (specialState[i-1]!=0)||(specialState[i+1])!=0){
	  fprintf(stderr,
		  "%s: Pipe or redirection destination not specified\n",
		  argv[0]);
	  breakcount++;
	  break;	    
	}
      }
	
      if(strcmp(oDynArray->ppvArray[i], "|")==0){
	if(stdoutstate!=0){
	  fprintf(stderr,
		  "%s: Multiple redirection of standard output\n",
		  argv[0]);
	  breakcount++;
	  break;
	}
	pipestate=1;
      }
      else if(strcmp(oDynArray->ppvArray[i], ">")==0){
	if(stdoutstate!=0){
	  fprintf(stderr,
		  "%s: Multiple redirection of standard output\n",
		  argv[0]);
	  breakcount++;
	  break;
	}
	stdoutstate++;
      }
      else if(strcmp(oDynArray->ppvArray[i], "<")==0){
	if(pipestate!=0 || stdinstate!=0){
	  fprintf(stderr,
		  "%s: Multiple redirection of standard input\n",
		  argv[0]);
	  breakcount++;
	  break;
	}
	stdinstate++;
      }
      else{
	fprintf(stderr, "cannot reach here\n");
	assert(0);
      }
    }
  }
  
  if(breakcount==1) return 0;
  Execution_stage(oDynArray, specialState);
  DynArray_free(oDynArray);    
  return 1;
}


/*----------------------------------------------------------
main function
make signals available, 
Main function calls do_lex, and
do_lex calls execution_stage.
----------------------------------------------------------*/
int main(int argc, char *argv[]){
  char* home = strdup(getenv("HOME"));
  sigset_t sSet;
  
  sigemptyset(&sSet);
  sigaddset(&sSet, SIGINT);
  sigaddset(&sSet, SIGQUIT);
  sigaddset(&sSet, SIGALRM);
  sigprocmask(SIG_UNBLOCK, &sSet, NULL);

  void (*pfRet)(int); 
  pfRet = signal(SIGINT, SIG_IGN);
  assert(pfRet != SIG_ERR);
  
  pfRet = signal(SIGQUIT, mysigquitHandler);
  assert(pfRet != SIG_ERR); // ignoring SIG_INT
  signal(SIGALRM, myHandler);
  // get the ishrc 
  FILE* fp;
  char line[MAX_INPUT_LENGTH];
  chdir(home);
  fp = fopen(strcat(home, "/.ishrc"), "r");
  if(fp!=NULL){
    fgets(line, MAX_INPUT_LENGTH, fp);
    while(*line != EOF){
      do_lex(line, argc, argv);
      fgets(line, MAX_INPUT_LENGTH, fp);
    }
    fclose(fp);
  }
  
  while(1){
    fprintf(stdout, "%% ");
    fflush(stdout);
    char buff[MAX_INPUT_LENGTH] = {'\0'};
    char* check = fgets(buff, MAX_INPUT_LENGTH, stdin);
    if(check==NULL) exit(0);

    if(buff[MAX_INPUT_LENGTH-1] != '\0'){
      // buff[1023] should be '\0'
      fprintf(stderr, "%s : File Name Too long\n", argv[0]);
      continue;
    }    // input is in buff now    
    do_lex(buff, argc, argv);
    
  }

  return 1;
}
