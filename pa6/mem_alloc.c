#include "mem_alloc.h"

/*----- Evil Global Declarations -----*/

static bool initialized = false;
static char memory[MAX_MEMORY];
static short host_hash;

/*----- Internal Function Declarations -----*/

char *find_best_fit(char *start, size_t size);
void merge(char *block);
char *backtrack(char *block);
void split_block(char *block, size_t size);
void set_block_links(char *block, size_t size);
char *get_next_block(char *block, bool forward);
size_t get_block_size(char *block);
size_t get_block_data_size(char *block);
void set_block_status(char *block, bool used);
bool get_block_status(char *block);
size_t coerce_size(size_t size, bool up);
size_t size_of_header();
size_t size_of_footer();
short generate_hash(char *hostname);
short get_block_hash(char *block);
void panic_full(char *reason, const char *file, const int line);
void panic(char *reason);

/*----- Memory Allocation Function Implementations -----*/

void *allocate(size_t size, const char *file, const int line) {
    char *start = memory + size_of_footer();

    if (size == 0 || size > MAX_MEMORY) {
        panic_full("User passed bad value", file, line);
    }

    size = coerce_size(size, true);

    if (!initialized) {
        initialized = true;

        char hostname[HOST_NAME_MAX];
        gethostname(hostname, HOST_NAME_MAX);
        host_hash = generate_hash(hostname);

        char *start = memory + size_of_footer();
        memset(memory, 0, sizeof(char) * MAX_MEMORY);
        set_block_links(start, MAX_MEMORY - (size_of_header() + (size_of_footer() * 3)));
        set_block_status(start, false);
    }

    char *best_fit = find_best_fit(start, size);
    if (best_fit) {
        size_t block_size = get_block_data_size(best_fit);
        float diff = block_size - size;
        if (diff / MAX_MEMORY > SPLIT_THRESHOLD) {
            split_block(best_fit, size);
        }
        set_block_status(best_fit, true);
    }

    return best_fit + size_of_header();
}

void deallocate(void *ptr, const char *file, const int line) {
    if (!initialized) {
        panic_full("User attempted to call free before calling malloc", file, line);
    } else if (ptr < (void *) memory || ptr > (void *) (memory + MAX_MEMORY)) {
        panic_full("User attempted to free a pointer outside of range", file, line);
    }

    char *block = ((char *) ptr) - size_of_header();

    if (get_block_hash(block) == host_hash && !get_block_status(block)) {
        panic_full("User attempted to free a pointer twice", file, line);
    } else if (get_block_hash(block) != host_hash) {
        panic_full("User attempted to free a pointer not returned by malloc", file, line);
    }

    set_block_status(block, false);
    block = backtrack(block);
    merge(block);
}

/*----- Debugging Function Implementations -----*/

void print_heap(char *start_block) {
    while (get_block_data_size(start_block) > 0) {
        printf("[Size: %d, In-Use: %d]->", get_block_data_size(start_block), get_block_status(start_block));
        start_block = get_next_block(start_block, true);
    }
    printf("[]\n");
}

/*----- Internal Function Implementations -----*/

char *find_best_fit(char *start, size_t size) {
    char *block = start, *best_fit = NULL;
    while (get_block_data_size(block) > 0) {
        if (get_block_status(block)) {
            block = get_next_block(block, true);
            continue;
        }

        size_t new_fit = get_block_data_size(block);

        if (best_fit) {
            size_t old_fit = get_block_data_size(best_fit);
            if (new_fit >= size && (size - new_fit) < (size - old_fit)) {
                best_fit = block;
            }
        } else if (new_fit >= size) {
            best_fit = block;
        }

        block = get_next_block(block, true);
    }
    return best_fit;
}

void merge(char *block) {
    char *next = get_next_block(block, true);
    while (!get_block_status(next) && get_block_data_size(next) > 0) {
        size_t curr_size = get_block_data_size(block);
        size_t next_size = get_block_data_size(next);
        size_t total_size = curr_size + next_size + (size_of_header() + size_of_footer());
        set_block_links(block, total_size);

        next = get_next_block(block, true);
    }
}

char *backtrack(char *block) {
    char *next = get_next_block(block, false);
    while (!get_block_status(next) && get_block_data_size(next) > 0) {
        block = next;
        next = get_next_block(block, false);
    }
    return block;
}

void split_block(char *block, size_t size) {
    size_t start_data_size = get_block_data_size(block);
    set_block_links(block, size);
    size_t end_full_size = get_block_size(block);
    char *new_block = get_next_block(block, true);
    set_block_links(new_block, start_data_size - end_full_size);
}

void set_block_links(char *block, size_t size) {
    size_t head_length = size_of_header(), foot_length = size_of_footer();
    void *header = block, *footer = block + head_length + size;
    if (foot_length == sizeof(char)) {
        char data = (char) size, *cast_head = header, *cast_foot = footer;
        *cast_head = data;
        *cast_foot = data;
        *((short *) (cast_head + 1)) = host_hash;
    } else if (foot_length == sizeof(short)) {
        short data = (short) size, *cast_head = header, *cast_foot = footer;
        *cast_head = data;
        *cast_foot = data;
        *((short *) (cast_head + 1)) = host_hash;
    } else if (foot_length == sizeof(int)) {
        int data = (int) size, *cast_head = header, *cast_foot = footer;
        *cast_head = data;
        *cast_foot = data;
        *((short *) (cast_head + 1)) = host_hash;
    } else {
        size_t data = (size_t) size, *cast_head = header, *cast_foot = footer;
        *cast_head = data;
        *cast_foot = data;
        *((short *) (cast_head + 1)) = host_hash;
    }
}

char *get_next_block(char *block, bool forward) {
    if (forward) {
        return block + get_block_size(block);
    } else {
        return block - get_block_size(block - size_of_footer());
    }
}

size_t get_block_size(char *block) {
    size_t foot_length = size_of_footer();
    size_t size;
    if (foot_length == sizeof(char)) {
        size = (size_t) *block;
    } else if (foot_length == sizeof(short)) {
        size = (size_t) *((short *) block);
    } else if (foot_length == sizeof(int)) {
        size = (size_t) *((int *) block);
    } else {
        size = *((size_t *) block);
    }
    size = coerce_size(size, false);
    return size + (size_of_header() + foot_length);
}

size_t get_block_data_size(char *block) {
    return get_block_size(block) - (size_of_header() + size_of_footer());
}

void set_block_status(char *block, bool used) {
    size_t head_length = size_of_header(), foot_length = size_of_footer();
    void *header = block, *footer = block + head_length + get_block_data_size(block);
    if (foot_length == sizeof(char)) {
        if (used) {
            *((char *) header) |= 1;
            *((char *) footer) |= 1;
        } else {
            *((char *) header) &= ~1;
            *((char *) footer) &= ~1;
        }
    } else if (foot_length == sizeof(short)) {
        if (used) {
            *((short *) header) |= 1;
            *((short *) footer) |= 1;
        } else {
            *((short *) header) &= ~1;
            *((short *) footer) &= ~1;
        }
    } else if (foot_length == sizeof(int)) {
        if (used) {
            *((int *) header) |= 1;
            *((int *) footer) |= 1;
        } else {
            *((int *) header) &= ~1;
            *((int *) footer) &= ~1;
        }
    } else {
        if (used) {
            *((size_t *) header) |= 1;
            *((size_t *) footer) |= 1;
        } else {
            *((size_t *) header) &= ~1;
            *((size_t *) footer) &= ~1;
        }
    }
}

bool get_block_status(char *block) {
    size_t foot_length = size_of_footer();
    if (foot_length == sizeof(char)) {
        return *block & 1;
    } else if (foot_length == sizeof(short)) {
        return *((short *) block) & 1;
    } else if (foot_length == sizeof(int)) {
        return *((int *) block) & 1;
    } else {
        return *((size_t *) block) & 1;
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

size_t size_of_header() {
    size_t size;

    if (MAX_MEMORY <= SINGLE_BYTE_MAX) {
        size = sizeof(char);
    } else if (MAX_MEMORY <= DOUBLE_BYTE_MAX) {
        size = sizeof(short);
    } else if (MAX_MEMORY <= QUAD_BYTE_MAX) {
        size = sizeof(int);
    } else {
        size = sizeof(size_t);
    }

    return size + sizeof(short);
}

size_t size_of_footer() {
    if (MAX_MEMORY <= SINGLE_BYTE_MAX) {
        return sizeof(char);
    } else if (MAX_MEMORY <= DOUBLE_BYTE_MAX) {
        return sizeof(short);
    } else if (MAX_MEMORY <= QUAD_BYTE_MAX) {
        return sizeof(int);
    } else {
        return sizeof(size_t);
    }
}

short generate_hash(char *hostname) {
    int length = strlen(hostname);
    short total = 1;
    for (int i = 0; i < length; i++) {
        total *= hostname[i];
    }
    return total;
}

short get_block_hash(char *block) {
    size_t foot_length = size_of_footer();
    if (foot_length == sizeof(char)) {
        return *((short *) (block + sizeof(char)));
    } else if (foot_length == sizeof(short)) {
        return *((short *) (block + sizeof(short)));
    } else if (foot_length == sizeof(int)) {
        return *((short *) (block + sizeof(int)));
    } else {
        return *((short *) (block + sizeof(size_t)));
    }
}

void panic_full(char *reason, const char *file, const int line) {
    fprintf(stderr, "Encountered error [%s] at line #%d in file %s\n", reason, line, file);
    exit(EXIT_FAILURE);
}

void panic(char *reason) {
    fprintf(stderr, "Encountered error [%s]\n", reason);
    exit(EXIT_FAILURE);
}
