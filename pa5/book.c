#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "definitions.h"
#include "thread_hash.h"
#include "list.h"
#include "business.h"

/*----- Function stub declarations -----*/

void produce(FILE *input, thread_hash *consumers);
void *consume(void *args);
void construct_database(thread_hash *table, FILE *database);
thread_hash *generate_consumers(FILE *categories, thread_hash *table);
char *get_line(FILE *file);
char *unquote(char *str);
void panic(char *reason);

/*----- Function Implementations -----*/

int main(int argc, char **argv) {
    if (argc != 4) {
        panic("Wrong number of arguments");
    }

    // Open necessary files.
    FILE *database = fopen(argv[1], "r"), *input = fopen(argv[2], "r"), *categories = fopen(argv[3], "r");

    // Check status of opened files.
    if (!database || !input || !categories) {
        panic("Could not open one of the input files. Perhaps it doesn't exist?");
    }

    // Perform setup and begin producing.
    thread_hash *table = create_thread_hash();
    construct_database(table, database);
    thread_hash *consumers = generate_consumers(categories, table);
    produce(input, consumers);

    // Iterate across consumer hash and join every thread.
    char **keys = get_keys(consumers);
    for (int i = 0; i < consumers->count; i++) {
        char *key = keys[i];
        consumer *worker = get(consumers, key);
        pthread_join(*worker->thread, NULL);
    }

    // Deallocations.
    free(keys);
    destroy_thread_hash(consumers);
    destroy_thread_hash(table);
    fclose(database);
    fclose(input);
    fclose(categories);
}

void produce(FILE *input, thread_hash *consumers) {
    for (char *line = get_line(input); line; line = get_line(input)) {
        char *title = unquote(strtok(line, "|"));
        char *price_str = unquote(strtok(NULL, "|"));
        char *id_str = unquote(strtok(NULL, "|"));
        char *category = unquote(strtok(NULL, "|"));
        float price = strtof(price_str, NULL);
        int id = strtol(id_str, NULL, 0);
        if (price && id) {
            order *book = create_order(title, category, price, id);
            consumer *worker = get(consumers, category);
            if (worker) {
                lpush(worker->queue, book);
            } else {
                panic("Book order file is malformatted. It contains an invalid category");
            }
        } else {
            panic("Book order file is malformatted. Either ID or price integer/float conversion failed");
        }
        free(line);
    }
    char **keys = get_keys(consumers);
    for (int i = 0; i < consumers->count; i++) {
        char *key = keys[i];
        consumer *worker = get(consumers, key);
        order *book = create_order("stop", "#!~=", 0.0, 0);
        lpush(worker->queue, book);
    }
    free(keys);
}

void *consume(void *args) {
    void_args *arguments = (void_args *) args;
    thread_hash *table = arguments->table;
    list *queue = arguments->queue;
    free(args);
    while(true) {
        order *book = rpop(queue);
        printf("Received order for %s for user %d on thread %s\n", book->title, book->id, book->category);
        if (!strcmp(book->title, "stop") && !strcmp(book->category, "#!~=")) {
            break;
        }
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
            put(table, name, money, CUSTOMER);
        } else {
            panic("Database file is malformatted. Either ID or credit integer/float conversion failed");
        }
        free(line);
    }
}

thread_hash *generate_consumers(FILE *categories, thread_hash *table) {
    thread_hash *consumers = create_thread_hash();
    for (char *line = get_line(categories); line; line = get_line(categories)) {
        consumer *worker = create_consumer(consume, line, table);
        put(consumers, line, worker, CONSUMER);
    }
    return consumers;
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
