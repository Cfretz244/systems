#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "definitions.h"
#include "hash.h"
#include "array.h"
#include "list.h"
#include "business.h"

#define BEGIN_STR_LENGTH 59
#define ID_STR_LENGTH 20
#define BALANCE_STR_LENGTH 64
#define SUCCESSFUL_STR_LENGTH 26
#define REJECTED_STR_LENGTH 24
#define END_STR_LENGTH 27

/*----- Function stub declarations -----*/

void produce(FILE *input, hash *consumers);
void *consume(void *args);
void construct_database(array *users, FILE *database);
hash *generate_consumers(FILE *categories, array *users);
void handle_output(array *users);
char *get_line(FILE *file);
char *enforce(char *str, int needed, int size, int filled, int *new_size);
int digits(int num);
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
    array *users = create_array();
    construct_database(users, database);
    hash *consumers = generate_consumers(categories, users);
    produce(input, consumers);

    // Iterate across consumer hash and join every thread.
    char **keys = get_keys(consumers);
    for (int i = 0; i < consumers->count; i++) {
        char *key = keys[i];
        consumer *worker = get(consumers, key);
        pthread_join(*worker->thread, NULL);
    }
    free(keys);

    handle_output(users);

    // Deallocations.
    destroy_hash(consumers);
    destroy_array(users, (void (*) (void *)) destroy_customer);
    fclose(database);
    fclose(input);
    fclose(categories);
}

void produce(FILE *input, hash *consumers) {
    for (char *line = get_line(input); line; line = get_line(input)) {
        char *title = strtok(line, "|");
        char *price_str = strtok(NULL, "|");
        char *id_str = strtok(NULL, "|");
        char *category = strtok(NULL, "|");
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
    array *users = arguments->users;
    list *queue = arguments->queue;
    free(args);
    while(true) {
        order *book = rpop(queue);
        if (!strcmp(book->title, "stop") && !strcmp(book->category, "#!~=")) {
            break;
        }
        customer *user = retrieve(users, book->id);
        pthread_mutex_lock(user->mutex);
        if (user->credit >= book->price) {
            user->credit -= book->price;
            book->remaining = user->credit;
            lpush(user->approved, book);
            printf("Order Confirmation: %s (listed for $%.2f) will be delivered to %s at %s, %s %s.\n",
                    book->title, book->price, user->name, user->street, user->state, user->zip);
        } else {
            lpush(user->rejected, book);
            printf("Order Rejection: %s does not have sufficient funds to order %s. Remaining funds: $%.2f\n",
                    user->name, book->title, user->credit);
        }
        pthread_mutex_unlock(user->mutex);
    }
    return NULL;
}

void construct_database(array *users, FILE *database) {
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
            customer *user = create_customer(name, street, state, zip, id, credit);
            insert(users, id, user);
        } else {
            panic("Database file is malformatted. Either ID or credit integer/float conversion failed");
        }
        free(line);
    }
}

hash *generate_consumers(FILE *categories, array *users) {
    hash *consumers = create_hash();
    for (char *line = get_line(categories); line; line = get_line(categories)) {
        consumer *worker = create_consumer(consume, line, users);
        put(consumers, line, worker, CONSUMER);
    }
    return consumers;
}

void handle_output(array *users) {
    int size = 128, filled = 0, needed = 0;
    char *output = (char *) malloc(sizeof(char) * size);
    for (int i = 0; i < users->size; i++) {
        customer *user = retrieve(users, i);
        if (user) {
            needed = strlen(user->name) + BEGIN_STR_LENGTH + 1;
            output = enforce(output, needed, size, filled, &size);
            sprintf(output + filled, "=== BEGIN CUSTOMER INFO ===\n### BALANCE ###\nCustomer name: %s\n", user->name);
            filled += needed;

            needed = digits(user->id) + ID_STR_LENGTH + 1;
            output = enforce(output, needed, size, filled, &size);
            sprintf(output + filled, "Customer ID number: %d\n", user->id);
            filled += needed;

            needed = BALANCE_STR_LENGTH + digits((int) user->credit) + 4;
            output = enforce(output, needed, size, filled, &size);
            sprintf(output +  filled, "Remaining credit balance after all purchases (a dollar amount): %.2f\n", user->credit);
            filled += needed;

            needed = SUCCESSFUL_STR_LENGTH;
            output = enforce(output, needed, size, filled, &size);
            sprintf(output + filled, "### SUCCESSFUL ORDERS ###\n");
            filled += needed;

            list *approved = user->approved;
            for (order *success = rpop(approved); success; success = rpop(approved)) {
                needed = strlen(success->title) + 1 + digits((int) success->price) + 4 + digits((int) success->remaining) + 4;
                output = enforce(output, needed, size, filled, &size);
                sprintf(output + filled, "%s|%.2f|%.2f\n", success->title, success->price, success->remaining);
                filled += needed;
                destroy_order(success);
            }

            needed = REJECTED_STR_LENGTH;
            output = enforce(output, needed, size, filled, &size);
            sprintf(output + filled, "### REJECTED ORDERS ###\n");
            filled += needed;

            list *rejected = user->rejected;
            for (order *failure = rpop(rejected); failure; failure = rpop(rejected)) {
                needed = strlen(failure->title) + 1 + digits((int) failure->price) + 4;
                output = enforce(output, needed, size, filled, &size);
                sprintf(output + filled, "%s|%.2f\n", failure->title, failure->price);
                filled += needed;
                destroy_order(failure);
            }

            needed = END_STR_LENGTH;
            output = enforce(output, needed, size, filled, &size);
            sprintf(output + filled, "=== END CUSTOMER INFO ===\n\n");
            filled += needed;
        }
    }
    output[filled - 2] = '\0';
    puts(output);
    remove("output.txt");
    FILE *file = fopen("output.txt", "wr");
    fputs(output, file);
    fclose(file);
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

int digits(int num) {
    int count = 0;
    while (num > 0) {
        num /= 10;
        count++;
    }
    return count;
}

// Function is responsible for reporting a fatal error and halting execution.
void panic(char *reason) {
    fprintf(stderr, "Sorry, the book order simulation has encountered an unrecoverable error. Given reason was: %s\n", reason);
    exit(EXIT_FAILURE);
}
