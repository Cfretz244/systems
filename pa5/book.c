#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"
#include "thread_hash.h"
#include "list.h"
#include "business.h"
#include "hash_node.h"

void construct_database(thread_hash *table, FILE *database);
char *get_line(FILE *file);
void panic(char *reason);

int main(int argc, char **argv) {
    if (argc != 4) {
        panic("Wrong number of arguments");
    }

    FILE *database = fopen(argv[1], "r"), *input = fopen(argv[2], "r"), *categories = fopen(argv[3], "r");
    if (database && input && categories) {
        thread_hash *table = create_thread_hash();
        construct_database(table, database);
    } else {
        panic("Could not open one of the input files. Perhaps it doesn't exist?");
    }
}

void construct_database(thread_hash *table, FILE *database) {
    for (char *line = get_line(database); line; line = get_line(database)) {
        char *name = strtok(line, "|");
        char *id_str = strtok(NULL, "|");
        char *credit_str = strtok(NULL, "|");
        char *street = strtok(NULL, "|");
        char *state = strtok(NULL, "|");
        char *zip = strtok(NULL, "|");

        int id = strtol(id_str, NULL, 0);
        float credit = strtof(credit_str, NULL);

        if (id && credit) {
            customer *money = create_customer(name, street, state, zip, id, credit);
            put(table, name, money);
        } else {
            panic("Database file is malformatted. Either ID or credit float/integer conversion failed");
        }
        free(line);
    }
}

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

// Function is responsible for reporting a fatal error and halting execution.
void panic(char *reason) {
    fprintf(stderr, "Sorry, the book order simulation has encountered an unrecoverable error. Given reason was: %s\n", reason);
    exit(EXIT_FAILURE);
}
