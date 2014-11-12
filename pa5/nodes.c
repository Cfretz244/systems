#include "nodes.h"

/*---- Generic Functions ----*/

// Function handles checking if two nodes of a single specified type are equal.
bool nodes_are_equal(void *f, void *s, node_type type) {
    if (type == LIST) {
        // cool stuff
    } else {
        hash_node *first = (hash_node *) f, *second = (hash_node *) s;
        return strcmp(first->key, second->key) == 0 && customers_are_equal(first->data, second->data);
    }
}

/*---- Hash Node Functions ----*/

// Function handles the creation of a hash_node struct.
hash_node *create_hash_node(char *key, customer *data) {
    hash_node *node = (hash_node *) malloc(sizeof(hash_node));

    if (node) {
        char *intern_key = (char *) malloc(sizeof(char) * (strlen(key) + 1));
        if (intern_key) {
            strcpy(intern_key, key);
            node->key = intern_key;
            node->data = data;
            node->next = NULL;
        }
    }

    return node;
}

// Function handles inserting a hash node into a linked list of hash nodes.
bool insert_hash_node(hash_node *head, hash_node *new) {
    // Validate paramaters and insert if the list doesn't already contain
    // the given node.
    if (head && new) {
        for (hash_node *current = head; current; current = current->next) {
            if (nodes_are_equal(current, new, HASH)) {
                return false;
            } else if(!current->next) {
                current->next = new;
                return true;
            }
        }
        return false;
    } else {
        return false;
    }
}

// Function handles finding hash_node with a specific key in a linked list
// of nodes.
hash_node *find_hash_node(hash_node *head, char *key) {
    // Validate parameters and search.
    if (head && key) {
        for (hash_node *current = head; current; current = current->next) {
            if (!strcmp(current->key, key)) {
                return current;
            }
        }
        return NULL;
    } else {
        return NULL;
    }
}

// Function handles removing a hash_node specified by key from a linked
// list of nodes.
hash_node *remove_hash_node(hash_node *head, char *key) {
    if (head && key) {
        hash_node *prev = NULL;
        for (hash_node *current = head; current; current = current->next) {
            if (!strcmp(current->key, key)) {
                if (prev) {
                    prev->next = current->next;
                    return head;
                } else {
                    return head->next;
                }
            }
            prev = current;
        }
    }
    return head;
}

// Function handles the destruction of an entire linked list of hash_nodes.
void destroy_hash_chain(hash_node *head) {
    while (head) {
        hash_node *tmp = head;
        head = head->next;
        destroy_hash_node(tmp);
    }
}

// Function handles the destruction of a specific hash_node struct.
void destroy_hash_node(hash_node *node) {
    free(node->key);
    destroy_customer(node->data);
    free(node);
}
