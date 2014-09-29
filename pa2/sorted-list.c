#include "sorted-list.h"

// Declare function in advance.
int SLTraverse(SortedList *list, void *func, void *data);

ListNode *LNCreate(void *data) {
    ListNode *node = (ListNode *) malloc(sizeof(ListNode));

    if (node != NULL) {
        node->flag = 1;
        node->data = data;
        node->next = NULL;
        node->opt1 = NULL;
        node->opt2 = NULL;
        node->opt3 = NULL;
    }

    return node;
}

// Destroys a list node struct. Should only be called after the node
// has been safely removed from the list.
void LNDestroy(ListNode *node) {
    DestructFuncT destruct = (DestructFuncT) node->opt3;
    destruct(node->data);
    free(node);
}

SortedList *SLCreate(CompareFuncT cf, DestructFuncT df) {
    SortedList *list = (SortedList *) malloc(sizeof(SortedList));

    if (list != NULL) {
        list->flag = 0;
        list->head = NULL;
        list->comparator = cf;
        list->destructor = df;
    }

    return list;
}

void SLDestroy(SortedList *list) {
    int *flag = (int *) list;
    if (*flag == 0) {
        // We're working with a SortedList struct
        SLTraverse(list, &SLDestroy, NULL);
        free(list);
    } else {
        // We're actually working with a ListNode struct
        ListNode *node = (ListNode *) list;
        LNDestroy(node);
    }
}

int SLInsert(SortedList *list, void *newObj) {
    // Check if we've been passed a null pointer.
    if (newObj == NULL) {
        return 0;
    }

    int *flag = (int *) list;
    if (*flag == 0) {
        // We're working with a SortedList struct.
        if (list->head == NULL) {
            list->head = LNCreate(newObj);
            return 1;
        } else {
            return SLTraverse(list, (void *) &SLInsert, newObj);
        }
    } else {
        // We're actually working with a ListNode struct.
        ListNode *current = (ListNode *) list, *prev = current->opt1;
        CompareFuncT compare = (CompareFuncT) current->opt2;
        if (compare(current->data, newObj) != 0) {
            ListNode *new = LNCreate(newObj);
            if (prev == NULL) {
                new->next = current;
                current->opt1 = (void *) new;
                current->opt2 = NULL;
            } else {
                prev->next = new;
                new->next = current;
            }
            return 1;
        } else {
            return 0;
        }
    }
}

int SLRemove(SortedList *list, void *newObj) {
    if (newObj == NULL) {
        return 0;
    }

    int *flag = (int *) list;
    if (*flag == 0) {
        // We're working with a SortedList struct.
        return SLTraverse(list, (void *) &SLRemove, newObj);
    } else {
        // We're actually working with a ListNode struct.
        ListNode *current = (ListNode *) list, *prev = (ListNode *) current->opt1;
        CompareFuncT compare = (CompareFuncT) current->opt2;
        if (compare(current->data, newObj) == 0) {
            prev->next = current->next;
            LNDestroy(current);
            return 1;
        } else {
            return 0;
        }
    }
}


int SLTraverse(SortedList *list, void *func, void *data) {
    ListNode *prev = NULL;
    ListNode *current = list->head;
    CompareFuncT compare = list->comparator;
    while (current != NULL) {
        ListNode *current_cpy = current, *prev_cpy = prev;
        prev = current;
        current = current->next;
        if (data == NULL) {
            // We've been called by SLDestroy
            void (*destroy) (SortedList *) = (void (*)(SortedList *)) func;
            current_cpy->opt3 = list->destructor;
            SortedList *current_node = (SortedList *) current_cpy;
            destroy(current_node);
            return 1;
        } else if (compare(data, current_cpy->data) >= 0) {
            // We've been called by either SLInsert or SLRemove
            int (*handle) (SortedList *, void *) = (int (*)(SortedList *, void *)) func;
            current_cpy->opt1 = (void *) prev_cpy;
            current_cpy->opt2 = (void *) compare;
            current_cpy->opt3 = (void *) list->destructor;
            SortedList *current_node = (SortedList *) current_cpy;
            int success = handle(current_node, data);
            if (prev_cpy == NULL && current_cpy->opt2 == NULL) {
                list->head = (ListNode *) current_cpy->opt1;
            }
            return success;
        }
    }
    if (compare(data, prev->data) != 0) {

    }
    return 0;
}


SortedListIterator *SLCreateIterator(SortedList *list) {
    return NULL;
}

void SLDestroyIterator(SortedListIterator *iter) {

}

void *SLGetItem(SortedListIterator *iter) {
    return NULL;
}

void *SLNextItem(SortedListIterator *iter) {
    return NULL;
}
