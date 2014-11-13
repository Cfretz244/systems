#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "definitions.h"
#include "business.h"

#ifndef ORDER_DECLARE
#define ORDER_DECLARE
typedef struct order order;
#endif

// List Node Struct.
typedef struct list_node {
    order *data;
    struct list_node *next, *prev;
} list_node;

// Circularly dependent List struct declaration.
// I know I probably should have just designed things differently,
// but I wanted experience resolving a circular dependency.
#ifdef LIST_DECLARE
struct list {
#else
typedef struct list {
#endif
    list_node *head, *tail;
    int size;
    bool threaded;
    pthread_mutex_t *mutex;
#ifdef LIST_DECLARE
};
#else
#define LIST_DECLARE
} list;
#endif

/*----- Thread Safe Functions -----*/

void lpush(list *lst, order *data);
order *rpop(list *lst);

/*----- Unsafe Functions -----*/

list *create_list(bool threaded);
void destroy_list(list *lst);

list_node *create_list_node(order *data);
void destroy_list_node(list_node *node);

#endif
