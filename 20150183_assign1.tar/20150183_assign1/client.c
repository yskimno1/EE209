/* 20150183 Kim Yunseong */

/**********************
 * EE209 Assignment 1 *
 **********************/


/* client.c */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_NUM_NAME_CHARS 63
#define MAX_NUM_ID_CHARS 63
#define MAX_NUM_DIGITS   10

/*--------------------------------------------------------------------*/
/* Pre-defined error messages */
#define ERR_UNDEF_CMD   "ERROR: Undefined Command\n"
#define ERR_UNDEF_OPT   "ERROR: Undefined Option\n"
#define ERR_NEED_OPT    "ERROR: Need More Option\n"
#define ERR_SAME_OPT    "ERROR: Multiple Same Options\n"
#define ERR_AMBIG_ARG   "ERROR: Ambiguous Argument\n"
#define ERR_INVALID_ARG "ERROR: Invalid Option Argument\n"

/*--------------------------------------------------------------------*/
enum { FALSE = 0, TRUE };

typedef enum {
    C_EXIT,       /* exit */
    C_REG,        /* register */
    C_UNREG,      /* unregister */
    C_FIND,       /* find */
    C_FAIL,       /* failure */
    C_EOF         /* end of file */
} Command_T;



/*--------------------------------------------------------------------*/
/* Check whether an exit command valid.                               */
/* Input: no argument                                                 */
/* Return value: TRUE(valid) or FALSE(invalid)                        */
/*--------------------------------------------------------------------*/
static int
ValidateExitCommand(void)
{
    int c;

    /* only blank command is allowed */
    while ((c = getchar()) != EOF && c != '\n' && isspace(c))
        ;
    if (c == EOF)
        exit(0);
    if (c != '\n' && c != EOF) {
        fprintf(stderr, "%s", ERR_UNDEF_OPT);

        /* eat the line */
        while ((c = getchar()) != EOF && c != '\n')
            ;
        if (c == EOF)
            exit(0);
        return FALSE;
    }
    return TRUE;
}

static int
Validateid(int *dcount, int *validi, int *validn, int regornot)
{

  /* Validateid has some parameters.                        */
  /*   to check whether there are ID and Name already       */
  /*   because find and unreg should determine existance    */
  /*   of id and name. regornot means whether this function */
  /*   is called by Validateregister.                       */
     
  /*   Firstly, this function check the first, and check    */
  /*   later words by the rule, and returns whether this is */
  /*   valid or not.                                        */
  int idlength = 0;
  int c;
  int spacecount = 0;
  
  while((c=getchar()) != '\n' && isspace(c) && c != EOF){
    spacecount++;
  }
  while(TRUE){
    if (c =='\n'){
      fprintf(stderr, "%s", ERR_UNDEF_OPT);
      *dcount = 1;
      return FALSE;
    }
    else if (c == EOF){
      fprintf(stderr, "%s", ERR_UNDEF_OPT);
      exit(0); 
    }
    else{
      break;
    }
  }
  if(spacecount==0){
    fprintf(stderr, "%s", ERR_UNDEF_OPT);
    /* eat the line */
    while(TRUE){
      while ((c = getchar()) != EOF && c != '\n') ;
      if (c == EOF){
	exit(0);
      }
      else{ // c == '\n'
	*dcount = 1;
	return FALSE;
      }
    }    
  }
  // checking whether there is same option
  if(*validi != 0){
    fprintf(stderr, "%s", ERR_SAME_OPT);
    /* eat the line */
    while(TRUE){
      while ((c = getchar()) != EOF && c != '\n') ;
      if (c == EOF){
	exit(0);
      }
      else{ // c == '\n'
	*dcount = 1;
	return FALSE;
      }
    }
    return FALSE;
  }
  
  if (regornot==1 && *validn != 0){
    /* eat the line */
    while(TRUE){
      while ((c = getchar()) != EOF && c != '\n') ;
      if (c == EOF){
	fprintf(stderr, "%s", ERR_AMBIG_ARG);
	exit(0);
      }

      else{ // c == '\n'
	*dcount = 1;
	fprintf(stderr, "%s", ERR_AMBIG_ARG);
	return FALSE;
      }
    }
  }
  // first input already came.
  while(isalpha(c)||isdigit(c)||c=='-'||c=='_'||c=='.'||c=='\n'){
    idlength++;
    c = getchar();
    if (isalpha(c)==0 && isdigit(c)==0 && c!='-'&&c!='_'&& c!='.'){
      if (c == '\n'){
	if (idlength>63){
	  fprintf(stderr, "%s", ERR_INVALID_ARG);
	  *dcount = 1;
	  return FALSE;
	}
	else{
	  *dcount = 1;
	  return TRUE;
	}
      }
      else if (c == EOF){
	if (idlength>64){
	  fprintf(stderr, "%s", ERR_INVALID_ARG);
	  exit(0);
	}
	else{
	  exit(0);
	}
      }
      else if (isspace(c)){
	if (idlength>64){
	  fprintf(stderr, "%s", ERR_INVALID_ARG);
	  return FALSE;
	}
	else{
	  return TRUE;
	}	
      }
      else{
	/* eat the line */
	while(TRUE){
	  while ((c = getchar()) != EOF && c != '\n') ;
	  if (c == EOF){
	    fprintf(stderr, "%s", ERR_INVALID_ARG);
	    exit(0);
	  }
	  else{ // c == '\n'
	    *dcount = 1;
	    fprintf(stderr, "%s", ERR_INVALID_ARG);
	    return FALSE;
	  }
	}
      }
    }
  }
  /* eat the line */
  while(TRUE){
    while ((c = getchar()) != EOF && c != '\n') ;
    if (c == EOF){
      fprintf(stderr, "%s", ERR_INVALID_ARG);
      exit(0);
    }
    else{ // c == '\n'
      *dcount = 1;
      fprintf(stderr, "%s", ERR_INVALID_ARG);
      return FALSE;
    }
  }
}

static int
Validatename(int* dcount, int* validn, int* validi, int regornot)
{

  /*   Validateid has some parameters.                      */
  /*   to check whether there are ID and Name already       */
  /*   because find and unreg should determine existance    */
  /*   of id and name. regornot means whether this function */
  /*   is called by Validateregister.(similar to Validateid)*/
     
  /*   Firstly, this function check the first, and check    */
  /*   later words by the rule, because to determine single */
  /*   quote. Also, each if state determines backslash      */
  /*   character and each other characters                  */
  /*   it returns True or False, the validation result.     */
  int namelength = 0;
  int c;
  int spacecount = 0;
  
  while((c=getchar()) != '\n' && isspace(c) && c != EOF){
    spacecount++;
  }
  while(TRUE){
    if (c =='\n'){
      fprintf(stderr, "%s", ERR_UNDEF_OPT);
      *dcount = 1;
      return FALSE;
    }
    else if (c == EOF){
      fprintf(stderr, "%s", ERR_UNDEF_OPT);
      exit(0);
    } 
    else{
      break;
    }
  }
  if(spacecount==0){
    /* eat the line */
    while(TRUE){
      while ((c = getchar()) != EOF && c != '\n') ;
      if (c == EOF){
	fprintf(stderr, "%s", ERR_UNDEF_OPT);
	exit(0);
      }
      else{ // c == '\n'
	*dcount = 1;
	fprintf(stderr, "%s", ERR_UNDEF_OPT);
	return FALSE;
      }
    }    
  }
  if(*validn != 0){
    /* eat the line */
    while(TRUE){
      while ((c = getchar()) != EOF && c != '\n') ;
      if (c == EOF){
	fprintf(stderr, "%s", ERR_SAME_OPT);
	exit(0);
      }
      else{ // c == '\n'
	*dcount = 1;
	fprintf(stderr, "%s", ERR_SAME_OPT);
	return FALSE;
      }
    }
  }
  if (regornot==1 && *validi != 0){
    /* eat the line */
    while(TRUE){
      while ((c = getchar()) != EOF && c != '\n') ;
      if (c == EOF){
	*dcount = 1;
	fprintf(stderr, "%s", ERR_AMBIG_ARG);
	return FALSE;
      }
      else{ // c == '\n'
	*dcount = 1;
	fprintf(stderr, "%s", ERR_AMBIG_ARG);
	return FALSE;
      }
    }
  }
  int squotecount = 0;
  // first input already came.
  if (c == '\''){ // it is special case
    c = getchar();
    while(TRUE){
      if(isalpha(c)||c==' '||c=='-'||c=='.'||c=='\\'||c=='\''||c=='\n'){
	if(squotecount!=0  && c!='\n'){
	  /* eat the line */
	  while(TRUE){
	    while ((c = getchar()) != EOF && c != '\n') ;
	    if (c == EOF){
	      fprintf(stderr, "%s", ERR_INVALID_ARG);
	      exit(0);
	    }
	    else{ // c == '\n'
	      *dcount = 1;
	      fprintf(stderr, "%s", ERR_INVALID_ARG);
	      return FALSE;
	    }
	  }
	}

	if(c=='\\'){
	  c = getchar();
	  if(isalpha(c)||c==' '||c=='-'||c=='.'||c=='\''){
	    namelength++;
	    c = getchar();
	    continue;
	  }
	  else{ // invalid argument
	    /* eat the line */
	    while(TRUE){
	      while ((c = getchar()) != EOF && c != '\n') ;
	      if (c == EOF){
		fprintf(stderr, "%s", ERR_INVALID_ARG);
		exit(0);
	      }	
	      else{ // c == '\n'
		*dcount = 1;
		fprintf(stderr, "%s", ERR_INVALID_ARG);
		return FALSE;
	      }
	    }
	  }
	}
	else if(c=='\''){
	  squotecount++;
	  c = getchar();
	  continue;
	}
	else if(c=='\n'){
	  if((squotecount==1)&&(namelength<64)&&(namelength>0)){
	    *dcount = 1;
	    return TRUE;
	  }
	  else{
	    fprintf(stderr, "%s", ERR_INVALID_ARG);
	    *dcount = 1;
	    return FALSE;
	  }
	}
	else{
	  namelength++;
	  c = getchar();
	  continue;
	}
      }
      else if(c==EOF){
	if((squotecount==1)&&(namelength<64)&&(namelength>0)){
	  exit(0);
	}
	else{
	  fprintf(stderr, "%s", ERR_INVALID_ARG);
	  exit(0);
	}
      }
      else{
	/* eat the line */
	while(TRUE){
	  while ((c = getchar()) != EOF && c != '\n') ;
	  if (c == EOF){
	    fprintf(stderr, "%s", ERR_INVALID_ARG);
	    exit(0);
	  }
	  else{ // c == '\n'
	    *dcount = 1;
	    fprintf(stderr, "%s", ERR_INVALID_ARG);
	    return FALSE;
	  }
	}
      }
    }
  }
  else { // not '\n', not EOF, not '\''
    while(TRUE){
      if (c == '\\'){
	c = getchar();
	if (isalpha(c)||c==' '||c=='-'||c=='.'||c=='\''){
	  namelength++;
	  c = getchar();
	  continue;
	}
	else{ // invalid argument
	  /* eat the line */
	  while(TRUE){
	    while ((c = getchar()) != EOF && c != '\n') ;
	    if (c == EOF){
	      fprintf(stderr, "%s", ERR_INVALID_ARG);
	      exit(0);
	    }
	    else{ // c == '\n'
	      *dcount = 1;
	      fprintf(stderr, "%s", ERR_INVALID_ARG);
	      return FALSE;
	    }
	  }
	}
      }
      else if (isalpha(c)||c=='-'||c=='.'|| c=='\''){
	namelength++;
	c = getchar();
	continue;
      }
      else if (c=='\n'){
	if((namelength<64) && (namelength>0)){
	  *dcount = 1;
	  return TRUE;
	}
	else{
	  fprintf(stderr, "%s", ERR_INVALID_ARG);
	  *dcount = 1;
	  return FALSE;
	}
      }
      else if (c == EOF){
	if((namelength<64)&&(namelength>0)){
	  exit(0);
	}
	else{
	  fprintf(stderr, "%s", ERR_INVALID_ARG);
	  exit(0);
	}
      }
      else if(isspace(c)){
        if((namelength<64)&&(namelength>0)){
	  return TRUE;
	}
	else{
	  fprintf(stderr, "%s", ERR_INVALID_ARG);
	  return FALSE;
	}
      }
      else{ // invalid argument
	/* eat the line */
	while(TRUE){
	  while ((c = getchar()) != EOF && c != '\n') ;
	  if (c == EOF){
	    fprintf(stderr, "%s", ERR_INVALID_ARG);
	    exit(0);
	  }
	  else{ // c == '\n'
	    *dcount = 1;
	    fprintf(stderr, "%s", ERR_INVALID_ARG);
	    return FALSE;
	  }
	}
      }
    }
  }
  return TRUE;
}
static int
Validatepurch(int* dcount, int* validp)
{
  /* This function is different with name and id.    */
  /* Because find and unreg does not call this       */
  /* function, it has only two parameters, to        */
  /* determine whether purchase is already called.   */

  /* it also checks first word because 0 character   */
  /* could not be at first place, unless purchase    */
  /* value is 0. It returns the validation result.   */
  int purchlength = 0;
  int c;
  int spacecount = 0;
  while((c=getchar()) != '\n' && isspace(c) && c != EOF){
    spacecount++;
  }
  while(TRUE){
    if (c =='\n'){
      fprintf(stderr, "%s", ERR_UNDEF_OPT);
      *dcount = 1;
      return FALSE;
    }
    else if (c == EOF){
      fprintf(stderr, "%s", ERR_UNDEF_OPT);
      exit(0);
    }
    else{
      break;
    }
  }
  if(spacecount==0){
    /* eat the line */
    while(TRUE){
      while ((c = getchar()) != EOF && c != '\n') ;
      if (c == EOF){
	fprintf(stderr, "%s", ERR_UNDEF_OPT);
	exit(0);
      }
      else{ // c == '\n'
	*dcount = 1;
	fprintf(stderr, "%s", ERR_UNDEF_OPT);
	return FALSE;
      }
    }
  }
  //checking whether there is a same option
  if(*validp != 0){
    /* eat the line */
    while(TRUE){
      while ((c = getchar()) != EOF && c != '\n') ;
      if (c == EOF){
	fprintf(stderr, "%s", ERR_SAME_OPT);
	exit(0);
      }
      else{ // c == '\n'
	*dcount = 1;
	fprintf(stderr, "%s", ERR_SAME_OPT);
	return FALSE;
      }
    }
  }
  //first letter already input.
  if(c=='0' || isdigit(c)==0 ){
    if(c=='0'){
      while(TRUE){
	c = getchar();
	if (c=='\n'){
	  *dcount = 1;
	  return TRUE;
	}
	else if (c==EOF){
	  return TRUE;
	  exit(0);
	}
	else if(isspace(c)){
	  return TRUE;
	}
	else{
	  break;
	}
      }
    }
    /* eat the line */
    while(TRUE){
      while ((c = getchar()) != EOF && c != '\n') ;
      if (c == EOF){
	fprintf(stderr, "%s", ERR_INVALID_ARG);
	exit(0);
      }
      else{ // c == '\n'
	*dcount = 1;
	fprintf(stderr, "%s", ERR_INVALID_ARG);
	return FALSE;
      }
    }
  }

  
  while (isdigit(c)){
    purchlength++;
    c = getchar();
    if(!(isdigit(c))){
      if(c == '\n'){
	if (purchlength > 10){
	  fprintf(stderr, "%s", ERR_INVALID_ARG);
	  *dcount = 1;
	  return FALSE;
	}
	else{
	  *dcount = 1;
	  return TRUE;
	}
      }
      else if (c == EOF){
	if (purchlength > 10){
	  fprintf(stderr, "%s", ERR_INVALID_ARG);
	  exit(0);
	}
	else{
	  exit(0);
	}
      }
      else if (isspace(c)){
	if (purchlength > 10){
	  fprintf(stderr, "%s", ERR_INVALID_ARG);
	  return FALSE;
	}
	else{
	  return TRUE;
	}
      }
      else{
	/* eat the line */
	while(TRUE){
	  while ((c = getchar()) != EOF && c != '\n') ;
	  if (c == EOF){
	    fprintf(stderr, "%s", ERR_INVALID_ARG);
	      exit(0);
	  }
	  else{ // c == '\n'
	    *dcount = 1;
	    fprintf(stderr, "%s", ERR_INVALID_ARG);
	    return FALSE;
	  }
	}
      }
    }
  }
  /* eat the line */
  while(TRUE){
    while ((c = getchar()) != EOF && c != '\n') ;
    if (c == EOF){
      fprintf(stderr, "%s", ERR_INVALID_ARG);
      exit(0);
    }
    else{ // c == '\n'
      *dcount = 1;
      fprintf(stderr, "%s", ERR_INVALID_ARG);
      return FALSE;
    }
  }
}


/*--------------------------------------------------------------------*/
/* Check whether a reg(register) command valid.                       */
/* Input: no argument                                                 */
/* Return value: TRUE(valid) or FALSE(invalid)                        */
/*--------------------------------------------------------------------*/


static int
ValidateRegisterCommand(void)
{
    /* TODO: Implement this function. */
/*--------------------------------------------------------------------*/
/* it checks the letter which comes after '-' character               */
/* and call Validateid, name, or purchase.                            */
/* dcount means that whether user input '\n', to determine            */
/* time to return True or False                                       */  
/*--------------------------------------------------------------------*/
  int validi = 0;
  int validn = 0;
  int validp = 0;
  int dcount = 0; // donecount
  int c;
  
  while (dcount == 0){
    /* eat the space*/
    while((c=getchar()) != '\n' && isspace(c) && c != EOF) ;
    
    if (c == '\n'){
      if(validi==1 && validn==1 && validp==1){
	return TRUE;
      }
      fprintf(stderr, "%s", ERR_NEED_OPT);
      return FALSE;
    }
    else if (c == EOF){
      fprintf(stderr, "%s", ERR_NEED_OPT);
      exit(0);
    }
    else if (c != '-'){
      /* eat the line */
      while(TRUE){
	while ((c = getchar()) != EOF && c != '\n') ;
	if (c == EOF){
	  fprintf(stderr, "%s", ERR_UNDEF_OPT);
	  exit(0);
	}
	else{ // c == '\n'
	  fprintf(stderr, "%s", ERR_UNDEF_OPT);
	  return FALSE;
	}
      }
      if (c == EOF)
	fprintf(stderr, "%s", ERR_UNDEF_OPT);
	exit(0);
      return FALSE;
    }
    /* get letter */
    c = getchar();
    switch (c){
      case 'i':
	if (Validateid(&dcount, &validi, &validn, 0) == FALSE){
	  return FALSE;
	}
	else{
	  validi++;
	  break;
	}
      case 'p':
	if (Validatepurch(&dcount, &validp) == FALSE){
	  return FALSE;
	}
	else{
	  validp++;
	  break;
	}
      case 'n':
	if (Validatename(&dcount, &validn, &validi, 0) == FALSE){
	  return FALSE;
	}
	else{
	  validn++;
	  break;
	}
      case '\n':
	fprintf(stderr, "%s", ERR_UNDEF_OPT);
	return FALSE;
      case EOF:
	fprintf(stderr, "%s", ERR_UNDEF_OPT);
	exit(0);
      default:
	/* eat the line */
	while(TRUE){
	  while ((c = getchar()) != EOF && c != '\n') ;
	  if (c == EOF){
	    exit(0);
	    fprintf(stderr, "%s", ERR_UNDEF_OPT);
	  }
	  else{ // c == '\n'
	    dcount++;
	    fprintf(stderr, "%s", ERR_UNDEF_OPT);
	    return FALSE;
	  }
	}
    }
  }
  
  if(validi==0 || validp==0 || validn==0){
    fprintf(stderr, "%s", ERR_NEED_OPT);
    return FALSE;
  }
  
  return TRUE;
}
/*--------------------------------------------------------------------*/
/* Check whether an unreg(unregister) or a find(search) command valid.*/
/* Input: no argument                                                 */
/* Return value: TRUE(valid) or FALSE(invalid)                        */
/*--------------------------------------------------------------------*/
static int
ValidateUnregisterOrFindCommand(void)
{

/*--------------------------------------------------------------------*/
/* The only difference between this function and ValidateRegister is  */
/* that there is only 1 option in this function                       */
/* Because of this, the escape condition is different, it escapes     */
/* when only i or n is valid (only 1 thing among id and name)         */
/*--------------------------------------------------------------------*/
  int validi = 0;
  int validn = 0;
  int dcount = 0; // donecount

  int c;
  
  while (dcount == 0){
    /* eat the space*/
    while((c=getchar()) != '\n' && isspace(c) && c != EOF) ;

    if (c == '\n'){
      if(validi==1 || validn==1){
	return TRUE;
      }
      fprintf(stderr, "%s", ERR_NEED_OPT);
      return FALSE;
    }
    else if (c == EOF){
      fprintf(stderr, "%s", ERR_NEED_OPT);
      exit(0);
    }
    else if (c != '-'){
      /* eat the line */
      while(TRUE){
	while ((c = getchar()) != EOF && c != '\n') ;
	if (c == EOF){
	  fprintf(stderr, "%s", ERR_UNDEF_OPT);
	  exit(0);
	}
	else{ // c == '\n'
	  fprintf(stderr, "%s", ERR_UNDEF_OPT);
	  return FALSE;
	}
      }
    }
    /* get letter */
    c = getchar();
    switch (c){
      case 'i':
	if (Validateid(&dcount, &validi, &validn, 1) == FALSE){
	  return FALSE;
	}
	else{
	  validi++;
	  break;
	}
      case 'n':
	if (Validatename(&dcount, &validn, &validi, 1) == FALSE){
	  return FALSE;
	}
	else{
	  validn++;
	  break;
	}
      case '\n':
	fprintf(stderr, "%s", ERR_UNDEF_OPT);
	return FALSE;
      case EOF:
	fprintf(stderr, "%s", ERR_UNDEF_OPT);
	exit(0);
      default:
	/* eat the line */
	while(TRUE){
	  while ((c = getchar()) != EOF && c != '\n') ;
	  if (c == EOF){
	    fprintf(stderr, "%s", ERR_UNDEF_OPT);
	    exit(0);
	  }
	  else{ // c == '\n'
	    fprintf(stderr, "%s", ERR_UNDEF_OPT);
	    return FALSE;
	  }
	}
    }
  }
  return TRUE;
}
/*--------------------------------------------------------------------*/
/* Read the first word, and figure out and return the command type.   */
/* Input: no argument                                                 */
/* Return value: Command_T value                                      */
/*  - In case of an error, it eats the entire line and returns C_FAIL */
/*  - In case there's no more input (EOF), it returns C_EOF           */
/*--------------------------------------------------------------------*/
static Command_T
GetCommandType(void)
{
    Command_T type = C_FAIL;
    const char *cmds[] = {
        "exit",   /* exit */
        "reg",    /* reg */
        "unreg",  /* unreg */
        "find",   /* find */
    };
    int i, len;
    int c;
     
    /* eat space */
    while ((c = getchar()) != EOF && c != '\n' && isspace(c))
        ;

    switch (c) {
        case '\n':return C_FAIL;  /* no command */
        case EOF: return C_EOF;   /* no more input */
        case 'e': type = C_EXIT;  break;
        case 'r': type = C_REG;   break;
        case 'u': type = C_UNREG; break;
        case 'f': type = C_FIND;  break;
        default:
            fprintf(stderr, "%s", ERR_UNDEF_CMD);
            goto EatLineAndReturn;
    }
     
    /* see the rest of the command chars actually match */
    len = strlen(cmds[type]);
    for (i = 1; i < len; i++) {
        c = getchar();
        if (c == '\n' || c == EOF) {   /* line finished too early */
            fprintf(stderr, "%s", ERR_UNDEF_CMD);
            return (c == EOF) ? C_EOF : C_FAIL;
        }
        if (c != cmds[type][i]) {    /* wrong command */
            fprintf(stderr, "%s", ERR_UNDEF_CMD);
            goto EatLineAndReturn;
        }
    }

    /* check the following character of a command */
    c = getchar();
    if (c != '\n' && isspace(c)) {
        return type;
    } else if (c == '\n' || c == EOF) {
        /* only exit can be followed by '\n' */
        if (type != C_EXIT)
            fprintf(stderr, "%s", ERR_NEED_OPT);
        if (c == EOF)
            return C_EOF;
        else
            ungetc(c, stdin);
        if (type == C_EXIT)
            return type;
    } else {
        fprintf(stderr, "%s", ERR_UNDEF_CMD);
    }
    
EatLineAndReturn:
    while ((c = getchar()) != EOF && (c != '\n'))
        ;
    return (c == EOF) ? C_EOF : C_FAIL;
}
/*--------------------------------------------------------------------*/
int
main(int argc, const char *argv[])
{
    Command_T command;
    int res;
    printf("======================================\n" \
           "  Customer Manager Program\n" \
           "======================================\n\n");

    /* start prompt */
    while (TRUE) {
        printf("\n> ");

        /* check command type */
        command = GetCommandType();

        /* command validation */
        switch (command) {
            case C_EOF:
                return 0;
            case C_FAIL:
                res = FALSE;
                break;
            case C_EXIT:
                res = ValidateExitCommand();
                break;
            case C_REG:
                res = ValidateRegisterCommand();
                break;
            case C_FIND:
                res = ValidateUnregisterOrFindCommand();
                break;
            case C_UNREG:
                res = ValidateUnregisterOrFindCommand();
                break;
            default:
                assert(0); /* can't reach here */
                break;
        }

        if (res == FALSE) {
            /* validation fail */
            continue;
        }

        /* command functionalities */
        switch (command) {
            case C_EXIT:
                exit(0);
                return 0;
            default:
                /* This will be expanded in assignment 3. */
                break;
        }
    }
    assert(0);  /* can't reach here */
    return 0;
}
