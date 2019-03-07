#include <assert.h> /* to use assert() */
#include <stdio.h>
#include "str.h"

/*-----------------------------*/
/* EE209 Assignment 2          */
/* Student name : Kim Yunseong */
/* Student number : 20150183   */
/* file name : str.c           */
/*-----------------------------*/

/* Your task is: 
   1. Rewrite the body of "Part 1" functions - remove the current
      body that simply calls the corresponding C standard library
      function.
   2. Write appropriate comment per each function
*/

/* Part 1 */
/*------------------------------------------------------------------------*/
size_t StrGetLength(const char* pcSrc)
{
  const char *pcEnd;
  assert(pcSrc != NULL); /* NULL address, 0, and FALSE are identical. */
  pcEnd = pcSrc;
	
  while (*pcEnd) /* null character and FALSE are identical. */
    pcEnd++;

  return (size_t)(pcEnd - pcSrc);
}

/*------------------------------------------------------------------------*/
char *StrCopy(char *pcDest, const char* pcSrc)
{
  /* parameters : destination pointer and Source to copy
     It copies pcSrc into pcDest, and returns the first
     location of the pointer, which I named pcDeststart. */
  assert(pcSrc != NULL);
  assert(pcDest != NULL);
  char* pcDeststart;
  pcDeststart = pcDest;
  size_t SrcLength = StrGetLength(pcSrc);
  size_t i=0;

  for(i=0; i<SrcLength; i++){
    *pcDest = *pcSrc;
    pcSrc++;
    pcDest++;
  }
  *pcDest = '\0';
  return pcDeststart;
}

/*------------------------------------------------------------------------*/
int StrCompare(const char* pcS1, const char* pcS2)
{
  /* parameters : two sources to compare
     It compares two sources by pointer.
     return value is 0 if two of them are same,
     positive if *pcS2>*pcS1
     else, negative.                            */
  assert(pcS1 != NULL);
  assert(pcS2 != NULL);
  size_t S1length = StrGetLength(pcS1);
  size_t S2length = StrGetLength(pcS2);

  size_t i=0;

  
  if(S1length > S2length){
    for(i=0; i < S2length; i++){
      if(*pcS1 < *pcS2){
	return -1;
      }
      else if(*pcS1 > *pcS2){
	return 1;
      }
      else{
	pcS1++;
	pcS2++;
      }
    }
    return 1;
  }
  else if(S1length < S2length){
    for(i=0; i < S1length; i++){
      if(*pcS1 < *pcS2){
	return -1;
      }
      else if(*pcS1 > *pcS2){
	return 1;
      }
      else{
	pcS1++;
	pcS2++;
      }
    }
    return -1;
  }
  else{ //S1length == S2length
    for(i=0; i < S1length; i++){
      if(*pcS1 < *pcS2){
	return -1;
      }
      else if(*pcS1 > *pcS2){
	return 1;
      }
      else{
	pcS1++;
	pcS2++;
      }
    }
    return 0;
  }
}
/*------------------------------------------------------------------------*/
char *StrSearch(const char* pcHaystack, const char *pcNeedle)
{
  /* This function searches pcNeedle word in pcHaystack array.
     I used for loop to compare in every location of the pointer.
     Haystart and Needlestart are the first location of the
     pointer of pcHaystack and pcNeedle.                        */
  assert(pcHaystack != NULL);
  assert(pcNeedle != NULL);

  size_t Needlelength = StrGetLength(pcNeedle);
  size_t Haylength = StrGetLength(pcHaystack);

  char* Haystart = (char* )pcHaystack; /* pointer that is used for comparing */
  char* Needlestart = (char* )pcNeedle;

  size_t i, j;

  for(i=0; i<Haylength-Needlelength+1; i++){
    for(j=0; j<Needlelength; j++){
      if (*pcHaystack == *pcNeedle){
	pcHaystack++;
	pcNeedle++;
	continue;
      }
      else{
        Haystart++;
	pcNeedle = Needlestart;
	pcHaystack = Haystart;
	break;
      }
    }
    if(j==Needlelength){
      return Haystart;
    }
  }
  return NULL;
}
/*------------------------------------------------------------------------*/
char *StrConcat(char *pcDest, const char* pcSrc)
{
  /* This function concatenates the end of pcDest and pcSrc,
     by using Strcopy.
     First of all, pcDest should points the end of pcDest array,
     which has Null Value, so I used for loop, and using StrCopy. */
  assert(pcDest != NULL);
  assert(pcSrc != NULL);
  char* pcDeststart = pcDest;
  size_t Destlength = StrGetLength(pcDest);

  size_t i=0;
  for(i=0; i<Destlength; i++){
    pcDest++;
  }
  StrCopy(pcDest, pcSrc);
  
  return pcDeststart;
}
