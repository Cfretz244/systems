#include "nodes.h"

/*---- Generic Functions ----*/

// Function handles checking if two nodes of a single specified type are equal.
bool nodes_are_equal(void *f, void *s, node_type type) {
    if (type == INDEX) {
        index_node *first = (index_node *) f, *second = (index_node *) s;
        return strcmp(first->filename, second->filename) == 0 && first->count == second->count;
    } else {
        hash_node *first = (hash_node *) f, *second = (hash_node *) s;
        return strcmp(first->key, second->key) == 0 && nodes_are_equal(first->data, second->data, INDEX);
    }
}

/*---- Hash Node Functions ----*/

// Function handles the creation of a hash_node struct.
hash_node *create_hash_node(char *key, index_node *data) {
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
    destroy_index_node(node->data);
    free(node);
}

/*---- Index Node Functions ----*/

// Function handles the creation of an index_node struct.
index_node *create_index_node(char *filename) {
    index_node *node = (index_node *) malloc(sizeof(index_node));

    if (node) {
        char *intern_filename = (char *) malloc(sizeof(char) * (strlen(filename) + 1));
        if (intern_filename) {
            strcpy(intern_filename, filename);
            node->filename = intern_filename;
            node->count = 0;
            node->next = NULL;
        }
    }

    return node;
}

// Function handles the insertion of an index_node into a linked list of
// index_nodes, returning the new/old head.
index_node *insert_index_node(index_node *head, index_node *new) {
    // Validate parameters and traverse.
    if (head && new) {
        index_node *current = head, *prev = NULL;
        while (current) {
            if (index_node_is_larger(new, current)) {
                if (prev) {
                    // Node belongs somewhere in the middle of the list.
                    prev->next = new;
                    new->next = current;
                    return head;
                } else {
                    // Node must replace head at the beginning of the list.
                    new->next = head;
                    return new;
                }
            }
            prev = current;
            current = current->next;
        }
        prev->next = new;
    }
    return head;
}

// Function handles finding an index_node for a specific filename in a linked
// list of index_nodes.
index_node *find_index_node(index_node *head, char *filename) {
    // Validate parameters and search.
    if (head && filename) {
        for (index_node *current = head; current; current = current->next) {
            if (!strcmp(current->filename, filename)) {
                return current;
            }
        }
        return NULL;
    } else {
        return NULL;
    }
}

// Function handles the comparison of two index_nodes, and returns whether the
// first is larger than the second.
bool index_node_is_larger(index_node *node, index_node *other) {
    if (node && other) {
        if (node->count != other->count) {
            return node->count > other->count;
        } else {
            return strcmp(node->filename, other->filename) > 0;
        }
    } else {
        return false;
    }
}

// Function handles removing an index_node, specified by filename, from a
// linked list of index_nodes.
index_node *remove_index_node(index_node *head, char *filename) {
    if (head && filename) {
        index_node *prev = NULL;
        for (index_node *current = head; current; current = current->next) {
            if (!strcmp(current->filename, filename)) {
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

// Function handles destroying a linked list of index_nodes.
void destroy_index_chain(index_node *head) {
    while (head) {
        index_node *tmp = head;
        head = head->next;
        destroy_index_node(tmp);
    }
}

// Function handles destroying a single index_node.
void destroy_index_node(index_node *node) {
    free(node->filename);
    free(node);
}
