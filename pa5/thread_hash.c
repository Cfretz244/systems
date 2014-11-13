#include "thread_hash.h"

// Function handles creation of a hash struct.
thread_hash *create_thread_hash() {
    thread_hash *table = (thread_hash *) malloc(sizeof(thread_hash));

    if (table) {
        // Allocate table with calloc to allow for NULL checks.
        table->data = (hash_node **) calloc(START_SIZE, sizeof(hash_node *));
        table->count = 0;
        table->size = START_SIZE;
        pthread_mutex_init(table->mutex, NULL);
    }

    return table;
}

// Function handles creation of a hash value for a given string.
int hash_key(char *key, int size) {
    int proto_hash = 0;
    for (int i = 0; i < strlen(key); i++) {
        proto_hash += (int) key[i];
    }
    return proto_hash % size;
}

// Function handles the rehash process encountered when a hash reaches
// 80% capacity.
void rehash(thread_hash *table) {
    // Allocate new table with calloc to allow for NULL checks.
    hash_node **old_data = table->data;
    hash_node **new_data = (hash_node **) calloc(table->size * 2, sizeof(hash_node *));

    // Copy all previous data into new, larger, hash.
    for(int i = 0; i < table->size; i++) {
        hash_node *current = old_data[i];
        while (current) {
            hash_node *tmp = current->next;
            current->next = NULL;

            // Calculate new hash value and insert.
            int hash = hash_key(current->key, table->size * 2);
            if (new_data[hash]) {
                insert_hash_node(new_data[hash], current);
            } else {
                new_data[hash] = current;
            }
            current = tmp;
        }
    }

    // Update hash struct with changes.
    table->data = new_data;
    table->size *= 2;
    free(old_data);
}

// Insert data into a hash for a specific key.
bool put(thread_hash *table, char *key, customer *data) {
    // Verify parameters.
    if (!table || !key || !data) {
        return false;
    }

    // Locking the mutex this early, and for this long, makes me sad,
    // but I don't see too many ways around it without introducing small
    // race conditions.
    pthread_mutex_lock(table->mutex);

    // Check if table needs a rehash.
    if (table->count / (float) table->size > 0.8) {
        rehash(table);
    }

    // Generate hash value and insert.
    int hash = hash_key(key, table->size);

    // Verify that table does not already contain given key.
    if (table->data[hash]) {
        // Check if we're dealing with a hash collision, or a repeat
        // key.
        if (!find_hash_node(table->data[hash], key)) {
            // Data is new.
            hash_node *node = create_hash_node(key, data);
            insert_hash_node(table->data[hash], node);
            table->count++;
            pthread_mutex_unlock(table->mutex);
            return true;
        } else {
            // Key already exists in table.
            pthread_mutex_unlock(table->mutex);
            return false;
        }
    } else {
        // Insert new data into table.
        hash_node *node = create_hash_node(key, data);
        table->data[hash] = node;
        table->count++;
        pthread_mutex_unlock(table->mutex);
        return true;
    }
}

// Function handles getting data out of a hash for a specific key.
customer *get(thread_hash *table, char *key) {
    // Verify parameters.
    if (!table || !table->count || !key) {
        return NULL;
    }

    pthread_mutex_lock(table->mutex);
    
    // Generate hash value and find data.
    int hash = hash_key(key, table->size);
    hash_node *found = find_hash_node(table->data[hash], key);

    pthread_mutex_unlock(table->mutex);
    if (found) {
        return found->data;
    } else {
        return NULL;
    }

}

// Handle removal of a key from hash. Although never actually called in the
// project, it seemed dishonest not to include it.
bool drop(thread_hash *table, char *key) {
    // Verify parameters.
    if (!table || table->count == 0 || !key) {
        return false;
    }

    pthread_mutex_lock(table->mutex);

    // Generate hash value and find data.
    int hash = hash_key(key, table->size);
    if (table->data[hash]) {
        if (find_hash_node(table->data[hash], key)) {
            // Remove appropriate data.
            table->data[hash] = remove_hash_node(table->data[hash], key);
            table->count--;
            pthread_mutex_unlock(table->mutex);
            return true;
        } else {
            // Key does not exist in table.
            pthread_mutex_unlock(table->mutex);
            return false;
        }
    } else {
        // Key does not exist in table.
        pthread_mutex_unlock(table->mutex);
        return false;
    }
}

// Function handles the enumeration of all keys currently stored in hash.
// Returns said keys in any order.
char **get_keys(thread_hash *table) {
    if (!table) {
        return NULL;
    }
    int current = 0;
    char **keys = (char **) malloc(sizeof(char *) * table->count);

    pthread_mutex_lock(table->mutex);

    for (int i = 0; i < table->size; i++) {
        if (table->data[i]) {
            for (hash_node *tmp = table->data[i]; tmp; tmp = tmp->next) {
                keys[current] = tmp->key;
                current++;
            }
        }
    }

    pthread_mutex_unlock(table->mutex);
    
    return keys;
}

// Function handles the destruction of hash struct.
void destroy_thread_hash(thread_hash *table) {
    // Verify parameters.
    if (!table) {
        return;
    }

    if (table->count > 0) {
        // Destroy all necessary data.
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
