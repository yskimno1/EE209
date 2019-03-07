/*******************************
**********Assignment 3**********
ID: 20150183
Name: Kim Yunseong
File name : customer_manager1.c
*******************************/



#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"

/*--------------------------------------------------------------------*/

#define UNIT_ARRAY_SIZE 1024
/*UserInfo :
struct that includes name, id, purchase value, and
pointer which points to next UserInfo
This struct is pointed at first time by struct DB  */
struct UserInfo {
  char *name;                // customer name
  char *id;                  // customer id
  int purchase;              // purchase amount (> 0)
  struct UserInfo* pNextArray;
};

/*DB: The first struct which points to UserInfo Array.
Size is limited
contains the information of maximum size and nuber of UserInfos  */
struct DB {
  struct UserInfo *pArray;   // pointer to the array
  int curArrSize;            // current array size (max # of elements)
  int numItems;              // # of stored items, needed to determine
			     // # whether the array should be expanded
			     // # or not
};

//DB_T = struct DB*
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
DB_T
CreateCustomerDB(void)
{
  /* This function creates customer DB, which is named d,
     by using memory allocation.
     it returns NULL if creation is failed,
     or returns d if it is success
   */
  DB_T d;
  
  d = (DB_T) calloc(1, sizeof(struct DB));
  if (d == NULL) {
    fprintf(stderr, "Can't allocate a memory for DB_T\n");
    assert(0);
    return NULL;
  }
  d->numItems = 0;
  d->curArrSize = UNIT_ARRAY_SIZE; // start with 1024 elements
  d->pArray = (struct UserInfo *)calloc(d->curArrSize,
               sizeof(struct UserInfo));
  if (d->pArray == NULL) {
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	    d->curArrSize);   
    free(d);
    return NULL;
  }
  return d;

}
/*--------------------------------------------------------------------*/
void
DestroyCustomerDB(DB_T d)
{
  /* This function calls d, and destroy DB_T d
     it doesn't returns anything, just free all memories*/
  struct UserInfo* pCurrentArray;
  struct UserInfo* ptempNextArray;
  assert(d!=NULL);
  
  for(pCurrentArray = d->pArray;
      pCurrentArray != NULL;
      pCurrentArray = ptempNextArray) {
    ptempNextArray = pCurrentArray -> pNextArray;
    free(pCurrentArray->id);
    free(pCurrentArray->name);
    free(pCurrentArray);
  }
  free(d);
}
/*--------------------------------------------------------------------*/
int
RegisterCustomer(DB_T d, const char *id,
		 const char *name, const int purchase)
{
  /*This function makes new UserInfo by parameters, which are id,
    name, and purchase value. Actually, id and name is called by 
    pointer. This information is stored in d, specifically d->pArray
    by using stack.
    it returns 0 if success, -1 if fail*/
  
  if (purchase<=0 || d==NULL || id==NULL || name==NULL){
    fprintf(stderr, "Error: unvalid input value\n");
    return (-1);
  }

  struct UserInfo* pcheck;
    
  //id check

  for (pcheck=d->pArray; (pcheck!=NULL)&&(pcheck->id!=NULL)
	 &&(pcheck->name!=NULL); pcheck=pcheck->pNextArray){
    if(strcmp(pcheck->id, id)==0){
      fprintf(stderr, "Error: Same ID already exists\n");
      fflush(stderr);
      return (-1);
    }
  }

  //name check

  for (pcheck=d->pArray; (pcheck!=NULL)&&(pcheck->id!=NULL)
	 &&(pcheck->name!=NULL); pcheck=pcheck->pNextArray){
    if(strcmp(pcheck->name, name)==0){
      fprintf(stderr, "Error: Same name already exists");
      return (-1);
    }
  }

  if(d->numItems >= d->curArrSize){
    d->curArrSize += 1024;
    //grow Array
    struct UserInfo* pdNewArray;
    assert(d!=NULL);
    pdNewArray = (struct UserInfo*)realloc(d->pArray, (d->curArrSize)
					  *sizeof(struct UserInfo));
    if(pdNewArray == NULL){
      fprintf(stderr, "grow Failed");
      return -1; /*grow Fail*/
    } // start with 1024 elements
    
    d->pArray = pdNewArray;
    if (d->pArray == NULL) {
      fprintf(stderr,"Can't allocate a memory for array of size %d\n",
	      d->curArrSize);   
      free(d);
      return (-1);
    }
  }

  struct UserInfo* pNew;  
  pNew = (struct UserInfo*)malloc(sizeof(struct UserInfo));
  if (pNew == NULL) return (-1);
  pNew->id = strdup(id);
  pNew->name = strdup(name);
  pNew->purchase = purchase;
  pNew->pNextArray = d-> pArray;
  d->pArray = pNew;
  
  d->numItems += 1;
  return (0);
}
/*--------------------------------------------------------------------*/
int
UnregisterCustomerByID(DB_T d, const char *id)
{
  /* This function unregister UserInfo by ID.
     By using d and id, which are parameter, finds id which 
     is stored in UserInfo, and free the elements.
     it returns 0 if success, -1 if fails  */
  struct UserInfo* pSearch;
  struct UserInfo* ptempPastArray;
  int count=0;
  //to check whether we should free top element of UserInfo
  for (pSearch=d->pArray; (pSearch!=NULL)&&(pSearch->id!=NULL);
       pSearch=pSearch->pNextArray){
    if(strcmp(pSearch->id, id)==0){
      if (count==0){
	d->pArray = pSearch->pNextArray;
	free(pSearch->id);
	free(pSearch->name);
	free(pSearch);
	d->numItems -= 1;
	return (0);
      }
      ptempPastArray->pNextArray = pSearch->pNextArray;
      free(pSearch->id);
      free(pSearch->name);
      free(pSearch);
      d->numItems -= 1;
      return (0);
    }
    count++;
    ptempPastArray = pSearch;
  }
  return (-1);
}

/*--------------------------------------------------------------------*/
int
UnregisterCustomerByName(DB_T d, const char *name)
{
  /* This function unregister UserInfo by name.
     By using d and name, which are parameter, finds name which 
     is stored in UserInfo, and free the elements.
     it returns 0 if success, -1 if fails  */
  struct UserInfo* pSearch;
  struct UserInfo* ptempPastArray;
  int count=0;
  //to check whether we should free top element of UserInfo
  for (pSearch=d->pArray; (pSearch!=NULL)&&(pSearch->name!=NULL);
       pSearch=pSearch->pNextArray){
    if(strcmp(pSearch->name, name)==0){
      if (count==0){
	d->pArray = pSearch->pNextArray;
	free(pSearch->id);
	free(pSearch->name);
	free(pSearch);
	d->numItems -= 1;
	return (0);
      }
      ptempPastArray->pNextArray = pSearch->pNextArray;
      free(pSearch->id);
      free(pSearch->name);
      free(pSearch);
      d->numItems -= 1;
      return (0);
    }
    count++;
    ptempPastArray = pSearch;
  }
  fprintf(stderr, "%d\n", count);
  fflush(stderr);
  return (-1);
}
/*--------------------------------------------------------------------*/
int
GetPurchaseByID(DB_T d, const char* id)
{
    /* This function get purchase value by ID.
     By using d and id, which are parameter, finds id which 
     is stored in UserInfo, and returns purchase value.
     if it fails, returns -1. */
  struct UserInfo* pSearch;
  for (pSearch=d->pArray; (pSearch!=NULL)&&(pSearch->id!=NULL);
       pSearch=pSearch->pNextArray){
    if(strcmp(pSearch->id, id)==0){
      return (pSearch->purchase);
    }
  }
  return (-1);
}
/*--------------------------------------------------------------------*/
int
GetPurchaseByName(DB_T d, const char* name)
{
  /* This function get purchase value by name.
     By using d and name, which are parameter, finds id which 
     is stored in UserInfo, and returns purchase value.
     if it fails, returns -1. */
  struct UserInfo* pSearch;
  for (pSearch=d->pArray; (pSearch!=NULL)&&(pSearch->name!=NULL);
       pSearch=pSearch->pNextArray){
    if(strcmp(pSearch->name, name)==0){
      return (pSearch->purchase);
    }
  }
  return (-1);
}
/*--------------------------------------------------------------------*/
int
GetSumCustomerPurchase(DB_T d, FUNCPTR_T fp)
{
  /*This function calculates the sum of all of purchase values.
    By using function pointer, it calculates purchase values which
    user wants. it returns sum value if success, -1 if fail.     */
  int value=0;
  struct UserInfo* pSearch;
  if(d==NULL || fp==NULL) return -1;
  for (pSearch=d->pArray; (pSearch!=NULL)&&(pSearch->id!=NULL)&&
	 (pSearch->name!=NULL); pSearch=pSearch->pNextArray){
    value += fp(pSearch->id, pSearch->name, pSearch->purchase);
  }
  return (value);
}
