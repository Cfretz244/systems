#include "array.h"

/*----- Array Functions -----*/

array *create_array() {
    array *arr = (array *) malloc(sizeof(array));

    if (arr) {
        arr->storage = (void **) calloc(INIT_ARRAY_LENGTH, sizeof(void **));
        arr->size = INIT_ARRAY_LENGTH;
        arr->count = 0;;
    }

    return arr;
}

bool insert(array *arr, int index, void *data) {
    while (index >= arr->size) {
        int start_size = arr->size;
        arr->size *= 2;
        void **temp = (void **) realloc(arr->storage, sizeof(void *) * arr->size);
        if (temp) {
            memset(&temp[start_size], 0, (arr->size - start_size) * sizeof(void *));
            arr->storage = temp;
        } else {
            return false;
        }
    }

    if (!arr->storage[index]) {
        arr->storage[index] = data;
        arr->count++;
        return true;
    } else {
        return false;
    }
}

void *retrieve(array *arr, int index) {
    if (index < arr->size) {
        return arr->storage[index];
    } else {
        return NULL;
    }
}

void *clear(array *arr, int index) {
    if (index < arr->size) {
        void *temp = arr->storage[index];
        arr->storage[index] = NULL;
        arr->count--;
        return temp;
    } else {
        return NULL;
    }
}

void destroy_array(array *arr, void (*func) (void *)) {
    for (int i = 0; i < arr->size; i++) {
        if (arr->storage[i]) {
            func(arr->storage[i]);
        }
    }

    free(arr);
}
