#ifndef LIST_H
#define LIST_H

/*----- Includes without dependencies -----*/

#include <stdlib.h>
#include <pthread.h>
#include "definitions.h"
#include "business.h"

/*----- Struct Declarations -----*/

#ifndef ORDER_DECLARE
#define ORDER_DECLARE
typedef struct order order;
#endif

// List Node Struct.
typedef struct list_node {
    order *data;
    struct list_node *next, *prev;
} list_node;

// List is part of a circular dependency with order and customer, and therefore
// must be declared differently based on which file we're currently processing.
// List struct represents a queue implemented using a doubly linked list.
#ifdef LIST_DECLARE
struct list {
#else
typedef struct list {
#endif
    list_node *head, *tail;
    int size;
    bool threaded;
    pthread_mutex_t *mutex;
    pthread_cond_t *signal;
#ifdef LIST_DECLARE
};
#else
#define LIST_DECLARE
} list;
#endif

/*----- Thread Safe List Functions -----*/

void lpush(list *lst, order *data);
order *rpop(list *lst);

/*----- Unsafe List Functions -----*/

list *create_list(bool threaded);
void destroy_list(list *lst);

/*----- List Node Functions -----*/

list_node *create_list_node(order *data);
void destroy_list_node(list_node *node);

#endif
