#include "nodes.h"

/*---- Generic Functions ----*/

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

bool insert_hash_node(hash_node *head, hash_node *new) {
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

hash_node *find_hash_node(hash_node *head, char *key) {
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

void destroy_hash_chain(hash_node *head) {
    while (head) {
        hash_node *tmp = head;
        head = head->next;
        destroy_hash_node(tmp);
    }
}

void destroy_hash_node(hash_node *node) {
    free(node->key);
    destroy_index_node(node->data);
    free(node);
}

/*---- Index Node Functions ----*/

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

index_node *insert_index_node(index_node *head, index_node *new) {
    if (head && new) {
        index_node *prev = NULL;
        for (index_node *current = head; current; current = current->next) {
            if (index_node_is_larger(new, current)) {
                if (prev) {
                    prev->next = new;
                    new->next = current;
                    return head;
                } else {
                    new->next = head;
                    return new;
                }
            }
            prev = current;
        }
    } else {
        return head;
    }
}

index_node *find_index_node(index_node *head, char *filename) {
    if (head && filename) {
        for (index_node *current = head; current; current = current->next) {
            if (!strcmp(current->filename, filename)) {
                return current;
            }
        }
    } else {
        return NULL;
    }
}

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

void destroy_index_chain(index_node *head) {
    while (head) {
        index_node *tmp = head;
        head = head->next;
        destroy_index_node(tmp);
    }
}
void destroy_index_node(index_node *node) {
    free(node->filename);
    free(node);
}
