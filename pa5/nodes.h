#ifndef NODES
#define NODES

#include <stdlib.h>
#include <string.h>
#include "definitions.h"
#include "business.h"

// Enum used for distinguishing between nodes.
typedef enum node_type {
    LIST,
    HASH
} node_type;

// hash_node struct represents a collection of all filenames that reference
// a particular token.
typedef struct hash_node {
    char *key;
    customer *data;
    struct hash_node *next;
} hash_node;

// list_node struct
typedef struct list_node {
    // stuff
} list_node;

bool nodes_are_equal(void *f, void *s, node_type type);

hash_node *create_hash_node(char *key, customer *data);
bool insert_hash_node(hash_node *head, hash_node *new);
hash_node *find_hash_node(hash_node *head, char *key);
hash_node *remove_hash_node(hash_node *head, char *key);
void destroy_hash_chain(hash_node *head);
void destroy_hash_node(hash_node *node);

#endif
