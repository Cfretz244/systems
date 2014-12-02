#include "mem_alloc.h"

int main() {
    int count = 1;
    void *ptr = malloc(count);
    while (ptr) {
        count *= 2;
        ptr = malloc(count);
    }
}
