#ifndef HASH
#define HASH

#include <stdlib.h>
#include <stdio.h>
#include "nodes.h"
#include "definitions.h"
#define START_SIZE 10

typedef struct {
    hash_node **data;
    int count;
    int size;
} hash;

hash *create_hash();
bool put(hash *table, char *key, index_node *data);
index_node *get(hash *table, char *key);
bool drop(hash *table, char *key);
void destroy_hash(hash *table);

#endif