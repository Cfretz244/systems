#include <string.h>
#include "sorted-list.h"

// Function creates a ListNode struct
ListNode *LNCreate(void *data) {
    ListNode *node = (ListNode *) malloc(sizeof(ListNode));

    if (node) {
        node->pointers = 0;
        node->in_list = 1;
        node->data = data;
        node->next = NULL;
    }

    return node;
}

// Destroys a list node struct. Function calls destructor provided to SLCreate.
// Should only be called after the node has been safely removed from the list.
void LNDestroy(ListNode *node, DestructFuncT destruct) {
    destruct(node->data);
    free(node);
}

// Function creates a SortedList struct
SortedList *SLCreate(CompareFuncT cf, DestructFuncT df) {
    SortedList *list = (SortedList *) malloc(sizeof(SortedList));

    if (list) {
        list->head = NULL;
        list->size = 0;
        list->comparator = cf;
        list->destructor = df;
    }

    return list;
}

// Function destroys a SortedList struct
// Iterates across entire list calling LNDestroy as it goes.
void SLDestroy(SortedList *list) {
    ListNode *current = list->head;
    while (current) {
        ListNode *tmp = current;

        // Advance local pointer before freeing node.
        current = current->next;

        LNDestroy(tmp, list->destructor);
    }
    free(list);
}

int SLInsert(SortedList *list, void *newObj) {
    if (!newObj) {
        // We've been passed a null pointer.
        return 0;
    } else if (!list->head) {
        // Handle edge case of inserting into an empty list.
        ListNode *head = LNCreate(newObj);
        list->head = head;
        list->size++;
        return 1;
    }

    ListNode *current = list->head, *prev = NULL;
    CompareFuncT compare = list->comparator;
    while (current) {
        // Check if our current position is right for the given data.
        if (compare(newObj, current->data) >= 0) {
            ListNode *node = LNCreate(newObj);
            if (!prev) {
                // Handle situation where given data needs to be inserted
                // at the front of the list.
                node->next = list->head;
                list->head = node;
            } else {
                // Handle typical insertion.
                prev->next = node;
                node->next = current;
            }
            
            // Increment list size.
            list->size++;

            return 1;
        }
        // Advance pointers.
        prev = current;
        current = current->next;
    }

    // Code inserts given data at the end of the list if no suitable spot
    // was found.
    ListNode *node = LNCreate(newObj);
    prev->next = node;

    // Increment list size.
    list->size++;
    return 1;
}

// Function removes given data from list if present
int SLRemove(SortedList *list, void *newObj) {
    if (!newObj) {
        // We've been passed a null pointer.
        return 0;
    }

    ListNode *current = list->head, *prev = NULL;
    CompareFuncT compare = list->comparator;
    while (current) {
        // Check if we've found the right node.
        if (compare(newObj, current->data) == 0) {
            // Let any possible iterators know that this node has been removed
            // from the list.
            current->in_list = 0;
            if (!prev) {
                // Handle edge case where the list head needs to be removed.
                list->head = list->head->next;
            } else {
                // Handle a typical removal.
                prev->next = current->next;
            }

            // Check if node can be safely destroyed (i.e. there are no iterators
            // parked on node).
            if (!current->pointers) {
                LNDestroy(current, list->destructor);
            }

            // Decrement list size.
            list->size--;

            return 1;
        }

        // Advance pointers.
        prev = current;
        current = current->next;
    }

    return 0;
}

// Function handles the creation of an iterator struct
SortedListIterator *SLCreateIterator(SortedList *list) {
    SortedListIterator *iterator = (SortedListIterator *) malloc(sizeof(SortedListIterator));

    if (iterator) {
        iterator->list = list;
        iterator->current = list->head;
        iterator->started = 0;
    }

    return iterator;
}

// Function finds the next node in a traversal in the case that the node an
// iterator was parked on was removed from the list.
ListNode *SLFindNext(SortedList *list, ListNode *old) {
    CompareFuncT compare = list->comparator;
    ListNode *current = list->head;
    while (current && compare(old->data, current->data) < 0) {
        current = current->next;
    }
    return current;
}

// Function advances the given iterator and returns the next element in
// the list
void *SLNextItem(SortedListIterator *iter) {
    if (!iter->list->size) {
        return NULL;
    }
    if (!iter->started) {
        // Iteration has only just been allocated. Begin traversal.
        iter->started = 1;
        if (!iter->current) {
            iter->current = iter->list->head;
        }
        void *data = iter->current->data;
        iter->current = iter->current->next;
        iter->current->pointers++;
        return data;
    } else if (!iter->current) {
        // Iteration is already over. Return NULL.
        return NULL;
    }

    ListNode *old = iter->current;

    // Iterator is no longer parked on this node. Decrement pointer count
    // to reflect this.
    old->pointers--;
    if (old->in_list) {
        // Node is still in the list, so we can trust node->next field to
        // lead to the correct node.
        iter->current = old->next;
    } else {
        // Node is no longer contained in the list, so we call SLFindNext
        // to find the next node in the traversal.
        iter->current = SLFindNext(iter->list, old);

        // Check if we're the only iterator that was parked on this node,
        // and destroy it if so.
        if (!old->pointers) {
            LNDestroy(old, iter->list->destructor);
        }
    }
    if (iter->current) {
        // We are now parked on a new node, so increment pointer count
        // to reflect this.
        iter->current->pointers++;
        return iter->current->data;
    } else {
        return NULL;
    }
}

// Function returns data for the node iterator is currently parked on
void *SLGetItem(SortedListIterator *iter) {
    if (!iter->list->size) {
        return NULL;
    }
    if (!iter->started) {
        iter->started = 1;
        iter->current = iter->list->head;
        iter->current->pointers++;
    }
    return iter->current->data;
}

// Function destroys an iterator. As an iterator struct doesn't actually
// own any of the pointers it keeps track of, we simply call free.
void SLDestroyIterator(SortedListIterator *iter) {
    free(iter);
}
