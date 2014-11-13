#include "consumer.h"

consumer *create_consumer(void *(*thread_func) (void *), char *category) {
    consumer *worker = (consumer *) malloc(sizeof(consumer));

    if (worker) {
        worker->thread = (pthread_t *) malloc(sizeof(pthread_t));
        worker->category = category;
        worker->queue = create_list(true);
        pthread_create(worker->thread, NULL, thread_func, worker->queue);
    }

    return worker;
}

void destroy_consumer(consumer *worker) {
    // Need to look up how to destroy a pthread.
    free(worker);
}
