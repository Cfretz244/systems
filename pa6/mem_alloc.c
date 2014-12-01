#include "mem_alloc.h"

/*----- Internal Function Declarations -----*/

void set_block_size(char *header, size_t size);
size_t get_block_size(char *header);
void set_block_status(char *header, bool used);
inline int size_of_header();
inline void panic_full(char *reason, const char *file, const int line);
inline void panic(char *reason);

/*----- Memory Allocation Functions -----*/

void *allocate(size_t size, const char *file, const int line) {
    static bool initialized = false;
    static char memory[MAX_MEMORY];

    if (!initialized) {
        initialized = true;
        memset(memory, 0, sizeof(char) * MAX_MEMORY);
        set_block_size(memory, MAX_MEMORY);
        set_block_status(memory, false);
    }

    if (size == 0 || size > MAX_MEMORY) {
        panic_full("User passed bad value", file, line);
    }

    // Actual work goes here.
}

void deallocate(void *ptr, const char *file, const int line) {
    // Cool stuff to come.
}

/*----- Internal Function Implementations -----*/

void set_block_size(char *header, size_t size) {
    int head_len = size_of_header(), input_len = sizeof(size_t);
    int diff = input_len - head_len;
    if (diff < 0) {
        panic("MAX_MEMORY constant is too large");
    }
    char *input = (char *) &size;
    memcpy(header, input + diff, head_len);
}

size_t get_block_size(char *header) {
    int head_len = size_of_header(), input_len = sizeof(size_t);
    int diff = input_len - head_len;
    if (diff < 0) {
        panic("MAX_MEMORY constant is too large");
    }
    size_t size = 0;
    memcpy(&size + diff, header, head_len);
    return size;
}

void set_block_status(char *header, bool used) {
    int length = size_of_header();
    *(header + (length - 1)) |= used;
}

inline int size_of_header() {
    int power = 0, total = 1;
    while (total < MAX_MEMORY) {
        power++;
        total *= 2;
    }
    return (int) ceil((float) power / 8);
}

inline void panic_full(char *reason, const char *file, const int line) {
    fprintf(stderr, "Encountered error [%s] at line #%d in file %s.", reason, line, file);
    exit(EXIT_FAILURE);
}

inline void panic(char *reason) {
    fprintf(stderr, "Encountered error [%s].", reason);
    exit(EXIT_FAILURE);
}
