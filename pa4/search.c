#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "definitions.h"
#include "nodes.h"
#include "hash.h"

#define OPEN_TAG_LENGTH 7

void construct(FILE *file, hash *table);
char *get_line(FILE *file);
void panic(char *reason);

int main(int argc, char **argv) {
    if (argc != 2) {
        panic("Wrong number of arguments");
    }

    FILE *input = fopen(argv[1], "r");
    if (input) {
        hash *table = create_hash();
        construct(input, table);
        printf("Finished.\n");
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
                    panic("Could not allocate space for input string");
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
        panic("Could not allocate space for input string");
    }
}

void panic(char *reason) {
    fprintf(stderr, "Sorry, the indexer has encountered an unrecoverable error. Given reason was: %s\n", reason);
    exit(EXIT_FAILURE);
}
