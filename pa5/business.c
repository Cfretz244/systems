#include "business.h"

/*----- Customer Functions -----*/

customer *create_customer(char *name, char *street, char *state, char *zip, int id, float credit) {
    customer *user = (customer *) malloc(sizeof(customer));

    if (user) {
        user->name = (char *) malloc(sizeof(char) * (strlen(name) + 1));
        strcpy(user->name, name);
        user->street = (char *) malloc(sizeof(char) * (strlen(street) + 1));
        strcpy(user->street, street);
        user->state = (char *) malloc(sizeof(char) * (strlen(state) + 1));
        strcpy(user->state, state);
        user->zip = (char *) malloc(sizeof(char) * (strlen(zip) + 1));
        strcpy(user->zip, zip);
        user->id = id;
        user->start_credit = credit;
        user->credit = credit;
        user->approved = create_list(false);
        user->rejected = create_list(false);
        user->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
        pthread_mutex_init(user->mutex, NULL);
    }

    return user;
}

bool customers_are_equal(customer *f, customer *s) {
    int names = !strcmp(f->name, s->name), streets = !strcmp(f->street, s->street);
    int states = !strcmp(f->state, s->state), zips = !strcmp(f->zip, s->zip);
    return names && streets && states && zips && f->id == s->id && f->credit == s->credit;
}

void destroy_customer(customer *user) {
    destroy_list(user->approved);
    destroy_list(user->rejected);
    free(user->name);
    free(user->street);
    free(user->state);
    free(user->zip);
    pthread_mutex_destroy(user->mutex);
    free(user);
}

/*----- Order Functions -----*/

order *create_order(char *title, char *category, float price, int id) {
    order *book = (order *) malloc(sizeof(order));

    if (book) {
        book->title = (char *) malloc(sizeof(char) * (strlen(title) + 1));
        strcpy(book->title, title);
        book->category = (char *) malloc(sizeof(char) * (strlen(category) + 1));
        strcpy(book->category, category);
        book->price = price;
        book->remaining = 0;
        book->id = id;
    }

    return book;
}

bool orders_are_equal(order *f, order *s) {
    int titles = !strcmp(f->title, s->title), categories = !strcmp(f->category, s->category);
    return titles && categories && f->price == s->price && f->id == s->id;
}

void destroy_order(order *book) {
    free(book->title);
    free(book->category);
    free(book);
}

/*----- Consumer Functions -----*/

consumer *create_consumer(void *(*thread_func) (void *), char *category, array *users) {
    consumer *worker = (consumer *) malloc(sizeof(consumer));

    if (worker) {
        worker->thread = (pthread_t *) malloc(sizeof(pthread_t));
        worker->category = category;
        worker->queue = create_list(true);
        void_args *args = (void_args *) malloc(sizeof(void_args));
        args->users = users;
        args->queue = worker->queue;
        pthread_create(worker->thread, NULL, thread_func, args);
    }

    return worker;
}

void destroy_consumer(consumer *worker) {
    // Need to look up how to destroy a pthread.
    free(worker);
}
