#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "definitions.h"
#include "nodes.h"
#include "hash.h"

// Constant declarations.
#define OPEN_TAG_LENGTH 7
#define AND_OR_LENGTH 3

// Function declarations.
void construct(FILE *file, hash *table);
void handle_query(hash *table, char **queries, int count, query_type type);
void handle_output(index_node **results, int count);
char *get_line(FILE *file);
char *get_user_input();
void sort(index_node **keys, int left, int right);
char *enforce(char *str, int needed, int size, int filled, int *new_size);
void panic(char *reason);

int main(int argc, char **argv) {
    // Validate argument count.
    if (argc != 2) {
        panic("Wrong number of arguments");
    }

    // Open specified file for reading.
    FILE *input = fopen(argv[1], "r");
    if (input) {
        hash *table = create_hash();

        // Construct inverted index in memory from file.
        construct(input, table);

        // Begin query loop.
        printf("Enter Queries:\n");
        for (char *input = get_user_input(); input[0] != 'q'; input = get_user_input()) {
            // Tokenize input string into an array using spaces as delimeters.
            char *tmp = strtok(input, " ");
            tmp = strtok(NULL, " ");
            int size = 1, current = 0;
            char **keys = (char **) malloc(sizeof(char *) * size);
            while (tmp) {
                keys[current] = tmp;
                current++;
                if (current == size) {
                    size *= 2;
                    keys = (char **) realloc(keys, sizeof(char *) * size);
                    if (!keys) {
                        panic("Unable to allocate space for query array");
                    }
                }
                tmp = strtok(NULL, " ");
            }
            
            // Verify type of search being conducted.
            if (strstr(input, "sa") == input) {
                // Handle AND search.
                handle_query(table, keys, current, AND);
            } else if (strstr(input, "so") == input) {
                // Handle OR search.
                handle_query(table, keys, current, OR);
            } else {
                fprintf(stderr, "Please enter a valid query, or q to quit.\n");
            }

            // Cleanup.
            free(keys);
            free(input);
        }
    } else {
        panic("Error opening file. Perhaps it doesn't exist?");
    }
}

// Function is responsible for reconstructing an inverted index in memory from a given input file.
void construct(FILE *file, hash *table) {
    for (char *open_tag = get_line(file); open_tag; open_tag = get_line(file)) {
        // Check if line begins with a opening tag as it should.
        if (strstr(open_tag, "<list>") != open_tag) {
            panic("Index file is malformatted");
        }

        // Isolate token name and get first line of data.
        char *token = open_tag + OPEN_TAG_LENGTH, *data = get_line(file);
        
        // Grab list for this token if one exists.
        index_node *head = get(table, token);

        // Loop until encountering a closing tag.
        while(!strstr(data, "</list>")) {
            // Tokenize data into filenames...
            char *name = strtok(data, " ");
            while (name) {
                // ...and numerical counts.
                char *potential = strtok(NULL, " ");
                if (potential) {
                    int count = strtol(potential, NULL, 0);
                    if (count) {
                        // Current entry is well formatted, time to insert and/or update.
                        index_node *node = create_index_node(name);
                        node->count = count;
                        if (head) {
                            head = insert_index_node(head, node);
                            update(table, token, head);
                        } else {
                            head = node;
                            put(table, token, node);
                        }
                    } else {
                        panic("Index file is malformatted");
                    }
                    // Grab next name.
                    name = strtok(NULL, " ");
                } else {
                    panic("Index file is malformatted");
                }
            }
            // Free and grab next line of data.
            free(data);
            data = get_line(file);
        }
        // Cleanup.
        free(data);
        free(open_tag);
    }
}

// Function is responsible for performing a lookup on the inverted index for a given query.
void handle_query(hash *table, char **queries, int count, query_type type) {
    // Declare a new hash to allow for easy de-duping.
    hash *findings = create_hash();

    // Iterate across and perform each lookup.
    for (int i = 0; i < count; i++) {
        char *query = queries[i];
        for (index_node *curr = get(table, query); curr; curr = curr->next) {
            index_node *found = get(findings, curr->filename);
            if (type == AND) {
                if (found) {
                    found->count++;
                } else {
                    found = create_index_node("");
                    found->count = 1;
                    found->next = curr;
                    put(findings, curr->filename, found);
                }
            } else {
                if (!found) {
                    put(findings, curr->filename, curr);
                }
            }
        }
    }

    // Setup for iteration across hash.
    char **files = get_keys(findings);
    index_node *results[findings->count];
    int successes = 0;
    
    // Iterate across findings hash, and put each qualifying node into results array.
    for (int i = 0; i < findings->count; i++) {
        char *file = files[i];
        if (type == AND) {
            index_node *node = get(findings, file);
            if (node->count == count) {
                results[successes++] = node->next;
            }
        } else {
            results[successes++] = get(findings, file);
        }
    }

    // Sort the results array.
    sort(results, 0, successes - 1);

    // Print out the results array.
    handle_output(results, successes);

    // Cleanup.
    free(files);
}

// Function is responsible for printing out the results of a query.
void handle_output(index_node **results, int count) {
    // Set intial size of output string, all necessary counters, and allocate string.
    int size = 100, filled = 0, needed = 0;
    char *output = (char *) malloc(sizeof(char) * size);

    // Iterate across results.
    for (int i = 0; i < count; i++) {
        index_node *result = results[i];
        char *format;
        if (i != count - 1) {
            // Not on last result, append result with a comma and space.
            needed = strlen(result->filename) + 2;
            output = enforce(output, needed, size, filled, &size);
            format = "%s, ";
        } else {
            // On last result, no comma or space.
            needed = strlen(result->filename) + 1;
            output = enforce(output, needed, size, filled, &size);
            format = "%s";
        }
        sprintf(output + filled, format, result->filename);
        filled += needed;
    }
    // Null terminate string, print it, and free it.
    output = enforce(output, 1, size, filled, &size);
    output[filled] = '\0';
    puts(output);
    free(output);
}

// Function is responsible for reading input from a file and dynamically allocating space for it.
char *get_line(FILE *file) {
    // Declare string and initial size.
    int size = 1;
    char *line = (char *) malloc(sizeof(char) * size);

    if (line) {
        // Loop until EOF or newline.
        for (int i = 0; true; i++) {
            char c = fgetc(file);
            if (c == '\n' || c == EOF) {
                line[i] = '\0';
                break;
            }
            line[i] = c;
            if (i == size - 1) {
                size *= 2;
                line = (char *) realloc(line, size);
                if (!line) {
                    panic("Could not allocate space for input string while reading from file");
                }
            }
        }
        
        if (strcmp(line, "")) {
            // String is valid.
            return line;
        } else {
            // String is empty.
            free(line);
            return NULL;
        }
    } else {
        panic("Could not allocate space for input string while reading from file");
    }
}

// Function is responsible for reading input from the user and dynamically allocating space for it.
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
            if (c == '\r' || c == '\n' || c == EOF) {
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
                    panic("Could not allocate space for input string from user");
                }
            }
        }
        return input;
    } else {
        // Allocation failed. Out of memory.
        panic("Could not allocate space for input string from user");
    }
}

// Function takes an array of index nodes and sorts them with compare_nodes using Quicksort.
void sort(index_node **keys, int left, int right) {
    // Check base case.
    if (left < right) {
        index_node *first = keys[left], *last = keys[right], *mid = keys[(left + right) / 2], *chosen;
        int pivot;

        // Chose middle value as pivot to ensure against worst case behavior.
        if (compare_nodes(first, mid) >= 0 && compare_nodes(first, last) <= 0) {
            pivot = left;
            chosen = first;
        } else if(compare_nodes(mid, first) >= 0 && compare_nodes(mid, last) <= 0) {
            pivot = (left + right) / 2;
            chosen = mid;
        } else {
            pivot = right;
            chosen = last;
        }

        // Sort.
        index_node *tmp = keys[right];
        keys[right] = keys[pivot];
        keys[pivot] = tmp;
        int index = left;
        for (int i = left; i < right; i++) {
            if (compare_nodes(keys[i], chosen) > 0) {
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

// Function is responsible for reporting a fatal error and halting execution.
void panic(char *reason) {
    fprintf(stderr, "Sorry, the search tool has encountered an unrecoverable error. Given reason was: %s\n", reason);
    exit(EXIT_FAILURE);
}
