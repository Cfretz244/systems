#ifndef ARRAY_H
#define ARRAY_H

/*----- Includes without dependencies -----*/

#include <stdlib.h>
#include <string.h>
#include "definitions.h"
#define INIT_ARRAY_LENGTH 8

typedef struct array {
    void **storage;
    int size, count;
} array;

array *create_array();
bool insert(array *arr, int index, void *data);
void *retrieve(array *arr, int index);
void *clear(array *arr, int index);
void destroy_array(array *arr, void (*func) (void *));

#endif
