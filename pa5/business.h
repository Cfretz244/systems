#ifndef BUSINESS_H
#define BUSINESS_H

/*----- Includes without dependencies -----*/

#include <stdlib.h>
#include "definitions.h"
#include "array.h"
#include "list.h"

/*----- Numerical Constants -----*/

// I was told that, for the purposes of the project, although a linked list
// obviously has no maximum size, I was required to enforce one.
#define CONSUMER_QUEUE_SIZE 8

/*----- Struct Declarations -----*/

#ifndef LIST_DECLARE
#define LIST_DECLARE
typedef struct list list;
#endif

// Order is part of a circular dependency with list and customer, and therefore
// must be declared differently based on while file we're processing.
// Order struct represents a specific order read out of the order file.
#ifdef ORDER_DECLARE
struct order {
#else
typedef struct order {
#endif
    char *title, *category;
    float price, remaining;
    int id;
#ifdef ORDER_DECLARE
};
#else
#define ORDER_DECLARE
} order;
#endif

// Customer Struct represents a single customer read out of the database
// file.
typedef struct customer {
    char *name, *street, *state, *zip;
    int id;
    float start_credit, credit;
    list *approved, *rejected;
    pthread_mutex_t *mutex;
} customer;

// Consumer struct represents a single consumer thread and its work
// queue.
typedef struct consumer {
    pthread_t *thread;
    char *category;
    list *queue;
} consumer;

// Convenience struct for passing arguments to consumer threads.
typedef struct void_args {
    array *users;
    list *queue;
} void_args;

/*----- Customer Functions -----*/

customer *create_customer(char *name, char *street, char *state, char *zip, int id, float credit);
bool customers_are_equal(customer *f, customer *s);
void destroy_customer(customer *user);

/*----- Order Functions -----*/

order *create_order(char *title, char *category, float price, int id);
bool orders_are_equal(order *f, order *s);
void destroy_order(order *book);

/*----- Customer Functions -----*/

consumer *create_consumer(void *(*thread_func) (void *), char *category, array *users);
void destroy_consumer(consumer *worker);

#endif
