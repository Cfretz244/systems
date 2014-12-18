#include "list.h"

/*----- List Functions -----*/

// Function is responsible for creating a non-threaded list struct.
list *create_list() {
    list *lst = (list *) malloc(sizeof(list));

    if (lst) {
        lst->head = NULL;
        lst->count = 0;
        lst->size = -1;
        lst->threaded = false;
        lst->category = NULL;
        lst->mutex = NULL;
        lst->signal = NULL;
    }

    return lst;
}

// Function is responsible for creating a threaded list struct.
list *create_threaded_list(char *category, int size) {
    list *lst = (list *) malloc(sizeof(list));

    if (lst) {
        lst->head = NULL;
        lst->count = 0;
        
        // This line makes me sad.
        lst->size = size;

        lst->threaded = true;
        lst->category = (char *) malloc(sizeof(char) * (strlen(category) + 1));
        strcpy(lst->category, category);
        lst->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
        lst->signal = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
        pthread_mutex_init(lst->mutex, NULL);
        pthread_cond_init(lst->signal, NULL);
    }

    return lst;
}

void lpush(list *lst, order *data) {
    // Validate given parameters.
    if (!lst || !data) {
        return;
    }

    // Check if list is being used in a multithreaded environment,
    // and lock its mutex if so.
    list_node *node = create_list_node(data);
    if (lst->threaded) {
        pthread_mutex_lock(lst->mutex);
        // I was told that, although a linked list obviously has no maximum size,
        // I was required to enforce one for the purposes of the assignment.
        // So this code blocks of the queue is full and waits until a consumer
        // pops an element off of the queue.
        if (lst->count == lst->size) {
            puts("Producer waiting on consumers...");
            pthread_cond_wait(lst->signal, lst->mutex);
            puts("Producer resuming...");
        }
    }

    // Push data into list at head and increment count.
    if (lst->head) {
        node->next = lst->head;
        lst->head->prev = node;
        lst->head = node;
    } else {
        lst->head = node;
        lst->tail = node;
    }
    lst->count++;

    // Check if list is being used in a multithreaded environment, and
    // if so, unlock its mutex, and signal consumer thread that data has
    // been added.
    if (lst->threaded) {
        pthread_cond_signal(lst->signal);
        pthread_mutex_unlock(lst->mutex);
    }
}

order *rpop(list *lst) {
    // Validate given parameters and return immediately if given
    // list is empty and not slated for multithreaded use.
    if (!lst || (!lst->count && !lst->threaded)) {
        return NULL;
    }

    // Check if list is being used in a multithreaded environment, and
    // if so, check if the list currently contains any orders. If not,
    // release its lock and wait until signaled by the producer that data
    // has been added.
    if (lst->threaded) {
        pthread_mutex_lock(lst->mutex);
        if (!lst->count) {
            printf("Consumer for %s category waiting on producer...\n", lst->category);
            pthread_cond_wait(lst->signal, lst->mutex);
            puts("Consumer resuming...");
        }
    }

    // Pop data off the end of the queue and decrement count.
    list_node *node = lst->tail;
    if (lst->count > 1) {
        lst->tail = lst->tail->prev;
        lst->tail->next = NULL;
    } else {
        lst->head = NULL;
        lst->tail = NULL;
    }
    lst->count--;

    // Check if list is being used in a multithreaded environemnt, and
    // unlock its mutex if so.
    if (lst->threaded) {
        pthread_cond_signal(lst->signal);
        pthread_mutex_unlock(lst->mutex);
    }

    // Isolate the data, destroy the node, and return.
    order *book = node->data;
    destroy_list_node(node);
    return book;
}

// Function is responsible for destroying a list.
void destroy_list(list *lst) {
    // If list contains data, iterate across it, freeing nodes as we go.
    if (lst->head) {
        list_node *current = lst->head;
        while (current) {
            list_node *tmp = current;
            current = current->next;
            destroy_order(tmp->data);
            destroy_list_node(tmp);
        }
    }

    // Release the list's mutex and signal var if it has them.
    if (lst->threaded) {
        free(lst->category);
        pthread_mutex_destroy(lst->mutex);
        pthread_cond_destroy(lst->signal);
    }

    free(lst);
}

/*----- List Node Functions -----*/

// Function is responsible for creating a list node struct.
list_node *create_list_node(order *data) {
    list_node *node = (list_node *) malloc(sizeof(list_node));

    if (node) {
        node->data = data;
        node->next = NULL;
        node->prev = NULL;
    }

    return node;
}

// Function is responsible for destroying a list node.
void destroy_list_node(list_node *node) {
    free(node);
}
