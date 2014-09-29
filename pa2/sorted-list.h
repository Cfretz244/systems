#ifndef SORTED_LIST_H
#define SORTED_LIST_H

#include <stdlib.h>

// Function pointer typedefs
typedef int (*CompareFuncT)( void *, void * );
typedef void (*DestructFuncT)( void * );

typedef struct ListNode {
    void *data;
    struct ListNode *next;
} ListNode;

typedef struct SortedList {
    ListNode *head;
    CompareFuncT comparator;
    DestructFuncT destructor;
} SortedList;

typedef SortedList* SortedListPtr;

typedef struct SortedListIterator {
    SortedList *head;
    SortedList *current;
} SortedListIterator;

typedef struct SortedListIterator* SortedListIteratorPtr;

// Create a list
SortedListPtr SLCreate(CompareFuncT cf, DestructFuncT df);

// Destory a list
void SLDestroy(SortedListPtr list);

// Insert a value into a list
int SLInsert(SortedListPtr list, void *newObj);

// Remove a value from a list
int SLRemove(SortedListPtr list, void *newObj);

// Create an iterator to traverse a list
SortedListIteratorPtr SLCreateIterator(SortedListPtr list);

// Destroy an iterator
void SLDestroyIterator(SortedListIteratorPtr iter);

// Returns the value the iterator is currently on
void * SLGetItem( SortedListIteratorPtr iter );

// Increments the iterator, and returns the next value.
void * SLNextItem(SortedListIteratorPtr iter);

#endif
