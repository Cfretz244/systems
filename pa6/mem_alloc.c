#include "mem_alloc.h"

/*----- Internal Function Declarations -----*/

void set_block_size(char *header, size_t size);
size_t get_block_size(char *header);
void set_block_status(char *header, bool used);
bool get_block_status(char *header);
int size_of_header();
size_t coerce_size(size_t size, bool up);
void panic_full(char *reason, const char *file, const int line);
void panic(char *reason);

/*----- Memory Allocation Functions -----*/

void *allocate(size_t size, const char *file, const int line) {
    static bool initialized = false;
    static char memory[MAX_MEMORY];

    if (size == 0 || size > MAX_MEMORY) {
        panic_full("User passed bad value", file, line);
    }

    size = coerce_size(size, true);

    if (!initialized) {
        initialized = true;
        memset(memory, 0, sizeof(char) * MAX_MEMORY);
        set_block_size(memory, MAX_MEMORY);
        set_block_status(memory, true);
    }

    // Actual work goes here.
}

void deallocate(void *ptr, const char *file, const int line) {
    // Cool stuff to come.
}

/*----- Internal Function Implementations -----*/

void set_block_size(char *header, size_t size) {
    int length = size_of_header();
    if (length == 1) {
        *header = (char) size;
    } else if (length == 2) {
        short *cast_header = (short *) header;
        *cast_header = (short) size;
    } else if (length == 4) {
        int *cast_header = (int *) header;
        *cast_header = (int) size;
    } else {
        size_t *cast_header = (size_t *) header;
        *cast_header = size;
    }
}

size_t get_block_size(char *header) {
    int length = size_of_header();
    if (length == 1) {
        return (size_t) *header;
    } else if (length == 2) {
        return (size_t) *((short *) header);
    } else if (length == 4) {
        return (size_t) *((int *) header);
    } else {
        return *((size_t *) header);
    }
}

void set_block_status(char *header, bool used) {
    int length = size_of_header();
    if (length == 1) {
        *header |= used;
    } else if (length == 2) {
        *((short *) header) |= used;
    } else if (length == 4) {
        *((int *) header) |= used;
    } else {
        *((size_t *) header) |= used;
    }
}

bool get_block_status(char *header) {
    int length = size_of_header();
    if (length == 1) {
        return *header & 1;
    } else if (length == 2) {
        return *((short *) header) & 1;
    } else if (length == 4) {
        return *((int *) header) & 1;
    } else {
        return *((size_t *) header) & 1;
    }
}

int size_of_header() {
    if (MAX_MEMORY <= SINGLE_BYTE_MAX) {
        return 1;
    } else if (MAX_MEMORY <= DOUBLE_BYTE_MAX) {
        return 2;
    } else if (MAX_MEMORY <= QUAD_BYTE_MAX) {
        return 4;
    } else {
        return sizeof(size_t);
    }
}

size_t coerce_size(size_t size, bool up) {
    if (size % MIN_ALLOC) {
        int next = MIN_ALLOC;
        while (next < size) {
            next += MIN_ALLOC;
        }
        return up ? next : next - MIN_ALLOC;
    } else {
        return size;
    }
}

void panic_full(char *reason, const char *file, const int line) {
    fprintf(stderr, "Encountered error [%s] at line #%d in file %s.", reason, line, file);
    exit(EXIT_FAILURE);
}

void panic(char *reason) {
    fprintf(stderr, "Encountered error [%s].", reason);
    exit(EXIT_FAILURE);
}
