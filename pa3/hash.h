#ifndef HASH
#define HASH

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "nodes.h"
#include "definitions.h"
#define START_SIZE 10

// Main hashtable struct.
typedef struct hash {
    hash_node **data;
    int count;
    int size;
} hash;

// Function declarations.
hash *create_hash();
bool put(hash *table, char *key, index_node *data);
index_node *get(hash *table, char *key);
bool update(hash *table, char *key, index_node *head);
char **get_keys(hash *table);
bool drop(hash *table, char *key);
void destroy_hash(hash *table);

#endif
