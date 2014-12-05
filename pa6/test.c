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

// Main testing function. Test is considered a pass if the program doesn't
// segfault, and if, at each call to print heap, the heap consists of only
// free space.
int main() {
    // For loop randomly fills and empties the heap 1000 times.
    for (int i = 0; i < 1000; i++) {
        array *arr = create_array();

        fill(arr);
        check_integrity(arr);
        empty(arr);

        destroy_array(arr, no_op);
    }

    print_heap();

    // For loop randomly fills the heap, then makes LOTS of random allocations and
    // deallocations, and then empties the heap and repeats 1000 times.
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

// Function makes sure that allocate is always handing out unique addresses.
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

// Function makes random allocations until the heap is full and fills all
// allocated sections with random data to ensure no memory corruption
// occurs.
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

// Function frees a random number of random addresses that have been allocated.
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

// Function calls free on all allocated pointers.
void empty(array *arr) {
    for (int i = 0; i < arr->size; i++) {
        void *tmp = retrieve(arr, i);
        if (tmp) {
            clear(arr, i);
            free(tmp);
        }
    }
}

// Function returns the first index of the given array
// that is guaranteed not to have any data after it.
int last_element(array *arr) {
    int last = 0;
    for (int i = 0; i < arr->size; i++) {
        if (retrieve(arr, i)) {
            last = i;
        }
    }
    return last + 1;
}

// Function does nothing and is passed to destroy_array
// as a destructor function.
void no_op(void *ignored) {
    return;
}
