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
void panic(char *reason, const char *file, const int line);

/*----- Memory Allocation Function Implementations -----*/

// Function is responsible for memory allocations.
// This is the malloc implementation.
void *allocate(size_t size, const char *file, const int line) {
    // Memory is padded with a empty footer at the beginning and end that serves as a
    // stopper for the merge and backtrack functions. This grabs a pointer to the first
    // valid address beyond the padding.
    char *start = memory + size_of_footer();

    // Basic parameter checking. Can't allocate negative memory or more than our max.
    if (size <= 0 || size > MAX_MEMORY - (size_of_header() + (size_of_footer() * 3))) {
        panic("User passed bad value", file, line);
    }

    // Implementation enforces that allocations adhere to multiples of two. This allows
    // marking of blocks without requiring an extra byte. This rounds size up to the
    // nearest multiple of two.
    size = coerce_size(size, true);

    // First time allocate has been called. Set up.
    if (!initialized) {
        initialized = true;

        // Implementation detects valid blocks by checking a hash value. This generates
        // said hash value off of the host name. Seemed as sensible a thing as any to hash.
        char hostname[HOST_NAME_MAX];
        gethostname(hostname, HOST_NAME_MAX);
        host_hash = generate_hash(hostname);

        // Set up header and footer for initial max size block.
        char *start = memory + size_of_footer();
        memset(memory, 0, sizeof(char) * MAX_MEMORY);
        set_block_links(start, MAX_MEMORY - (size_of_header() + (size_of_footer() * 3)));
        set_block_status(start, false);
    }

    // Implementation uses a best fit algorithm. In case of a tie, it picks the first.
    char *best_fit = find_best_fit(start, size);
    if (best_fit) {
        // Implementation only bothers to split a block if the extra memory accounts for
        // more than 1% of the total heap size. This checks that.
        size_t block_size = get_block_data_size(best_fit);
        float diff = block_size - size;
        if (diff / MAX_MEMORY > SPLIT_THRESHOLD) {
            split_block(best_fit, size);
        }
        set_block_status(best_fit, true);
    }

    // Return pointer if a fit was found.
    if (best_fit) {
        return best_fit + size_of_header();
    } else {
        return NULL;
    }
}

// Function is responsible for freeing memory allocated by allocate.
// This is the free implementation.
void deallocate(void *ptr, const char *file, const int line) {
    // Basic argument validation. We can't free memory if allocate has never been
    // called or if it is out of range.
    if (!initialized) {
        panic("User attempted to call free before calling malloc", file, line);
    } else if (ptr < (void *) memory || ptr > (void *) (memory + MAX_MEMORY)) {
        panic("User attempted to free a pointer outside of range", file, line);
    }

    // The pointer passed to us will be pointing to the data segment of the block.
    // This decrements the pointer so that it points to the header.
    char *block = ((char *) ptr) - size_of_header();

    // More advanced validation. If the given block passes the hash check, but is not
    // in use, implementation assumes that the user has tried to double free.
    // If the given block fails the hash check, implementation assumes we've been passed
    // a pointer we didn't allocate.
    if (get_block_hash(block) == host_hash && !get_block_status(block)) {
        panic("User attempted to free a pointer twice", file, line);
    } else if (get_block_hash(block) != host_hash) {
        panic("User attempted to free a pointer not returned by malloc", file, line);
    }

    // Mark block as unused, backtrack pointer to the furthest back consecutive free block,
    // and merge forward.
    set_block_status(block, false);
    block = backtrack(block);
    merge(block);
}

/*----- Debugging Function Implementations -----*/

// Function is responsible for printing out a list representation of the current heap
// contents.
void print_heap() {
    char *start_block = memory + size_of_footer();
    while (get_block_data_size(start_block) > 0) {
        printf("[Size: %d, In-Use: %d]->", get_block_data_size(start_block), get_block_status(start_block));
        start_block = get_next_block(start_block, true);
    }
    printf("[]\n");
}

/*----- Internal Function Implementations -----*/

// Function is responsible for finding the best fit for an allocation request.
// In the event of a tie, it returns the first one it found.
char *find_best_fit(char *start, size_t size) {
    char *block = start, *best_fit = NULL;

    // Continue looping until we reach the blank padding footer at the end of the
    // heap.
    while (get_block_data_size(block) > 0) {
        // Continue if found block is in use.
        if (get_block_status(block)) {
            block = get_next_block(block, true);
            continue;
        }

        // Get the size of the block.
        size_t new_fit = get_block_data_size(block);

        if (best_fit) {
            // Compare found block against previous best fit.
            size_t old_fit = get_block_data_size(best_fit);
            if (new_fit >= size && (size - new_fit) < (size - old_fit)) {
                best_fit = block;
            }
        } else if (new_fit >= size) {
            // No previous best fit. Found block promoted by default.
            best_fit = block;
        }

        // Get the next block.
        block = get_next_block(block, true);
    }

    return best_fit;
}

// Function is responsible for merging consecutive free blocks. Merge runs from
// low to high addresses.
void merge(char *block) {
    // Get the first block we could be merging into.
    char *next = get_next_block(block, true);

    // Continue looping until we hit a block in use, or we hit the blank padding
    // footer at the end of the heap.
    while (!get_block_status(next) && get_block_data_size(next) > 0) {
        // Recalculate block size and re-write the headers and footers.
        size_t curr_size = get_block_data_size(block);
        size_t next_size = get_block_data_size(next);
        size_t total_size = curr_size + next_size + (size_of_header() + size_of_footer());
        set_block_links(block, total_size);

        // Get the next block.
        next = get_next_block(block, true);
    }
}

// Function is responsible for stepping back through the heap to find the earliest
// consecutive free block.
char *backtrack(char *block) {
    // Get the first block we could be stepping back to.
    char *next = get_next_block(block, false);

    // Continue looping until we hit a block in use, or we hit the blank padding
    // footer at the beginning of the heap.
    while (!get_block_status(next) && get_block_data_size(next) > 0) {
        block = next;
        next = get_next_block(block, false);
    }

    return block;
}

// Function is responsible for splitting a block into two if the wasted space accounts
// for more than 1% of the heap.
void split_block(char *block, size_t size) {
    // Save the original size and overwrite the header and footer with the new size.
    size_t start_data_size = get_block_data_size(block);
    set_block_links(block, size);

    // Create a new block with the remaining space from the original block.
    size_t end_full_size = get_block_size(block);
    char *new_block = get_next_block(block, true);
    set_block_links(new_block, start_data_size - end_full_size);
}

// Function is responsible for writing headers and footers for blocks.
void set_block_links(char *block, size_t size) {
    // Get our sizes and find the links.
    size_t head_length = size_of_header(), foot_length = size_of_footer();
    void *header = block, *footer = block + head_length + size;

    // If statements wouldn't really be necessary according to the project specification,
    // but hard coding the 5000 byte memory limit made me sad, so the implementation can
    // actually handle arbitrary amounts of memory. The logic changes a bit with larger
    // amounts of memory, so this handles that.
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

// Function is responsible for finding the next block in the specified direction.
char *get_next_block(char *block, bool forward) {
    if (forward) {
        return block + get_block_size(block);
    } else {
        return block - get_block_size(block - size_of_footer());
    }
}

// Function is responsible for getting the size of a block including header and footer.
size_t get_block_size(char *block) {
    size_t foot_length = size_of_footer();
    size_t size;

    // Once again, if statement wouldn't be strictly necessary, but my implementation can
    // handle arbitrary amounts of memory, controlled by a preprocessor constant in the
    // header file. This handles the logic for that.
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

// Function is responsible for getting the size of just the data segment of a block.
size_t get_block_data_size(char *block) {
    return get_block_size(block) - (size_of_header() + size_of_footer());
}

// Function is responsible for setting the specified usage status for a block. As my
// implemetation requires that block sizes be multiples of two, we can use the lowest
// order bit in the size fields of the header and footer to denote usage status.
void set_block_status(char *block, bool used) {
    size_t head_length = size_of_header(), foot_length = size_of_footer();
    void *header = block, *footer = block + head_length + get_block_data_size(block);

    // Once again, if statement wouldn't be strictly necessary, but my implementation can
    // handle arbitrary amounts of memory, controlled by a preprocessor constant in the
    // header file. This handles the logic for that.
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

// Function is responsible for getting the usage status of a block.
// As stated previously, I use the lowest order bit of the size field of the header
// and footer to denote usage status.
bool get_block_status(char *block) {
    size_t foot_length = size_of_footer();

    // Once again, if statement wouldn't be strictly necessary, but my implementation can
    // handle arbitrary amounts of memory, controlled by a preprocessor constant in the
    // header file. This handles the logic for that.
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

// Function is responsible for coercing the giving size to either the next, or
// previous, multiple of two depending on need.
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

// Function is responsible for returning the size of the header.
size_t size_of_header() {
    size_t size;

    // Once again, if statement wouldn't be strictly necessary, but my implementation can
    // handle arbitrary amounts of memory, controlled by a preprocessor constant in the
    // header file. This handles the logic for that.
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

// Function is responsible for returning the size of the footer.
size_t size_of_footer() {
    // Once again, if statement wouldn't be strictly necessary, but my implementation can
    // handle arbitrary amounts of memory, controlled by a preprocessor constant in the
    // header file. This handles the logic for that.
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

// Function is responsible for generating a hash for a given hostname.
short generate_hash(char *hostname) {
    int length = strlen(hostname);
    short total = 1;
    for (int i = 0; i < length; i++) {
        total *= hostname[i];
    }
    return total;
}

// Function is responsible for getting the hash value for a given block.
short get_block_hash(char *block) {
    size_t foot_length = size_of_footer();
    
    // Once again, if statement wouldn't be strictly necessary, but my implementation can
    // handle arbitrary amounts of memory, controlled by a preprocessor constant in the
    // header file. This handles the logic for that.
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

// Function handles error reporting in the event of a detected memory error.
void panic(char *reason, const char *file, const int line) {
    fprintf(stderr, "Encountered error [%s] at line #%d in file %s\n", reason, line, file);
    exit(EXIT_FAILURE);
}
