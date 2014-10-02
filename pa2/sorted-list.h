#ifndef SORTED_LIST_H
#define SORTED_LIST_H

#include <stdlib.h>

// Function pointer typedefs
typedef int (*CompareFuncT)( void *, void * );
typedef void (*DestructFuncT)( void * );

// ListNode Struct
typedef struct ListNode {
    void *data;
    struct ListNode *next;
} ListNode;

// Sorted List Struct
typedef struct SortedList {
    ListNode *head;
    CompareFuncT comparator;
    DestructFuncT destructor;
} SortedList;

// I don't like typedeffing pointers like this. I know that the professor thinks that it leads
// to cleaner code, but I think that it obscures the fact that you're working with a pointer,
// and makes it more difficult for a third party to read through your code.
typedef SortedList* SortedListPtr;

typedef struct SortedListIterator {
    SortedList *list;
    ListNode *current;
} SortedListIterator;

// Once again, don't like typedeffing away pointer syntax.
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
void *SLGetItem( SortedListIteratorPtr iter );

// Increments the iterator, and returns the next value.
void *SLNextItem(SortedListIteratorPtr iter);

#endif
