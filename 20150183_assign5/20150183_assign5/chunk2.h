/*--------------------------------------------------------------------*/
/* Assignment 5                                                       */
/* Filename : chunk2.h                                                */
/* Author: Kim Yunseong                                               */
/* Student ID : 20150183                                              */
/*--------------------------------------------------------------------*/

#ifndef _CHUNK_BASE_H_
#define _CHUNK_BASE_H_

#pragma once

#include <stdbool.h>
#include <unistd.h>


typedef struct Chunk* Chunk_T;
typedef struct Footer* Footer_T;

struct Chunk{
  Chunk_T next;
  size_t units;
};
struct Footer{
  Chunk_T back;
  size_t units;
};

enum{
  BIN_SIZE = 1024,
};

enum {
   CHUNK_FREE,
   CHUNK_IN_USE,
};

enum {
   CHUNK_UNIT = 16, /* 16 = sizeof(struct Chunk) */
};

Chunk_T bin[BIN_SIZE];

/* ChunkFirstFreeChunk:
 * Returns the first chunk in the free chunk list.
 * Returns NULL if there is no free chunk. */
Chunk_T
ChunkNextFreeChunk(Chunk_T c);

/* ChunkRemoveFromList_New:
 * Removes 'c' from the free chunk list.
 * It needs no prev 
 * If 'c' is the only free chunk in the list, prev should be 'c'
 */
void
ChunkRemoveFromList_New(Chunk_T c);

/* ChunkInsertFirst:
 * Insert a chunk, 'c', into the head of the free chunk list.
 * It attempts to merge with the next adjacent chunk if possible. */
void
ChunkInsertFirst(Chunk_T c);

/* ChunkGetUnits:
 * get units value of Chunk C
 */
int
ChunkGetUnits(Chunk_T c);

/* ChunkNextAdjacent:
 * Returns the next adjacent chunk to 'c' in memory space.
 * Returns NULL if 'c' is the last chunk in memory space. */
Chunk_T
ChunkNextAdjacent(Chunk_T c);

/* ChunkPrevAdjacent:
 * Returns the previous adjacent chunk to 'c' in memory space.
 * Returns NULL if 'c' is the first chunk in memory space. */
Chunk_T
ChunkPrevAdjacent(Chunk_T c);

/* ChunkGetStatus:
 * Returns a chunk's status which shows whether the chunk is in use or free.
 * Return value is either CHUNK_IN_USE or CHUNK_FREE. */
int
ChunkGetStatus(Chunk_T c);

/* ChunkAllocateMem: 
 * Allocate a new chunk which is capable of holding 'units' chunk
 * units in memory by expanding the heap, and return the new
 * chunk. This function also performs chunk merging if possible after
 * new chunk allocation. */
Chunk_T
ChunkAllocateMem(int units);

/* ChunkSplit:
 * Shrink 'c' to 'units' and return a new chunk with the reduced space.
 * Note that 'c' must be large enough to be split into two.
 * The status of the allocated chunk is set to CHUNK_IN_USE.
 * Returns the allocted chunk. */
Chunk_T 
ChunkSplit(Chunk_T c, int units);

/* ChunkMerge:
 * Merge two adjacent chunks and return the merged chunk.
 * Note that (1) c1 < c2, (2) they should be adjacent, and (3) they
 * should both be free chunks (with valid next pointers).
 * Returns the merged chunk pointer. */
Chunk_T
ChunkMerge(Chunk_T c1, Chunk_T c2);

/* ChunkInitDS:
 * Initialize data structures and global variables for chunk management. */
void
ChunkInitDS(void);

/*--------------------------------------------------------------*/
/* ChunkFreeChunk:
 * Mark a chunk as free by inserting it to free list. */
/*--------------------------------------------------------------*/
void
ChunkFreeChunk(Chunk_T c);

/* Following two functions are for debugging.
 * These will be removed by C preprocessor if you compile the code with
 * -DNDEBUG option. */

#ifndef NDEBUG

/* ChunkValidityCheck:
 * Validity check for entire data structures for chunks. Note that this
 * is basic sanity check, and passing this test does not guarantee the
 * integrity of your code. 
 * Returns 'true' (non-zero value) on success, 'false' (zero) on failure. */
int
ChunkValidityCheck(void);

/* ChunkPrint:
 * Print out entire data sturucture for chunks. This function will print
 * all chunks in free chunks list and in memory. */
void
ChunkPrint(void);

#endif /* NDEBUG */

#endif /* _CHUNK_BASE_H_ */
