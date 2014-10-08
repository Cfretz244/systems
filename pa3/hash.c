#include "hash.h"

hash *create_hash() {
    hash *table = (hash *) malloc(sizeof(hash));

    if (table) {
        table->data = (hash_node **) malloc(sizeof(hash_node *) * START_SIZE);
        table->count = 0;
        table->size = START_SIZE;
    }

    return table;
}

int hash_key(char *key, int size) {
    int proto_hash = 0;
    for (int i = 0; i < strlen(key); i++) {
        proto_hash += (int) key[i];
    }
    return proto_hash % size;
}

void rehash(hash *table) {
    hash_node **old_data = table->data;
    hash_node **new_data = (hash_node **) malloc(sizeof(hash_node *) * (table->size * 2));
    for(int i = 0; i < table->size; i++) {
        hash_node *current = old_data[i];
        while (current) {
            hash_node *tmp = current->next;
            current->next = NULL;
            int hash = hash_key(current->key, table->size * 2);
            if (new_data[hash]) {
                insert_hash_node(new_data[hash], current);
            } else {
                new_data[hash] = current;
            }
            current = tmp;
        }
    }
    table->data = new_data;
    table->size *= 2;
    free(old_data);
}

bool put(hash *table, char *key, hash_node *data) {
    if (table->count / (float) table->size > 0.8) {
        rehash(table);
    }
    int hash = hash_key(key, table->size);
    if(table->data[hash]) {
        if (insert_hash_node(table->data[hash], data)) {
            table->count++;
            return true;
        } else {
            return false;
        }
    } else {
        table->data[hash] = data;
        table->count++;
        return true;
    }
}

hash_node *get(hash *table, char *key) {
    if (!table->count) {
        return NULL;
    }
    int hash = hash_key(key, table->size);
    return find_hash_node(table->data[hash], key);
}

bool drop(hash *table, char *key) {
    if (table->count == 0) {
        return false;
    }
    int hash = hash_key(key, table->size);
    if (find_hash_node(table->data[hash], key)) {
        table->data[hash] = remove_hash_node(table->data[hash], key);
        table->count--;
        return true;
    } else {
        return false;
    }
}

void destroy_hash(hash *table) {
    if (table->count > 0) {
        for (int i = 0; i < table->size; i++) {
            hash_node *node = table->data[i];
            if (node) {
                destroy_hash_chain(node);
            }
        }
    }
    free(table->data);
    free(table);
}
