#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include "definitions.h"
#include "nodes.h"
#include "hash.h"
#include "splitter.h"

#define OPEN_TAG_LENGTH 7
#define CLOSE_TAG_LENGTH 8

void handle_output(char *path, hash *table);
void handle_file(char *path, hash *table);
void handle_dir(char *path, hash *table);
bool is_file(char *path);
bool is_dir(char *path);
char *get_user_input();
void sort(char **keys, int left, int right);
char *enforce(char *str, int needed, int size, int filled, int *new_size);
void panic(char *reason);
void sanitize(char **argv);
int digits(int num);

// Handles main program execution.
int main(int argc, char **argv) {
    // Simple argument count check.
    if (argc != 3) {
        panic("Wrong number of arguments");
    }

    // Strip '/' off of directory names if present
    sanitize(argv);

    hash *table = create_hash();

    // Check if table was successfully allocated.
    if (!table) {
        panic("Could not allocate memory for hash table");
    }

    if (is_file(argv[2])) {
        // Handle single input file.
        handle_file(argv[2], table);
    } else if (is_dir(argv[2])) {
        // Handle input folder.
        handle_dir(argv[2], table);
    } else {
        // File either doesn't exist, or is of some unexpected type.
        panic("Could not open specified file. It either does not exist or is of unexpected type.");
    }

    bool should_continue = false;
    if (is_file(argv[1])) {
        // Chosen output file already exists.
        printf("Warning: The file you requested to save to already exists. Would you like to overwrite?\n");
        char *response = get_user_input();

        // Force user to enter valid decision.
        while (strncmp(response, "y", 1) && strncmp(response, "n", 1)) {
            printf("Please be a good user and enter either yes or no.\n");
            response = get_user_input();
        }

        if (!strncmp(response, "y", 1)) {
            // User chose to remove the file.
            if (remove(argv[1])) {
                // File removal failed.
                panic("File could not be removed.");
            }
        } else {
            // User chose not to remove file.
            panic("Requested output file already exists.");
        }
        free(response);
    } else if (is_dir(argv[1])) {
        // Chosen output file already exists, and is a directory.
        printf("Warning: The file you requested to save to is a directory. Would you like to recursively delete said directory?\n");
        char *response = get_user_input();

        // Force user to enter valid decision.
        while (strncmp(response, "y", 1) && strncmp(response, "n", 1)) {
            printf("Please be a good user and enter either yes or no\n");
            response = get_user_input();
        }

        if (!strncmp(response, "y", 1)) {
            // User chose to remove the directory.
            if (rmdir(argv[1])) {
                // Directory removal failed.
                panic("Directory could not be removed.");
            }
        } else {
            // User chose not to remove directory.
            panic("Requested output file is a directory.");
        }
        free(response);
    } else {
        // Chosen output file either does not already exist, or is some unexpected type of file.
        remove(argv[1]);
    }

    // Having successfully generated the index, write it out to a file.
    handle_output(argv[1], table);

    return EXIT_SUCCESS;
}

// Function handles generating output in specified XML like syntax.
void handle_output(char *path, hash *table) {
    // Open output file, get all keys (unique tokens) from the table, and sort them.
    FILE *output = fopen(path, "wr");
    char **keys = get_keys(table);
    sort(keys, 0, table->count - 1);

    // Generate XML like output detailed in assignment PDF.
    for (int i = 0; i < table->count; i++) {
        int size = 100, filled = 0, word_count = 0;
        char *line = (char *) malloc(sizeof(char) * size), *key = keys[i];

        // Calculate space needed for the opening tag and ensure line is big enough.
        if (strstr(key, "championship")) {
            printf("Problem found\n");
        }
        int needed = OPEN_TAG_LENGTH + strlen(key) + 1;
        line = enforce(line, needed, size, filled, &size);

        sprintf(line, "<list> %s\n", key);
        filled += needed;

        // Iterate across all files that included the token.
        for (index_node *node = get(table, key); node; node = node->next) {
            word_count++;
            char *filename = node->filename, *format;
            int count = node->count, count_size = digits(count);

            // Calculate space needed for the current filename.
            if (word_count && word_count % 5 == 0 || !node->next) {
                needed = strlen(filename) + count_size + 1;
                format = "%s %d";
            } else {
                needed = strlen(filename) + count_size + 2;
                format = "%s %d ";
            }

            // Ensure line is big enough.
            line = enforce(line, needed, size, filled, &size);

            sprintf(line + filled, format, filename, count);
            filled += needed;

            // One line can contain at most 5 files, so this splits lines every 5 files.
            if (word_count && word_count % 5 == 0) {
                line = enforce(line, 1, size, filled, &size);
                line[filled] = '\n';
                filled++;
            }
        }
        // Handle edge case.
        if (word_count % 5) {
            line = enforce(line, 1 + CLOSE_TAG_LENGTH, size, filled, &size);
            strcpy(line + filled, "\n</list>\n");
            filled += CLOSE_TAG_LENGTH + 1;
        } else {
            line = enforce(line, CLOSE_TAG_LENGTH, size, filled, &size);
            strcpy(line + filled, "</list>\n");
            filled += CLOSE_TAG_LENGTH;
        }
        
        // Terminate string.
        line = enforce(line, 1, size, filled, &size);
        line[filled] = '\0';
        if (fputs(line, output) == EOF) {
            // File could not be written to.
            panic("Could not write to specified file");
        }
    }

    // Close file to finalize changes.
    fclose(output);
}

// Function handles recursing through a directory and iterating across all files contained within.
void handle_dir(char *path, hash *table) {
    DIR *directory = opendir(path);
    if (directory) {
        // Iterate across all directory entries.
        for (struct dirent *entry = readdir(directory); entry; entry = readdir(directory)) {
            // Check that entry is not the directory itself or its parent directory.
            if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
                // Generate relative path to file.
                char new_path[strlen(path) + strlen(entry->d_name) + 2];
                sprintf(new_path, "%s/%s", path, entry->d_name);

                if (entry->d_type & DT_REG) {
                    // Entry is a file, parse it.
                    handle_file(new_path, table);
                } else if (entry->d_type & DT_DIR) {
                    // Entry is a directory, recursively call handle_dir to handle it.
                    handle_dir(new_path, table);
                } else {
                    // Entry is neither a directory nor a normal file. Report error and keep going.
                    fprintf(stderr, "Warning: Directory hierarchy contains an entry which is neither a regular file nor directory, skipping it\n");
                }
            }
        }
    } else {
        // Report error opening directory, but allow indexing to continue.
        fprintf(stderr, "Error: Directory %s could not be opened\n", path);
    }
}

// Function handles parsing a file for all unique tokens.
void handle_file(char *path, hash *table) {
    // Open file for reading.
    FILE *file = fopen(path, "r");

    if (file) {
        // Create splitter to tokenize given file.
        splitter *split = create_splitter(file);

        // Loop until no tokens remain.
        for (char *token = split_next(split); token; token = split_next(split)) {
            // Attempt to get existing data from the table.
            index_node *head = get(table, token);

            if (head) {
                // Hash already contained an entry for this token, either find entry for current file and
                // increment counter, or add current file to the chain.
                index_node *target = find_index_node(head, path);

                if (target) {
                    // Entry for current token exists, increment counter.
                    target->count++;
                    head = remove_index_node(head, target->filename);
                    head = insert_index_node(head, target);
                    update(table, token, head);
                } else {
                    // First time encountering this token in this file. Add current filename to the chain.
                    target = create_index_node(path);
                    target->count++;
                    head = insert_index_node(head, target);
                    update(table, token, head);
                }
            } else {
                // Token is unique, create an entry for it.
                index_node *node = create_index_node(path);
                node->count++;
                put(table, token, node);
            }
        }
        if (errno) {
            // Splitter sets errno and returns NULL if it fails to allocate memory for a string.
            // Since the loop terminates upon a NULL return, this checks to make sure there wasn't
            // an error.
            panic("Splitter could not allocate memory for string");
        }
    } else {
        // Report error opening file, however, in the case of multiple files, allow indexing to continue.
        fprintf(stderr, "Error: File %s could not be opened\n", path);
    }
}

// Function checks if given path refers to a file.
bool is_file(char *path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISREG(buf.st_mode);
}

// Function checks if given path refers to a directory.
bool is_dir(char *path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}

// Dynamically allocated string to take input from the user and discard any
// leading whitespace.
char *get_user_input() {
    int size = 1;
    char *input = (char *) malloc(sizeof(char) * size);
    if (input) {
        while (true) {
            char c = getchar();
            if (c == EOF) {
                break;
            }
            if (!isspace(c)) {
                // Real input has begun.
                ungetc(c, stdin);
                break;
            }
        }
        for (int i = 0; true; i++) {
            char c = getchar();
            if (isspace(c) || c == EOF) {
                // Input is complete.
                input[i] = '\0';
                break;
            }
            input[i] = c;
            if (i == size - 1) {
                // Out of space, reallocation required.
                size *= 2;
                input = (char *) realloc(input, size);
                if (!input) {
                    // Reallocation failed. Out of memory.
                    panic("Could not allocate space for input string");
                }
            }
        }
        return input;
    } else {
        // Allocation failed. Out of memory.
        panic("Could not allocate space for input string");
    }
}

// Function handles sorting an array of strings. Uses Quicksort to do so.
void sort(char **keys, int left, int right) {
    // Check base case.
    if (left < right) {
        char *first = keys[left], *last = keys[right], *mid = keys[(left + right) / 2], *chosen;
        int pivot;
        
        // Chose middle value as pivot to ensure against worst case behavior.
        if (strcmp(first, mid) >= 0 && strcmp(first, last) <= 0) {
            pivot = left;
            chosen = first;
        } else if(strcmp(mid, first) >= 0 && strcmp(mid, last) <= 0) {
            pivot = (left + right) / 2;
            chosen = mid;
        } else {
            pivot = right;
            chosen = last;
        }

        // Sort.
        char *tmp = keys[right];
        keys[right] = keys[pivot];
        keys[pivot] = tmp;
        int index = left;
        for (int i = left; i < right; i++) {
            if (strcmp(keys[i], chosen) > 0) {
                tmp = keys[i];
                keys[i] = keys[index];
                keys[index] = tmp;
                index++;
            }
        }
        tmp = keys[index];
        keys[index] = keys[right];
        keys[right] = tmp;
        
        // Recursively sort subarrays.
        sort(keys, left, index - 1);
        sort(keys, index + 1, right);
    }
}

// Function takes care of any reallocations required to fit data into given string.
char *enforce(char *str, int needed, int size, int filled, int *new_size) {
    while (needed >= size - filled) {
        size *= 2;
        str = (char *) realloc(str, size);
        if (!str) {
            // Out of memory.
            panic("Could not allocate memory for string");
        }
    }
    *new_size = size;
    return str;
}

// Handle reporting of, and terminate due to, a fatal error.
void panic(char *reason) {
    fprintf(stderr, "Sorry, the indexer has encountered an unrecoverable error. Given reason was: %s\n", reason);
    exit(EXIT_FAILURE);
}

// Strip trailing slashes after directory names given from the command line.
void sanitize(char **argv) {
    if (argv[1][strlen(argv[1]) - 1] == '/') {
        argv[1][strlen(argv[1]) - 1] = '\0';
    }
    if (argv[2][strlen(argv[2]) - 1] == '/') {
        argv[2][strlen(argv[2]) - 1] = '\0';
    }
}

// Return number of digits in a particular number.
int digits(int num) {
    int count = 0;
    while (num > 0) {
        num /= 10;
        count++;
    }
    return count;
}
