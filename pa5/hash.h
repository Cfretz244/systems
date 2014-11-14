#ifndef HASH
#define HASH

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "definitions.h"
#define START_SIZE 10

// Thread_hash stores opaque types, so this enum allows it to
// distinguish between them when it must.
typedef enum {
    CUSTOMER,
    CONSUMER
} node_type;

// Node struct used for chaining in hash.
typedef struct hash_node {
    char *key;
    void *data;
    node_type type;
    struct hash_node *next;
} hash_node;

// Main hashtable struct.
typedef struct hash {
    hash_node **data;
    int count;
    int size;
} hash;

/*----- Hash Functions -----*/

hash *create_hash();
bool put(hash *table, char *key, void *data, node_type type);
void *get(hash *table, char *key);
char **get_keys(hash *table);
bool drop(hash *table, char *key);
void destroy_hash(hash *table);

/*----- Hash Node Functions -----*/

hash_node *create_hash_node(char *key, void *data, node_type type);
bool insert_hash_node(hash_node *head, hash_node *insert);
hash_node *find_hash_node(hash_node *head, char *key);
hash_node *remove_hash_node(hash_node *head, char *key);
void destroy_hash_chain(hash_node *head);
void destroy_hash_node(hash_node *node);

/*----- Includes depdendent on hash declarations -----*/

#include "business.h"

#endif
