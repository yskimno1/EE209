//*--------------------------------------------------------------------*/
/* Assignment 5                                                       */
/* Filename : chunk2.c                                                */
/* Author: Kim Yunseong                                               */
/* Student ID : 20150183                                              */
/*--------------------------------------------------------------------*/
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "chunk2.h"

enum {
   MEMALLOC_MIN = 1024,
};

/****************************************************
 *************All included at chunk2.h***************
 ****************************************************/


/* gHeapStart, gHeapEnd: start and end of the heap area.
 * gHeapEnd will move if you increase the heap */
static void *gHeapStart = NULL, *gHeapEnd = NULL;

/*--------------------------------------------------------------------*/
void
ChunkInitDS(void)
{
   /* Initialize gHeapStart if it is not initialized yet */
   gHeapStart = gHeapEnd = sbrk(0);
   if (gHeapStart == (void *)-1) {
      fprintf(stderr, "sbrk(0) failed\n");
      exit(-1);
   }
}
/*--------------------------------------------------------------------*/
int
ChunkGetStatus(Chunk_T c)
{
   /* If a chunk is not in the free chunk list, then the next pointer
    * must be NULL. We can figure out whether a chunk is in use or
    * free by looking at the next chunk pointer. */
   if (c->next == NULL)
      return CHUNK_IN_USE;
   else
      return CHUNK_FREE;
}
/*--------------------------------------------------------------------*/
/* Returns the Next pointer of present chunk                        */
/*--------------------------------------------------------------------*/
Chunk_T
ChunkNextFreeChunk(Chunk_T c)
{
  return c->next;
}

/*--------------------------------------------------------------------*/
/* Returns the Footer pointer of present chunk                        */
/*--------------------------------------------------------------------*/

Footer_T
ChunkFooterChunk(Chunk_T c){
  return (Footer_T)(c + c->units + 1);
}

/*--------------------------------------------------------------------*/
/* ChunkInsertFirst                                                   */
/* Insert Chunk into the first place of the bin                       */
/* The index is determined in this function. (units -1) is index      */
/* maximum index value is 1023.                                       */
/* if size is more than 1024, every chunks goes to bin[1023]          */
/* No return value, also merges the adjacent memory blocks            */
/*--------------------------------------------------------------------*/
void
ChunkInsertFirst(Chunk_T c)
{
   assert (ChunkGetStatus(c) != CHUNK_FREE);
   assert (c->units >= 1);

   int i = (int)(c->units -1);
   if(i>=1024) i=1023;
   assert (i>=0 && i<1024);
   
   /* If free chunk list is empty, chunk c points to itself. */
   Footer_T f1, f2 = NULL;
   
   if (bin[i] == NULL) {
      bin[i] = c; 
      c->next = c;
      
      f1 = ChunkFooterChunk(c);
      f1->back = c;
      f1->units = c->units;
   }
   else {
      c->next = bin[i]; /* now c is CHUNK_FREE */
      f1 = ChunkFooterChunk(c);
      f2 = ChunkFooterChunk(bin[i]);
      
      f1->back = f2->back;
      (f2->back)->next = c;
      f2->back = c;

      if(ChunkNextAdjacent(c) == bin[i]){
	ChunkMerge(c, bin[i]);
      }
      bin[i] = c;      
   }

}
/*--------------------------------------------------------------------*/
int
ChunkGetUnits(Chunk_T c)
{ // get the units of chunk which contains.
   return c->units;
}
/*--------------------------------------------------------------------*/
Chunk_T
ChunkNextAdjacent(Chunk_T c)
{
   Chunk_T n;
   
   /* Note that a chunk consists of one chunk unit for a header and
    * many chunk units for data. */
   n = c + c->units + 2;
   
   /* If 'c' is the last chunk in memory space, then return NULL. */
   if ((void *)n >= gHeapEnd)
      return NULL;
   
   return n;
}
/*--------------------------------------------------------------------*/
Chunk_T
ChunkPrevAdjacent(Chunk_T c)
{
  /* by using Footer pointer, find previous adjacent chunk
   * It can be either CHUNK_FREE or CHUNK_IN_USE
   */
  Footer_T f;
  
  f = (Footer_T)(c-1);
  if((void *)(f) <= gHeapStart) return NULL;

  Chunk_T n = (Chunk_T)(f - f->units -1);
  return n;
}
/*--------------------------------------------------------------------*/
Chunk_T
ChunkMerge(Chunk_T c1, Chunk_T c2)
{
  /*--------------------------------------------------------------------*/
  /* ChunkMerge                                                         */
  /* Merges the two adjacent chunks                                     */
  /* Returns the previous chunk(standard chunk is previous chunk(c1))   */
  /* Also units of chunk changes                                        */
  /*--------------------------------------------------------------------*/
   /* c1 and c2 must be be adjacent */
   assert (c1 < c2);
   assert (ChunkNextAdjacent(c1) == c2);
   assert (ChunkGetStatus(c1) == CHUNK_FREE);
   assert (ChunkGetStatus(c2) == CHUNK_FREE);
   Footer_T f1, f2;
   /* Adjust the merged chunk */
   f1 = ChunkFooterChunk(c1);
   f2 = ChunkFooterChunk(c2);

   f2->units += (f1->units + 2);
   c1->units += (c2->units + 2);
   
   c1->next = c2->next; //c1 CHUNK_FREE
   f2->back = f1->back; //c1 CHUNK_FREE

   
   /* Update tail if w2 was last chunk in free chunk list. */
   assert ((ChunkFooterChunk(c1))==f2);
   return c1;
}
/*--------------------------------------------------------------------*/
Chunk_T
ChunkSplit(Chunk_T c, int units)
{
/*--------------------------------------------------------------------*/
/* ChunkSplit                                                         */
/* Splits the chunk into two chunks                                   */
/* The second(after) chunk size is determined by function input       */
/* Also, insert Chunk c which has smaller size than before in tnis ft.*/ 
/* Returns the second chunk                                           */
/*--------------------------------------------------------------------*/
   Chunk_T c2;
   Footer_T f, f2;
   assert (c >= (Chunk_T)gHeapStart && c <= (Chunk_T)gHeapEnd);
   assert (ChunkGetStatus(c) == CHUNK_FREE);
   assert (c->units > units + 2); /* assume chunk with header unit */
   
   ChunkRemoveFromList_New(c); // remove the chunk from bin[i]
   
   /* prepare for the second chunk */   
   c2 = c + c->units - units;
   c2->units = units;
   c2->next = NULL; /* set next to NULL since it will be used soon */
   f2 = ChunkFooterChunk(c2);
   f2->units = units; 
   f2->back = NULL;
   
   /* the original chunk size should be decreased by c2's size */
   c->units -= (units + 2);
   f = ChunkFooterChunk(c);

   f->back = NULL;
   c->next = NULL;
   f->units = c->units;

   ChunkInsertFirst(c); // insert new chunk into bin[i_newindex]
   
   return c2;
}
/*--------------------------------------------------------------------*/

void
ChunkRemoveFromList_New(Chunk_T c)
{
/*--------------------------------------------------------------------*/
/* ChunkRemoveFromList_New                                            */
/* Removes the chunk c by using chunk prev(previous free chunk)       */
/* No return value, input chunk should be CHUNK_FREE                  */
/*--------------------------------------------------------------------*/
   assert (ChunkGetStatus(c) == CHUNK_FREE);
   Footer_T f, p;
   
   int i = (int)(c->units -1);
   if(i>=1024) i=1023;

   f = ChunkFooterChunk(c);
   p = ChunkFooterChunk(ChunkNextFreeChunk(c));
   Chunk_T prev = f->back;

   if (prev == c) { 
      assert(c->next == c);
      /* c is the only chunk in the free list */
      bin[i] = NULL;
   }
   else {
      assert(c->next != c);
      prev->next = c->next;
      p->back = prev;
      
      if (c == bin[i])
         bin[i] = c->next;
   }
   c->next = NULL; /* its status is now CHUNK_IN_USE */
   f->back = NULL;
   
   return;
}


/*--------------------------------------------------------------------*/
Chunk_T
ChunkAllocateMem(int units)
{
/*--------------------------------------------------------------------*/
/* ChunkAllocateMem                                                   */
/* Allocate new memory by using sbrk function                         */
/* New allocated chunk should be merged with previous free chunk      */
/*--------------------------------------------------------------------*/
   Chunk_T c;
   Chunk_T p;
   Footer_T f;
   /* Note that we need to allocate two more unit for header & footer.*/
   c = (Chunk_T)sbrk((units + 2) * CHUNK_UNIT);
   if (c == (Chunk_T)-1)
      return NULL;
   gHeapEnd = sbrk(0);
   
   c->units = units;
   c->next = c;//temporary making c CHUNK_FREE
   f = ChunkFooterChunk(c);
   
   f->units = units;
   f->back = c;
   
   /* Insert the newly allocated chunk 'c' to the free list.
    * Note that the list is sorted in ascending order. */

   p = ChunkPrevAdjacent(c);

   if((p!=NULL) && ChunkGetStatus(p)==CHUNK_FREE){
     ChunkRemoveFromList_New(p);
     p->next = p;//temporary making c CHUNK_FREE
     ChunkFooterChunk(p)->back = p; // temporary making c CHUNK_FREE    
     c = ChunkMerge(p, c);
   }
   
   c->next = NULL; // make c CHUNK_IN_USE 
   f = ChunkFooterChunk(c);
   f->back = NULL; //make c CHUNK_IN_USE

   ChunkInsertFirst(c);

   assert(ChunkValidityCheck());   
   return c;
}
/*--------------------------------------------------------------------*/
void
ChunkFreeChunk(Chunk_T c)
{
/*--------------------------------------------------------------------*/
/* ChunkFreeChunk                                                     */
/* Free the IN_USE Chunk                                              */
/* When we do free, we should merge adjacent chunks                   */
/*--------------------------------------------------------------------*/
  
   Chunk_T w = NULL;
   int i = (int)(c->units -1);
   if(i>=1024) i=1023;
   w = bin[i];
 
   /* If the free chunk list is empty or new chunk's address is less
    * than first chunk in free chunk list, insert at beginning. */
   if (w == NULL||c<w) {
      ChunkInsertFirst(c);
      return;
   }

   assert(w<c);
   
   Chunk_T e, n = NULL;
   Footer_T f;
   f = ChunkFooterChunk(c);
   e = ChunkPrevAdjacent(c);
   n = ChunkNextAdjacent(c);

   c->next = n;
   f->back = e; // temporary making Chunk c CHUNK_FREE

   if (e != NULL && ChunkGetStatus(e) == CHUNK_FREE){
     ChunkRemoveFromList_New(e);
     e->next = c;
     /* We should make chunk e FREE temporary 
      * because ChunkRemoveFromList_New makes input CHUNK_IN_USE 
      * Also, parameter of ChunkMerge should be CHUNK_FREE      */
     ChunkFooterChunk(e)->back = e; 
     c = ChunkMerge(e, c);
   }
   if (n != NULL && ChunkGetStatus(n) == CHUNK_FREE){
     ChunkRemoveFromList_New(n);
     n->next = n;
     /* We should make chunk e FREE temporary 
      * because ChunkRemoveFromList_New makes input CHUNK_IN_USE 
      * Also, parameter of ChunkMerge should be CHUNK_FREE      */
     ChunkFooterChunk(n)->back = c; 
     c = ChunkMerge(c, n);
   }
   f = ChunkFooterChunk(c);
   c->next = NULL;
   f->back = NULL;    // making c CHUNK_IN_USE

   ChunkInsertFirst(c);
   
   return;
}

#ifndef NDEBUG
/*--------------------------------------------------------------------*/
static int 
ChunkIsValid(Chunk_T c)
/* Return 1 (TRUE) iff c is valid */
{
   assert(c != NULL);
   assert(gHeapStart != NULL);
   assert(gHeapEnd != NULL);

   if (c < (Chunk_T)gHeapStart)
      {fprintf(stderr, "Bad heap start\n"); return 0; }
   if (c >= (Chunk_T)gHeapEnd)
      {fprintf(stderr, "Bad heap end\n"); return 0; }
   if (c->units == 0)
      {fprintf(stderr, "Zero units\n"); return 0; }
   return 1;
}

/*--------------------------------------------------------------------*/
int
ChunkValidityCheck(void)
{
  /* check the validity of all chunks */
   Chunk_T w;
   int i;
   if (gHeapStart == NULL) {
      fprintf(stderr, "Uninitialized heap start\n");
      return 0;
   }

   if (gHeapEnd == NULL) {
      fprintf(stderr, "Uninitialized heap end\n");
      return 0;
   }
   if (gHeapStart == gHeapEnd) {
     for(i=0; i<1024; i++){
       if (bin[i] != NULL){
	 fprintf(stderr, "Inconsistent empty heap\n");
	 return 0;
       }
     }
   }
   
   for (w = (Chunk_T)gHeapStart; 
        w && w < (Chunk_T)gHeapEnd;
        w = ChunkNextAdjacent(w)) {
     if (!ChunkIsValid(w)) 
	 return 0;
   }
   
   for (i=0; i<1023; i++){
     for (w = bin[i]; w && (w->next==bin[i]); w = w->next) {
       if (ChunkGetStatus(w) != CHUNK_FREE) {
         fprintf(stderr, "Non-free chunk in the free chunk list\n");
         return 0;
       }
       if (!ChunkIsValid(w))
         return 0;
       if (ChunkNextAdjacent(w) == w->next) {
         fprintf(stderr, "Uncoalesced chunks\n");
         return 0;
       }
       if (ChunkPrevAdjacent(w) == (ChunkFooterChunk(w)->back)){
	 fprintf(stderr, "Uncoalesced chunks\n");
	 return 0;
       }
       if (w->units != (i+1)){
	 fprintf(stderr, "Wrong Index Match %d\n %d", i, (int)(w->units));
	 return 0;
       }
       if (w->next == bin[i])
	 break;
     }
   }
   for (w = bin[1023]; w && (w->next==bin[1023]); w = w->next) {
     if (ChunkGetStatus(w) != CHUNK_FREE) {
       fprintf(stderr, "Non-free chunk in the free chunk list\n");
       return 0;
     }
     if (!ChunkIsValid(w))
       return 0;
     if (ChunkNextAdjacent(w) == w->next) {
       fprintf(stderr, "Uncoalesced chunks\n");
       return 0;
     }
     if (ChunkPrevAdjacent(w) == (ChunkFooterChunk(w)->back)){
       fprintf(stderr, "Uncoalesced chunks\n");
       return 0;
     }
     if (w->next == bin[i])
       break;
   }
   return 1;
}

/*--------------------------------------------------------------------*/
/*void
ChunkPrint(void)
{
   Chunk_T w, p = NULL;
   
   printf("heap: %p ~ %p\n", gHeapStart, gHeapEnd);
   printf("bin: %p\n", (void *)gFreeHead);
   
   printf("free chain\n");
   for (w = gFreeHead; w; w = w->next) {
      if (p == w) {
	printf("%p->next == %p, weird!\n", 
	       (void *)p, (void *)w); exit(0);
      }
      printf("[%p: %ld units]\n", (void *)w, w->units);
      p = w;

      if (w->next == gFreeHead)
	break;
   }
   
   printf("mem\n");
   for (w = (Chunk_T)gHeapStart;
        w < (Chunk_T)gHeapEnd;
        w = (Chunk_T)((char *)w + (w->units + 1) * CHUNK_UNIT))
     printf("[%p (%c): %ld units]\n", 
	    (void *)w, w->next ? 'F' : 'U', w->units);
	    }*/
#endif
