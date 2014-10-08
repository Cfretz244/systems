#ifndef HASH
#define HASH

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "nodes.h"
#include "definitions.h"
#define START_SIZE 10

typedef struct {
    hash_node **data;
    int count;
    int size;
} hash;

hash *create_hash();
bool put(hash *table, char *key, hash_node *data);
hash_node *get(hash *table, char *key);
bool drop(hash *table, char *key);
void destroy_hash(hash *table);

#endif
