#ifndef HASH_NODE_H
#define HASH_NODE_H

#include <stdlib.h>
#include <string.h>
#include "definitions.h"
#include "business.h"

// hash_node struct represents a collection of all filenames that reference
// a particular token.
typedef struct hash_node {
    char *key;
    customer *data;
    struct hash_node *next;
} hash_node;

bool hash_nodes_are_equal(hash_node *f, hash_node *s);
hash_node *create_hash_node(char *key, customer *data);
bool insert_hash_node(hash_node *head, hash_node *new);
hash_node *find_hash_node(hash_node *head, char *key);
hash_node *remove_hash_node(hash_node *head, char *key);
void destroy_hash_chain(hash_node *head);
void destroy_hash_node(hash_node *node);

#endif
