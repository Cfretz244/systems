#ifndef SORTED_LIST_H
#define SORTED_LIST_H

#include <stdlib.h>

// Function pointer typedefs. Don't typically like typedeffing pointers, but with
// function pointers it makes sense.
typedef int (*CompareFuncT)( void *, void * );
typedef void (*DestructFuncT)( void * );

// ListNode Struct
// Pointers field is used to keep track of how many iterators are currently parked at the
// node, and in_list field is used to keep track of whether or not a node is currently
// in a list. An example of a time when it wouldn't be is if SLRemove is called, targeting
// the node, when an iterator is parked at the node.
typedef struct ListNode {
    int pointers;
    int in_list;
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

// Iterator struct
// Started field is used to handle the edge case logic for when an iterator is just beginning
// its traversal.
typedef struct SortedListIterator {
    SortedList *list;
    ListNode *current;
    int started;
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
