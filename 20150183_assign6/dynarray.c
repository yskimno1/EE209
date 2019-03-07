/*--------------------------------------------------------------------*/
/* dynarray.c                                                         */
/* Author: Bob Dondero                                                */
/* Modified by Younghwan Go                                           */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>

#include "dynarray.h"

enum { MIN_PHYS_LENGTH = 2 };
enum { GROWTH_FACTOR = 2 };

/*--------------------------------------------------------------------*/
/* A DynArray consists of an array, along with its logical and
   physical lengths. */

/*--------------------------------------------------------------------*/
/* Check the invariants of oDynArray.  Return 1 (TRUE) iff oDynArray
   is in a valid state. */
#ifndef NDEBUG
static int DynArray_isValid(DynArray_T oDynArray)
{
	if (oDynArray->iLength < 0) return 0;
	if (oDynArray->iPhysLength < MIN_PHYS_LENGTH) return 0;
	if (oDynArray->iLength > oDynArray->iPhysLength) return 0;
	if (oDynArray->ppvArray == NULL) return 0;
	return 1;
}
#endif
/*--------------------------------------------------------------------*/
/* Return a new DynArray_T object whose length is iLength, or
   NULL if insufficient memory is available. */
DynArray_T DynArray_new(int iLength)
{
	DynArray_T oDynArray;
	
	assert(iLength >= 0);
	
	oDynArray = (struct DynArray*)malloc(sizeof(struct DynArray));
	if (oDynArray == NULL)
		return NULL;
	
	oDynArray->iLength = iLength;
	if (iLength > MIN_PHYS_LENGTH)
		oDynArray->iPhysLength = iLength;
	else
		oDynArray->iPhysLength = MIN_PHYS_LENGTH;
	
	oDynArray->ppvArray =
		(void**)calloc((size_t)oDynArray->iPhysLength,
							 sizeof(void*));
	if (oDynArray->ppvArray == NULL) {
		free(oDynArray);
		return NULL;
	}
	
	return oDynArray;
}
/*--------------------------------------------------------------------*/
/* Free oDynArray. */
void DynArray_free(DynArray_T oDynArray)
{
	assert(oDynArray != NULL);
	assert(DynArray_isValid(oDynArray));
	//void* pvtemp = oDynArray->ppvArray;
	int count = 0;
	for(count=0; count < oDynArray->iLength; count++){
	  if(oDynArray->ppvArray[count] !=NULL)
	    free(oDynArray->ppvArray[count]);
	}
	free(oDynArray->ppvArray);
	free(oDynArray);
}
/*--------------------------------------------------------------------*/
/* Return the length of oDynArray. */
int DynArray_getLength(DynArray_T oDynArray)
{
	assert(oDynArray != NULL);
	assert(DynArray_isValid(oDynArray));
	
	return oDynArray->iLength;
}
/*--------------------------------------------------------------------*/
/* Return the iIndex'th element of oDynArray. */
void *DynArray_get(DynArray_T oDynArray, int iIndex)
{
	assert(oDynArray != NULL);
	assert(iIndex >= 0);
	assert(iIndex < oDynArray->iLength);
	assert(DynArray_isValid(oDynArray));
	
	return (void*)(oDynArray->ppvArray)[iIndex];
}
/*--------------------------------------------------------------------*/
/* Increase the physical length of oDynArray.  Return 1 (TRUE) if
   successful and 0 (FALSE) if insufficient memory is available. */
static int DynArray_grow(DynArray_T oDynArray)
{
	int iNewLength;
	void **ppvNewArray;
	
	assert(oDynArray != NULL);
	
	iNewLength = oDynArray->iPhysLength * GROWTH_FACTOR;
	
	ppvNewArray = (void**)
		realloc(oDynArray->ppvArray, sizeof(void*) * iNewLength);
	if (ppvNewArray == NULL)
		return 0;
	
	oDynArray->iPhysLength = iNewLength;
	oDynArray->ppvArray = ppvNewArray;

	return 1;
}
/*--------------------------------------------------------------------*/
/* Add pvElement to the end of oDynArray, thus incrementing its length.
   Return 1 (TRUE) if successful, or 0 (FALSE) if insufficient memory
   is available. */
int DynArray_add(DynArray_T oDynArray, void *pvElement)
{
	assert(oDynArray != NULL);
	assert(DynArray_isValid(oDynArray));
	
	if (oDynArray->iLength == oDynArray->iPhysLength)
		if (!DynArray_grow(oDynArray))
			return 0;
	
	oDynArray->ppvArray[oDynArray->iLength] = pvElement;
	oDynArray->iLength++;
	
	assert(DynArray_isValid(oDynArray));
	
	return 1;
}
