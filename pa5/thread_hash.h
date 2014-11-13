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
typedef struct hash {
    hash_node **data;
    int count;
    int size;
    pthread_mutex_t *mutex;
} hash;

/*----- Thread Safe Functions -----*/

bool put(hash *table, char *key, customer *data);
customer *get(hash *table, char *key);
char **get_keys(hash *table);
bool drop(hash *table, char *key);

/*----- Unsafe Functions -----*/

hash *create_hash();
void destroy_hash(hash *table);

#endif
