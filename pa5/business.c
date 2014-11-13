#include "business.h"

/*----- Customer Functions -----*/

customer *create_customer(char *name, char *street, char *state, char *zip, int id, float credit) {
    customer *money = (customer *) malloc(sizeof(customer));

    if (money) {
        money->name = name;
        money->street = street;
        money->state = state;
        money->zip = zip;
        money->id = id;
        money->credit = credit;
        money->approved = create_list(false);
        money->rejected = create_list(false);
    }

    return money;
}

void destroy_customer(customer *money) {
    destroy_list(money->approved);
    destroy_list(money->rejected);
    free(money->name);
    free(money->street);
    free(money->state);
    free(money->zip);
    free(money);
}

/*----- Order Functions -----*/

order *create_order(char *title, char *category, float price, int id) {
    order *book = (order *) malloc(sizeof(order));

    if (book) {
        book->title = title;
        book->category = category;
        book->price = price;
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
