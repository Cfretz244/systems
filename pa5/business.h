#ifndef BUSINESS
#define BUSINESS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "definitions.h"
#include "list.h"

// Customer Struct.
typedef struct customer {
    char *name;
    int id;
    float credit;
    list *approved, *rejected;
} customer;

// Order Struct.
typedef struct order {
    char *title, *category;
    float price;
}

customer *create_customer(/* stuff */);
bool customers_are_equal(customer *f, customer *s);
void destroy_customer(customer *cust);

#endif
