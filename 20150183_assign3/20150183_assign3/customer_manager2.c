/*******************************
**********Assignment 3**********
ID: 20150183
Name: Kim Yunseong
File name : customer_manager2.c
*******************************/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"

/*--------------------------------------------------------------------*/

#define UNIT_ARRAY_SIZE 1024
enum {HASH_MULTIPLIER = 65599};

/*UserInfo :
struct that includes name, id, purchase value, and
pointer which points to next UserInfo.
However, it has 2 pointers, to classify by id or name
(actually, id or name hash values)
This struct is pointed at first time by struct DB  */
struct UserInfo {
  char *name;                // customer name
  char *id;                  // customer id
  int purchase;              // purchase amount (> 0)
  struct UserInfo* pNextidArray;
  struct UserInfo* pNextnameArray;
};

/* Difference from customer_manager1.c
   pHT_id and pHT_name points to the id, name pointer array.
   also have size and number of UserInfos.
   (same as customer_manager1)
 */
struct DB {
  struct UserInfo** pHT_id;
  // pointer to the id pointer array
  struct UserInfo** pHT_name;
  // pointer to the name pointer array

  int curArrSize;          // current array size (max # of elements)
  int numItems;            // # of stored items, needed to determine
			   // # whether the array should be expanded
			   // # or not
};
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
static int hash_function(const char *pcKey, int iBucketCount){
  /* gets key and bucketsize ,and returns hash value
     hash value separates most of datas into each arrays      */
  int i;
  unsigned int uiHash = 0U;
  for (i=0; pcKey[i] != '\0'; i++)
    uiHash = uiHash + (unsigned int)HASH_MULTIPLIER
      + (unsigned int)pcKey[i];
  return (int) (uiHash % (unsigned int)iBucketCount);
}

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
  d->curArrSize = UNIT_ARRAY_SIZE;
  d->curArrSize = UNIT_ARRAY_SIZE;
  d->numItems = 0;
  d->pHT_id = (struct UserInfo **)calloc(d->curArrSize,
               sizeof(struct UserInfo*));
  d->pHT_name = (struct UserInfo **)calloc(d->curArrSize,
	       sizeof(struct UserInfo*));
  if (d->pHT_id == NULL) {
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	    d->curArrSize);   
    free(d);
    return NULL;
  }
  if (d->pHT_name == NULL) {
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

  int i;
  for(i=0; i< d->curArrSize; i++){
    for(pCurrentArray = d->pHT_id[i];
	(pCurrentArray != NULL);
	pCurrentArray = ptempNextArray) {
      ptempNextArray = pCurrentArray -> pNextidArray;
      free(pCurrentArray->id);
      free(pCurrentArray->name);
      free(pCurrentArray);
    }
  }
  free(d->pHT_id);
  free(d->pHT_name);
  free(d);
}

int
GrowHash(DB_T d){
  /* this function grows the arraysize of pHT_id and pHT_name
     parameter is d, and it maeks new UserInfo** by using
     memory allocation, and rearrange all of UserInfos because
     hash value is changed, and make d point to new UserInfo**.
     and free past UserInfo**s. 
     returns 1 if success, 0 if fail. */
  assert(d!=NULL);
  int i;
  int newlength;
  int hid;
  int hname;
  struct UserInfo* pCurrentArray;
  struct UserInfo* ptempNextArray;;
  newlength = (d->curArrSize)*2;
  struct UserInfo** pnewHT_id=(struct UserInfo**)calloc(
			   newlength,sizeof(struct UserInfo*));
  if(pnewHT_id==NULL) return 0;
  struct UserInfo** pnewHT_name=(struct UserInfo**)calloc(
			   newlength,sizeof(struct UserInfo*));
  if(pnewHT_name==NULL) return 0;  

  struct UserInfo* pNewtemp;
  for(i=0; i< d->curArrSize; i++){
    for(pCurrentArray = d->pHT_id[i];
	(pCurrentArray!=NULL)&&(pCurrentArray->id!=NULL)
	  &&(pCurrentArray->name!=NULL);
	pCurrentArray = ptempNextArray) {
      ptempNextArray = pCurrentArray -> pNextidArray;
      pNewtemp=(struct UserInfo*)malloc(sizeof(struct UserInfo));
      if (pNewtemp==NULL) return (0);

      hid=hash_function(pCurrentArray->id, newlength);
      hname=hash_function(pCurrentArray->name, newlength);

      pNewtemp->id = strdup(pCurrentArray->id);
      pNewtemp->name = strdup(pCurrentArray->name);
      pNewtemp->purchase = pCurrentArray->purchase;
  
      pNewtemp->pNextidArray = pnewHT_id[hid];
      pNewtemp->pNextnameArray = pnewHT_name[hname];

      pnewHT_id[hid] = pNewtemp;
      pnewHT_name[hname] = pNewtemp;
    }
  }
  //free the past nodes
  for(i=0; i<d->curArrSize; i++){
    for(pCurrentArray = d->pHT_id[i];
	pCurrentArray != NULL;
	pCurrentArray = ptempNextArray) {
      ptempNextArray = pCurrentArray -> pNextidArray;
      free(pCurrentArray->id);
      free(pCurrentArray->name);
      free(pCurrentArray);
    }
  }
  struct UserInfo** ptempid = d->pHT_id;
  struct UserInfo** ptempname = d-> pHT_name;
  d->pHT_id=pnewHT_id;
  d->pHT_name=pnewHT_name;
  d->curArrSize = newlength;
  d->curArrSize = newlength;
  free(ptempid);
  free(ptempname);
  
  return 1;
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
  int hid = hash_function(id, d->curArrSize);
  for (pcheck=d->pHT_id[hid]; (pcheck!=NULL)&&(pcheck->id!=NULL)
	 &&(pcheck->name!=NULL); pcheck=pcheck->pNextidArray){
    if(strcmp(pcheck->id, id)==0){
      fprintf(stderr, "Error: Same ID already exists\n");
      return (-1);
    }
  }
  //name check
  int hname = hash_function(name, d->curArrSize);
  for (pcheck=d->pHT_name[hname];
       (pcheck!=NULL)&&(pcheck->id!=NULL)
	 &&(pcheck->name!=NULL); pcheck=pcheck->pNextnameArray){
    if(strcmp(pcheck->name, name)==0){
      fprintf(stderr, "Error: Same name already exists\n");
      return (-1);
    }
  }
  // grow part ///////////////////////////////////
  if((d->numItems >= (d->curArrSize)*0.75) &&
     d->curArrSize < 1048576){
    if(!GrowHash(d)){
      fprintf(stderr, "grow fail");
      return (-1);
    }
  }
  ////////////////////////////////////////
  struct UserInfo* pNew;  
  pNew = (struct UserInfo*)malloc(sizeof(struct UserInfo));
  if (pNew == NULL) return (-1);
  pNew->id = strdup(id);
  pNew->name = strdup(name);
  pNew->purchase = purchase;
  pNew->pNextidArray = d-> pHT_id[hid];
  pNew->pNextnameArray = d->pHT_name[hname];
  d->pHT_id[hid] = pNew;
  d->pHT_name[hname] = pNew;
  d->numItems += 1;
  return (0);
}
/*--------------------------------------------------------------------*/
int
UnregisterCustomerByID(DB_T d, const char *id)
{ 
  /* This function unregister UserInfo by id.
     By using d and id, which are parameter, finds id which 
     is stored in UserInfo, and free the elements.
     it returns 0 if success, -1 if fails  */
  struct UserInfo* pidSearch;
  struct UserInfo* pidtempPastArray;
  struct UserInfo* pnameSearch;
  struct UserInfo* pnametempPastArray;
  int countid=0;
  int countname=0;
  int hid = hash_function(id, d->curArrSize);
  char* nametemp;
  //to check whether we should free top element of UserInfo
  for (pidSearch=d->pHT_id[hid];
       (pidSearch!=NULL)&&(pidSearch->id!=NULL);
       pidSearch=pidSearch->pNextidArray){
    if(strcmp(pidSearch->id, id)==0){
      if (countid==0){
	d->pHT_id[hid] = pidSearch->pNextidArray;
	//searching in name hash table
	nametemp = strdup(pidSearch->name);
	int hname = hash_function(nametemp, d->curArrSize);
	for(pnameSearch=d->pHT_name[hname];
	    (pnameSearch!=NULL)&&(pnameSearch->name!=NULL);
	    pnameSearch=pnameSearch->pNextnameArray){
	  if(strcmp(pnameSearch->name, nametemp)==0){
	    if(countname==0){
	      d->pHT_name[hname]=pnameSearch->pNextnameArray;
	      free(nametemp);
	      break;
	    }  
	    else{
	      pnametempPastArray->pNextnameArray
		=pnameSearch->pNextnameArray;
	      free(nametemp);
	      break;
	    }
	  }
	  countname++;
	  pnametempPastArray=pnameSearch;
	}
      }
      else{
	pidtempPastArray->pNextidArray = pidSearch->pNextidArray;
	//searching in name hash table
	nametemp = strdup(pidSearch->name);
	int hname = hash_function(nametemp, d->curArrSize);
	for(pnameSearch=d->pHT_name[hname];
	    (pnameSearch!=NULL)&&(pnameSearch->name!=NULL);
	    pnameSearch=pnameSearch->pNextnameArray){
	  if(strcmp(pnameSearch->name, nametemp)==0){
	    if(countname==0){
	      d->pHT_name[hname]=pnameSearch->pNextnameArray;
	      free(nametemp);
	      break;
	    }  
	    else{
	      pnametempPastArray->pNextnameArray
		=pnameSearch->pNextnameArray;
	      free(nametemp);
	      break;
	    } 
	  }
	  countname++;
	  pnametempPastArray=pnameSearch;
	}
	fprintf(stderr, "Cannot Come Here\n");
      }
      //free the UserInfo
      free(pidSearch->id);
      free(pidSearch->name);
      free(pidSearch); // pidSearch = pnameSearch      
      d->numItems -= 1;
      return (0);
    }
    countid++;
    pidtempPastArray = pidSearch;
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
  struct UserInfo* pidSearch;
  struct UserInfo* pidtempPastArray;
  struct UserInfo* pnameSearch;
  struct UserInfo* pnametempPastArray;
  int countid=0;
  int countname=0;
  int hname = hash_function(name, d->curArrSize);
  char* idtemp;
  //to check whether we should free top element of UserInfo
  for (pnameSearch=d->pHT_name[hname];
       (pnameSearch!=NULL)&&(pnameSearch->name!=NULL);
       pnameSearch=pnameSearch->pNextnameArray){
    if(strcmp(pnameSearch->name, name)==0){
      if (countname==0){
	d->pHT_name[hname] = pnameSearch->pNextnameArray;
	//searching in id hash table
	idtemp = strdup(pnameSearch->id);
	int hid = hash_function(idtemp, d->curArrSize);
	for(pidSearch=d->pHT_id[hid];
	    (pidSearch!=NULL)&&(pidSearch->id!=NULL);
	    pidSearch=pidSearch->pNextidArray){
	  if(strcmp(pidSearch->id, idtemp)==0){
	    if(countid==0){
	      d->pHT_id[hid]=pidSearch->pNextidArray;
	      free(idtemp);
	      break;
	    }  
	    else{
	      pidtempPastArray->pNextidArray=pidSearch->pNextidArray;
	      free(idtemp);
	      break;
	    }
	  }
	  countid++;
	  pidtempPastArray=pidSearch;
	}
      }
      else{
	pnametempPastArray->pNextnameArray=pnameSearch->pNextnameArray;
	//searching in id hash table
	idtemp = strdup(pnameSearch->id);
	int hid = hash_function(idtemp, d->curArrSize);
	for(pidSearch=d->pHT_id[hid];
	    (pidSearch!=NULL)&&(pidSearch->id!=NULL);
	    pidSearch=pidSearch->pNextidArray){
	  if(strcmp(pidSearch->id, idtemp)==0){
	    if(countid==0){
	      d->pHT_id[hid]=pidSearch->pNextidArray;
	      free(idtemp);
	      break;
	    }  
	    else{
	      pidtempPastArray->pNextidArray
		=pidSearch->pNextidArray;
	      free(idtemp);
	      break;
	    }
	  }
	  countid++;
	  pidtempPastArray=pidSearch;
	}	
      }
      //free the UserInfo
      free(pidSearch->id);
      free(pidSearch->name);
      free(pidSearch); // pidSearch = pnameSearch      
      d->numItems -= 1;
      return (0);
    }
    countname++;
    pnametempPastArray = pnameSearch;
  }
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
  int hid = hash_function(id, d->curArrSize);
  for (pSearch=d->pHT_id[hid];
       (pSearch!=NULL)&&(pSearch->id!=NULL);
       pSearch=pSearch->pNextidArray){
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
     By using d and id, which are parameter, finds name which 
     is stored in UserInfo, and returns purchase value.
     if it fails, returns -1. */
  struct UserInfo* pSearch;
  int hname = hash_function(name, d->curArrSize);
  for (pSearch=d->pHT_name[hname];
       (pSearch!=NULL)&&(pSearch->name!=NULL);
       pSearch=pSearch->pNextnameArray){
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
  int i;
  struct UserInfo* pSearch;
  if(d==NULL || fp==NULL) return -1;
  for(i=0; i<d->curArrSize; i++){
    for (pSearch=d->pHT_id[i];
	 (pSearch!=NULL)&&(pSearch->id!=NULL)&&
	   (pSearch->name!=NULL);
	 pSearch=pSearch->pNextidArray){
      value += fp(pSearch->id, pSearch->name, pSearch->purchase);
    }
  }
  return (value);
}
