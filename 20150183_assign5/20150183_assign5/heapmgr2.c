/*--------------------------------------------------------------*/
/* Assignment 5                                                 */
/* Filename : heapmgr2.c                                        */
/* Author: Kim Yunseong                                         */
/* Student ID : 20150183                                        */
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <assert.h>

#include "chunk2.h"

/*--------------------------------------------------------------*/
/* SizeToUnits:
 * Returns capable number of units for 'size' bytes. */
/*--------------------------------------------------------------*/
int
SizeToUnits(int size)
{
   return (size + (CHUNK_UNIT-1))/CHUNK_UNIT;
}
/*--------------------------------------------------------------*/
/* GetChunkFromDataPtr:
 * Returns the header pointer that contains data 'm'. */
/*--------------------------------------------------------------*/
Chunk_T
GetChunkFromDataPtr(void *m)
{
   return (Chunk_T)((char *)m - CHUNK_UNIT);
}

/*--------------------------------------------------------------*/
/* mymalloc:
 * Dynamically allocate a memory capable of holding size bytes. 
 * Substitute for GNU malloc().                                 */
/*--------------------------------------------------------------*/
void *
mymalloc(size_t size)
{
   static int isInitialized = 0;
   Chunk_T c;
   int units;
   int i, j;
   if (size <= 0)
      return NULL;
   
   if (isInitialized == 0) {
      ChunkInitDS();
      isInitialized = 1;
   }

   assert(ChunkValidityCheck());

   units = SizeToUnits(size);
   i = units-1; // determine index
   if(i>=1024) i=1023;
   /* Find the bin[i], and check whether there is a free chunk. */

   if(i==1023){
     /* bin[1023] has various sizes of chunks.
      * in here, we should find chunk
      * which has size bigger than 1023. */
     if((c = ChunkAllocateMem(units)) == NULL){
       assert(0); /* should not happen */
       return NULL;
     }
     for(c = bin[1023]; c; c = c->next){
       if (ChunkGetUnits(c) >= units) {
	 if (ChunkGetUnits(c) > units + 2)
	   /* for header and footer */
	   c = ChunkSplit(c, units);
	 else
	   ChunkRemoveFromList_New(c);
	 assert(ChunkValidityCheck());
	 return ((char *)c + CHUNK_UNIT);
       }
     }   
     assert(0); 
   }
   else { // 0 < i < 1023
      if(bin[i] == NULL){ // need bigger size chunk
        for(j=i+1; j<1024; j++){
	  if(bin[j] != NULL){
	    c = bin[j];
	    if(ChunkGetUnits(c) > units + 2)
	      c = ChunkSplit(c, units);
	    else
	      ChunkRemoveFromList_New(c);
	    assert(ChunkValidityCheck());
	    return ((char *)c + CHUNK_UNIT);  
	  }
	  if(j==1023){
	    /* in here, Failed to Find.
	     * We need to allocate new memories. */
	    if ((c = ChunkAllocateMem(units)) == NULL){ //Fail
	      assert(0);
	      return NULL;
	    }
	    
	    if(ChunkGetUnits(c) > units + 2)
	      c = ChunkSplit(c, units);
	    else
	      ChunkRemoveFromList_New(c);
	    assert(ChunkValidityCheck());
	    return ((char *)c + CHUNK_UNIT);
	  }
	}
      }
      else{ //bin[i] != NULL
	c = bin[i];
	// we don't need to do ChunkSplit
	ChunkRemoveFromList_New(c);
	
	assert(ChunkValidityCheck());
	return ((char *)c + CHUNK_UNIT);
      }
   }
   /* Unreachable here */
   return NULL;
}

/*--------------------------------------------------------------*/
/* myfree:
 * Releases dynamically allocated memory. 
 * Substitute for GNU free().                                   */
/*--------------------------------------------------------------*/
void
myfree(void *m)
{
   Chunk_T c;
   
   if (m == NULL)
      return;

   /* Check everything is OK before freeing 'm' */
   assert(ChunkValidityCheck());

   /* Get the chunk header pointer from m */
   c = GetChunkFromDataPtr(m);
   assert (ChunkGetStatus(c) != CHUNK_FREE);

   /* Insert the chunk into the free chunk list */
   ChunkFreeChunk(c);

   /* Check everything is OK after freeing 'm' */
   assert(ChunkValidityCheck());
}
