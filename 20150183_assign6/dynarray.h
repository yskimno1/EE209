/*--------------------------------------------------------------------*/
/* dynarray.h                                                         */
/* Author: Bob Dondero                                                */
/* Modified by Younghwan Go                                           */
/*--------------------------------------------------------------------*/

#ifndef DYNARRAY_INCLUDED
#define DYNARRAY_INCLUDED

struct DynArray
{
  int iLength;
  int iPhysLength;

  void **ppvArray;
};
/* A DynArray_T is an array whose length can expand dynamically. */
typedef struct DynArray * DynArray_T;

/* Return a new DynArray_T object whose length is iLength, or
   NULL if insufficient memory is available. */
DynArray_T DynArray_new(int iLength);

/* Free oDynArray. */
void DynArray_free(DynArray_T oDynArray);

/* Return the length of oDynArray. */
int DynArray_getLength(DynArray_T oDynArray);

/* Return the iIndex'th element of oDynArray. */
void *DynArray_get(DynArray_T oDynArray, int iIndex);

/* Add pvElement to the end of oDynArray, thus incrementing its length.
   Return 1 (TRUE) if successful, or 0 (FALSE) if insufficient memory
   is available. */
int DynArray_add(DynArray_T oDynArray, void *pvElement);


#endif
