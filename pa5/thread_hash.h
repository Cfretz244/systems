#ifndef HASH_H
#define HASH_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "definitions.h"
#include "business.h"
#define START_SIZE 10

typedef enum {
    CUSTOMER,
    CONSUMER
} node_type;

// Node struct used for chaining in thread_hash.
typedef struct hash_node {
    char *key;
    void *data;
    node_type type;
    struct hash_node *next;
} hash_node;

// Threadsafe Hashtable Struct.
typedef struct thread_hash {
    hash_node **data;
    int count;
    int size;
    pthread_mutex_t *mutex;
} thread_hash;

/*----- Thread Safe Hash Functions -----*/

bool put(thread_hash *table, char *key, void *data, node_type type);
void *get(thread_hash *table, char *key);
char **get_keys(thread_hash *table);
bool drop(thread_hash *table, char *key);

/*----- Unsafe Hash Functions -----*/

thread_hash *create_thread_hash();
void destroy_thread_hash(thread_hash *table);

/*----- Hash Node Functions -----*/

hash_node *create_hash_node(char *key, void *data, node_type type);
bool insert_hash_node(hash_node *head, hash_node *insert);
hash_node *find_hash_node(hash_node *head, char *key);
hash_node *remove_hash_node(hash_node *head, char *key);
void destroy_hash_chain(hash_node *head);
void destroy_hash_node(hash_node *node);

#endif
