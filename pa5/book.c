#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"
#include "thread_hash.h"
#include "list.h"
#include "business.h"
#include "hash_node.h"

void construct_database(thread_hash *table, FILE *database);
pthread_t **generate_consumers(FILE *categories);
char *get_line(FILE *file);
char *unquote(char *str);
void panic(char *reason);

int main(int argc, char **argv) {
    if (argc != 4) {
        panic("Wrong number of arguments");
    }

    FILE *database = fopen(argv[1], "r"), *input = fopen(argv[2], "r"), *categories = fopen(argv[3], "r");
    if (database && input && categories) {
        thread_hash *table = create_thread_hash();
        construct_database(table, database);
        pthread_t **threads = generate_consumers(categories);
    } else {
        panic("Could not open one of the input files. Perhaps it doesn't exist?");
    }
}

void construct_database(thread_hash *table, FILE *database) {
    for (char *line = get_line(database); line; line = get_line(database)) {
        char *name = unquote(strtok(line, "|"));
        char *id_str = unquote(strtok(NULL, "|"));
        char *credit_str = unquote(strtok(NULL, "|"));
        char *street = unquote(strtok(NULL, "|"));
        char *state = unquote(strtok(NULL, "|"));
        char *zip = unquote(strtok(NULL, "|"));

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

pthread_t **generate_consumers(FILE *categories) {
    // Revisit this in a bit
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

char *unquote(char *str) {
    if (*str == '"') {
        str++;
    }
    int length = strlen(str);
    if (*(str + length - 1) == '"') {
        *(str + length - 1) = '\0';
    }
    return str;
}

// Function is responsible for reporting a fatal error and halting execution.
void panic(char *reason) {
    fprintf(stderr, "Sorry, the book order simulation has encountered an unrecoverable error. Given reason was: %s\n", reason);
    exit(EXIT_FAILURE);
}
