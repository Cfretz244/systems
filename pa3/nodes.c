#include "nodes.h"

/*---- Hash Node Functions ----*/

hash_node *create_hash_node(char *key, index_node *data) {
    return NULL;
}

bool insert_hash_node(hash_node *head, hash_node *new) {
    return false;
}

hash_node *find_hash_node(hash_node *head, char *key) {
    return NULL;
}

hash_node *remove_hash_node(hash_node *head, char *key) {
    return NULL;
}

void destroy_hash_chain(hash_node *head) {
    
}

void destroy_hash_node(hash_node *node) {

}

/*---- Index Node Functions ----*/

index_node *create_index_node(char *filename) {
    return NULL;
}

bool insert_index_node(index_node *head, index_node *new) {
    return false;
}

index_node *find_index_node(index_node *head, char *filename) {
    return NULL;
}

index_node *remove_index_node(index_node *head, char *filename) {
    return NULL;
}

void destroy_index_chain(index_node *head) {

}
void destroy_index_node(index_node *node) {

}
