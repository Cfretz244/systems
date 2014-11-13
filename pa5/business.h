#ifndef BUSINESS_H
#define BUSINESS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "definitions.h"
#include "list.h"

#ifndef LIST_DECLARE
#define LIST_DECLARE
typedef struct list list;
#endif

// Circularly dependent Order struct declaration.
// I know I probably should have just designed things differently,
// but I wanted experience resolving a circular dependency.
#ifdef ORDER_DECLARE
struct order {
#else
typedef struct order {
#endif
    char *title, *category;
    float price;
    int id;
#ifdef ORDER_DECLARE
};
#else
#define ORDER_DECLARE
} order;
#endif

// Customer Struct.
typedef struct customer {
    char *name, *street, *state, *zip;
    int id;
    float credit;
    list *approved, *rejected;
} customer;

customer *create_customer(char *name, char *street, char *state, char *zip, int id, float credit);
bool customers_are_equal(customer *f, customer *s);
void destroy_customer(customer *money);

order *create_order(char *title, char *category, float price, int id);
bool orders_are_equal(order *f, order *s);
void destroy_order(order *book);

#endif
