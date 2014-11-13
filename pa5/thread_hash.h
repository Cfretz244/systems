#ifndef HASH_H
#define HASH_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "hash_node.h"
#include "definitions.h"
#define START_SIZE 10

// Main hashtable struct.
typedef struct thread_hash {
    hash_node **data;
    int count;
    int size;
    pthread_mutex_t *mutex;
} thread_hash;

/*----- Thread Safe Functions -----*/

bool put(thread_hash *table, char *key, customer *data);
customer *get(thread_hash *table, char *key);
char **get_keys(thread_hash *table);
bool drop(thread_hash *table, char *key);

/*----- Unsafe Functions -----*/

thread_hash *create_thread_hash();
void destroy_thread_hash(thread_hash *table);

#endif
