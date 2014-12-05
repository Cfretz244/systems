#include "mem_alloc.h"
#include "array.h"
#include "assert.h"

/*----- Debugging Helper Function Declarations -----*/

void check_integrity(array *arr);
void fill(array *arr);
void rand_dump(array *arr);
void empty(array *arr);
int last_element(array *arr);
void no_op(void *ignored);

/*----- Debugging Main -----*/

int main() {
    for (int i = 0; i < 1000; i++) {
        array *arr = create_array();

        fill(arr);
        check_integrity(arr);
        empty(arr);

        destroy_array(arr, no_op);
    }

    print_heap();

    for (int i = 0; i < 1000; i++) {
        array *arr = create_array();

        for (int j = 0; j < 1000; j++) {
            fill(arr);
            check_integrity(arr);
            rand_dump(arr);
        }
        empty(arr);

        destroy_array(arr, no_op);
    }

    print_heap();
}

/*----- Debugging Helper Function Implementations -----*/

void check_integrity(array *arr) {
    for (int i = 0; i < arr->count; i++) {
        void *elem = retrieve(arr, i);
        if (elem) {
            for (int k = i + 1; k < arr->count; k++) {
                void *tmp = retrieve(arr, k);
                assert(tmp != elem);
            }
        }
    }
}

void fill(array *arr) {
    for (int i = last_element(arr); true; i++) {
        int size = (rand() % 20) + 1;
        void *ptr = malloc(size);

        if (ptr) {
            for (int j = 0; j < size; j++) {
                char *tmp = ptr;

                if (j == size - 1) {
                    tmp[j] = '\0';
                } else {
                    tmp[j] = i + '0';
                }
            }
            insert(arr, i, ptr);
        } else {
            break;
        }
    }
}

void rand_dump(array *arr) {
    int dump = rand() % arr->count;

    for (int i = 0; i < dump; i++) {
        int chosen = rand() % arr->count;
        void *elem = retrieve(arr, chosen);
        if (elem) {
            clear(arr, chosen);
            free(elem);
        }
    }
}

void empty(array *arr) {
    for (int i = 0; i < arr->size; i++) {
        void *tmp = retrieve(arr, i);
        if (tmp) {
            clear(arr, i);
            free(tmp);
        }
    }
}

int last_element(array *arr) {
    int last = 0;
    for (int i = 0; i < arr->size; i++) {
        if (retrieve(arr, i)) {
            last = i;
        }
    }
    return last + 1;
}

void no_op(void *ignored) {
    return;
}
