#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "definitions.h"
#include "nodes.h"
#include "hash.h"

#define OPEN_TAG_LENGTH 7
#define AND_OR_LENGTH 3

void construct(FILE *file, hash *table);
void handle_query(hash *table, char **queries, int count, query_type type);
void handle_output(index_node **results, int count);
char *get_line(FILE *file);
char *get_user_input();
void sort(index_node **keys, int left, int right);
char *enforce(char *str, int needed, int size, int filled, int *new_size);
void panic(char *reason);

int main(int argc, char **argv) {
    if (argc != 2) {
        panic("Wrong number of arguments");
    }

    FILE *input = fopen(argv[1], "r");
    if (input) {
        hash *table = create_hash();
        construct(input, table);
        printf("Enter Queries:\n");
        for (char *input = get_user_input(); input[0] != 'q'; input = get_user_input()) {
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
            if (strstr(input, "sa") == input) {
                handle_query(table, keys, current, AND);
            } else if (strstr(input, "so") == input) {
                handle_query(table, keys, current, OR);
            } else {
                fprintf(stderr, "Please enter a valid query, or q to quit.\n");
            }
            free(keys);
            free(input);
        }
    } else {
        panic("Error opening file. Perhaps it doesn't exist?");
    }
}

void construct(FILE *file, hash *table) {
    for (char *open_tag = get_line(file); open_tag; open_tag = get_line(file)) {
        if (!strstr(open_tag, "<list>")) {
            panic("Index file is malformatted");
        }
        char *token = open_tag + OPEN_TAG_LENGTH, *data = get_line(file);
        index_node *head = get(table, token);
        while(!strstr(data, "</list>")) {
            char *name = strtok(data, " ");
            while (name) {
                char *potential = strtok(NULL, " ");
                if (potential) {
                    int count = strtol(potential, NULL, 0);
                    if (count) {
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
                    name = strtok(NULL, " ");
                } else {
                    panic("Index file is malformatted");
                }
            }
            free(data);
            data = get_line(file);
        }
        free(data);
        free(open_tag);
    }
}

void handle_query(hash *table, char **queries, int count, query_type type) {
    hash *findings = create_hash();
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
    char **files = get_keys(findings);
    index_node *results[findings->count];
    int successes = 0;
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
    sort(results, 0, successes - 1);
    handle_output(results, successes);
    free(files);
}

void handle_output(index_node **results, int count) {
    int size = 100, filled = 0, needed = 0;
    char *output = (char *) malloc(sizeof(char) * size);
    for (int i = 0; i < count; i++) {
        index_node *result = results[i];
        char *format;
        if (i != count - 1) {
            needed = strlen(result->filename) + 2;
            output = enforce(output, needed, size, filled, &size);
            format = "%s, ";
        } else {
            needed = strlen(result->filename) + 1;
            output = enforce(output, needed, size, filled, &size);
            format = "%s";
        }
        sprintf(output + filled, format, result->filename);
        filled += needed;
    }
    output = enforce(output, 1, size, filled, &size);
    output[filled] = '\0';
    puts(output);
    free(output);
}

char *get_line(FILE *file) {
    int size = 1;
    char *line = (char *) malloc(sizeof(char) * size);
    if (line) {
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
            return line;
        } else {
            free(line);
            return NULL;
        }
    } else {
        panic("Could not allocate space for input string while reading from file");
    }
}

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

void panic(char *reason) {
    fprintf(stderr, "Sorry, the indexer has encountered an unrecoverable error. Given reason was: %s\n", reason);
    exit(EXIT_FAILURE);
}
