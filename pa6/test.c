#include "mem_alloc.h"

int main() {
    void *start = malloc(1) - 2;
    void *ptrs[10];
    for (int i = 2; i < 12; i++) {
        int mem = 1;
        for (int k = 0; k < i; k++) {
            mem *= 2;
        }
        ptrs[i - 2] = malloc(mem);
    }

    print_heap(start);

    free(ptrs[1]);
    free(ptrs[3]);
    free(ptrs[5]);
    free(ptrs[7]);
    free(ptrs[9]);

    print_heap(start);

    free(ptrs[8]);
    print_heap(start);

    free(ptrs[6]);
    print_heap(start);

    free(ptrs[4]);
    print_heap(start);

    free(ptrs[2]);
    print_heap(start);

    free(ptrs[0]);
    print_heap(start);

    free(start + 2);
    print_heap(start);
}
