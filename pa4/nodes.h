#ifndef NODES
#define NODES

#include <stdlib.h>
#include <string.h>
#include "definitions.h"

// Enum used for distinguishing between nodes.
typedef enum node_type {
    INDEX,
    HASH
} node_type;

// index_node struct represents the current count for a specific token and filename.
typedef struct index_node {
    char *filename;
    int count;
    struct index_node *next;
} index_node;

// hash_node struct represents a collection of all filenames that reference
// a particular token.
typedef struct hash_node {
    char *key;
    index_node *data;
    struct hash_node *next;
} hash_node;

bool nodes_are_equal(void *f, void *s, node_type type);
int compare_nodes(index_node *f, index_node *s);

hash_node *create_hash_node(char *key, index_node *data);
bool insert_hash_node(hash_node *head, hash_node *new);
hash_node *find_hash_node(hash_node *head, char *key);
hash_node *remove_hash_node(hash_node *head, char *key);
void destroy_hash_chain(hash_node *head);
void destroy_hash_node(hash_node *node);

index_node *create_index_node(char *filename);
index_node *insert_index_node(index_node *head, index_node *new);
index_node *find_index_node(index_node *head, char *filename);
bool index_node_is_larger(index_node *node, index_node *other);
index_node *remove_index_node(index_node *head, char *filename);
void destroy_index_chain(index_node *head);
void destroy_index_node(index_node *node);

#endif
