#ifndef CONSUMER_H
#define CONSUMER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "list.h"

typedef struct consumer {
    pthread_t *thread;
    char *category;
    list *queue;
} consumer;

consumer *create_consumer(void *(*thread_func) (void *), char *category);
void destroy_consumer(consumer *worker);

#endif
